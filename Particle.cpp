#include "Particle.h"
#include "Photon.h";
#include <cmath>
#include <iostream>

particles_data Particle::my_particles[30];

Particle::Particle( int id, Vector* x_0, double e, double theta_0, double phi_0 ) :
p_id( id ), x( x_0 ), energy( e ), theta( theta_0 ), phi( phi_0 ) {
    
    std::random_device rdv;
    gen.seed( rdv() );
    
    position = new std::vector<Vector*>();
    position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
    
    particles_data data = Particle::my_particles[std::abs(p_id)];
    
    mass        = data.m;
    charge      = (p_id > 0) ? data.q : -data.q;
    step_length = data.step;
    p           = sqrt( energy*energy - mass*mass );
    v           = (mass == 0) ? 1.0/2 : p/energy; //TODO set massless particle speed to 1/n
    
    std::cout << std::endl;
    std::cout << "*************************** NEW PARTICLE *****************************" << std::endl;
    std::cout << "Created a new particle! " << std::endl;
    std::cout << "  -> id       : " << p_id << std::endl;
    std::cout << "  -> charge   : " << charge << std::endl;
    std::cout << "  -> mass     : " << mass << std::endl;
    std::cout << "  -> energy   : " << energy << std::endl;
    std::cout << "  -> momentum : " << p << std::endl;
    std::cout << "  -> speed    : " << v << std::endl;
    std::cout << "  -> initial position           : (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ") " << std::endl;
    std::cout << "  -> initial angle on x,y plane : " << phi << std::endl;
    std::cout << "  -> initial azimuthal angle    : " << theta << std::endl << std::endl;
    
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
    x->shift( step_length*sin(theta)*cos(phi), step_length*sin(theta)*sin(phi), step_length*cos(theta) );
<<<<<<< HEAD
    //     if ( p_id == 13 ) {
    position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
    std::cout << "New muon position: (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ") " << std::endl;
    //     }
}


Vector* Particle::getX() {
    return x;
=======
    position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
    std::cout << "New position: (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ") " << std::endl;
}

void Particle::rotatePosition( double theta_1, double phi_1 ) {
    double temp_x;
    double temp_y;
    double temp_z;
    
    temp_x = step_length*sin(theta)*cos(phi);
    temp_y = step_length*sin(theta)*sin(phi);
    temp_z = step_length*cos(theta);
    
    double proj_x;
    double proj_y;
    double proj_z;
    
    proj_x = temp_x*cos(theta_1)*cos(phi_1) - temp_y*cos(theta_1)*sin(phi_1) + temp_z*sin(theta_1)*cos(phi_1);
    proj_y = temp_x*cos(theta_1)*sin(phi_1) + temp_y*cos(theta_1)*cos(phi_1) + temp_z*sin(theta_1)*sin(phi_1);
    proj_z = -temp_x*sin(theta_1) - temp_y*sin(theta_1) + temp_z*cos(theta_1);
    
    x->shift(proj_x, proj_y, proj_z);
    position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
    std::cout << "New photon position: (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ")" << std::endl;
>>>>>>> 6641db7221ee782fa46adecc3ee30e88669206dd
}

Vector* Particle::getLastPosition() {
    return position->back();
}

std::vector<Vector*>* Particle::getPositionList() {
    return position;
}

void Particle::setParticlesData() {
<<<<<<< HEAD
    for(int i = 0; i < 30; i++) {
        Particle::my_particles[i] = particles_data();
    }
    Particle::my_particles[13] = particles_data( 105, -1, 0.05 );
    Particle::my_particles[22] = particles_data( 0, 0, 0.05 ); //TODO Set correct values for photons
=======
        for(int i = 0; i < 30; i++) {
            Particle::my_particles[i] = particles_data();
        }
        Particle::my_particles[13] = particles_data( 105, -1, 0.05 );
        Particle::my_particles[22] = particles_data( 0, 0, 0.05 ); //TODO Set correct values for photons
>>>>>>> 6641db7221ee782fa46adecc3ee30e88669206dd
}

