#ifndef object_h
#define object_h

#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cmath>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

using namespace std;

#include "intersect_info.h"
#include "utilites.h"

class object {
public:
    string name;
    dvec3 position = dvec3(0,0,0);
    dvec3 x_dir = dvec3(1,0,0);
    dvec3 y_dir = dvec3(0,1,0);
    dvec3 color = dvec3(0,1,0);
    double x_size = 1;
    double y_size = 1;
    double diffuse = 0.5;
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

#endif /* object_h */
