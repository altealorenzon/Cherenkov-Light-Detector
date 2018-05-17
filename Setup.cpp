#include <cmath>
#include "Setup.h"

Setup::Setup( std::string& type ): type_of_detector( type ) {
    
    d = 0; //TODO
    if( type_of_detector == "cylinder" ) {
        r = 2.5; //cm raggio
        h = 8.0; //cm altezza
    }
    else if ( type_of_detector == "parallelepiped" ) {
        r = 0; //TODO cm lato del quadrato di base
        h = 0; //TODO cm altezza
    }
    std::random_device rdv;
    gen.seed(rdv());
}

Vector* Setup::generateInitialPoint() {
    
    std::uniform_real_distribution<double> dist(0, 1);
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
    angle[0] = 2*M_PI*dist(gen); //angle on x,y plane
    
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
    
    angle[1] = max_angle*dist( gen );
    
    return angle;
}
    
bool Setup::checkPosition( Vector* x ) {
    
    if( type_of_detector == "cylinder" ) {
        return ( sqrt( x->getX()*x->getX() + x->getY()*x->getY() ) == r && x->getZ() < h );
    }
    else if( type_of_detector == "parallelepiped" ) {
        
    }
    
}