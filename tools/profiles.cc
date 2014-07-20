#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream> 
#include <cmath>
#include <string>

#include "TROOT.h"
#include "TH1.h"
#include "TFile.h"

#include "TCanvas.h"
#include "TGraph.h"
#include "TMath.h"

#include "TStyle.h"
#include "TColor.h"
#include "TLatex.h"

#include <omp.h> 

#include "saga/AMRgrid.h"
#include "saga/LocalProperties.h"
#include "saga/Referenced.h"
#include "saga/MagneticField.h"



int main(int argc, char* argv[])
{

    // Parsing command line
    if(argc!=4) {
        std::cerr << "Incorrect number of arguments. The correct usage is: \n" 
                  << argv[0] << " <inputfile> <outputfile> <inputsize>" <<   std::endl;
        return 1;
    }
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    int N = atoi(argv[3]);

    std::cout << "input file: " << inputFile << std::endl;
    std::cout << "output file: " << outputFile << std::endl;
    std::cout << "size of file: " << N << std::endl;
    
    // ROOT output file
    TFile *f = new TFile(outputFile.c_str(),"recreate");

        
    // Unit conversion factors
    const double convLength=8.57599e+24; // m
    const double convDensity=2.49651e-27;  // kg/m^3
    const double convMagneticField=1.11754673542e-09; // T
    int nbins = 400;

    // SQLite file
    saga::ref_ptr<saga::AMRgrid> amr = new saga::AMRgrid(inputFile, 10);

    double Bx, By, Bz, rho, Bt;

    TH1D *hBx = new TH1D("hBx","Bx distribution",nbins,-1e-22,1e22);



    for(int i=0; i<N; i++) {                
        saga::LocalProperties *lp = new saga::LocalProperties();
        *lp = amr->getLocalPropertiesFromIndex(i);
        lp->convertUnits(convLength, convDensity, convMagneticField);
        Bx = lp->getBx();
        By = lp->getBy();
        Bz = lp->getBz();
        Bt = lp->getBtot();
        rho = lp->getDensity();
        hBx->Fill(Bx);
        delete lp;
    }

    hBx->Write("hBx");
    f->Close();


    return 0;
}