#pragma once

#include <utility>
#include "glm/glm.hpp"
#include "intersect_info.h"

using namespace glm;

using namespace std;

#define MIN2(x,y) ((x)<(y)?(x):(y))
#define MAX2(x,y) ((x)>(y)?(x):(y))
#define MIN3(x,y,z) (MIN2(x,y)<(z)?MIN2(x,y):(z))
#define MAX3(x,y,z) (MAX2(x,y)>(z)?MAX2(x,y):(z))
#define MIN4(x,y,z,w) (MIN3(x,y,z)<(w)?MIN3(x,y,z):(w))
#define MAX4(x,y,z,w) (MAX3(x,y,z)>(w)?MAX3(x,y,z):(w))

double brightness(dvec3 cur);

bool cmp(pair<intersect_info, double> a, pair<intersect_info, double> b);

