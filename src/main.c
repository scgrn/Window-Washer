//  only define decoders that we have encoders for
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include <ctype.h>
#include <string.h>

const int BORDER_WIDTH = 1;
const int TITLE_BAR_HEIGHT = 25;

const char *getFilenameExt(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return "";
    }
    return dot + 1;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Strips the window border and title bar from a screenshot image in-place.\n\n");
        printf("Usage: Window-Washer.exe filename\n");
    } else {
        char* filename = argv[1];

        int inputWidth, inputHeight, bpp;
        unsigned char *inputData = stbi_load(filename, &inputWidth, &inputHeight, &bpp, 0);
        if (inputData != 0) {
            printf("Input: (%d, %d) %d bytes per pixel\n", inputWidth, inputHeight, bpp);
            
            int outputWidth = inputWidth - (BORDER_WIDTH * 2);
            int outputHeight = inputHeight - (BORDER_WIDTH * 2) - TITLE_BAR_HEIGHT;

            printf("Output: (%d, %d)\n", outputWidth, outputHeight);

            int xOfs = BORDER_WIDTH;
            int yOfs = (BORDER_WIDTH + TITLE_BAR_HEIGHT);
            
            //  build the cropped output image
            //  TODO: use memcpy for better performance
            unsigned char* outputData = malloc(outputWidth * outputHeight * bpp);
            for (int y = 0; y < outputHeight; y++) {
                for (int x = 0; x < outputWidth; x++) {
                    for (int b = 0; b < bpp; b++) {
                        outputData[(y * outputWidth + x) * bpp + b] =
                        inputData[((y + yOfs) * inputWidth + x + xOfs) * bpp + b];
                    }
                }
            }
            
            //  grab the input file's extenstion
            const char* ext = getFilenameExt(filename);
            char* extLower = malloc(strlen(ext) + 1);
            strncpy(extLower, ext, strlen(ext));
            for(int i = 0; ext[i]; i++) {
                extLower[i] = tolower(ext[i]);
            }
            extLower[strlen(ext)] = '\0';

            //  write the cropped image back out
            if (strcmp(extLower, "jpg") == 0 || strcmp(extLower, "jpeg") == 0) {
                stbi_write_jpg(filename, outputWidth, outputHeight, bpp, outputData, 90);
            }
            if (strcmp(extLower, "png") == 0) {
                stbi_write_png(filename, outputWidth, outputHeight, bpp, outputData, outputWidth * bpp);
            }
            if (strcmp(extLower, "bmp") == 0) {
                stbi_write_bmp(filename, outputWidth, outputHeight, bpp, outputData);
            }
            if (strcmp(extLower, "tga") == 0) {
                stbi_write_tga(filename, outputWidth, outputHeight, bpp, outputData);
            }

            //  clean up
            stbi_image_free(inputData);
            free(extLower);
            free(outputData);
        } else {
            printf("Error loading image: %s\n", stbi_failure_reason());
        }
    }

    return 0;
}
