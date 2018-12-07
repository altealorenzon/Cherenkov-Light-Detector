#include "Particle.h"
#include "Photon.h"
#include <cmath>
#include <iostream>
#include <time.h>

particles_data Particle::my_particles[30];

Particle::Particle( int id, Vector* x_0, double e, double theta_0, double phi_0 ) :
p_id( id ), x( x_0 ), energy( e ), theta( theta_0 ), phi( phi_0 ), nPos(0) {
    
    position = new std::vector<Vector*>();
    position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
    
    particles_data data = Particle::my_particles[std::abs(p_id)];
    
    mass        = data.m;
    charge      = (p_id > 0) ? data.q : -data.q;
    step_length = data.step;
    p           = sqrt( energy*energy - mass*mass );
    v           = (mass == 0) ? 1.0/2 : p/energy; //TODO set massless particle speed to 1/n
    
    if(VERBOSE){
        std::cout << std::endl;
        std::cout << "*** NEW PARTICLE ***" << std::endl;
        std::cout << "Created a new particle! " << std::endl;
        std::cout << "  -> id       : " << p_id << std::endl;
        std::cout << "  -> charge   : " << charge << std::endl;
        std::cout << "  -> mass     : " << mass << std::endl;
        std::cout << "  -> energy   : " << energy << std::endl;
        std::cout << "  -> momentum : " << p << std::endl;
        std::cout << "  -> speed    : " << v << std::endl;
        std::cout << "  -> theta    : " << theta << std::endl;
        std::cout << "  -> phi      : " << phi << std::endl;
        std::cout << "  -> initial position: (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ") " << std::endl;
    }
    
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

double Particle::getSpeed() {
    return v;
}

void Particle::updatePosition() {
    this->nPos++;
    x->shift( step_length*sin(theta)*cos(phi), step_length*sin(theta)*sin(phi), step_length*cos(theta) );
    position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
    if(VERBOSE) std::cout << "New muon position: (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ") " << std::endl;
}


Vector* Particle::getX() {
    return x;
}

Vector* Particle::getLastPosition() {
    return position->back();
}

std::vector<Vector*>* Particle::getPositionList() {
    return position;
}

void Particle::setParticlesData() {
    for(int i = 0; i < 30; i++) {
        Particle::my_particles[i] = particles_data();
    }
    //Here you can set mass (MeV), charge (e), step_length (cm) 0.006 cm PMMA 1.0 air/co2
    Particle::my_particles[13] = particles_data( 105, -1, 0.006 );
    Particle::my_particles[22] = particles_data( 0, 0, 0.1 );
}

