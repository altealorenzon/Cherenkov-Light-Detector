#ifndef Setup_h
#define Setup_h

#include "Vector.h"
#include <string>
#include <random>

class Setup {

public:
    Setup( std::string type, std::string ref );
    Vector* generateInitialPoint();
    double* generateInitialAngle();
    std::string getTypeOfDetector();
    bool    checkPosition( Vector* x );
    double  getRadius();
    double  getHeight();
    double  getRefractionIndex();
    double  getCriticalAngle();
    double  getPMdistance();
    double  ReflectionThreshold();
    
private:
    std::string type_of_detector;
    std::string reflection;
    double n;              //refraction index
    double r;              //cm radius 
    double h;              //cm height
    double d;              //cm distance from trigger scintillators
    double PMdistance;     //cm distance of PM plane from radiator
    Vector* initialPoint;  //coordinates of initial point
    double angle[2];       //rad initial angles
    //std::mt19937 gen;
    std::default_random_engine GEN;
    
};

#endif
