#include "graphics.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/dynarr/dynString/dynstring.h"
#include "../include/dynarr/dynChar/dynchar.h"
#include "../include/logger/log.h"
#define GLFW_INCLUDE_VULKAN
#define debug_print(fmt, ...) \
    do ( if (DBUG) fprintf(stderr, fmt, __V_ARGS_); ) while (0)

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif


#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>




void mainloop(GLFWwindow *);
void glfw_exit(GLFWwindow *);
dynStr get_required_ext_properties();
dynStr get_requested_layers();



VkInstance vulkan_init() {
    VkApplicationInfo appinfo;
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pApplicationName = "Vulkapetl";
    appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.pEngineName = "No Engine";
    appinfo.apiVersion = VK_API_VERSION_1_4;
    appinfo.pNext = NULL;

    VkInstanceCreateInfo createinfo;
    createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createinfo.pApplicationInfo = &appinfo;
    // requesting glfw extensions

    dynStr layers = get_requested_layers();
    dynStr extproperties = get_required_ext_properties();

    createinfo.enabledLayerCount = layers.length;
    createinfo.ppEnabledLayerNames = (const char * const *) layers.items;
    createinfo.enabledExtensionCount = extproperties.length;
    createinfo.ppEnabledExtensionNames = (const char * const *)extproperties.items;

    createinfo.pNext = NULL;

    VkInstance instance;
    if (vkCreateInstance(&createinfo, NULL, &instance) == VK_SUCCESS) {
        info("Vulkan instance succesfully created");
        return instance;
    } else {
        panic("Vulkan instance could not be created...");
    }


}

dynStr get_required_ext_properties() {
    uint32_t e_count = 0;
    dynStr req_ext = initDynStr(5);

    const char**  a = glfwGetRequiredInstanceExtensions(&e_count);
    for (int i = 0; i<e_count; ++i) {
        pushDynStr(&req_ext, (char*) a[i]);
    }
    uint32_t p_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &p_count, NULL);
    VkExtensionProperties extention_properties[p_count];
    VkResult result = vkEnumerateInstanceExtensionProperties(NULL, &p_count, extention_properties);
    switch (result) {
        case VK_SUCCESS:
            success("All extension properties returned");
            break;
        case VK_INCOMPLETE:
            success("Not all extension properties returned");
            break;
        default:
            panic("Failed to return extension properties");
    }

    for (int i = 0; i < p_count; ++i) {
        if (req_ext.length == 0) {
            break;
        }
        for (int l = 0; l < req_ext.length; ++l) {
            if (strcmp(req_ext.items[l], extention_properties[i].extensionName)) {
                delDynStr(&req_ext, l);
            };
        }
    }

    if (req_ext.length == 0 ) {
        success("All requirements met (extention properties)");
    } else {
        panic("Missing required extention properties");
    }
    return req_ext;
}

dynStr get_requested_layers() {
    dynStr layers_req = initDynStr(1);
    pushDynStr(&layers_req, "VK_LAYER_KHRONOS_validation");

    int lp_count;
    vkEnumerateInstanceLayerProperties(&lp_count, nullptr);
    printf("%i \n", lp_count);
    VkLayerProperties layer_properties[lp_count];
    VkResult resultlp = vkEnumerateInstanceLayerProperties(&lp_count, layer_properties);
    switch (resultlp) {
        case VK_SUCCESS:
            success("All layer properties found");
            break;
        case VK_INCOMPLETE:
            warning("Some layer properties found");
            break;
        default:
            panic("Layer properties not found");
            break;
    }

    for (int j = 0; j< lp_count; ++j) {
        for (int i = 0; i < layers_req.length; ++i) {
            if (strcmp(layers_req.items[i], layer_properties[j].layerName)) {
                delDynStr(&layers_req, i);
            }
        }
    }

    if (layers_req.length == 0 ) {
        success("All requirements met (layer properties)");
    } else {
        panic("Missing required validation layers");
    }
    return layers_req;
}

GLFWwindow* glfw_init() {
    glfwInit();
    if (GLFW_FALSE == glfwInit()) {
        printf("Failed to initalize glfw");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    const uint32_t SWIDTH = 800;
    const uint32_t SHEIGHT = 600;

    GLFWwindow *window = glfwCreateWindow(SWIDTH, SHEIGHT, "Vulkan is fun guys", NULL, NULL);
    return window;
}


int findQueueFamilies(const VkPhysicalDevice* device) {
    printf("Function: Find Queue Families\n");
    uint32_t queue_count;
    vkGetPhysicalDeviceQueueFamilyProperties2(*device, &queue_count, nullptr);

    printf("> Queue Total: %i\n", queue_count);
    VkQueueFamilyProperties properties[queue_count];

    vkGetPhysicalDeviceQueueFamilyProperties(*device, &queue_count, properties);

    for (int i = 0; i < queue_count; ++i) {
        printf("> > Queue Family #%i\n", i);
        printf("> > > Queues in family: %i\n", properties[i].queueCount);

        if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            printf("> > > contains graphics queue!\n");
            return i;
        }

    }
    return -1;
}

void pickVulkanPhysDevice(Vulkapetl* model) {
    uint32_t device_count = 0;
    VkResult result = vkEnumeratePhysicalDevices(model->instance, &device_count, nullptr);
    switch (result) {
        default:
            panic("Failed to return physical devices");
            break;
        case VK_SUCCESS:
            success("All Physical devices returned");
            break;
        case VK_INCOMPLETE:
            success("Not all physical devices were returned");
            break;
    }

    printf("Device Count: %i\n", device_count);
    VkPhysicalDevice devices[device_count];
    VkResult a = vkEnumeratePhysicalDevices(model->instance, &device_count, devices);

    for (int i = 0; i < device_count; ++i) {
        printf("Device #%i\n", i);
        VkPhysicalDevice* device = &devices[i];
        VkPhysicalDeviceProperties2 properties;
        VkPhysicalDeviceFeatures2 features;


        vkGetPhysicalDeviceProperties2(*device, &properties);
        vkGetPhysicalDeviceFeatures2(*device, &features);

        printf("> Name: %s \n> Id: %i\n", properties.properties.deviceName, properties.properties.deviceID);
        printf("> Geometry Shader?: %i \n> Tesselation Shader?: %i\n", features.features.geometryShader, features.features.tessellationShader);

        if (findQueueFamilies(device) != -1) {
            printf("PHYS DEVICE PICKED \n");
            model->phys_gpu = *device;
            break;
        }
    }
    printf("EXITING PICK DEVICE... \n");
}

void createLogicalDevice(Vulkapetl* model) {
    VkDeviceQueueCreateInfo info;
    float queue_priority = 1.0f;


    info.queueFamilyIndex = findQueueFamilies(&model->phys_gpu);
    info.queueCount = 1;
    info.pQueuePriorities = &queue_priority;
    info.pNext = nullptr;
    info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    // doesnt do anything :(
    info.flags = 0;

    // Vulkan Feature Structs
    VkPhysicalDeviceFeatures2 vulkan_device_features2;
    vulkan_device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    VkPhysicalDeviceVulkan13Features device_vulkan13_features;
    device_vulkan13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    device_vulkan13_features.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT ext_dyn_state_features;
    ext_dyn_state_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
    ext_dyn_state_features.extendedDynamicState = VK_TRUE;

    // --- Feature struct chaining
    ext_dyn_state_features.pNext = nullptr;
    device_vulkan13_features.pNext = &ext_dyn_state_features;
    vulkan_device_features2.pNext = &device_vulkan13_features;


}

void mainloop(GLFWwindow *window) {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void glfw_exit(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

