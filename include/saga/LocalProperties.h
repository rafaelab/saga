#ifndef SAGA_LOCALPROPERTIES_H
#define SAGA_LOCALPROPERTIES_H


namespace saga{

class LocalProperties 
{
private:
    double LocalDensity;
    double LocalMagneticFieldX;
    double LocalMagneticFieldY;
    double LocalMagneticFieldZ;

public:

    LocalProperties(double rho, double Bx, double By, double Bz);
    LocalProperties();
    ~LocalProperties();

    void setDensity(double rho);
    void setBx(double Bx);
    void setBy(double By);
    void setBz(double Bz);

    double getDensity();
    double getBx();
    double getBy();
    double getBz(); 
    double getBtot();      

    void convertUnits(double convLength, double convDensity, double convMagneticField);

};

} // namespace


#endif