#ifndef TRIANGLE_H
#define TRIANGLE_H
#include<utility.hpp>
#include<hittable.hpp>
#include<material.hpp>
#include<BVH.hpp>
#include<array>
#include<vector>
class triangle : public hittable{
    public:
        triangle(){}
        triangle(point3 v0, point3 v1, point3 v2, shared_ptr<material> m);
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(aabb& output_box) const override;
    public:
        point3 v0;
        point3 v1;
        point3 v2;
        glm::vec3 e1;
        glm::vec3 e2;
        glm::vec3 normal;
        glm::vec2 v0t;
        glm::vec2 v1t;
        glm::vec2 v2t;
        float area;
        shared_ptr<material> mat_ptr;
    private:
        void get_uv(const point3& p, float& u, float& v)const{
            glm::vec3 v0p = p-v0;
            glm::vec3 v1p = p-v1;
            glm::vec3 v2p = p-v2;
            auto e3 = v1 - v2;
            auto in0 = glm::length(glm::cross(v0p,e1));
            auto in1 = glm::length(glm::cross(v1p,e3));
            auto in2 = glm::length(glm::cross(v2p,e2));
            u = in0*v2t.x+in1*v0p.x+in2*v1t.x;
            v = in0*v2t.y+in1*v0p.y+in2*v1t.y;
        };
};

triangle::triangle(point3 v0, point3 v1, point3 v2, shared_ptr<material> m) : v0(v0), v1(v1), v2(v2), mat_ptr(m) {
    e1 = v1 - v0;
    e2 = v2 - v0;
    normal = glm::normalize(glm::cross(e1, e2));
    area = glm::length(glm::cross(e1, e2))*0.5f;
};

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    if(glm::dot(r.direction(),normal)>0)
        return false;
    double u,v,t_tmp=0;
    glm::vec3 s1 = glm::cross(r.direction(), e2);
    auto det = glm::dot(e1, s1);
    if(fabs(det)<EPSILON)
        return false;
    
    auto det_inv = 1.0f/det;
    glm::vec3 s = r.origin() - v0;
    u = glm::dot(s1, s) * det_inv;
    if (u<0 || u>1)
        return false;
    glm::vec3 s2 = glm::cross(s, e1);
    v = glm::dot(r.direction(), s2)*det_inv;
    if (v<0||u + v >1)
        return false;
    
    t_tmp = glm::dot(e2, s2)*det_inv;
    
    if (t_tmp<0)
        return false;
    if (t_tmp < t_min || t_max < t_tmp) 
        return false;
    rec.t = t_tmp;
    rec.p = r.at(rec.t);
    rec.normal = normal;
    rec.mat_ptr = mat_ptr;
    get_uv(rec.p, rec.u, rec.v);
    return true;
};

bool triangle::bounding_box(aabb& output_box) const{
    glm::vec3 min_v,max_v; 
    min_v.x=std::min(std::min(v0.x,v1.x),v2.x);
    min_v.y=std::min(std::min(v0.y,v1.y),v2.y);
    min_v.z=std::min(std::min(v0.z,v1.z),v2.z);
    max_v.x=std::max(std::max(v0.x,v1.x),v2.x);
    max_v.y=std::max(std::max(v0.y,v1.y),v2.y);
    max_v.z=std::max(std::max(v0.z,v1.z),v2.z);
    output_box.maximum = max_v;
    output_box.minimum = min_v;
    return true;
}

class MeshTriangle : public hittable{
    public:
        MeshTriangle(){}
        MeshTriangle(const std::string& filename, shared_ptr<material> mt);
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(aabb& output_box) const override;

        aabb boundingBox;
        std::unique_ptr<glm::vec3[]> vertices;
        std::unique_ptr<glm::vec2[]> stCoordinates;
        std::unique_ptr<unit32_t[]> vertexIndex;
        unit32_t numTriangles;
        std::vector<shared_ptr<triangle>> triangles;
        shared_ptr<bvh_node> bvh;
        float area;
        shared_ptr<material> m;
};

MeshTriangle::MeshTriangle(const std::string& filename, shared_ptr<material> mt){
    objl::Loader loader;
    loader.LoadFile(filename);
    m = mt;
    assert(loader.LoadedMeshes.size()==1);
    auto mesh = loader.LoadedMeshes[0];
    glm::vec3 min = glm::vec3(std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity());
    glm::vec3 max = glm::vec3(-std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity());

    for (int i = 0; i < mesh.Vertices.size(); i = i+3)
    {
        std::array<glm::vec3, 3> face_vertices;
        for (int j = 0; j < 3; j++)
        {
            auto vert = glm::vec3(mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z);
            face_vertices[j] = vert;
            min = glm::vec3(std::min(min.x,vert.x),std::min(min.y,vert.y),std::min(min.z,vert.z));
            max = glm::vec3(std::max(max.x,vert.x),std::max(max.y,vert.y),std::max(max.z,vert.z));
        }
        triangles.emplace_back(make_shared<triangle>(face_vertices[0],face_vertices[1],face_vertices[2],mt));
    }
    boundingBox = aabb(min,max);
    std::vector<shared_ptr<hittable>> ptrs;
    for(int i=0;i<triangles.size();i++)
    {
        shared_ptr<triangle> tri;
        tri = triangles[i];
        ptrs.push_back(tri);
        area += triangles[i]->area;
    }
    shared_ptr<bvh_node> n = make_shared<bvh_node>(ptrs,0,ptrs.size());
    bvh = n;
}

bool MeshTriangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    return bvh->hit(r, t_min, t_max, rec);
}

bool MeshTriangle::bounding_box(aabb& output_box) const {
    output_box = boundingBox;
    return true;
}
#endif