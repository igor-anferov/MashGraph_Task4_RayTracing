#pragma once

#include "glm/glm.hpp"

using namespace glm;

using namespace std;

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
