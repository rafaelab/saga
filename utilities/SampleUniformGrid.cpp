#include <iostream>
#include <fstream>
#include <cstring>
// #include <sstream>

#include "saga/AMRcell.h"
#include "saga/AMRgrid.h"
#include "saga/LocalProperties.h"
#include "saga/MagneticField.h"
#include "saga/SQLiteInterface.h"
#include "saga/Referenced.h"


int main(int argc, char** argv )
{

    if(argc != 4 && argc != 5)
    {
        std::cout << "Running tests." << std::endl;
        std::cout << "USAGE" << std::endl;
        std::cout << "  ./" << argv[0] << " <path_to_SQL_file> <output_binary_file> <size_of_grid> <conversion_factor_B>" <<  std::endl;
        return -1;
    }
    std::string filename = argv[1];
    std::string outputfile = argv[2];
    int sz = atoi(argv[3]);
    double convB = 1;
    if (argc == 5) convB = atof(argv[4]);

    std::cout << "Input file: " << filename << std::endl;
    saga::ref_ptr<saga::AMRgrid> amr = new saga::AMRgrid(filename, 10);
    std::cout << "Input file opened." << std::endl;
    std::cout << "The AMR grid will be uniformly sampled in " << sz << "^3 points." << std::endl;
    std::cout << "Output file: " << outputfile << std::endl;
    std::ofstream fout(outputfile.c_str(), std::ios::binary);

    for(int i=0; i<sz; i++) {
        for(int j=0; j<sz; j++) {
            for(int k=0; k<sz; k++) {
                double x = ((double)i ) / sz;
                double y = ((double)j ) / sz;
                double z = ((double)k ) / sz;
                //std::cout << x << " " << y << " "<< z << std::endl;
                saga::LocalProperties lp = amr->getLocalProperties(x,y,z);
                float Bx = lp.getBx() * convB;
                float By = lp.getBy() * convB;
                float Bz = lp.getBz() * convB;
                fout.write((char*) &Bx, sizeof(float));
                fout.write((char*) &By, sizeof(float));
                fout.write((char*) &Bz, sizeof(float));
                // std::cout << Bx << " " << By << " " << Bz << std::endl;
            }
        }
    }



    return 0;
}