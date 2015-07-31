/*
Creates uniform grids (binary files) containing the magnetic field and the 
 density, for a given number of sampling points.
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
    std::cout << "Running tests." << std::endl;
    std::cout << "USAGE" << std::endl;
    std::cout << "./" << name << " <path_to_SQL_file> <output_binary_file_Bfield> <output_binary_file_density> <size_of_grid> <conversion_factor_B> <conversion_factor_rho>" <<  std::endl;    
    std::cout << "  arg 1: path to SQL file containing the magnetic field and density" << std::endl;
    std::cout << "  arg 2: name of output file 1 (magnetic field)" << std::endl;
    std::cout << "  arg 3: name of output file 2 (baryon density)" << std::endl;
    std::cout << "  arg 4: number of sampling points in each side." << std::endl;
    std::cout << "  arg 5: conversion factor for the magnetic field [optional; default=1]" << std::endl;
    std::cout << "  arg 6: conversion factor for the density [optional; default=1]" << std::endl;
    std::cout << "  * if arg5 or arg6 is provided, the other one must also be" << std::endl;
}

int main(int argc, char** argv )
{

    if(argc != 5 && argc != 7)
    {
        Usage(argv[0]);
        return -1;
    }
    std::string filename = argv[1];
    std::string outputfile1 = argv[2];
    std::string outputfile2 = argv[3];
    int sz = atoi(argv[4]);
    double convB = 1;
    double convRho = 1;
    if (argc == 7) {
        convB = atof(argv[5]);
        convRho = atof(argv[6]);
    }

    std::cout << "Input file: " << filename << std::endl;
    saga::ref_ptr<saga::AMRgrid> amr = new saga::AMRgrid(filename, 10);
    std::cout << "Input file opened." << std::endl;

    std::cout << "The AMR grid will be uniformly sampled in " << sz << "^3 points." << std::endl;
    std::cout << "Output file (B field): " << outputfile1 << std::endl;
    std::cout << "Output file (density): " << outputfile2 << std::endl;
    std::ofstream fout1(outputfile1.c_str(), std::ios::binary);
    std::ofstream fout2(outputfile2.c_str(), std::ios::binary);


    for(int i=0; i<sz; i++) {
        for(int j=0; j<sz; j++) {
            for(int k=0; k<sz; k++) {
                double x = ((double)i ) / sz;
                double y = ((double)j ) / sz;
                double z = ((double)k ) / sz;
                saga::LocalProperties lp = amr->getLocalProperties(x, y, z);
                float Bx = lp.getBx() * convB;
                float By = lp.getBy() * convB;
                float Bz = lp.getBz() * convB;
                float rho = lp.getDensity() * convRho;
                fout1.write((char*) &Bx, sizeof(float));
                fout1.write((char*) &By, sizeof(float));
                fout1.write((char*) &Bz, sizeof(float));
                fout2.write((char*) &rho, sizeof(float));
            }
        }
    }
    std::cout << "Files written" << std::endl;
    fout1.close();
    fout2.close();

    return 0;
}