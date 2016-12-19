#pragma once
#include "rectangle.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

using namespace std;

template <int text_x, int text_y>
class LED: object {
public:
    LED(string name):object(name) {
        x_size = text_x / 1000.0;
        y_size = text_y / 1000.0;
        
        object *square;
        
        square = (object *) new rectangle<text_x, text_y> ("LED_White_Sqr");
        square->color = dvec3(1,1,1);
        square->diffuse = 0.9;
        parts.push_back(square);
        for (int i=0; i<text_x; i++) {
            for (int j=0; j<text_y; j++) {
                ((rectangle<text_x, text_y> *)square)->texture[i][j] = square->color;
            }
        }
    };
    ~LED();
    void emit() {
        static default_random_engine generator;
        normal_distribution<float> angle (0, 30);
        uniform_real_distribution<double> x_pos (-x_size/2, x_size/2);
        uniform_real_distribution<double> y_pos (-y_size/2, y_size/2);
        uniform_real_distribution<float> plane (0, 180);
        dvec3 ray_pos = position + y_dir * y_pos(generator) + x_dir * x_pos(generator);
        dvec3 ray_dir = cross(x_dir, y_dir);
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
            tmp.obj->trace(color, ray_pos, ray_dir, tmp);
        }
    };
    
    intersect_info is_intersected_by(dvec3 beam_pos, dvec3 beam_dir) {
        intersect_info tmp = parts[0]->is_intersected_by(beam_pos, beam_dir);
        if (!tmp.res)
            return intersect_info();
        tmp.obj = this;
        return tmp;
    }
    
    void trace(dvec3 colour, dvec3 beam_pos, dvec3 beam_dir, intersect_info i) {
        colour = dvec3(color);
        parts[0]->trace(colour, beam_pos, beam_dir, i);
    }
    
    dvec3 backtrace(dvec3 beam_pos, dvec3 beam_dir, intersect_info i) {
        return parts[0]->backtrace(beam_pos, beam_dir, i);
    }
};
