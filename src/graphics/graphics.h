#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#ifndef graphics
#define graphics

struct vulkapetl {
    VkInstance instance;
} typedef Vulkapetl;

VkInstance vulkan_init();
GLFWwindow *glfw_init();
void pickVulkanPhysDevice(Vulkapetl*);
void mainloop(GLFWwindow* window);
void glfw_exit(GLFWwindow* window);

#endif
