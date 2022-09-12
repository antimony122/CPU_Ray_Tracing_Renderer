#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.hpp"
#include <aabb.hpp>
#include <BVH.hpp>
#include <memory>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <material.hpp>
#include <sphere.hpp>
#include <triangle.hpp>
using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
    public:
        hittable_list() {}
        hittable_list(shared_ptr<hittable> object); 
        hittable_list(const std::string &filename);
        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) { objects.push_back(object); }
        void build_bvh();
        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(aabb& output_box) const override;

    public:
        std::vector<shared_ptr<hittable>> objects;
        shared_ptr<bvh_node> bvh;
};

hittable_list::hittable_list(shared_ptr<hittable> object) { 
    add(object);
}

hittable_list::hittable_list(const std::string &filename){
    std::fstream fin;
    fin.open(filename.c_str(), std::ios::in);
    while(fin.good())
    {
        shared_ptr<material> m;
        std::string eachline;
        getline(fin, eachline);
        if(eachline[0] == '#')
            continue;
        std::istringstream stream(eachline);
        std::string header;
        stream>>header;
        if(header=="LAMBERTIAN")
        {
            color m_color;
            std::string temp;
            stream>>temp;
            m_color.x=float(std::stof(temp));
            stream>>temp;
            m_color.y=float(std::stof(temp));
            stream>>temp;
            m_color.z=float(std::stof(temp));
            m = make_shared<lambertian>(m_color);
        }
        if(header=="METAL")
        {
            float fuzz;
            color m_color;
            std::string temp;
            stream>>temp;
            m_color.x=float(std::stof(temp));
            stream>>temp;
            m_color.y=float(std::stof(temp));
            stream>>temp;
            m_color.z=float(std::stof(temp));
            stream>>temp;
            fuzz=float(std::stof(temp));
            m = make_shared<metal>(m_color, fuzz);
        }
        if(header=="DIFFUSELIGHT")
        {
            color m_color;
            std::string temp;
            stream>>temp;
            m_color.x=float(std::stof(temp));
            stream>>temp;
            m_color.y=float(std::stof(temp));
            stream>>temp;
            m_color.z=float(std::stof(temp));
            m = make_shared<diffuse_light>(m_color);
        }
        if(header=="DIELECTRIC")
        {
            float ir;
            std::string temp;
            stream>>temp;
            ir=float(std::stof(temp));
            m = make_shared<dielectric>(ir);
        }
        stream >> header;
        if(header=="MESHTRIANGLE")
        {
            std::string path;
            stream >> path;
            add(make_shared<MeshTriangle>(path, m));
        }
        if(header=="SPHERE")
        {
            point3 center;
            double radius;
            std::string temp;
            stream>>temp;
            center.x=float(std::stof(temp));
            stream>>temp;
            center.y=float(std::stof(temp));
            stream>>temp;
            center.z=float(std::stof(temp));
            stream>>temp;
            radius=std::stof(temp);
            add(make_shared<sphere>(center,radius,m));
        }
    }
}

void hittable_list::build_bvh(){
    size_t end = objects.size(); 
    bvh = make_shared<bvh_node>(objects,0,end);
}

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return bvh->hit(r, t_min, t_max, rec);
}

bool hittable_list::bounding_box(aabb& output_box) const {
    if (objects.empty()) return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(temp_box)) return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

#endif