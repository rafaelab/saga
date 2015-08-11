#include "saga/AMRgrid.h"


namespace saga{

/*********************************************************************************************************/ 
// Constructor
AMRgrid::AMRgrid(std::string filename, int nLevels)
{
    saga::SQLiteDB *DB = new saga::SQLiteDB();
    DB->open(filename);
    setMaxRefinementLevel(nLevels);
    setMinCellSize(nLevels);
}

/*********************************************************************************************************/ 
// Destructor
AMRgrid::~AMRgrid()
{
    DB->close();
}

/*********************************************************************************************************/ 
// Returns the maximum number of refinement levels
//
int AMRgrid::getMaxRefinementLevel()
{
    return refinementLevel;
}

/*********************************************************************************************************/ 
// Sets the maximum number of refinement levels
// Input:
//   nLevels: maximum level of refinement of the grid
//
void AMRgrid::setMaxRefinementLevel(int nLevels)
{
    refinementLevel = nLevels;
}

/*********************************************************************************************************/ 
// Get minimum cell size
//
double AMRgrid::getMinCellSize()
{
    return minCellSize;
}

/*********************************************************************************************************/ 
// Set minimum cell size
//
void AMRgrid::setMinCellSize(int nLevels)
{
    minCellSize = 1 / pow(2, nLevels);
}


/*********************************************************************************************************/ 
// Given a range in space returns a vector with the indexes of the closest cells.
// Input:
//   (xmin,ymin,zmin): minimum point coordinates in grid units
//   (xmax,ymax,zmax): maximum point coordinates in grid units
// Output:
//   idx: index of the nearest cell
//
std::vector<AMRcell> AMRgrid::getCellsRegion(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
    char query[512];
    sprintf(query, "SELECT * FROM Cell_tree WHERE maxX >= %lf AND minX <= %lf AND maxY >= %lf AND minY <= %lf AND maxZ >= %lf AND minZ <= %lf;", xmin, xmax, ymin, ymax, zmin, zmax);

    std::vector<std::vector<std::string> > queryResults = DB->query(query);
    std::vector<AMRcell> cells;

    for (int i=0; i<queryResults.size(); i++){
        AMRcell *cell = new AMRcell(atoi(queryResults[i][0].c_str()), atof(queryResults[i][1].c_str()), atof(queryResults[i][2].c_str()), atof(queryResults[i][3].c_str()), atof(queryResults[i][4].c_str()), atof(queryResults[i][5].c_str()), atof(queryResults[i][6].c_str()));
        cells.push_back(*cell);
    }
    return cells;
}

/*********************************************************************************************************/ 
// Given a point with coordinates (x,y,z), returns the index of the nearest neighbors.
// Input:
//   (x,y,z): point coordinates in grid units
// Output:
//   indexes: indexes of the 8 (or less) nearest neighbors.
//
std::vector<AMRcell>  AMRgrid::getNearestNeighbors(double x, double y, double z)
{
    double prefactor = 0.5;
    double xmin = x - prefactor * minCellSize;
    double xmax = x + prefactor * minCellSize;
    double ymin = y - prefactor * minCellSize;
    double ymax = y + prefactor * minCellSize;
    double zmin = z - prefactor * minCellSize;
    double zmax = z + prefactor * minCellSize;

   return getCellsRegion(xmin, xmax, ymin, ymax, zmin, zmax);
}

/*********************************************************************************************************/ 
// Given a point with coordinates (x,y,z), returns the index of the nearest neighbor.
// Input:
//   (x,y,z): point coordinates in grid units
// Output:
//   index: index nearest neighbor.
//
AMRcell AMRgrid::selectNearestNeighbor(double x, double y, double z)
{
    std::vector<AMRcell> cells = getNearestNeighbors(x, y, z);
    double d = 1.;
    int idx;
    for (int i=0; i<cells.size(); i++) {
        double dc = cells[i].distanceToPoint(x, y, z);
        if(d >= dc) {
            d = dc;
            idx = i;
        }
    }       
    return cells[idx];
}


/*********************************************************************************************************/ 
// Given a point with index idx, returns the cell with its properties .
// Input:
//   idx: index
// Output:
//   cell: cell with information.
//
AMRcell AMRgrid::getCellWithIndex(int idx)
{
    char query[512];
    sprintf(query,"SELECT * FROM Cell_tree WHERE id = %i LIMIT 1;", idx);
    std::vector<std::vector<std::string> > queryResults = DB->query(query);
    double xmin = atof(queryResults[0][1].c_str());
    double xmax = atof(queryResults[0][2].c_str());
    double ymin = atof(queryResults[0][3].c_str());
    double ymax = atof(queryResults[0][4].c_str());
    double zmin = atof(queryResults[0][5].c_str());
    double zmax = atof(queryResults[0][6].c_str());
    
    return AMRcell(idx, xmin, xmax, ymin, ymax, zmin, zmax);
}

/*********************************************************************************************************/ 
// Given a range in space returns a vector with the local properties for the points.
// Input:
//   (xmin,ymin,zmin): minimum point coordinates in grid units
//   (xmax,ymax,zmax): maximum point coordinates in grid units
// Output:
//    LocalProperties: density, (Bx,By,Bz) [magnetic field] in simulation units.
// 
// Unit conversion is done offline
//
std::vector<LocalProperties> AMRgrid::getLocalPropertiesRegion(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
    std::vector<LocalProperties> LP;
    std::vector<AMRcell> cells = getCellsRegion(xmin, xmax, ymin, ymax, zmin, zmax);
    for (int i=0; i<cells.size(); i++) {
        int index = cells[i].getCellIndex();
        char query[512] = "";
        sprintf(query, "SELECT * FROM Cell WHERE rowid = %i;", index);
        std::vector<std::vector<std::string> > queryResults = DB->query(query);
        LocalProperties *lp = new LocalProperties(atof(queryResults[0][0].c_str()), atof(queryResults[0][1].c_str()), atof(queryResults[0][2].c_str()), atof(queryResults[0][3].c_str()));
        LP.push_back(*lp);
        delete lp;
    }

	return LP;  
}

/*********************************************************************************************************/
// Given a point with coordinates (x,y,z), returns the local properties for this point.
// Input:
//   (x,y,z): point coordinates in grid units
// Output:
//    LocalProperties: density, (Bx,By,Bz) [magnetic field] in simulation units.
// 
// Unit conversion is done offline
//
LocalProperties AMRgrid::getLocalProperties(double x, double y, double z)
{

    char query[512];
    AMRcell cell = selectNearestNeighbor(x, y, z);
    //std::cout << "cell index: " << cell.getCellIndex() << std::endl;
    sprintf(query, "SELECT * FROM Cell WHERE rowid = %i;", cell.getCellIndex());
    std::vector<std::vector<std::string> > queryResults = DB->query(query);

    return LocalProperties(atof(queryResults[0][0].c_str()), atof(queryResults[0][1].c_str()), atof(queryResults[0][2].c_str()), atof(queryResults[0][3].c_str()));

}

/*********************************************************************************************************/
// Given a point with coordinates (x,y,z), returns the density at this position
// Input:
//   (x,y,z): point coordinates in grid units
// Output:
//    rho: density in simulation units.
// 
// Unit conversion is done offline
//
LocalProperties AMRgrid::getLocalPropertiesFromIndex(int index)
{

    char query[512];
    sprintf(query,"SELECT * FROM Cell WHERE rowid = %i;", index);
    std::vector<std::vector<std::string> > queryResults = DB->query(query);

    return LocalProperties(atof(queryResults[0][0].c_str()), atof(queryResults[0][1].c_str()), atof(queryResults[0][2].c_str()), atof(queryResults[0][3].c_str()));

}

/*********************************************************************************************************/
// Given a point with coordinates (x,y,z), returns the density at this position
// Input:
//   (x,y,z): point coordinates in grid units
// Output:
//    rho: density in simulation units.
// 
// Unit conversion is done offline
//
double AMRgrid::getDensity(double x, double y, double z)
{

    char query[512];
    AMRcell cell = selectNearestNeighbor(x, y, z);
    sprintf(query,"SELECT * FROM Cell WHERE rowid = %i;", cell.getCellIndex());
    std::vector<std::vector<std::string> > queryResults = DB->query(query);

    return atof(queryResults[0][1].c_str());

}

/*********************************************************************************************************/
// Given a point with coordinates (x,y,z), returns the density at this position
// Input:
//   (x,y,z): point coordinates in grid units
// Output:
//    rho: density in simulation units.
// 
// Unit conversion is done offline
//
std::vector<double> AMRgrid::getMagneticField(double x, double y, double z)
{
    char query[512];
    AMRcell cell = selectNearestNeighbor(x, y, z);
    sprintf(query,"SELECT * FROM Cell WHERE rowid = %i;", cell.getCellIndex());
    std::vector<std::vector<std::string> > queryResults = DB->query(query);

    std::vector<double> b;
    b.push_back(atof(queryResults[0][2].c_str()));
    b.push_back(atof(queryResults[0][3].c_str()));
    b.push_back(atof(queryResults[0][4].c_str()));

    return b;
}


/*********************************************************************************************************/ 
// Get size of the table
// Input:
// Output:
//   size: number of entries
//
int AMRgrid::getGridSize()
{

    int size=-1;
    char query[512];
    sprintf(query,"SELECT COUNT(*) FROM Cell_tree_rowid;");
    std::vector<std::vector<std::string> > queryResults = DB->query(query);

    return atoi(queryResults[0][0].c_str());
}


/*********************************************************************************************************/ 
// Closes the AMRgrid. Equivalent to closing the SQL file.
void AMRgrid::close()
{
    DB->close();
}

} // namespace