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
    vkEnumeratePhysicalDevices(model->instance, &device_count, devices);

    for (int i = 0; i < device_count; ++i) {
        printf("Device #%i\n", i);
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceProperties(devices[i], &properties);
        vkGetPhysicalDeviceFeatures(devices[i], &features);

        printf("> Name: %s \n> Id: %i\n", properties.deviceName, properties.deviceID);
        printf("> Geometry Shader?: %i \n> Tesselation Shader?: %i\n", features.geometryShader, features.tessellationShader);
    }
}

void findQueueFamilies(VkPhysicalDevice* device) {
    VkQueueFamilyProperties properties;
    int32_t queue_count;
    vkGetPhysicalDeviceQueueFamilyProperties2(*device, &queue_count, NULL);

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

