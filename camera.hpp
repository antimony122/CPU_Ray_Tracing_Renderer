#ifndef CAMERA_H
#define CAMERA_H

#include <utility.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
class camera {
    public:
        camera(const std::string &filename);
        void set_camera(point3 ori,point3 lookat, glm::vec3 upDir,float fov, float aspect_ratio) {
            float theta = degrees_to_radians(fov);
            float h = tan(theta/2);
            auto viewport_height =2.0f*h;
            auto viewport_width = aspect_ratio * viewport_height;
            
            auto w = glm::normalize(-lookat);
            auto u = glm::normalize(glm::cross(upDir, w));
            auto v = glm::normalize(glm::cross(w, u));
            u = -u;
            origin = ori;
            horizontal = viewport_width * u;
            vertical = viewport_height * v;
            lower_left_corner = origin - horizontal*0.5f - vertical*0.5f -w;
        }

        ray get_ray(float u, float v) const {
            return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
        }

    private:
        point3 origin;
        point3 lower_left_corner;
        glm::vec3 horizontal;
        glm::vec3 vertical;
};

//load camera setting parameters from file
camera::camera(const std::string &filename){
    point3 o;
    point3 d;
    point3 u;
    float f;
    float a;
    std::fstream fin;
    fin.open(filename.c_str(), std::ios::in);
    while(fin.good())
    {
        std::string eachline;
        getline(fin, eachline);
        if(eachline[0] == '#')
            continue;
        std::istringstream stream(eachline);
        std::string header;
        stream>>header;
        if(header=="POSITION")
        {   
            std::string temp;
            stream>>temp;
            o.x=float(std::stof(temp));
            stream>>temp;
            o.y=float(std::stof(temp));
            stream>>temp;
            o.z=float(std::stof(temp));
        }
        if(header=="VIEW_DIRECTION")
        {   
            std::string temp;
            stream>>temp;
            d.x=float(std::stof(temp));
            stream>>temp;
            d.y=float(std::stof(temp));
            stream>>temp;
            d.z=float(std::stof(temp));
        }
        if(header=="UP_DIRECTION")
        {   
            std::string temp;
            stream>>temp;
            u.x=float(std::stof(temp));
            stream>>temp;
            u.y=float(std::stof(temp));
            stream>>temp;
            u.z=float(std::stof(temp));
        }
        if(header=="FOV")
        {   
            std::string temp;
            stream>>temp;
            f=float(std::stof(temp));
        }
        if(header=="ASPECT_RATIO")
        {   
            std::string temp;
            stream>>temp;
            a=float(std::stof(temp));
        }
    }
    set_camera(o,d,u,f,a);
}
#endif