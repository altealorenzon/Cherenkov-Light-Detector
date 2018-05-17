#include "Particle.h"
#include <cmath>

std::vector<particles_data*> Particle::my_particles = std::vector<particles_data*>();

Particle::Particle( int id, Vector* x_0, double e, double theta_0, double phi_0 ) : p_id( id ), x( x_0 ), energy( e ), theta( theta_0 ), phi( phi_0 ) {
    position->push_back( x_0 );
    
    particles_data data = *Particle::my_particles[std::abs(p_id)];
//     if(data) {
        mass = data.m;
        charge = (p_id > 0) ? data.q : -data.q;
        step_length = data.step;
        p      = sqrt( energy*energy - mass*mass );
        v      = (mass == 0) ? 1/2 : p/energy; //TODO set massless particle speed to 1/n
//     } else {
//         std::cerr << "ERROR: No particle with ID = " << p_id << std::endl;
//     }
}

int Particle::getID() {
    return p_id;
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
    x->shift( step_length*sin(theta)*cos(phi), step_length*sin(theta)*sin(phi), step_length*cos(theta) );
    position->push_back( x );
}

Vector* Particle::getLastPosition() {
    return position->back();
}

std::vector<Vector*>* Particle::getPositionList() {
    return position;
}

void Particle::setParticlesData() {
        Particle::my_particles[13] = new particles_data( 105, -1, 0.5 );
        Particle::my_particles[22] = new particles_data( 0, 0, 0 ); //TODO Set correct values for photons
};
