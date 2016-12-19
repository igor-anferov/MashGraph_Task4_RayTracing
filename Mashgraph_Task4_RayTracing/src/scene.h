#pragma once

#include "parallelepiped.h"
#include "sphere.h"
#include "cornell_box.h"
#include "LED.h"
#include "parameters.h"

class scene: object {
public:
    void light(long primary_rays_count);
    scene();
    ~scene();
};
