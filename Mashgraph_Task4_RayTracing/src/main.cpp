#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

#include <libiomp/omp.h>

#include "object.h"

using namespace std;

map<string, double> parameters;

int main(int argc, char** argv) {
    parameters[ "camera_position_x" ] = 0;
    parameters[ "camera_position_y" ] = 0;
    parameters[ "camera_position_z" ] = 1850;
    parameters[ "camera_right_x" ] = 1;
    parameters[ "camera_right_y" ] = 0;
    parameters[ "camera_right_z" ] = 0;
    parameters[ "camera_up_x" ] = 0;
    parameters[ "camera_up_y" ] = 1;
    parameters[ "camera_up_z" ] = 0;
    parameters[ "LED_position_x" ] = 0;
    parameters[ "LED_position_y" ] = 499.9;
    parameters[ "LED_position_z" ] = 0;
    parameters[ "LED_x_side_dir_x" ] = 1;
    parameters[ "LED_x_side_dir_y" ] = 0;
    parameters[ "LED_x_side_dir_z" ] = 0;
    parameters[ "LED_y_side_dir_x" ] = 0;
    parameters[ "LED_y_side_dir_y" ] = 0;
    parameters[ "LED_y_side_dir_z" ] = 1;
    parameters[ "LED_color_R" ] = 255;
    parameters[ "LED_color_G" ] = 255;
    parameters[ "LED_color_B" ] = 255;
    parameters[ "focal_length" ] = 50;
    parameters[ "res_w" ] = 512;
    parameters[ "res_h" ] = 512;
    parameters[ "SSAA" ] = 1;
    parameters[ "primary_rays_count" ] = 1000000;
    
    if (argc == 3) {
        fstream configfile(argv[1]);
        if (!configfile) {
            cout << "Configuration file opening failed. Using stardard parameters:" << endl << endl;
        }
        string curstr;
        string parameter;
        double value;
        int parameters_count = 0;
        getline(configfile, curstr);
        while (configfile) {
            stringstream ss(curstr.substr(0,curstr.find("//")));
            if (ss >> parameter >> value) {
                parameters[parameter] = value;
                parameters_count++;
            }
            getline(configfile, curstr);
        }
        if (parameters_count) {
            cout << "Configuration file successfully handled. " << parameters_count << " parameter(s) was recieved.";
            if (parameters.size() - parameters_count) {
                cout << " Other " << parameters.size() - parameters_count << " parameter(s) is(are) default.";
            }
            cout << endl << endl << "Using parameters:" << endl << endl;
        }
    } else {
        cout << "There is no configuration file. Using stardard parameters:" << endl << endl;
    }
    for (pair<string, int> tmp: parameters) {
        cout << tmp.first << " = " << tmp.second << endl;
    }
    
    object *sc = (object *) new scene;
    
    double start_time = omp_get_wtime();
    try {
        ((scene *)sc)->light(parameters[ "primary_rays_count" ]);
    } catch (const char * str) {
        cout << str << endl;
    } catch (...) {
        cout << "UNHANDLED ERROR!!!" << endl;
    }
    cout << endl << "Lighting took " << (omp_get_wtime() - start_time) << " sec." << endl;
    
    camera cam(dvec3(parameters[ "camera_position_x" ] / 1000.0,
                     parameters[ "camera_position_y" ] / 1000.0,
                     parameters[ "camera_position_z" ] / 1000.0),
               dvec3(parameters[ "camera_right_x" ],
                     parameters[ "camera_right_y" ],
                     parameters[ "camera_right_z" ]),
               dvec3(parameters[ "camera_up_x" ],
                     parameters[ "camera_up_y" ],
                     parameters[ "camera_up_z" ]),
               parameters[ "focal_length" ],
               parameters[ "res_w" ],
               parameters[ "res_h" ],
               parameters[ "SSAA" ], sc);
    
    start_time = omp_get_wtime();
    try {
        cam.draw();
    } catch (const char * str) {
        cout << str << endl;
    } catch (...) {
        cout << "UNHANDLED ERROR!!!" << endl;
    }
    cout << endl << "Drawing took " << (omp_get_wtime() - start_time) << " sec." << endl;
    cam.save_to_file(argc == 2 ? argv[1] : argv[2]);
    return 0;
}
