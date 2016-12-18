//
//  object.h
//  Mashgraph_Task4_RayTracing
//
//  Created by Игорь Анфёров on 10.12.2016.
//  Copyright © 2016 Игорь Анфёров. All rights reserved.
//

#ifndef object_h
#define object_h

#define MIN2(x,y) ((x)<(y)?(x):(y))
#define MAX2(x,y) ((x)>(y)?(x):(y))
#define MIN3(x,y,z) (MIN2(x,y)<(z)?MIN2(x,y):(z))
#define MAX3(x,y,z) (MAX2(x,y)>(z)?MAX2(x,y):(z))
#define MIN4(x,y,z,w) (MIN3(x,y,z)<(w)?MIN3(x,y,z):(w))
#define MAX4(x,y,z,w) (MAX3(x,y,z)>(w)?MAX3(x,y,z):(w))

#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cmath>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Triangle_3.h>
#include <CGAL/Point_3.h>
#include <CGAL/Ray_3.h>
#include <CGAL/Direction_3.h>
#include <CGAL/intersections.h>
#include <CGAL/Cartesian.h>

#include "SOIL/SOIL.h"

using namespace CGAL;
using namespace std;
using namespace glm;

class object;

struct intersect_info {
    bool res;
    dvec3 point;
    object *obj;
    
    intersect_info();
    intersect_info(bool r, dvec3 p, void *o);
};

double brightness(dvec3 cur);

bool cmp(pair<intersect_info, double> a, pair<intersect_info, double> b);

class object {
public:
    string name;
    dvec3 position = dvec3(0,0,0);
    dvec3 x_dir = dvec3(1,0,0);
    dvec3 y_dir = dvec3(0,1,0);
    dvec3 color = dvec3(0,1,0);
    double x_size = 1;
    double y_size = 1;
    double diffuse = 0.4;
    double reflection = 0;
    double refraction = 0;
    vector<object *> parts;
    object * main_scene;
    
    object(string s);
    ~object();
    virtual void translate(dvec3 trans);
    virtual void rotate(dvec3 rot_axis, float rot_angle);
    virtual intersect_info is_intersected_by(dvec3 beam_pos, dvec3 beam_dir);
    virtual void trace(dvec3 color, dvec3 beam_pos, dvec3 beam_dir, intersect_info);
    virtual dvec3 backtrace(dvec3 beam_pos, dvec3 beam_dir, intersect_info);
    virtual void emit();
    void set_main_scene(object *);
};

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
#pragma omp critical RECT_TEXTURE
        texture[int((text_x-1)*x)][int((text_y-1)*y)] += self_col;
        
        dvec3 diff_col = color * ray_color * diffuse;
        
        if (brightness(diff_col) > 0.01) {
            static default_random_engine generator;
            normal_distribution<float> angle (0, 120);
            uniform_real_distribution<float> plane (0, 180);
            dvec3 ray_pos = int_p;
            dvec3 ray_dir = cross(x_dir, y_dir);
            float cur_angle;
            do {
                cur_angle = angle(generator);
            } while (cur_angle > 90 || cur_angle < -90);
            dmat4 rot_matrix = dmat4(glm::rotate(mat4(), cur_angle, vec3(x_dir)));
            dmat4 rot_matrix2 = dmat4(glm::rotate(mat4(), plane(generator), vec3(ray_dir)));
            ray_dir = dvec3(rot_matrix * dvec4(ray_dir, 1));
            ray_dir = dvec3(rot_matrix2 * dvec4(ray_dir, 1));
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
#pragma omp critical SPHERE_TEXTURE
        texture[int((text-1)*theta)][int((text-1)*phi)]+= self_col;
        
        dvec3 diff_col = color * ray_color * diffuse;
        
        if (brightness(diff_col) > 0.01) {
            static default_random_engine generator;
            normal_distribution<float> angle (0, 120);
            uniform_real_distribution<float> plane (0, 180);
            dvec3 ray_pos = int_p;
            dvec3 ray_dir = normalize(ray_pos - position);
            float cur_angle;
            do {
                cur_angle = angle(generator);
            } while (cur_angle > 90 || cur_angle < -90);
            dmat4 rot_matrix = dmat4(glm::rotate(mat4(), cur_angle, vec3(x_dir)));
            dmat4 rot_matrix2 = dmat4(glm::rotate(mat4(), plane(generator), vec3(ray_dir)));
            ray_dir = dvec3(rot_matrix * dvec4(ray_dir, 1));
            ray_dir = dvec3(rot_matrix2 * dvec4(ray_dir, 1));
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

class cornellBox: object {
public:
    cornellBox();
    ~cornellBox();
};
                
class parallelepiped: object {
public:
    parallelepiped();
    ~parallelepiped();
};

template <int text_x, int text_y>
class LED: object {
public:
    LED(string name):object(name) {
        x_size = text_x / 1000.0;
        y_size = text_y / 1000.0;

        object *square;
        
        square = (object *) new rectangle<text_x, text_y> ("LED_White_Sqr");
        square->color = dvec3(1,1,1);
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
        normal_distribution<float> angle (0, 120);
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
        dmat4 rot_matrix2 = dmat4(glm::rotate(mat4(), plane(generator), vec3(ray_dir)));
        ray_dir = dvec3(rot_matrix * dvec4(ray_dir, 1));
        ray_dir = dvec3(rot_matrix2 * dvec4(ray_dir, 1));
        ray_pos += ray_dir*0.00001;
        intersect_info tmp = main_scene->is_intersected_by(ray_pos, ray_dir);
        if (tmp.res) {
            tmp.obj->trace(color, ray_pos, ray_dir, tmp);
        }
    };
};

class scene: object {
public:
    void light(long primary_rays_count);
    scene();
    ~scene();
};

class tex {
    dvec3 *data = NULL;
    int x = 0;
    int y = 0;
public:
    ~tex();
    void init(int xx, int yy);
    dvec3* operator[] (int i);
    double get_average();
};

class camera {
public:
    dvec3 position;
    dvec3 x_dir;
    dvec3 y_dir;
    dvec3 z_dir;
    double focal_length;
    int x_res;
    int y_res;
    int ssaa;
    object * scene;
    tex texture;
    
    camera(dvec3, dvec3, dvec3, double, int, int, int, object *);
    void draw();
    void save_to_file(string file);
};

#endif /* object_h */
