/*
Creates a text file containing a list of "sources".
Sources are defined as regions with density higher than rho0.
This approach is based on sampling the grid in N parts..
*/

#include <iostream>
#include <fstream>
#include <cstring>

#include "saga/AMRcell.h"
#include "saga/AMRgrid.h"
#include "saga/LocalProperties.h"
#include "saga/MagneticField.h"
#include "saga/SQLiteInterface.h"
#include "saga/Referenced.h"

void Usage(std::string name)
{
    std::cout << "USAGE" << std::endl;
    std::cout << "./" << name << " <path_to_SQL_file> arg.... " <<  std::endl;    
    std::cout << "  arg 1: path to SQL file containing the magnetic field and density" << std::endl;
    std::cout << "  arg 2: name of output text file" << std::endl;
    std::cout << "  arg 3: critical density (rho0) [suggested: 1e-29]" << std::endl;
    std::cout << "  arg 4: length conversion factor [to m]" << std::endl;
    std::cout << "  arg 5: box origin x-coordinate [use 0 for grid units]" << std::endl;
    std::cout << "  arg 6: box origin y-coordinate [use 0 for grid units]" << std::endl;
    std::cout << "  arg 7: box origin z-coordinate [use 0 for grid units]" << std::endl;
    std::cout << "  arg 8: conversion factor density" << std::endl;
    std::cout << "  arg 9: number of samples (in each dimension)" << std::endl;
}

int main(int argc, char** argv )
{

    if(argc != 10)
    {
        Usage(argv[0]);
        return -1;
    }
    std::string filename = argv[1];
    std::string outputfile = argv[2];
    double rho0 = atof(argv[3]);
    double cl = atof(argv[4]);
    double x0 = atof(argv[5]);
    double y0 = atof(argv[6]);
    double z0 = atof(argv[7]);
    double crho = atof(argv[8]);
    int N = atoi(argv[9]);

    std::cout << "Input file: " << filename << std::endl;
    saga::ref_ptr<saga::AMRgrid> amr = new saga::AMRgrid(filename, 10);
    std::cout << "Input file opened." << std::endl;

    std::cout << "Critical density = " << rho0 << std::endl;
    std::cout << "Output file: " << outputfile << std::endl;
    std::ofstream fout(outputfile.c_str());

    std::cout << "Number of sampled points: " << N << "^3." << std::endl;

    for(int i=1; i<N; i++) {
        for(int j=1; j<N; j++) {
            for(int k=1; k<N; k++) {
                double x = ((double)i ) / N;
                double y = ((double)j ) / N;
                double z = ((double)k ) / N;
                saga::LocalProperties lp = amr->getLocalProperties(x, y, z);
                double rho = lp.getDensity() * crho;
                x = x * cl + x0;
                y = y * cl + y0;
                z = z * cl + z0;
                if (rho > rho0) 
                    fout << x << "\t" << y << "\t" << z << std::endl;
            }
        }
    }

    fout.close();

    return 0;
}