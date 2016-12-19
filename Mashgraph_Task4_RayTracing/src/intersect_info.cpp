#include "intersect_info.h"

intersect_info::intersect_info(bool r, dvec3 p, void *o) {
    res = r;
    point = p;
    obj = (object *) o;
}

intersect_info::intersect_info() {
    res = false;
    point = dvec3(0);
    obj = NULL;
}
