#include "object.h"

double brightness(dvec3 cur) {
    return 0.2125 * cur.r + 0.7154 * cur.g + 0.0721 * cur.b;
}

bool cmp(pair<intersect_info, double> a, pair<intersect_info, double> b) {
    return a.second < b.second;
}

intersect_info::intersect_info(bool r, dvec3 p, void *o) {
    res = r;
    point = p;
    obj = (object *) o;
}

intersect_info::intersect_info() {
    res = false;
    point = dvec3(0);
    obj = NULL;
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
double tex::get_average() {
    long double sum = 0;
    long not_black = 0;
    for (long i=0; i<x*y; i++) {
        dvec3 cur = data[i];
        double light = brightness(cur);
        if (light > 1e-3) {
            not_black++;
            sum += light;
        }
    }
    return sum/not_black;
}


camera::camera(dvec3 p, dvec3 x_d, dvec3 y_d, double f_l, int x_r, int y_r, int ss, object * sc): position(p), x_dir(x_d), y_dir(y_d), focal_length(f_l), x_res(x_r), y_res(y_r), ssaa(ss), scene(sc) {
    
    if (dot(x_dir, y_dir) != 0)
        throw "ERROR! x_dir and y_dir aren't perpendicular!";
    texture.init(x_res, y_res);
    x_dir = normalize(x_dir);
    y_dir = normalize(y_dir);
    z_dir = cross(x_dir, y_dir);
    z_dir = normalize(z_dir);
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
#pragma omp parallel for schedule(dynamic)
    for (int i=0; i<x_res; i++) {
        dvec3 local_zero_i = local_zero + double(i)*step_x;
        for (int j=0; j<y_res; j++) {
            for (int k=0; k<ssaa; k++) {
                dvec3 dir = local_zero_i + ss_points[k];
                dir = normalize(dir);
                intersect_info tmp = scene->is_intersected_by(source, dir);
                if (tmp.res) {
                    dvec3 color = tmp.obj->backtrace(source, dir, tmp);
                    texture[i][j] += color;
                }
            }
            texture[i][j] /= ssaa;
            local_zero_i += step_y;
        }
    }
}

void camera::save_to_file(string file) {
    vector<unsigned char> map;
    double norm = texture.get_average()*2;
    for (int j=y_res-1; j>=0; j--) {
        for (int i=0; i<x_res; i++) {
            map.push_back((unsigned char)(MIN2(texture[i][j].r / norm * 255, 255)));
            map.push_back((unsigned char)(MIN2(texture[i][j].g / norm * 255, 255)));
            map.push_back((unsigned char)(MIN2(texture[i][j].b / norm * 255, 255)));
        }
    }
    SOIL_save_image ( file.c_str(), SOIL_SAVE_TYPE_BMP, x_res, y_res, SOIL_LOAD_RGB, map.data() );
}
