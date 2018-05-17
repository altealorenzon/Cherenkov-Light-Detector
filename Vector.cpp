#include "Vector.h"

Vector::Vector( double x_0, double y_0, double z_0 ) :
x( x_0 ), y( y_0 ), z( z_0 ) {
    
}

double Vector::getX() {
    return x;
}

double Vector::getY() {
    return y;
}

double Vector::getZ() {
    return z;
}

void Vector::shift( double x_1, double y_1, double z_1 ) {
    x += x_1;
    y += y_1;
    z += z_1;
}

void Vector::rotate( double theta, double phi ) {
    
}
