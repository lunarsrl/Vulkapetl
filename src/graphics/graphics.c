#include "graphics.h"
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <assert.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/dynarr/dynString/dynstring.h"
#include "../include/dynarr/dynChar/dynchar.h"
#include "../include/logger/log.h"
#define debug_print(fmt, ...) \
do ( if (DBUG) fprintf(stderr, fmt, __V_ARGS_); ) while (0)

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define max(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a > _b ? _a : _b; })

void mainloop(GLFWwindow *);
void glfw_exit(GLFWwindow *);
dynStr get_required_ext_properties();
dynStr get_requested_layers();
VkSurfaceFormatKHR choose_swap_format(VkSurfaceFormatKHR* formats, uint fnum);
VkExtent2D choose_swap_extent(Vulkapetl *model, VkSurfaceCapabilitiesKHR capabilities);
VkPresentModeKHR choose_swap_present_mode(Vulkapetl *model);
void create_surface(Vulkapetl*);
void eval_vkresult(VkResult a, char* specifier);



void vulkan_init(Vulkapetl* appmodel) {
    VkApplicationInfo appinfo = {0};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pApplicationName = "Vulkapetl";
    appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.pEngineName = "No Engine";
    appinfo.apiVersion = VK_API_VERSION_1_4;
    appinfo.pNext = NULL;

    VkInstanceCreateInfo createinfo = {0};
    createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createinfo.pApplicationInfo = &appinfo;
    // requesting glfw extensions

    dynStr layers = get_requested_layers();
    // printf("LAYERS: \n");
    // for (int i = 0; i < layers.length; ++i) {
    //     printf("%s\n", layers.items[i]);
    // }
    dynStr extproperties = get_required_ext_properties();
    // printf("EXTPRP: \n");
    // for (int i = 0; i < extproperties.length; ++i) {
    //     printf("%s\n", extproperties.items[i]);
    // }

    // createinfo.enabledLayerCount = layers.length;
    // createinfo.ppEnabledLayerNames = (const char* const *) layers.items;

    createinfo.enabledLayerCount = 0;
    createinfo.ppEnabledLayerNames = nullptr;
    createinfo.enabledExtensionCount = extproperties.length;
    createinfo.ppEnabledExtensionNames = (const char * const *)extproperties.items;
    createinfo.pNext = NULL;

    VkInstance instance = {0};
    VkResult result = vkCreateInstance(&createinfo, NULL, &instance);
    eval_vkresult(result, "vkCreateInstance");

    appmodel->instance = instance;
    create_surface(appmodel);
}

void create_surface(Vulkapetl* appmodel) {
    VkResult result = glfwCreateWindowSurface(appmodel->instance, appmodel->main_window, nullptr, &appmodel->main_surface);
    eval_vkresult(result, "glfwCreateWindowSurface");
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

    dynStr verified = initDynStr(req_ext.length);
    for (int i = 0; i < p_count; ++i) {
        if (req_ext.length == 0) {
            break;
        }
        for (int l = 0; l < req_ext.length; ++l) {
            if (strcmp(req_ext.items[l], extention_properties[i].extensionName)) {
                pushDynStr(&verified, req_ext.items[l]);
                delDynStr(&req_ext, l);
            };
        }
    }

    if (req_ext.length == 0 ) {
        success("All requirements met (extention properties)");
    } else {
        panic("Missing required extention properties");
    }
    return verified;
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

    dynStr verified = initDynStr(layers_req.length);
    for (int j = 0; j< lp_count; ++j) {
        for (int i = 0; i < layers_req.length; ++i) {
            if (strcmp(layers_req.items[i], layer_properties[j].layerName)) {
                pushDynStr(&verified, layers_req.items[i]);
                delDynStr(&layers_req, i);
            }
        }
    }

    if (layers_req.length == 0 ) {
        success("All requirements met (layer properties)");
    } else {
        panic("Missing required validation layers");
    }
    return verified;
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
    uint32_t queue_count;
    vkGetPhysicalDeviceQueueFamilyProperties2(*device, &queue_count, nullptr);

    // printf("> Queue Total: %i\n", queue_count);
    VkQueueFamilyProperties properties[queue_count];

    vkGetPhysicalDeviceQueueFamilyProperties(*device, &queue_count, properties);

    for (int i = 0; i < queue_count; ++i) {
        // printf("> > Queue Family #%i\n", i);
        // printf("> > > Queues in family: %i\n", properties[i].queueCount);
        if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            // printf("> > > contains graphics queue!\n");
            return i;
        }

    }
    return -1;
}

void pivk_vulkan_phys_device(Vulkapetl* model) {
    uint32_t device_count = 0;
    VkResult result = vkEnumeratePhysicalDevices(model->instance, &device_count, nullptr);
    eval_vkresult(result, "vkEnumeratePhysicalDevices [1]");


    // printf("Device Count: %i\n", device_count);
    VkPhysicalDevice devices[device_count];
    VkResult a = vkEnumeratePhysicalDevices(model->instance, &device_count, devices);
    eval_vkresult(a, "vkEnumeratePhysicalDevices [2]");

    for (int i = 0; i < device_count; ++i) {
        // printf("Device #%i\n", i);
        VkPhysicalDevice* device = &devices[i];
        VkPhysicalDeviceProperties2 properties = {0};
        VkPhysicalDeviceFeatures2 features = {0};


        vkGetPhysicalDeviceProperties2(*device, &properties);
        vkGetPhysicalDeviceFeatures2(*device, &features);

        // printf("> Name: %s \n> Id: %i\n", properties.properties.deviceName, properties.properties.deviceID);
        // printf("> Geometry Shader?: %i \n> Tesselation Shader?: %i\n", features.features.geometryShader, features.features.tessellationShader);

        if (findQueueFamilies(device) != -1) {
            model->phys_gpu = *device;
            break;
        }
    }
}

void create_logi_device(Vulkapetl* model) {
    int queue_index = ~0;
    uint32_t queue_count;
    vkGetPhysicalDeviceQueueFamilyProperties2(model->phys_gpu, &queue_count, nullptr);

    VkQueueFamilyProperties properties[queue_count];

    vkGetPhysicalDeviceQueueFamilyProperties(model->phys_gpu, &queue_count, properties);


    VkBool32 a;
    for (int qfp_index = 0; qfp_index < queue_count; ++qfp_index) {
        if ( properties[qfp_index].queueFlags & VK_QUEUE_GRAPHICS_BIT && vkGetPhysicalDeviceSurfaceSupportKHR(model->phys_gpu, qfp_index, model->main_surface, &a) == 0) {
            if (a == VK_TRUE) {
                queue_index = qfp_index;
                success("Surface Supported!");
                break;
            } else {
                panic("Surface Unsupported :(");
            }
        }
    }

    if (queue_index == ~0) {
        panic("No queue family that supports both graphics and surface presenetation found...");
    }

    VkDeviceQueueCreateInfo queue_info = {0};
    float queue_priority = 1.0f;
    queue_info.queueFamilyIndex = queue_index;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = &queue_priority;
    queue_info.pNext = nullptr;
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    // doesnt do anything :(
    queue_info.flags = 0;

    // Vulkan Feature Structs
    VkPhysicalDeviceFeatures2 vulkan_device_features2 = {0};
    vulkan_device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

    VkPhysicalDeviceVulkan13Features device_vulkan13_features = {0};
    device_vulkan13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    device_vulkan13_features.dynamicRendering = VK_TRUE;

    VkPhysicalDeviceExtendedDynamicStateFeaturesEXT ext_dyn_state_features = {0};
    ext_dyn_state_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
    ext_dyn_state_features.extendedDynamicState = VK_TRUE;

    ext_dyn_state_features.pNext = nullptr;
    device_vulkan13_features.pNext = &ext_dyn_state_features;
    vulkan_device_features2.pNext = &device_vulkan13_features;

    dynStr req_ext_devices = initDynStr(1);
    pushDynStr(&req_ext_devices, VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    VkDeviceQueueCreateInfo queuearr[1];
    queuearr[0] = queue_info;

    VkDeviceCreateInfo device_info = {0};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = nullptr;
    device_info.flags = 0;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = queuearr;
    // legacy unused never change ever
    device_info.enabledLayerCount = 0;
    device_info.ppEnabledLayerNames = nullptr;
    // --------------
    device_info.enabledExtensionCount = req_ext_devices.length;
    device_info.ppEnabledExtensionNames = (const char* const *) req_ext_devices.items;
    device_info.pEnabledFeatures = nullptr;

    VkDevice device = {0};
    VkResult result = vkCreateDevice(model->phys_gpu, &device_info, nullptr, &device);

    switch (result) {
        case VK_SUCCESS:
            model->logi_gpu = device;
            success("Logical Device successfully created");
            break;
        default:
            printf("Error Code: %i \n", result);
            panic("Logical Device Creation Failed");
            break;
    }

    VkQueue graphics_queue = VK_NULL_HANDLE;
    vkGetDeviceQueue(device, queue_index, 0, &graphics_queue);
    model->graphics_queue = graphics_queue;
}


int choose_min_swapi_mage_count(VkSurfaceCapabilitiesKHR* capabilities) {
    int minimagenum = max(3, capabilities->minImageCount);
    if (0 < capabilities->maxImageCount && capabilities->maxImageCount < capabilities->minImageCount) {
        minimagenum = capabilities->maxImageCount;
    }
    return minimagenum;
}

void create_swapchain(Vulkapetl* model) {
    VkSurfaceCapabilitiesKHR capabilities_khr = {0};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(model->phys_gpu, model->main_surface, &capabilities_khr);

    uint format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(model->phys_gpu, model->main_surface, &format_count, nullptr);
    VkSurfaceFormatKHR formats[format_count] = {};
    VkResult sfresult = vkGetPhysicalDeviceSurfaceFormatsKHR(model->phys_gpu, model->main_surface, &format_count, formats);

    switch (sfresult) {
        case VK_SUCCESS:
            success("Found all formats");
            break;
        default:
            printf("ERROR CODE: {}\n", sfresult);
            panic("Failed to get all formats.");
    }

    VkExtent2D swapchain_extent = choose_swap_extent(model, capabilities_khr);
    VkSurfaceFormatKHR surface_format = choose_swap_format(formats, format_count);
    int min_image_count = choose_min_swapi_mage_count(&capabilities_khr);

    VkSwapchainCreateInfoKHR swapchain_info = {0};
    swapchain_info.surface = model->main_surface;
    swapchain_info.minImageCount = min_image_count;
    swapchain_info.imageFormat = surface_format.format;
    swapchain_info.imageColorSpace = surface_format.colorSpace;
    swapchain_info.imageExtent = swapchain_extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_info.preTransform = capabilities_khr.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = choose_swap_present_mode(model);
    swapchain_info.clipped = 1;
    swapchain_info.oldSwapchain = nullptr;

    VkResult result = vkCreateSwapchainKHR(model->logi_gpu, &swapchain_info, nullptr, &model->swapchain);
    eval_vkresult(result, "vkCreateSwapchainKHR");


    uint sci_count = 0;
    vkGetSwapchainImagesKHR(model->logi_gpu, model->swapchain, &sci_count, nullptr);
    VkImage* image = malloc(sci_count * sizeof(VkImage));
    VkResult a = vkGetSwapchainImagesKHR(model->logi_gpu, model->swapchain, &sci_count, image);
    eval_vkresult(a, "vkGetSwapchainImagesKHR");

    model->swch_images.length = sci_count;
    model->swapchain_extent = swapchain_extent;
    model->swapchain_surface_format = surface_format;
    model->swch_images.images = image;

}

VkSurfaceFormatKHR choose_swap_format(VkSurfaceFormatKHR* formats, uint fnum) {
    assert(sizeof(formats) > 0);
    for (int i = 0; i < fnum; ++i) {
        if (formats[i].format == VK_FORMAT_R8G8B8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            success("Found the preferred format and colorspace combination");
        }
    }
}

VkPresentModeKHR choose_swap_present_mode(Vulkapetl *model) {
    uint p_count = 0;

    vkGetPhysicalDeviceSurfacePresentModesKHR(model->phys_gpu, model->main_surface, &p_count, nullptr);
    VkPresentModeKHR present_modes[p_count] = {};
    VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(model->phys_gpu, model->main_surface, &p_count, present_modes);
    eval_vkresult(result, "GetPhysDevPresentModes");



    for (int i = 0; i < p_count; ++i) {
        // printf("Present mode: %i \n", present_modes[i]);
        // printf("Req Mode: %i \n", VK_PRESENT_MODE_FIFO_KHR);
        if (present_modes[i] == VK_PRESENT_MODE_FIFO_KHR) {
            return VK_PRESENT_MODE_FIFO_KHR;
        }
    }
    panic("Requested present mode unavailable");
}

VkExtent2D choose_swap_extent(Vulkapetl *model, VkSurfaceCapabilitiesKHR capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(model->main_window, &width, &height);
    printf("width: %i, height: %i \n", width, height);
    VkExtent2D a = {width, height};

    return a;

}



            panic(fail);
            break;
    }
}

void create_image_views(Vulkapetl* model) {
    VkImageViewCreateInfo image_view_info = {0};

    image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_info.format = model->swapchain_surface_format.format;

    VkImageSubresourceRange rec_range = {0};
    rec_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    rec_range.baseArrayLayer = 0;
    rec_range.baseMipLevel = 0;
    rec_range.layerCount = 1;
    rec_range.levelCount = 1;
    image_view_info.subresourceRange = rec_range;

    VkComponentMapping component_mapping = {0, 0, 0, 0};
    image_view_info.components = component_mapping;
    image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_info.pNext = nullptr;
    image_view_info.flags = 0;

    VkImageView image_views[16];

    for (int i = 0; i < model->swch_images.length; ++i) {
        image_view_info.image = model->swch_images.images[i];
        VkResult result = vkCreateImageView(model->logi_gpu, &image_view_info, nullptr, &image_views[i]);
        eval_vkresult(result, "(vkCreateImageView)");
    }

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