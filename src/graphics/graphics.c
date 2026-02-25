#include "graphics.h"
#include <stdio.h>
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

void vulkan_init() {
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
        printf("No extensions needed \n");
    }
    if (glfwextcount == 0) {
        printf("No extensions found \n");
    }
    for (int i = 0; i > glfwextcount; ++i) {
        printf("extensions: %s \n", extensions[i]);
    }
    createinfo.enabledExtensionCount = glfwextcount;
    createinfo.ppEnabledExtensionNames = NULL;
    createinfo.pNext = NULL;

    VkInstance instance;
    if (vkCreateInstance(&createinfo, NULL, &instance) == VK_SUCCESS) {
        printf("Vulkan instance created! \n");
    }

}


GLFWwindow *glfw_init() {
    glfwInit();
    if (GLFW_FALSE == glfwInit()) {
        printf("Failed to initalize glfw");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    char* string1 = "Hello ";
    char* string2 = "World!";

    if (DEBUG_TEST == 0) {
        bool enable_validation = true;
    }

    const uint32_t SWIDTH = 800;
    const uint32_t SHEIGHT = 600;
    GLFWwindow *window = glfwCreateWindow(SWIDTH, SHEIGHT, "Vulkan is fun guys", NULL, NULL);

    return window;
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
