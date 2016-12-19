#include "parallelepiped.h"

parallelepiped::parallelepiped():object("Parallelepiped") {
    object *square;
    
    square = (object *) new rectangle<325, 650>("Back_wall");
    square->translate(dvec3(0,0,-0.1625));
    square->color = dvec3(0.57,0.46,0.18);
    parts.push_back(square);
    
    square = (object *) new rectangle<325, 650>("Front_wall");
    square->translate(dvec3(0,0,0.1625));
    square->color = dvec3(0.57,0.46,0.18);
    parts.push_back(square);
    
    square = (object *) new rectangle<325, 650>("Left_wall");
    square->translate(dvec3(0,0,-0.1625));
    square->rotate(dvec3(0,1,0), 90);
    square->color = dvec3(0.57,0.46,0.18);
    parts.push_back(square);
    
    square = (object *) new rectangle<325, 650>("Right_wall");
    square->translate(dvec3(0,0,-0.1625));
    square->rotate(dvec3(0,1,0), 270);
    square->color = dvec3(0.57,0.46,0.18);
    parts.push_back(square);
    
    square = (object *) new rectangle<325, 325>("Upper_wall");
    square->rotate(dvec3(1,0,0), 90);
    square->translate(dvec3(0,0.325,0));
    square->color = dvec3(0.57,0.46,0.18);
    parts.push_back(square);
    
    square = (object *) new rectangle<325, 325>("Floor");
    square->rotate(dvec3(1,0,0), 270);
    square->translate(dvec3(0,-0.325,0));
    square->color = dvec3(0.57,0.46,0.18);
    parts.push_back(square);
}
