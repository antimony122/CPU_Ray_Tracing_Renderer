#include<fstream>
#include<iostream>
#include "utility.hpp"
#include<glm/gtc/random.hpp>
#include<hittable_list.hpp>
#include<sphere.hpp>
#include<triangle.hpp>
#include<glm/glm.hpp>
#include<ppmWrite.hpp>
#include<camera.hpp>
#include<material.hpp>

color ray_color(const ray& r, const color& background ,const hittable& world, int depth) { //hittable to hittable_list
    if(depth<=0)
        return color(0,0,0);
    hit_record rec;
    if(!world.hit(r,EPSILON,infinity,rec))
        return background;
    ray scattered;
    color attenuation;
    color emit = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if(!rec.mat_ptr->scatter(r,rec, attenuation, scattered))
        return emit;
    return emit +attenuation*ray_color(scattered, background, world, depth-1);
}

int main() {

    // Image

    const auto aspect_ratio = 1;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 32;
    const int max_depth=10;
    //World
    hittable_list world;
    color background(0,0,0);
    auto material_ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto material_center = make_shared<dielectric>(1.5);
    auto material_left   = make_shared<lambertian>(color(0.5,0.0,0.0));
    auto material_right  = make_shared<lambertian>(color(0.0,0.0,0.5));
    auto light = make_shared<diffuse_light>(color(8.0, 8.0, 8.0));

    world.add(make_shared<sphere>(point3( 150.0, 200.0, 100.0), 100, material_center));
    world.add(make_shared<MeshTriangle>(".\\models\\cornellbox\\floor.obj", material_ground));
    world.add(make_shared<MeshTriangle>(".\\models\\cornellbox\\left.obj", material_left));
    world.add(make_shared<MeshTriangle>(".\\models\\cornellbox\\right.obj", material_right));
    world.add(make_shared<MeshTriangle>(".\\models\\cornellbox\\light.obj", light));
    world.add(make_shared<MeshTriangle>(".\\models\\cornellbox\\tallbox.obj", material_ground));
    // Camera

    camera cam;

    // Render
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P3\n%d %d\n255\n", image_width, image_height);

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for(int s=0;s< samples_per_pixel;s++){
                auto u = float(i+glm::linearRand(0.0f,1.0f)) / (image_width-1);
                auto v = float(j+glm::linearRand(0.0f,1.0f)) / (image_height-1);
                ray r = cam.get_ray(u,v);
                pixel_color += ray_color(r, background, world, max_depth);
            }   
            write_color(fp,pixel_color,samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
    fclose(fp);
}