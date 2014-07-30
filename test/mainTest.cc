#include <iostream>
#include <ctime>
#include <cmath>
#include "saga/AMRcell.h"
#include "saga/AMRgrid.h"
#include "saga/LocalProperties.h"
#include "saga/MagneticField.h"
#include "saga/SQLiteInterface.h"
#include "saga/Referenced.h"
using namespace std;
using namespace saga;


// Unit conversion factors
const double convLength=8.57599e+24; // m
const double convDensity=2.49651e-27;  // kg/m^3
const double convMagneticField=1.11754673542e-09; // T
const double Mpc=3.08567758e22; // SI to Mpc

void testGetCellsRegion(saga::ref_ptr<saga::AMRgrid> amr, int nRegions)
{
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TESTING.......... getCellsRegion()" << std::endl;
    for(int i=1; i<nRegions; i++) {
        for(int j=1; j<nRegions; j++) {
            for(int k=1; k<nRegions; k++) {
                double x = ((double)i ) / nRegions;
                double y = ((double)j ) / nRegions;
                double z = ((double)k ) / nRegions;
                std::vector<saga::AMRcell> vecc = amr->getCellsRegion(x-0.01,x+0.01,y-0.01,y+0.01,z-0.01,z+0.01);  
                /*for(int l=0; l<vecc.size(); l++) {
                    std::cout << vecc[l].getXcenter() << " " << vecc[l].getYcenter() << " " << vecc[l].getZcenter() << std::endl;
                }*/
            }
        }
    }
    std::cout << "TEST SUCCEEDED... end of getCellsRegion() test" << std::endl;
}

void testGetNearestNeighbors(saga::ref_ptr<saga::AMRgrid> amr, int nRegions)
{
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TESTING.......... getNearestNeighbors()" << std::endl;
    for(int i=1; i<nRegions; i++) {
        for(int j=1; j<nRegions; j++) {
            for(int k=1; k<nRegions; k++) {
                double x = ((double)i ) / nRegions;
                double y = ((double)j ) / nRegions;
                double z = ((double)k ) / nRegions;
                std::vector<saga::AMRcell> vecc = amr->getNearestNeighbors(x,y,z);  
                /*for(int l=0; l<vecc.size(); l++) {
                    std::cout << vecc[l].getXcenter() << " " << vecc[l].getYcenter() << " " << vecc[l].getZcenter() << std::endl;
                }*/
            }
        }
    }
    std::cout << "TEST SUCCEEDED... end of getNearestNeighbors() test" << std::endl;
}

void testSelectNearestNeighbor(saga::ref_ptr<saga::AMRgrid> amr, int nRegions)
{
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TESTING.......... selectNearesNeighbor()" << std::endl;
    for(int i=0; i<nRegions; i++) {
        for(int j=0; j<nRegions; j++) {
            for(int k=0; k<nRegions; k++) {
                double x = ((double)i ) / nRegions;
                double y = ((double)j ) / nRegions;
                double z = ((double)k ) / nRegions;
                //std::cout << x << "  " << y << "  " << z << "  ";
                saga::AMRcell cell = amr->selectNearestNeighbor(x,y,z);  
                //std::cout << cell.getXcenter() << " " << cell.getYcenter() << " " << cell.getZcenter() << std::endl;
            }
        }
    }
    std::cout << "TEST SUCCEEDED... end of selectNearestNeighbor() test" << std::endl;
}

void testGetLocalPropertiesRegion(saga::ref_ptr<saga::AMRgrid> amr, int nRegions)
{
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TESTING.......... getLocalPropertiesRegion()" << std::endl;
    for(int i=1; i<nRegions; i++) {
        for(int j=1; j<nRegions; j++) {
            for(int k=1; k<nRegions; k++) {
                double x = ((double)i ) / nRegions;
                double y = ((double)j ) / nRegions;
                double z = ((double)k ) / nRegions;
                std::vector<saga::LocalProperties> lp = amr->getLocalPropertiesRegion(x-0.001,x+0.001,y-0.001,y+0.001,z-0.001,z+0.001);
                /*for(int l=0; l<lp.size(); l++) {
                    std::cout << lp[l].getBx() << " " << lp[l].getBy() << " " << lp[l].getBz() << " " << lp[l].getDensity() << std::endl;
                }*/
            }
        }
    }
    std::cout << "TEST SUCCEEDED... end of getLocalPropertiesRegion() test" << std::endl;
}

void testGetLocalProperties(saga::ref_ptr<saga::AMRgrid> amr, int nRegions)
{
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TESTING.......... getLocalProperties()" << std::endl;
    for(int i=1; i<nRegions; i++) {
        for(int j=1; j<nRegions; j++) {
            for(int k=1; k<nRegions; k++) {
                double x = ((double)i ) / nRegions;
                double y = ((double)j ) / nRegions;
                double z = ((double)k ) / nRegions;
                //std::cout << x << " " << y << " "<< z << std::endl;
                saga::LocalProperties lp = amr->getLocalProperties(x,y,z);
                //std::cout << lp.getBx() << " " << lp.getBy() << " " << lp.getBz() << " " << lp.getDensity() << std::endl;
            }
        }
    }
    std::cout << "TEST SUCCEEDED... end of getLocalProperties() test" << std::endl;
}

void testGetLocalPropertiesFromIndex(saga::ref_ptr<saga::AMRgrid> amr)
{
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "TESTING.......... getLocalPropertiesFromIndex()" << std::endl;
    for(int i=1; i<100; i++) {
        saga::LocalProperties lp = amr->getLocalPropertiesFromIndex(i);
        //std::cout << lp.getBx() << " " << lp.getBy() << " " << lp.getBz() << " " << lp.getDensity() << std::endl;
    }
    std::cout << "TEST SUCCEEDED... end of getLocalPropertiesFromIndex() test" << std::endl;
}

int main(int argc, char** argv )
{

    if( argc != 2 )
    {
        std::cout << "Running tests." << std::endl;
        std::cout << "USAGE" << std::endl;
        std::cout << "  ./" << argv[0] << " <pathToSQLfile>" <<  std::endl;
        return -1;
    }
    std::string filename = argv[1];

    // position of interest in Mpc
    double Mpc=3.08567758e22;
    double x=13.*Mpc;
    double y=23.*Mpc;
    double z=17.*Mpc;
    x/=convLength;
    y/=convLength;
    z/=convLength;

    const int nRegions = 3;

    saga::ref_ptr<saga::AMRgrid> amr = new saga::AMRgrid(filename, 18);

    testGetCellsRegion(amr, nRegions);
    testGetNearestNeighbors(amr,nRegions);
    testSelectNearestNeighbor(amr, nRegions);
    testGetLocalPropertiesRegion(amr, nRegions);
    testGetLocalProperties(amr, nRegions);
    testGetLocalPropertiesFromIndex(amr);

    
    return 0;

}