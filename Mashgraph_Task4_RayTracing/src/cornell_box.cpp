#include "cornell_box.h"

cornellBox::cornellBox():object("cornellBox") {
    object *square;
    
    square = (object *) new rectangle<1000, 1000>("Back_wall");
    square->translate(dvec3(0,0,-0.5));
    square->color = dvec3(1,1,0.5);
    parts.push_back(square);
    
    square = (object *) new rectangle<1000, 1000>("Left_wall");
    square->translate(dvec3(0,0,-0.5));
    square->rotate(dvec3(0,1,0), 90);
    square->color = dvec3(1,0,0);
    parts.push_back(square);
    
    square = (object *) new rectangle<1000, 1000>("Right_wall");
    square->translate(dvec3(0,0,-0.5));
    square->rotate(dvec3(0,1,0), 270);
    square->color = dvec3(0.3,0.85,0);
    parts.push_back(square);
    
    square = (object *) new rectangle<1000, 1000>("Upper_wall");
    square->translate(dvec3(0,0,-0.5));
    square->rotate(dvec3(1,0,0), 90);
    square->color = dvec3(1,1,0.7);
    parts.push_back(square);
    
    square = (object *) new rectangle<1000, 1000>("Floor");
    square->translate(dvec3(0,0,-0.5));
    square->rotate(dvec3(1,0,0), 270);
    square->color = dvec3(1,0.6,0.3);
    parts.push_back(square);
}
