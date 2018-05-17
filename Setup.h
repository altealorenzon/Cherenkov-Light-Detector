#ifndef Setup_h
#define Setup_h

#include "Vector.h"
#include <string>
#include <random>

class Setup {

public:
    Setup( std::string& type );
    Vector* generateInitialPoint();
    double* generateInitialAngle();
    bool checkPosition( Vector* x );
    
private:
    std::string& type_of_detector;
    double r;        //cm lato di base o raggio
    double h;        //cm altezza
    double d;        //cm distanza degli scintillatori
    Vector* initialPoint;  //coordinates of initial point
    double angle[2];       //rad angoli di generazione
    //std::random_device rd;
    std::mt19937 gen;
    //std::uniform_real_distribution<std::mt19937::result_type> dist(0, 1);
    
};

#endif
