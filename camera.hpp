#ifndef CAMERA_H
#define CAMERA_H

#include <utility.hpp>

class camera {
    public:
        camera() {
            auto aspect_ratio = 1;
            auto viewport_height = 960;
            auto viewport_width = aspect_ratio * viewport_height;
            auto focal_length = 600.0;

            origin = point3(278, 273, -600);
            horizontal = glm::vec3(viewport_width, 0.0, 0.0);
            vertical = glm::vec3(0.0, viewport_height, 0.0);
            lower_left_corner = origin - horizontal*0.5f - vertical*0.5f + glm::vec3(0, 0, focal_length);
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
#endif