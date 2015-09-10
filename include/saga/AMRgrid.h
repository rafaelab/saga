#ifndef SAGA_AMRGRID_H
#define SAGA_AMRGRID_H


#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <stdexcept>

#include "sqlite3/sqlite3.h"
#include "saga/AMRcell.h"
#include "saga/LocalProperties.h"
#include "saga/SQLiteInterface.h"
#include "saga/Referenced.h"


namespace saga {

class AMRgrid : public Referenced
{
public:
    AMRgrid(std::string filename, int nRefinement);
    virtual ~AMRgrid();
	
    void setMaxRefinementLevel(int nLevels);
    int getMaxRefinementLevel();
    void setMinCellSize(int nLevels);
    double getMinCellSize();

    std::vector<AMRcell> getCellsRegion(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
    std::vector<AMRcell> getNearestNeighbors(double x, double y, double z);
    AMRcell selectNearestNeighbor(double x, double y, double z);
    AMRcell getCellWithIndex(int idx);
    std::vector<LocalProperties> getLocalPropertiesRegion(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
    LocalProperties getLocalProperties(double x, double y, double z);
    LocalProperties getLocalPropertiesFromIndex(int idx);
    double getDensity(double x, double y, double z);
    std::vector<double> getMagneticField(double x, double y, double z);

    int getGridSize();
    void close();
	
private:
    SQLiteDB *DB;
    int refinementLevel;
    double minCellSize;

};




} // namespace


#endif