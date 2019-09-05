#include <iostream>
#include <cmath>
#include <time.h>
#include "Setup.h"

Setup::Setup( std::string type, std::string ref ): type_of_detector( type ), reflection( ref ) {
    
    time_t timer;
    GEN.seed( time(&timer) );
    
    //Here you can set your detector's parameters   
    n = 1.4; //refraction index
    d = 100;   //cm distance of the trigger scintillators
    PMdistance = 0.3; //cm distance of PM plane from radiator
    
    if( type_of_detector == "c" ) {
        r = 1;  //cm radius
        h = 1; //cm height
    }
    else if ( type_of_detector == "p" ) {
        r = 6.0; //cm square basis dimension
        h = 1.0; //cm height
    }
    
    std::cout << "* Type of detector: " << type_of_detector << std::endl;
    std::cout << "* Dimensions of the detector: \n*   r = " << r << "\n*   h = " << h << std::endl;
    std::cout << "* Distance of trigger scintillators: \n*   d = " << d << std::endl;
    std::cout << "* Refraction index of the material: \n*   n = " << n << std::endl;
}

Vector* Setup::generateInitialPoint() {
    
    std::uniform_real_distribution<double> dist(0,1);
    double x_0, y_0;
    double sign_x_0, sign_y_0;
    
    sign_x_0 = dist( GEN );
    sign_y_0 = dist( GEN );
    
    if( type_of_detector == "c" ) {
        if( sign_x_0 >= 0.5 && sign_x_0 < 1) {
            x_0 =  r*dist( GEN );
        } 
        else {
            x_0 = -r*dist( GEN );
        }
        if( sign_y_0 >= 0.5 && sign_y_0 < 1) {
            y_0 =  sqrt( 1 - x_0*x_0/r/r )*dist( GEN );
        }
        else {
            y_0 = -sqrt( 1 - x_0*x_0/r/r )*dist( GEN );
        }
    }
    else if( type_of_detector == "p" ) {
        if( sign_x_0 >= 0.5 && sign_x_0 < 1) {
            x_0 =  r/2*dist( GEN );
        } 
        else {
            x_0 = -r/2*dist( GEN );
        }
        if( sign_y_0 >= 0.5 && sign_y_0 < 1) {
            y_0 =  r/2*dist( GEN );
        } 
        else {
            y_0 = -r/2*dist( GEN );
        }
    }
        
    initialPoint = new Vector( x_0, y_0, 0 );
    return initialPoint;
    
}

double* Setup::generateInitialAngle() {
    
    std::uniform_real_distribution<double> dist(0, 1);
    angle[1] = 2*M_PI*dist(GEN); //angle on x,y plane
    
    double max_angle;           //max azimuthal angle
    if( type_of_detector == "c" ) {
        max_angle = atan2( r, d+h/2 );
    }
    else if( type_of_detector == "p" ) {
        if( ( angle[0]>M_PI/4 && angle[0]<3*M_PI/4 ) || ( angle[0]>5*M_PI/4 && angle[0]<7*M_PI/4) ) {
            max_angle = atan2( r/2/sin( angle[0] ), d+h/2 );
        } 
        else {
            max_angle = atan2( r/2/cos( angle[0] ), d+h/2 );
        }
    }
    
    angle[0] = max_angle*dist( GEN );
    
    return angle;
}
    
bool Setup::checkPosition( Vector* x ) {
    
    double xpos = x->getX(), ypos = x->getY(), zpos = x->getZ();
    if( type_of_detector == "c" ) {
        return ( sqrt( xpos*xpos + ypos*ypos ) < r && zpos < h && zpos >= 0.0); 
    }
    else if( type_of_detector == "p" ) {
        return ( abs( xpos ) <= r/2 && abs( ypos ) <= r/2 && zpos < h && zpos >= 0.0 );
    }
    
}

std::string Setup::getTypeOfDetector() {
    return type_of_detector;
}

double Setup::getRefractionIndex() {
    return n;
}

double Setup::getRadius() {
    return r;
}

double Setup::getHeight() {
    return h;
}

double Setup::getPMdistance() {
    return PMdistance;
}

double Setup::getCriticalAngle() {
    return asin( 1/n );
}

double Setup::ReflectionThreshold() {
    //here you can change the reflection/absorption threshold
    if( reflection == "r" ) return 0.2;
    if( reflection == "a" ) return 0.999;
}
