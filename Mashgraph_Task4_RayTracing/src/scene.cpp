#include "scene.h"

scene::scene():object("Scene") {
    object *figure;
    
    figure = (object *) new cornellBox;
    parts.push_back(figure);
    
    figure = (object *) new LED<100,100>("Flashlight");
    figure->color = dvec3(parameters[ "LED_color_R" ] / 255.0,
                          parameters[ "LED_color_G" ] / 255.0,
                          parameters[ "LED_color_B" ] / 255.0);
    figure->x_dir = normalize( dvec3(parameters[ "LED_x_side_dir_x" ],
                                     parameters[ "LED_x_side_dir_y" ],
                                     parameters[ "LED_x_side_dir_z" ]) );
    figure->y_dir = normalize( dvec3(parameters[ "LED_y_side_dir_x" ],
                                     parameters[ "LED_y_side_dir_y" ],
                                     parameters[ "LED_y_side_dir_z" ]) );
    figure->translate(dvec3(parameters[ "LED_position_x" ] / 1000.0,
                            parameters[ "LED_position_y" ] / 1000.0,
                            parameters[ "LED_position_z" ] / 1000.0));
    figure->parts[0]->color = dvec3(parameters[ "LED_color_R" ] / 255.0,
                                    parameters[ "LED_color_G" ] / 255.0,
                                    parameters[ "LED_color_B" ] / 255.0);
    figure->parts[0]->x_dir = normalize( dvec3(parameters[ "LED_x_side_dir_x" ],
                                               parameters[ "LED_x_side_dir_y" ],
                                               parameters[ "LED_x_side_dir_z" ]) );
    figure->parts[0]->y_dir = normalize( dvec3(parameters[ "LED_y_side_dir_x" ],
                                               parameters[ "LED_y_side_dir_y" ],
                                               parameters[ "LED_y_side_dir_z" ]) );
    
    parts.push_back(figure);
    
    figure = (object *) new parallelepiped;
    figure->rotate(dvec3(0,1,0), 20);
    figure->translate(dvec3(-0.175,-0.175,-0.29));
    parts.push_back(figure);
    
    figure = (object *) new sphere<400>("Sphere");
    figure->color = dvec3(0.85,0.72,0.35);
    figure->translate(dvec3(0.220,-0.300,-0.050));
    
    parts.push_back(figure);
    
    set_main_scene(this);
}

void scene::light(long primary_rays_count) {
#pragma omp parallel for schedule(dynamic)
    for (long i=0; i<primary_rays_count; i++) {
        emit();
    }
}
