#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cmath>

#ifdef _OPENMP
    #include "omp.h"
#endif


#include "sqlite3.h"

#include "../include/saga/AMRgrid.h"
#include "../include/saga/LocalProperties.h"
#include "../include/saga/SQLiteInterface.h"
#include "../include/saga/AMRcell.h"


double unifRand()
{
  return rand() / double(RAND_MAX);
}

int  main(int argc, char *argv[])
{

    std::string filename = (std::string) argv[1];
//    int nTrials = strtol(argv[2], NULL, 10);


    //std::string filename="/Users/rafaelab/Work/Simulations/LSS-MinSu/SQLite_data/output_00083.sql";
    std::cout << "Filename: " << filename << std::endl;    

    saga::AMRgrid *amr = new saga::AMRgrid(filename, 18);

    // Prepare the search positions 
    int nTrials=100;
    for(int i=0; i<nTrials; i++) {
        double x = unifRand();
        double y = unifRand();
        double z = unifRand();
        saga::LocalProperties lp = amr->getLocalProperties(x,y,z);
        std::cout << lp.getBx() << " " << lp.getBy() << " " << lp.getBz() << " " << lp.getDensity() << std::endl;
    }    

    amr->close();

    return 0;

}