#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main() {
    int fb_fd = open("/dev/fb0", O_RDWR);  // Open framebuffer device
    if (fb_fd == -1) {
        perror("Error opening framebuffer device");
        return 1;
    }

    struct fb_var_screeninfo vinfo;
    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo)) {  // Get screen info
        perror("Error reading variable information");
        close(fb_fd);
        return 1;
    }

    long screensize = vinfo.yres_virtual * vinfo.xres_virtual * vinfo.bits_per_pixel / 8;  // Calculate screensize in bytes

    // Map framebuffer to memory
    char *fb_ptr = (char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if ((intptr_t)fb_ptr == -1) {
        perror("Error mapping framebuffer device to memory");
        close(fb_fd);
        return 1;
    }

    // Clear the screen (fill it with black)
    memset(fb_ptr, 0, screensize);

    // Set a simple pattern (like white pixel in top left corner)
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            long location = (i + vinfo.yoffset) * vinfo.xres_virtual * vinfo.bits_per_pixel / 8
                            + (j + vinfo.xoffset) * vinfo.bits_per_pixel / 8;

            // Set pixel (white color for RGB 888)
            *((unsigned int *)(fb_ptr + location)) = 0xFFFFFF;
        }
    }

    // Unmap framebuffer memory
    munmap(fb_ptr, screensize);
    close(fb_fd);

    return 0;
}
