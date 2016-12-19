#pragma once
#include "object.h"

#include "glm/glm.hpp"

using namespace glm;

using namespace std;

template <int text>
class sphere: object {
public:
    dvec3 texture[text][text];
    
    sphere(string name):object(name) {
        x_size = text / 1000.0;
        y_size = text / 1000.0;
    }
    
    intersect_info is_intersected_by(dvec3 beam_pos, dvec3 beam_dir) override {
        dvec3 k = beam_pos - position;
        double b = dot(k,beam_dir);
        double c = dot(k,k) - (x_size/2)*(x_size/2);
        double d = b*b - c;
        
        if(d >= 0) {
            double sqrtfd = sqrtf(d);
            double t1 = -b + sqrtfd;
            double t2 = -b - sqrtfd;
            
            double min_t  = MIN2(t1,t2);
            double max_t = MAX2(t1,t2);
            
            double t = (min_t >= 0) ? min_t : max_t;
            if (t<=0)
                return intersect_info();
                return intersect_info(true, dvec3(beam_pos + t * beam_dir), this);
                }
        return intersect_info();
    }
    
    void trace(dvec3 ray_color, dvec3 beam_pos, dvec3 beam_dir, intersect_info int_inf) override {
        dvec3 int_p = int_inf.point;
        dvec3 relative_coodrs(int_p  - position);
        relative_coodrs = normalize(relative_coodrs);
        double x = relative_coodrs.x;
        double y = relative_coodrs.y;
        double z = relative_coodrs.z;
        
        double theta = acos(z);
        double phi = (atan(y/x));
        theta += M_PI/2;
        phi += M_PI/2;
        if (x<0) {
            phi += M_PI;
        }
        theta /= M_PI;
        phi /= 2*M_PI;
        if (theta>1) {
            theta = 1;
        }
        if (phi>1) {
            phi = 1;
        }
        
        dvec3 self_col = color * ray_color * (1 - diffuse - reflection - refraction);
#pragma omp critical
        texture[int((text-1)*theta)][int((text-1)*phi)]+= self_col;
        
        dvec3 diff_col = color * ray_color * diffuse;
        
        if (brightness(diff_col) > 0.01) {
            static default_random_engine generator;
            normal_distribution<float> angle (0, 30);
            uniform_real_distribution<float> plane (0, 180);
            dvec3 ray_pos = int_p;
            dvec3 ray_dir = normalize(ray_pos - position);
            float cur_angle;
            do {
                cur_angle = angle(generator);
            } while (cur_angle > 90 || cur_angle < -90);
            dmat4 rot_matrix = dmat4(glm::rotate(mat4(), cur_angle, vec3(x_dir)));
            do {
                cur_angle = angle(generator);
            } while (cur_angle > 90 || cur_angle < -90);
            dmat4 rot_matrix2 = dmat4(glm::rotate(mat4(), cur_angle, vec3(y_dir)));
            dmat4 rot_matrix3 = dmat4(glm::rotate(mat4(), plane(generator), vec3(ray_dir)));
            ray_dir = dvec3(rot_matrix * dvec4(ray_dir, 1));
            ray_dir = dvec3(rot_matrix2 * dvec4(ray_dir, 1));
            ray_dir = dvec3(rot_matrix3 * dvec4(ray_dir, 1));
            ray_pos += ray_dir*0.00001;
            intersect_info tmp = main_scene->is_intersected_by(ray_pos, ray_dir);
            if (tmp.res) {
                tmp.obj->trace(diff_col, ray_pos, ray_dir, tmp);
            }
        }
    }
    
    dvec3 backtrace(dvec3 beam_pos, dvec3 beam_dir, intersect_info int_inf) override {
        dvec3 int_p = int_inf.point;
        dvec3 relative_coodrs(int_p  - position);
        relative_coodrs = normalize(relative_coodrs);
        double x = relative_coodrs.x;
        double y = relative_coodrs.y;
        double z = relative_coodrs.z;
        
        double theta = acos(z);
        double phi = (atan(y/x));
        theta += M_PI/2;
        phi += M_PI/2;
        if (x<0) {
            phi += M_PI;
        }
        theta /= M_PI;
        phi /= 2*M_PI;
        if (theta>1) {
            theta = 1;
        }
        if (phi>1) {
            phi = 1;
        }
        return texture[int((text-1)*theta)][int((text-1)*phi)];
    }
};
