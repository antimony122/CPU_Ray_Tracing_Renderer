#ifndef PPMWRITE_H
#define PPMWRITE_H
#include <glm/glm.hpp>
#include <iostream>
#include <utility.hpp>

void write_color(FILE* fp, color pixel_color, int samples_per_pixel) {
    // Write the translated [0,255] value of each color component.
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;

    // Divide the color by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale *r);
    g = sqrt(scale *g);
    b = sqrt(scale *b);

    // Write the translated [0,255] value of each color component.
    fprintf(fp,"%d ",static_cast<int>(255.999 * clamp(r,0.0f,1.0f)));
    fprintf(fp,"%d ",static_cast<int>(255.999 * clamp(g,0.0f,1.0f)));
    fprintf(fp,"%d\n",static_cast<int>(255.999 * clamp(b,0.0f,1.0f)));
}

#endif