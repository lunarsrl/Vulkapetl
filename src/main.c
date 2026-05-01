#include <stdio.h>
#include "graphics/graphics.h"
#include  "graphics/GraphicsPipeline.h"
#include "include/dynarr/dynString/dynstring.h"
#include "include/logger/log.h"


int main() {
    // Top Level Struct
    Vulkapetl appmodel;

    // Initialization
    appmodel.main_window = glfw_init();
    vulkan_init(&appmodel);
    pivk_vulkan_phys_device(&appmodel);
    create_logi_device(&appmodel);
    create_swapchain(&appmodel);
    create_image_views(&appmodel);

    create_graphics_pipeline(&appmodel);

    mainloop(appmodel.main_window);
    glfw_exit(appmodel.main_window);
    printf("Successful exit");
}

