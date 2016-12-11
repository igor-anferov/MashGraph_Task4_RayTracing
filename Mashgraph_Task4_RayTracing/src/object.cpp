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

object::~object() {
    for (object *i: parts)
        delete i;
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
    
    for (object *i: parts)
        i->trace(dvec3(0));
}

parallelepiped::parallelepiped():object("Parallelepiped") {
    object *square;
    
    square = (object *) new rectangle<350, 700>("Back_wall");
    square->translate(dvec3(0,0,-0.175));
    square->color = dvec3(0.4,0.22,0.1);
    parts.push_back(square);
    
    square = (object *) new rectangle<350, 700>("Front_wall");
    square->translate(dvec3(0,0,0.175));
    square->color = dvec3(1,0,0);
    parts.push_back(square);
    
    square = (object *) new rectangle<350, 700>("Left_wall");
    square->translate(dvec3(0,0,-0.175));
    square->rotate(dvec3(0,1,0), 90);
    square->color = dvec3(0,1,0);
    parts.push_back(square);
    
    square = (object *) new rectangle<350, 700>("Right_wall");
    square->translate(dvec3(0,0,-0.175));
    square->rotate(dvec3(0,1,0), 270);
    square->color = dvec3(0,0,1);
    parts.push_back(square);
    
    square = (object *) new rectangle<350, 350>("Upper_wall");
    square->rotate(dvec3(1,0,0), 90);
    square->translate(dvec3(0,0.350,0));
    square->color = dvec3(1,0,1);
    parts.push_back(square);
    
    square = (object *) new rectangle<350, 350>("Floor");
    square->rotate(dvec3(1,0,0), 270);
    square->translate(dvec3(0,-0.350,0));
    square->color = dvec3(0,0,0);
    parts.push_back(square);
    
    for (object *i: parts)
        i->trace(dvec3(0));
}

scene::scene():object("Scene") {
    object *figure;
    
    figure = (object *) new cornellBox;
    parts.push_back(figure);
    
    figure = (object *) new parallelepiped;
    figure->rotate(dvec3(0,1,0), 30);
    figure->translate(dvec3(-0.1,-0.150,-0.1));
    parts.push_back(figure);
}

tex::~tex() {
    delete data;
}
void tex::init(int xx, int yy) {
    x=xx;
    y=yy;
    data = new dvec3[xx*yy];
    memset(data, 0, xx*yy*sizeof(dvec3));
}
dvec3* tex::operator[] (int i) {
    if (!data)
        throw "ERROR! Trying to access to uninitialized tex!";
    return & data[ y * i ];
}


camera::camera(dvec3 p, dvec3 x_d, dvec3 y_d, double f_l, int x_r, int y_r, int ss, object * sc): position(p), x_dir(x_d), y_dir(y_d), focal_length(f_l), x_res(x_r), y_res(y_r), ssaa(ss), scene(sc) {
    
    if (dot(x_dir, y_dir) != 0)
        throw "ERROR! x_dir and y_dir aren't perpendicular!";
    texture.init(x_res, y_res);
    normalize(x_dir);
    normalize(y_dir);
    z_dir = cross(x_dir, y_dir);
    normalize(z_dir);
    z_dir *= focal_length/1000;
    x_dir *= (35 * 0.5)/1000;
    y_dir *= (35 * 0.5)/1000 / x_res * y_res;
}

void camera::draw() {
    dvec3 source = position + z_dir;
    dvec3 step_x = 2 * length(x_dir) / x_res * normalize(x_dir);
    dvec3 step_y = 2 * length(y_dir) / y_res * normalize(y_dir);
    default_random_engine generator;
    uniform_real_distribution<double> distribution_x (0, 2 * length(x_dir) / x_res);
    uniform_real_distribution<double> distribution_y (0, 2 * length(y_dir) / y_res);
    dvec3 local_zero = position - x_dir - y_dir - source;
    vector<dvec3> ss_points;
    for (int i=0; i<ssaa; i++) {
        ss_points.push_back(dvec3(normalize(x_dir)*distribution_x(generator) +
                                  normalize(y_dir)*distribution_y(generator)));
    }
    for (int i=0; i<x_res; i++) {
        for (int j=0; j<y_res; j++) {
            for (int k=0; k<ssaa; k++) {
                dvec3 dir = local_zero + ss_points[k];
                if (scene->is_intersected_by(source, dir).first) {
                    texture[i][j] += scene->backtrace();
                }
            }
            texture[i][j] /= ssaa;
            local_zero += step_y;
        }
        local_zero += step_x;
        local_zero -= 2.0 * y_dir;
    }
}

void camera::save_to_file(string file) {
    vector<unsigned char> map;
    for (int j=y_res-1; j>=0; j--) {
        for (int i=0; i<x_res; i++) {
            map.push_back((unsigned char)(texture[i][j].r * 255));
            map.push_back((unsigned char)(texture[i][j].g * 255));
            map.push_back((unsigned char)(texture[i][j].b * 255));
        }
    }
    SOIL_save_image ( file.c_str(), SOIL_SAVE_TYPE_BMP, x_res, y_res, SOIL_LOAD_RGB, map.data() );
}
