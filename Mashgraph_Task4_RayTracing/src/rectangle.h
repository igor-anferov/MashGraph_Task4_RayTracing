#pragma once
#include "object.h"

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Triangle_3.h>
#include <CGAL/Point_3.h>
#include <CGAL/Ray_3.h>
#include <CGAL/Direction_3.h>
#include <CGAL/intersections.h>
#include <CGAL/Cartesian.h>

using namespace CGAL;

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

using namespace std;

template <int text_x, int text_y>
class rectangle: object {
public:
    dvec3 texture[text_x][text_y];
    
    rectangle(string name):object(name) {
        x_size = text_x / 1000.0;
        y_size = text_y / 1000.0;
    }
    
    intersect_info is_intersected_by(dvec3 beam_pos, dvec3 beam_dir) override {
        
        dvec3 p1 = position + 0.5 * (  y_dir * y_size + x_dir * x_size );
        dvec3 p2 = position + 0.5 * ( -y_dir * y_size - x_dir * x_size );
        dvec3 p3 = position + 0.5 * (  y_dir * y_size - x_dir * x_size );
        dvec3 p4 = position + 0.5 * ( -y_dir * y_size + x_dir * x_size );
        
        Triangle_3<Cartesian<double>> tr1(Point_3<Cartesian<double>>(p1.x, p1.y, p1.z),
                                          Point_3<Cartesian<double>>(p4.x, p4.y, p4.z),
                                          Point_3<Cartesian<double>>(p2.x, p2.y, p2.z));
        Triangle_3<Cartesian<double>> tr2(Point_3<Cartesian<double>>(p1.x, p1.y, p1.z),
                                          Point_3<Cartesian<double>>(p2.x, p2.y, p2.z),
                                          Point_3<Cartesian<double>>(p3.x, p3.y, p3.z));
        Ray_3<Cartesian<double>>      ray(Point_3<Cartesian<double>>(beam_pos.x, beam_pos.y, beam_pos.z),
                                          Direction_3<Cartesian<double>>(beam_dir.x, beam_dir.y, beam_dir.z));
        
        if (do_intersect(tr1, ray)) {
            auto int_opt_point = intersection(tr1, ray);
            Point_3<Cartesian<double>> *int_point = boost::get<Point_3<Cartesian<double>>>(&*int_opt_point);
            if (!int_point)
                return intersect_info();
                dvec3 p(int_point->x(), int_point->y(), int_point->z());
                return intersect_info(true, p, this);
                } else if (do_intersect(tr2, ray)) {
                    auto int_opt_point = intersection(tr2, ray);
                    Point_3<Cartesian<double>> *int_point = boost::get<Point_3<Cartesian<double>>>(&*int_opt_point);
                    if (!int_point)
                        return intersect_info();
                        dvec3 p(int_point->x(), int_point->y(), int_point->z());
                        return intersect_info(true, p, this);
                        } else
                            return intersect_info();
                            }
    
    void trace(dvec3 ray_color, dvec3 beam_pos, dvec3 beam_dir, intersect_info int_inf) override {
        dvec3 int_p = int_inf.point;
        dvec3 relative_coodrs(int_p  - (position - 0.5 * ( y_dir * y_size + x_dir * x_size )) );
        Plane_3<Cartesian<double>> y_plane(Point_3<Cartesian<double>>(relative_coodrs.x, relative_coodrs.y, relative_coodrs.z),
                                           Direction_3<Cartesian<double>>(y_dir.x, y_dir.y, y_dir.z));
        Line_3<Cartesian<double>>   y_line(Point_3<Cartesian<double>>(0,0,0), Point_3<Cartesian<double>>(y_dir.x, y_dir.y, y_dir.z));
        Plane_3<Cartesian<double>> x_plane(Point_3<Cartesian<double>>(relative_coodrs.x, relative_coodrs.y, relative_coodrs.z),
                                           Direction_3<Cartesian<double>>(x_dir.x, x_dir.y, x_dir.z));
        Line_3<Cartesian<double>>   x_line(Point_3<Cartesian<double>>(0,0,0), Point_3<Cartesian<double>>(x_dir.x, x_dir.y, x_dir.z));
        
        if (!do_intersect(y_plane, y_line) || !do_intersect(x_plane, x_line))
            throw "trace() error 1!";
        
        auto int_opt_point = intersection(y_plane, y_line);
        Point_3<Cartesian<double>> *int_point = boost::get<Point_3<Cartesian<double>>>(&*int_opt_point);
        if (!int_point)
            throw "trace() error 2!";
        double y = length(dvec3(int_point->x(), int_point->y(), int_point->z()));
        
        auto int_opt_point_x = intersection(x_plane, x_line);
        Point_3<Cartesian<double>> *int_point_x = boost::get<Point_3<Cartesian<double>>>(&*int_opt_point_x);
        if (!int_point_x)
            throw "trace() error 3!";
        double x = length(dvec3(int_point_x->x(), int_point_x->y(), int_point_x->z()));
        
        if (x>x_size || y>y_size)
            throw "trace() error 4!";
        
        dvec3 self_col = color * ray_color * (1 - diffuse - reflection - refraction);
#pragma omp critical
        texture[int((text_x-1)*x)][int((text_y-1)*y)] += self_col;
        
        dvec3 diff_col = color * ray_color * diffuse;
        
        if (brightness(diff_col) > 0.01) {
            static default_random_engine generator;
            normal_distribution<float> angle (0, 30);
            uniform_real_distribution<float> plane (0, 180);
            dvec3 ray_pos = int_p;
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
                tmp.obj->trace(diff_col, ray_pos, ray_dir, tmp);
            }
        }
    }
    
    dvec3 backtrace(dvec3 beam_pos, dvec3 beam_dir, intersect_info int_inf) override {
        dvec3 int_p = int_inf.point;
        dvec3 relative_coodrs(int_p  - (position - 0.5 * ( y_dir * y_size + x_dir * x_size )) );
        Plane_3<Cartesian<double>> y_plane(Point_3<Cartesian<double>>(relative_coodrs.x, relative_coodrs.y, relative_coodrs.z),
                                           Direction_3<Cartesian<double>>(y_dir.x, y_dir.y, y_dir.z));
        Line_3<Cartesian<double>>   y_line(Point_3<Cartesian<double>>(0,0,0), Point_3<Cartesian<double>>(y_dir.x, y_dir.y, y_dir.z));
        Plane_3<Cartesian<double>> x_plane(Point_3<Cartesian<double>>(relative_coodrs.x, relative_coodrs.y, relative_coodrs.z),
                                           Direction_3<Cartesian<double>>(x_dir.x, x_dir.y, x_dir.z));
        Line_3<Cartesian<double>>   x_line(Point_3<Cartesian<double>>(0,0,0), Point_3<Cartesian<double>>(x_dir.x, x_dir.y, x_dir.z));
        
        if (!do_intersect(y_plane, y_line) || !do_intersect(x_plane, x_line))
            throw "backtrace() error 1!";
        
        auto int_opt_point = intersection(y_plane, y_line);
        Point_3<Cartesian<double>> *int_point = boost::get<Point_3<Cartesian<double>>>(&*int_opt_point);
        if (!int_point)
            throw "backtrace() error 2!";
        double y = length(dvec3(int_point->x(), int_point->y(), int_point->z()));
        
        auto int_opt_point_x = intersection(x_plane, x_line);
        Point_3<Cartesian<double>> *int_point_x = boost::get<Point_3<Cartesian<double>>>(&*int_opt_point_x);
        if (!int_point_x)
            throw "backtrace() error 3!";
        double x = length(dvec3(int_point_x->x(), int_point_x->y(), int_point_x->z()));
        
        if (x>x_size || y>y_size)
            throw "backtrace() error 4!";
        
        return texture[int((text_x-1)*x)][int((text_y-1)*y)];
    }
};
