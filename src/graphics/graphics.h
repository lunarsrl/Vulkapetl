#include <GLFW/glfw3.h>
#ifndef graphics
#define graphics

void vulkan_init();

GLFWwindow *glfw_init();
void mainloop(GLFWwindow* window);
void glfw_exit(GLFWwindow* window);

#endif
