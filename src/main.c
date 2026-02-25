#include <stdio.h>

#include "graphics/graphics.h"

int main() {
    // Initialization
    vulkan_init();
    GLFWwindow* main_window = glfw_init();
    mainloop(main_window);
    glfw_exit(main_window);
}

