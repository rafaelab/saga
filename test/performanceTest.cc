#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>
#include "saga/AMRgrid.h"
#include "saga/LocalProperties.h"
using namespace std;
using namespace saga;

// Unit conversion factors
const double convLength=8.57599e+24; // m
const double convDensity=2.49651e-27;  // kg/m^3
const double convMagneticField=1.11754673542e-09; // T
const double Mpc=3.08567758e22; // SI to Mpc


float simplePerformanceTest(AMRgrid *db, double x, double y, double z, bool printInfo=false) {

    clock_t t_initial=clock();


    LocalProperties lp=db->getLocalProperties(x,y,z);    
    lp.convertUnits(convLength,convDensity,convMagneticField);
    clock_t t_final=clock();
    double msecs = ((double) (t_final-t_initial))*1000/CLOCKS_PER_SEC;
        if(printInfo) {
        cout << "LocalProperties:" << endl;
        cout << "  density = " << lp.getDensity() << " kg/m^3" << endl;
        cout << "  Bx = " << lp.getBx() << " T" << endl;
        cout << "  By = " << lp.getBy() << " T" << endl;
        cout << "  Bz = " << lp.getBz() << " T" << endl;
        cout << "  Btot = " << sqrt(lp.getBx()*lp.getBx()+lp.getBy()*lp.getBy()+lp.getBz()*lp.getBz()) << " T" << endl;
        cout << "execution time: " << msecs << " ms" << endl;
    }
   

    return msecs;
}



int main(){

    cout << "Performance test" << endl;
    
    const char filename[]="/Users/rafaelab/Work/Simulations/LSS-MinSu/SQLite_data/output_00083.sql";
    const int nsamples=10;

    // position of interest in Mpc
    double Mpc=3.08567758e22;
    double x=13.*Mpc;
    double y=53.*Mpc;
    double z=17.*Mpc;
    x/=convLength;
    y/=convLength;
    z/=convLength;

    AMRgrid *db = new AMRgrid(filename);

    float t=0;
    for (i=0; i<nsamples; i++){
        t+=simplePerformanceTest(db, x, y, z);
    }
    t/=nsamples;
    
    cout << "average time: " << t << " ms" << endl;

    db->close();
    delete db;

    return 0;

}