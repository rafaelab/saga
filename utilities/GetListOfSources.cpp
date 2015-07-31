/*
Creates a text file containing a list of "sources".
Sources are defined as regions with density higher than rho0.
This approach considers all cells, which may be biased due to the
 non uniformity of the grid. See GetListOfSources2 for another approach.
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
}

int main(int argc, char** argv )
{

    if(argc != 9)
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

    std::cout << "Input file: " << filename << std::endl;
    saga::ref_ptr<saga::AMRgrid> amr = new saga::AMRgrid(filename, 10);
    std::cout << "Input file opened." << std::endl;

    std::cout << "Critical density = " << rho0 << std::endl;
    std::cout << "Output file: " << outputfile << std::endl;
    std::ofstream fout(outputfile.c_str());

    int nCells = amr->getGridSize();
    std::cout << "Number of cells: " << nCells << std::endl;
    for (int i=1; i<nCells; i++) {
        saga::AMRcell cell = amr->getCellWithIndex(i);
        double x = cell.getXcenter() * cl + x0;
        double y = cell.getYcenter() * cl + y0;
        double z = cell.getZcenter() * cl + z0;
        // std::cout << i << " " << x << " " << y << " " << z << std::endl;
        saga::LocalProperties lp = amr->getLocalPropertiesFromIndex(i);
        double rho = lp.getDensity() * crho;
        if (rho > rho0) fout << x << "\t" << y << "\t" << z << std::endl;
    }

    fout.close();

    return 0;
}