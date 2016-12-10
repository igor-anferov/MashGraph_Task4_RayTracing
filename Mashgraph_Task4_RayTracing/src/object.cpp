#include "object.h"

bool cmp(pair<object *, int> a, pair<object *, int> b) {
    return a.second < b.second;
}

void object::translate(dvec3 trans) {
    position+=trans;
    for (object *i: parts)
    i->translate(trans);
}

object::object(string n) {
    name = n;
}

void object::rotate(dvec3 rot_axis, float rot_angle) {
    dmat4 rot_matrix = dmat4(glm::rotate(mat4(), rot_angle, vec3(rot_axis)));
    position = dvec3(rot_matrix * dvec4(position, 1));
    x_dir = dvec3(rot_matrix * dvec4(x_dir, 1));
    y_dir = dvec3(rot_matrix * dvec4(y_dir, 1));
    for (object *i: parts)
        i->rotate(rot_axis, rot_angle);
}

pair<bool, dvec3> object::is_intersected_by(dvec3 beam_pos, dvec3 beam_dir) {
    pair<bool, dvec3> tmp;
    vector<pair<object *, int>> intersect_positions;
    for (object *i: parts) {
        tmp = i->is_intersected_by(beam_pos, beam_dir);
        if (!tmp.first) {
            continue;
        }
        intersect_positions.push_back(make_pair(i, length(tmp.second - beam_pos)));
    }
    if (intersect_positions.empty()) {
        last_beam_intersect = NULL;
        return make_pair(false, dvec3(0));
    }
    sort(intersect_positions.begin(), intersect_positions.end(), cmp);
    last_beam_intersect = intersect_positions[0].first;
    return last_beam_intersect->is_intersected_by(beam_pos, beam_dir);
}
void object::trace(dvec3 color) {
    last_beam_intersect->trace(color);
}
dvec3 object::backtrace() {
    return last_beam_intersect->backtrace();
}

