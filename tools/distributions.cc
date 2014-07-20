#include <iostream>
#include <ctime>
#include <cmath>
#include "saga/AMRgrid.h"
#include "saga/LocalProperties.h"
using namespace std;
using namespace saga;

int main(){


    cout << "Running main" << endl;

    
    char filename[]="/Users/rafaelab/Work/Simulations/LSS-MinSu/SQLite_data/output_00083.sql";

    // Unit conversion factors
    double convLength=8.57599e+24; // m
    double convDensity=2.49651e-27;  // kg/m^3
    double convMagneticField=1.11754673542e-09; // T

    // position of interest in Mpc
    double Mpc=3.08567758e22;
    double x=13.*Mpc;
    double y=53.*Mpc;
    double z=17.*Mpc;
    x/=convLength;
    y/=convLength;
    z/=convLength;

    cout << "x, y, z " << x << " " << y << " " << z << endl;

    double cellSize=1/pow(2.,18); // 0.01
    double minX=x;
    double minY=y;
    double minZ=z;
    double maxX=minX+cellSize;
    double maxY=minY+cellSize;
    double maxZ=minZ+cellSize;


    int nRegions=100;

    AMRgrid *db = new AMRgrid(filename);

    for(int i=0; i<nRegions; i++){
        for(int j=0; j<nRegions; j++){
            for(int k=0; k<nRegions; k++){
                LocalProperties lp=db->getLocalProperties(x,y,z);    
                lp.convertUnits(convLength,convDensity,convMagneticField);
                cout << lp.getDensity() << endl;
                //delete lp;
            } // k loop
        } // j loop
    } //i loop

    delete db;


    return 0;

}