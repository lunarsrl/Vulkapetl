#include <stdio.h>
#include "graphics/graphics.h"
#include "include/dynarr/dynString/dynstring.h"


int main() {
    // Top Level Struct
    Vulkapetl appmodel;

    // Initialization
    appmodel.instance = vulkan_init();
    pickVulkanPhysDevice(&appmodel);
    GLFWwindow* main_window = glfw_init();
    mainloop(main_window);
    glfw_exit(main_window);
}

