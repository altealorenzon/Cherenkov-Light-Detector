#include "Photon.h"
#include <iostream>

Photon::Photon( Vector* x_0, double e, double theta_0, double phi_0, int anti ) : Particle( 22, x_0, e, theta_0, phi_0 ) {
    
}

void Photon::rotateProjections( double theta_1, double phi_1 ) {
    
    if (p_id == 22) { //not necessary
        
        long double temp_x;
        long double temp_y;
        long double temp_z;
        
        temp_x = step_length*sin(theta)*cos(phi);
        temp_y = step_length*sin(theta)*sin(phi);
        temp_z = step_length*cos(theta);
        
        //Update the private variables: component of the shift in the global frame
        //This procedure could introduce an aproximation error.
        proj_x = temp_x*cos(theta_1)*cos(phi_1) - temp_y*cos(theta_1)*sin(phi_1) + temp_z*sin(theta_1)*cos(phi_1);
        proj_y = temp_x*cos(theta_1)*sin(phi_1) + temp_y*cos(theta_1)*cos(phi_1) + temp_z*sin(theta_1)*sin(phi_1);
        proj_z = -temp_x*sin(theta_1) - temp_y*sin(theta_1) + temp_z*cos(theta_1);
        
        norm_proj = sqrt((proj_x*proj_x + proj_y*proj_y + proj_z*proj_z));
        
        if(VERBOSE) {
            std::cout << "-> The original step length was: " << step_length << std::endl;
            std::cout << "sum squares " << (proj_x*proj_x + proj_y*proj_y + proj_z*proj_z) << std::endl;
            std::cout << "difference " << abs((proj_x*proj_x + proj_y*proj_y + proj_z*proj_z)-norm_proj*norm_proj) << std::endl;
            std::cout << "squared:    " << norm_proj*norm_proj << std::endl;
            std::cout << "-> Now the step length is      : " << norm_proj << std::endl;
        }
    }
                                          
    
}

void Photon::updatePositionPh( double theta_1, double phi_1, Setup* setup ) {
    this->nPos++;
    //Shift the photon position of one step length and check whether the photon is inside or outside the box.
    x->shift(proj_x, proj_y, proj_z); //these are the components of the shift in the global frame
    
    if ( setup->checkPosition(x) == true ) {
        
        position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
        
        if(VERBOSE) {
            std::cout << "Is it still inside? "<< setup->checkPosition(x) << std::endl;
            std::cout << "-> New photon position  : (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ")" << std::endl;
            std::cout << "-> New shift projections: (" << proj_x << ", " << proj_y<< ", " << proj_z << ")" << std::endl;
            std::cout << "-> Norm projection      : " << norm_proj << std::endl;   
        }
        
    } else if ( setup->checkPosition(x) == false ) {

        double ran = -1;
        double reflection_angle = 0;
        
        if(setup->getTypeOfDetector() == "c" ) {
            std::uniform_real_distribution<double> dist(0, 1);
            ran = dist(gen); //generate random number for absorption/reflection on lateral walls
            reflection_angle = getReflectionAngle(setup->getRadius());
        
            if(VERBOSE) {
                std::cout << "Is it still inside? "<< setup->checkPosition(x) << std::endl;
                std::cout << "-> Photon position  : (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ")" << std::endl;
                std::cout << "-> sin(theta)*n =" << setup->getRefractionIndex()*sin( acos( proj_z/norm_proj) ) << std::endl;
                std::cout << "-> The random number is: " << ran << std::endl;
            }
        }
        //REFLECTION ON TOP/BOTTOM
        if( ( x->getZ() >= setup->getHeight() || x->getZ() <= 0.0 ) && 
            setup->getRefractionIndex()*sin( acos( proj_z/norm_proj ) ) >= 1 ) { 
            
            nReflections += 1;
            
            proj_z = -1.0*proj_z; //update only the z direction
        
            //x->shift(proj_x/2, proj_y/2, proj_z/2);
            x->shift(proj_x, proj_y, proj_z);
            position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
        
            if(VERBOSE) {
                    std::cout << "+++++++++++++Reflection on the bottom/top wall!+++++++++++++" << std::endl;
                    std::cout << "-> New photon position  : (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ")" << std::endl;
                    std::cout << "-> New shift projection : (" << proj_x << ", " << proj_y << ", " << proj_z << ")" << std::endl;
                    std::cout << "-> Norm projection      : " << norm_proj << std::endl;
                    std::cout << "-> Original step length : " << step_length << std::endl;
            }
        //TOTAL REFLECTION ON LATERAL WALLS       
        } else if ( setup->getTypeOfDetector() == "c" && 
                      reflection_angle >= setup->getCriticalAngle() && ran <= 0.5 ) {
            
            nReflections += 1;
            //Remove the previous update in order to perform the reflection
            x->shift(-proj_x, -proj_y, -proj_z);
            //Do reflection
            reflectionPhWall();
            //Update the position of the photon
            position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
            
            if(VERBOSE) {
                std::cout << "-> The photons was reflected!" << std::endl;
                std::cout << "-> New photon position  : (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ")" << std::endl;
                std::cout << "-> New shift projections: (" << proj_x << ", " << proj_y<< ", " << proj_z << ")" << std::endl;
            }
            
        } else if ( setup->getTypeOfDetector() == "c" &&  ran > setup->ReflectionThreshold() ) { 

            nReflections += 1;
            //Remove the previous update in order to perform the reflection
            x->shift(-proj_x, -proj_y, -proj_z);
            //Do reflection
            reflectionPhWall();
            //Update the position of the photon
            position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
            
            if(VERBOSE) {
                std::cout << "-> The photons was reflected!" << std::endl;
                std::cout << "-> New photon position  : (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ")" << std::endl;
                std::cout << "-> New shift projections: (" << proj_x << ", " << proj_y<< ", " << proj_z << ")" << std::endl;
            }
        
        } else { //reflection false
            
            position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
            //Determine the angles of the photon at the exit (useful to plot at the angular distribution);
            theta_ph_out = acos( proj_z/norm_proj ); 
            if( proj_y >=0 ) {
                phi_ph_out = acos( proj_x/(sqrt(proj_x*proj_x+proj_y*proj_y)) );
            } else {
                phi_ph_out = - acos( proj_x/(sqrt(proj_x*proj_x+proj_y*proj_y)) );
            }
            //Check whether the photon exited from the bottom wall or not. 
            //Important to count the number of photons reaching the detector.
            if ( x->getZ() >= setup->getHeight() ) {
                position_out = 1;
            } else if (x->getZ() <= 0.0 ) {
                position_out = -1; 
            } else {
                position_out = 0;
            }
            
            if(VERBOSE) {
                std::cout << "-> No reflection, the photon is out of the box!"<< std::endl;
                std::cout << "-> Position out: " << position_out << std::endl;
                printSummary();
            }
        }
    }
}

double Photon::getReflectionAngle( double r ) { //input: the radius of the cylinder taken from setup
    
    double x0 = x->getX();
    double y0 = x->getY();
    double z0 = x->getZ();
    
    //I take this point as an approximation of the intersection point.
    double x1 = x0 + proj_x/2;
    double y1 = y0 + proj_y/2;
    double z1 = z0 + proj_z/2;
    
    if(VERBOSE) {
        std::cout << "-> Step length: " << step_length << std::endl;
        std::cout << "Difference with the radius" << sqrt(x1*x1+y1*y1) - r << std::endl;
    }
    
    //The three components of the versor u of the shift in order: 
    //necessary to computer the reflection angles.
    double vers_shift_x = proj_x/norm_proj; //I could use also step length
    double vers_shift_y = proj_y/norm_proj;
    double vers_shift_z = proj_z/norm_proj;

    
    //Compute the angle between the normal to the plane and the versor of the shift as the internal product between them. The normal to the plan is always the radius. Compute the versor of the radius. The coordinates of the radius are those of the intersection point.
    
    vers_r_x = x1/r;//this is cos of the angle between the radius and the x-axes
    vers_r_y = y1/r;//this is cos of the angle between the radius and the y-axes
    vers_r_z = 0;
    
    
    cos_theta_0 = (vers_r_x*vers_shift_x) + (vers_r_y*vers_shift_y);//Derived by means of tringonometry
    cos_phi_0   = sqrt(1 - cos_theta_0*cos_theta_0);
    
    return acos(cos_theta_0);
}


void Photon::reflectionPhWall() {

        //Find the component of the shift w.r. to the plan and it's normal vector.
        double shift_normal = step_length* cos_theta_0;
        double shift_plan   = step_length * cos_phi_0 ; //è la y
        
        
        //Find the new component of the shift versor after the reflection (w.r. to the g.f.)
        proj_x = shift_normal*(-1.0*vers_r_x) - shift_plan*vers_r_y;
        proj_y  = -1.0*shift_normal*vers_r_y + shift_plan*vers_r_x;
        
        norm_proj =  sqrt((proj_x*proj_x + proj_y*proj_y + proj_z*proj_z));
        
        if(VERBOSE) {
            std::cout << "+++++++++++++Reflection on the lateral wall!++++++++++++++++++" << std::endl;
            std::cout << "-> New shift projections: (" << proj_x << ", " << proj_y << ", " << proj_z << ")" << std::endl;
            std::cout << "-> Norm projection      : " << norm_proj << std::endl;
            std::cout << "-> Original step length : " << step_length << std::endl;
        }
}

void Photon::printSummary() {
    
    std::cout << "**********INFORMATION************" << std::endl;
    std::cout << "-> Theta_ph_out " << theta_ph_out << std::endl;
    std::cout << "-> Phi_ph_out "   << phi_ph_out   << std::endl;
    std::cout << "-> New photon position: (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ")" << std::endl;
    std::cout << "-> Number of reflections: " << nReflections << std::endl;
    std::cout << "*********************************" << std::endl;

}


int Photon::getPosition_out() {
    return position_out;
}

double Photon::getThetaOut_ph() {
    return theta_ph_out;
}

double Photon::getPhiOut_ph() {
    return phi_ph_out;
}

int Photon::getnReflections() {
    return nReflections;
}

