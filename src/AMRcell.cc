#include "saga/AMRcell.h"


namespace saga{

/*********************************************************************************************************/ 
// Constructors
AMRcell::AMRcell(int cellIdx, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
    setXmin(xmin);
    setYmin(ymin);
    setZmin(zmin);
    setXmax(xmax);
    setYmax(ymax);
    setZmax(zmax);
    setCellIndex(cellIdx);
    setCellCenter(xmin + (xmax - xmin) / 2, ymin + (ymax - ymin) / 2, zmin + (zmax - zmin) / 2);
    setCellSize(xmin - xmax);
}

AMRcell::AMRcell(double size, double xcenter, double ycenter, double zcenter, int cellIdx )
{
    setCellIndex(cellIdx);
    setCellCenter(xcenter, ycenter, zcenter);
    setCellSize(size);
    setXmin(Xcenter - size / 2);
    setYmin(Ycenter - size / 2);
    setZmin(Zcenter - size / 2);
    setXmax(Xcenter + size / 2);
    setYmax(Ycenter + size / 2);
    setZmax(Zcenter + size / 2);
}


/*********************************************************************************************************/ 
// Destructor
AMRcell::~AMRcell()
{
}

/*********************************************************************************************************/ 
// Sets the index of the cell
// Input:
//   idx: index of the cell
//
void AMRcell::setCellIndex(int idx)
{
    CellIndex = idx;
}


/*********************************************************************************************************/ 
// The following functions set xmin, xmax, ymin, ymax, zmin and zmax. 
// These are the coordinates of the border of each cell
// Input:
//   (x,y,z)(min,max): coordinates
//
void AMRcell::setXmin(double xmin)
{
    Xmin = xmin;
}
void AMRcell::setYmin(double ymin)
{
    Ymin = ymin;
}
void AMRcell::setZmin(double zmin)
{
    Zmin = zmin;
}
void AMRcell::setXmax(double xmax)
{
    Xmax = xmax;
}
void AMRcell::setYmax(double ymax)
{
    Ymax = ymax;
}
void AMRcell::setZmax(double zmax)
{
    Zmax = zmax;
}

/*********************************************************************************************************/ 
// Sets the index of the cell
// Input:
//   idx: index of the cell
//
void AMRcell::setCellSize(double size)
{
    CellSize = size;
}

/*********************************************************************************************************/ 
// Sets the position of the center
// Input:
//   (x,y,z): coordinates of the center of the cell
//
void AMRcell::setCellCenter(double x, double y, double z)
{
    Xcenter = x;
    Ycenter = y;
    Zcenter = z;
}

void AMRcell::setXcenter(double x)
{
    Xcenter = x;
}

void AMRcell::setYcenter(double y)
{
    Ycenter = y;
}

void AMRcell::setZcenter(double z)
{
    Zcenter = z;
}


/*********************************************************************************************************/ 
// Returns the index of the cell
// 
int AMRcell::getCellIndex()
{
    return CellIndex;
}

/*********************************************************************************************************/ 
// Returns the (x,y,z)(min,max) coordinates
// 
double AMRcell::getXmin()
{
    return Xmin;
}

double AMRcell::getYmin()
{
    return Ymin;
}

double AMRcell::getZmin()
{
    return Zmin;
}
double AMRcell::getXmax()
{
    return Xmax;
}

double AMRcell::getYmax()
{
    return Ymax;
}

double AMRcell::getZmax()
{
    return Zmax;
}

/*********************************************************************************************************/ 
// Returns the coordinates (x,y,z) of the center of the cell
// 
double AMRcell::getXcenter()
{
    return Xcenter;
}

double AMRcell::getYcenter()
{
    return Ycenter;
}

double AMRcell::getZcenter()
{
    return Zcenter;
}    

/*********************************************************************************************************/ 
// Returns the coordinates (x,y,z) of the center of the cell
// 
double AMRcell::distanceToPoint(double x, double y, double z)
{
    return sqrt(pow(Xcenter - x, 2) + pow(Ycenter - y, 2) + pow(Zcenter - z, 2));
}

}