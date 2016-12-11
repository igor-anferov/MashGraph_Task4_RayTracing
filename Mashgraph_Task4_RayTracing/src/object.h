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

#include <SOIL/SOIL.h>

using namespace CGAL;
using namespace std;
using namespace glm;

class object;

bool cmp(pair<object *, int> a, pair<object *, int> b);

class object {
public:
    string name;
    dvec3 position = dvec3(0,0,0);
    dvec3 x_dir = dvec3(1,0,0);
    dvec3 y_dir = dvec3(0,1,0);
    dvec3 color = dvec3(0,1,0);
    double x_size = 1;
    double y_size = 1;
    double reflection = 0;
    double refraction = 0;
    dvec3 last_beam_pos;
    dvec3 last_beam_dir;
    dvec3 last_intersect_point;
    object * last_beam_intersect;
    vector<object *> parts;
    
    object(string s);
    ~object();
    virtual void translate(dvec3 trans);
    virtual void rotate(dvec3 rot_axis, float rot_angle);
    virtual pair<bool, dvec3> is_intersected_by(dvec3 beam_pos, dvec3 beam_dir);
    virtual void trace(dvec3 color);
    virtual dvec3 backtrace();
};

template <int text_x, int text_y>
class rectangle: object {
public:
    dvec3 texture[text_x][text_y];
    
    rectangle(string name):object(name) {
        x_size = text_x / 1000.0;
        y_size = text_y / 1000.0;
    }
    
    pair<bool, dvec3> is_intersected_by(dvec3 beam_pos, dvec3 beam_dir) {
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
            return make_pair(false, dvec3(0));
            dvec3 p(int_point->x(), int_point->y(), int_point->z());
            last_beam_dir = beam_dir;
            last_beam_pos = beam_pos;
            last_intersect_point = p;
            return make_pair(true, p);
        } else if (do_intersect(tr2, ray)) {
            auto int_opt_point = intersection(tr2, ray);
            Point_3<Cartesian<double>> *int_point = boost::get<Point_3<Cartesian<double>>>(&*int_opt_point);
            if (!int_point)
            return make_pair(false, dvec3(0));
            dvec3 p(int_point->x(), int_point->y(), int_point->z());
            last_beam_dir = beam_dir;
            last_beam_pos = beam_pos;
            last_intersect_point = p;
            return make_pair(true, p);
        } else
        return make_pair(false, dvec3(0));
    }
    
    void trace(dvec3 col) {
        for (int i=0; i<text_x; i++) {
            for (int j=0; j<text_y; j++) {
                texture[i][j] = color;
            }
        }
    }
    
    dvec3 backtrace() {
        dvec3 relative_coodrs( last_intersect_point - (position - 0.5 * ( y_dir * y_size + x_dir * x_size )) );
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
        
        return texture[int(999*x)][int(999*y)];
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

class scene: object {
public:
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
