#include "utilites.h"

double brightness(dvec3 cur) {
    return 0.2125 * cur.r + 0.7154 * cur.g + 0.0721 * cur.b;
}

bool cmp(pair<intersect_info, double> a, pair<intersect_info, double> b) {
    return a.second < b.second;
}
