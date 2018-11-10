#include <iostream>
#include <cmath>
#include "Setup.h"

Setup::Setup( std::string type ): type_of_detector( type ) {
    
    //Here you can set your detector's parameters   
    n = 1.1; //refraction index
    d = 0;   //cm distance of the trigger scintilators
    if( type_of_detector == "cylinder" ) {
        r = 2.5; //cm radius
        h = 8.0; //cm height
    }
    else if ( type_of_detector == "parallelepiped" ) {
        r = 0; //cm square basis dimension
        h = 0; //cm height
    }
    
    std::cout << "* Type of detector: " << type_of_detector << std::endl;
    std::cout << "* Dimensions of the detector: \n*   r = " << r << "\n*   h = " << h << std::endl;

}

Vector* Setup::generateInitialPoint() {
    
    std::uniform_real_distribution<double> dist(0,1);
    double x_0, y_0;
    double sign_x_0, sign_y_0;
    
    sign_x_0 = dist( gen );
    sign_y_0 = dist( gen );
    
    if( sign_x_0 >= 0.5 && sign_x_0 < 1) {
        x_0 =  r*dist( gen );
    } 
    else {
        x_0 = -r*dist( gen );
    }
    
    if( type_of_detector == "cylinder" ) {
        if( sign_y_0 >= 0.5 && sign_y_0 < 1) {
            y_0 =  sqrt( 1 - x_0*x_0/r/r )*dist( gen );
        }
        else {
            y_0 = -sqrt( 1 - x_0*x_0/r/r )*dist( gen );
        }
    }
    else if( type_of_detector == "parallelepiped" ) {
        if( sign_y_0 >= 0.5 && sign_y_0 < 1) {
            y_0 =  r*dist( gen );
        } 
        else {
            y_0 = -r*dist( gen );
        }
    }
        
    initialPoint = new Vector( x_0, y_0, 0 );
    return initialPoint;
    
}

double* Setup::generateInitialAngle() {
    
    std::uniform_real_distribution<double> dist(0, 1);
    angle[1] = 2*M_PI*dist(gen); //angle on x,y plane
    
    double max_angle;           //max azimuthal angle
    if( type_of_detector == "cylinder" ) {
        max_angle = atan2( r, d+h/2 );
    }
    else if( type_of_detector == "parallelepiped" ) {
        if( ( angle[0]>M_PI/4 && angle[0]<3*M_PI/4 ) || ( angle[0]>5*M_PI/4 && angle[0]<7*M_PI/4) ) {
            max_angle = atan2( r/2/sin( angle[0] ), d+h/2 );
        } 
        else {
            max_angle = atan2( r/2/cos( angle[0] ), d+h/2 );
        }
    }
    
    angle[0] = max_angle*dist( gen );
    
    return angle;
}
    
bool Setup::checkPosition( Vector* x ) {
    
    double xpos = x->getX(), ypos = x->getY(), zpos = x->getZ();
    if( type_of_detector == "cylinder" ) {
        return ( sqrt( xpos*xpos + ypos*ypos ) < r && zpos < h && zpos >= 0.0); //to verify whether to use <= or < (>= or >);
    }
    else if( type_of_detector == "parallelepiped" ) {
        return ( abs( xpos ) <= r/2 && abs( ypos ) <= r/2 && zpos <= h );
    }
    
}

double Setup::getRefractionIndex() {
    return n;
}

double Setup::getRadius() {
    return r;
    
}
