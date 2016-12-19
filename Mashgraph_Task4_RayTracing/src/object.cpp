#include "object.h"

void object::translate(dvec3 trans) {
    position+=trans;
    for (object *i: parts)
    i->translate(trans);
}

object::object(string n) {
    name = n;
}

object::~object() {
    for (object *i: parts)
        delete i;
}

void object::rotate(dvec3 rot_axis, float rot_angle) {
    dmat4 rot_matrix = dmat4(glm::rotate(mat4(), rot_angle, vec3(rot_axis)));
    x_dir = dvec3(rot_matrix * dvec4(position+x_dir, 1));
    y_dir = dvec3(rot_matrix * dvec4(position+y_dir, 1));
    position = dvec3(rot_matrix * dvec4(position, 1));
    x_dir -= position;
    y_dir -= position;
    for (object *i: parts)
        i->rotate(rot_axis, rot_angle);
}

void object::emit() {
    for (object *i: parts)
    i->emit();
}

void object::set_main_scene(object *scene) {
    main_scene = scene;
    for (object *i: parts)
    i->set_main_scene(scene);
}

intersect_info object::is_intersected_by(dvec3 beam_pos, dvec3 beam_dir) {
    intersect_info tmp;
    vector<pair<intersect_info, double>> intersect_positions;
    for (object *i: parts) {
        tmp = i->is_intersected_by(beam_pos, beam_dir);
        if (!tmp.res) {
            continue;
        }
        intersect_positions.push_back(make_pair(tmp, length(tmp.point - beam_pos)));
    }
    if (intersect_positions.empty()) {
        return intersect_info(false, dvec3(0), NULL);
    }
    sort(intersect_positions.begin(), intersect_positions.end(), cmp);
    return intersect_positions[0].first;
}

void object::trace(dvec3 color, dvec3 beam_pos, dvec3 beam_dir, intersect_info i) {
    throw "ERROR! Calling trace from intermediate class " + name;
}

dvec3 object::backtrace(dvec3 beam_pos, dvec3 beam_dir, intersect_info i) {
    throw "ERROR! Calling backtrace from intermediate class " + name;
}

