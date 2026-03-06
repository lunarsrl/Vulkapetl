#include "graphics.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/dynarr/dynString/dynstring.h"
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
    createinfo.enabledLayerCount = 0;
    // requesting glfw extensions

    uint32_t glfwextcount= 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&glfwextcount);
    // VkResult extension Properties = vkEnumerateInstanceExtensionProperties("fe", glfwextcount, );


    if (extensions == NULL) {
        info("No extentions needed (EXTENTIONS NOT SUPPORTED YET)");
    } else {
        for (int i = 0; i > glfwextcount; ++i) {
            char* mesg = strcat("[ext|", "" + (0+i));
            info(mesg);
        }

        if (glfwextcount == 0) {
            warning("No extentions loaded");
        }
    }


    createinfo.enabledExtensionCount = glfwextcount;
    createinfo.ppEnabledExtensionNames = NULL;
    createinfo.pNext = NULL;

    VkInstance instance;
    if (vkCreateInstance(&createinfo, NULL, &instance) == VK_SUCCESS) {
        info("Vulkan instance succesfully created");
        return instance;
    } else {
        panic("Vulkan instance could not be created...");
    }


}

GLFWwindow *glfw_init() {
    glfwInit();
    if (GLFW_FALSE == glfwInit()) {
        printf("Failed to initalize glfw");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    const uint32_t SWIDTH = 800;
    const uint32_t SHEIGHT = 600;

    dynStr vaLayers = initDynStr(1);
    pushDynStr(&vaLayers, "VK_LAYER_KHRONOS_validation");

    if (DEBUG_TEST) {


    }

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
    return 0
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

        if (findQueueFamilies(device)) {
            model->phys_gpu = *device;
            break;
        }
    }
    printf("EXITING...");
}

void createLogicalDevice(Vulkapetl* model) {
    VkDeviceQueueCreateInfo info;
    float queue_priority = 1.0f;



    info.queueFamilyIndex= findQueueFamilies(&model->phys_gpu);
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

