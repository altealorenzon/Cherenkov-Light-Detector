#ifndef Setup_h
#define Setup_h

#include "Vector.h"
#include <string>
#include <random>

class Setup {

public:
    Setup( std::string type );
    Vector* generateInitialPoint();
    double* generateInitialAngle();
    bool    checkPosition( Vector* x );
    double  getRadius();
    double  getRefractionIndex();
    
private:
    std::string type_of_detector;
    double n;              //refraction index
    double r;              //cm radius 
    double h;              //cm height
    double d;              //cm distance from trigger scintillators
    Vector* initialPoint;  //coordinates of initial point
    double angle[2];       //rad initial angles
    std::mt19937 gen;
    
};

#endif
