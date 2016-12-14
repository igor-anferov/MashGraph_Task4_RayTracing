#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

#include <omp.h>

#include "object.h"

using namespace std;

int main(int argc, char** argv) {
    map<string, int> parameters;
    
    parameters[ "LED_side"          ] = 50;
    parameters[ "camera_position_x" ] = 50;
    parameters[ "focal_length"      ] = 100;
    parameters[ "res_h"             ] = 1080;
    parameters[ "res_w"             ] = 1920;
    
    if (argc == 2) {
        fstream configfile(argv[1]);
        if (!configfile) {
            cout << "Configuration file opening failed. Using stardard parameters:" << endl << endl;
        }
        string curstr;
        string parameter;
        int value;
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
    camera cam(dvec3(0,0,1.85), dvec3(1,0,0), dvec3(0,1,0), 50, 1920, 1080, 1, sc);
    
    double start_time = omp_get_wtime();
    cam.draw();
    cout << endl << "Drawing took " << (omp_get_wtime() - start_time) << " sec." << endl;
    cam.save_to_file("res.bmp");
    
    return 0;
}
