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
#include<vector>
#include<thread>

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

    // Global parameter settings
    int image_width = 800;
    int image_height = 800;
    int samples_per_pixel = 64;
    int max_depth = 15;
    int THREAD_NUM = 4;
    std::fstream fin;
    fin.open("global_settings.txt", std::ios::in);
    while(fin.good())
    {
        std::string eachline;
        getline(fin, eachline);
        if(eachline[0] == '#')
            continue;
        std::istringstream stream(eachline);
        std::string header;
        stream>>header;
        if(header=="IMAGE_WIDTH")
        {   
            std::string temp;
            stream>>temp;
            image_width=float(std::stof(temp));
        }
        if(header=="IMAGE_HEIGHT")
        {   
            std::string temp;
            stream>>temp;
            image_height=float(std::stof(temp));
        }
        if(header=="SAMPLE_PER_PIXEL")
        {   
            std::string temp;
            stream>>temp;
            samples_per_pixel=float(std::stof(temp));
        }
        if(header=="THREADS_NUM")
        {   
            std::string temp;
            stream>>temp;
            THREAD_NUM=float(std::stof(temp));
        }
        if(header=="MAX_DEPTH")
        {   
            std::string temp;
            stream>>temp;
            max_depth=float(std::stof(temp));
        }
    }

    //World
    hittable_list world("scene.txt");
    color background(0,0,0);
    std::cerr<<"\nBuilding BVH Tree\n";
    world.build_bvh();
    std::cerr<<"BVH Tree established\n";
    // Camera
    std::string camera_set = "camera_settings.txt";
    camera cam(camera_set);
    // Render
    FILE* fp = fopen("binary.ppm", "wb");

    std::vector<color> frame_buffer(image_height*image_width, color(0,0,0));

    (void)fprintf(fp, "P3\n%d %d\n255\n", image_width, image_height);
    std::vector<std::thread> threads;
    int remain_line = image_height;
    auto renderLoop=[&](int th_id)
    {
        int range = image_height/THREAD_NUM;
        int start_line = image_height-th_id*range-1;
        if(th_id == THREAD_NUM-1)
            range += image_height%THREAD_NUM;
        int bias = range*th_id*image_width;
        
        for (int j = start_line; j >start_line-range; j--)
        {
            std::cerr << "\rScanlines remaining: " << remain_line << ' ' << std::flush;
            //printf("thread%d remain %d\n",th_id,j);
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0,0,0);
                for(int s=0;s < samples_per_pixel;s++){
                    auto u = float(i+glm::linearRand(0.0f,1.0f)) / (image_width-1);
                    auto v = float(j+glm::linearRand(0.0f,1.0f)) / (image_height-1);
                    ray r = cam.get_ray(u,v);
                    frame_buffer[image_width*(image_height-j-1)+i] += ray_color(r, background, world, max_depth);
                }
            }
            remain_line--;
        }
    };
    
    for(int i=0;i<THREAD_NUM;i++)
    {
        threads.emplace_back(std::thread(renderLoop,i));
    }

    for(std::thread& t :threads)
    {
        t.join();
    }
    std::cerr << "\nwriting out put";
    write_color(fp,frame_buffer,samples_per_pixel);
    std::cerr << "\nDone.\n";
    fclose(fp);
}