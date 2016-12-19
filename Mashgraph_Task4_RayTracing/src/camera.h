#pragma once

#include <string>
#include "glm/glm.hpp"
#include "SOIL/SOIL.h"
#include "texture.h"
#include "object.h"

using namespace glm;

using namespace std;

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
