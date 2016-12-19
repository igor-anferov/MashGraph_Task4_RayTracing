#include "camera.h"

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
