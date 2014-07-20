#ifndef SAGA_AMRCELL_H
#define SAGA_AMRCELL_H

#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>


namespace saga{


class AMRcell
{
public:
    AMRcell(int cellIdx, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
    AMRcell(double size, double Xcenter, double Ycenter, double Zcenter, int cellIdx);
    ~AMRcell();

    void setCellIndex(int idx);
    void setXmin(double xmin);
    void setXmax(double xmax);
    void setYmin(double ymin);
    void setYmax(double ymax);
    void setZmin(double zmin);
    void setZmax(double zmax);
    void setCellSize(double size);
    void setCellCenter(double x, double y, double z);
    void setXcenter(double x);
    void setYcenter(double y);
    void setZcenter(double z);
    int getCellIndex();
    double getXmin();
    double getXmax();
    double getYmin();
    double getYmax();
    double getZmin();
    double getZmax();
    double getXcenter();
    double getYcenter();
    double getZcenter();
    double distanceToPoint(double x, double y, double z);

private:
    double Xmin;
    double Xmax;
    double Ymin;
    double Ymax;
    double Zmin;
    double Zmax;
    int CellIndex;
    double Xcenter;
    double Ycenter;
    double Zcenter;
    double CellSize;

};

}// namespace


#endif
