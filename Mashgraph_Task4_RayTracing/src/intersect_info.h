#pragma once

#include "glm/glm.hpp"

using namespace glm;

class object;

struct intersect_info {
    bool res;
    dvec3 point;
    object *obj;
    
    intersect_info();
    intersect_info(bool r, dvec3 p, void *o);
};
