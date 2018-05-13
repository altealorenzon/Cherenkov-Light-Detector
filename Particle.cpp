#include "Particle.h"
#include <cmath>

Particle::Particle( int id, Vector* x_0, double e, double theta_0, double phi_0 ) : p_id( id ), x( x_0 ), energy( e ), theta( theta_0 ), phi( phi_0 ) {
    position->push_back( x_0 );
    // muon
    if( id == 13 ) {
        mass   = 105; // MeV/c^2
        charge =  -1;
        p      = sqrt( energy*energy - mass*mass );
        step   =    ;
    } 
    // photon
    else if( id == 22 ) {
        mass   = 0;
        charge = 0;
        p      = sqrt( energy*energy - mass*mass );
        step   = 0;
    }
}

int Particle::getID() {
    return id;
}
    
double Particle::getMass() {
    return mass;
}

double Particle::getCharge() {
    return charge;
}

double Particle::getEnergy() {
    return energy;
}

double Particle::getP() {
    return p;
}
    
void Particle::updatePosition() {
    x->shift( step*sin(theta)*cos(phi), step*sin(theta)*sin(phi), step*cos(theta) );
    position->push_back( x );
}

Vector* Particle::getLastPosition() {
    return x;
}

vector<Vector*>* Particle::getPositionList() {
    return position;
}
