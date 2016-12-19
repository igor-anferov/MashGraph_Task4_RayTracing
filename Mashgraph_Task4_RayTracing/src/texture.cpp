#include "texture.h"
#include "utilites.h"

#include <cstring>

tex::~tex() {
    delete data;
}
void tex::init(int xx, int yy) {
    x=xx;
    y=yy;
    data = new dvec3[xx*yy];
    memset(data, 0, xx*yy*sizeof(dvec3));
}
dvec3* tex::operator[] (int i) {
    if (!data)
        throw "ERROR! Trying to access to uninitialized tex!";
    return & data[ y * i ];
}
double tex::get_average() {
    long double sum = 0;
    long not_black = 0;
    for (long i=0; i<x*y; i++) {
        dvec3 cur = data[i];
        double light = brightness(cur);
        if (light > 1e-3) {
            not_black++;
            sum += light;
        }
    }
    return sum/not_black;
}
