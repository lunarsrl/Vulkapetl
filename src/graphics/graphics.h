#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdlib.h>

#ifndef graphics
#define graphics

struct IterSwchImg {
    VkImage* images;
    uint length;
} typedef IterSwchImage;

struct vulkapetl {
    VkInstance instance;
    VkSurfaceKHR main_surface;
    GLFWwindow* main_window;
    VkQueue graphics_queue;
    VkSwapchainKHR swapchain;
    IterSwchImage swch_images;
    VkImageView swch_image_views;
    VkSurfaceFormatKHR swapchain_surface_format;
    VkExtent2D swapchain_extent;
    VkDevice logi_gpu;
    VkPhysicalDevice phys_gpu;
} typedef Vulkapetl;



void vulkan_init(Vulkapetl*);
GLFWwindow* glfw_init();
void pivk_vulkan_phys_device(Vulkapetl*);
void create_logi_device(Vulkapetl* appmodel);
void create_swapchain(Vulkapetl* model);
void create_image_views(Vulkapetl *appmodel);
void mainloop(GLFWwindow* window);
void glfw_exit(GLFWwindow* window);



#endif
