#include "Photon.h"
#include <iostream>


Photon::Photon( Vector* x_0, double e, double theta_0, double phi_0, int anti ) : Particle( 22, x_0, e, theta_0, phi_0 ) {
    
}

void Photon::updatePositionPh( double theta_1, double phi_1, Setup* setup ) {
    
    //Shift the photon position of one step length and check whether the photon is inside or outside the box.
    x->shift(proj_x, proj_y, proj_z); //these are the components of the shift in the global frame
    
    if ( setup->checkPosition(x) == true ) {
        std::cout << "Is it still inside? "<< setup->checkPosition(x) << std::endl;
        position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
        std::cout << "-> New photon position  : (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ")" << std::endl;
        std::cout << "-> New shift projections: (" << proj_x << ", " << proj_y<< ", " << proj_z << ")" << std::endl;
        std::cout << "-> Norm projection      : " << norm_proj << std::endl;
        
    } else if ( setup->checkPosition(x) == false ) {
        std::cout << "Is it still inside? "<< setup->checkPosition(x) << std::endl;
        std::uniform_real_distribution<double> dist(0, 1);
        double ran = dist(gen); //generate random number in order to determine whether the photon is reflected or not.
        std::cout << "-> The random number is: " << ran << std::endl;
        
        if ( ran > 0.1) { //reflection true (TODO >=, <=)
            
            std::cout << "-> The photons was reflected!" << std::endl;

            nReflections += 1;
            //Remove the previous update in order to perform the reflection
            x->shift(-proj_x, -proj_y, -proj_z);
            //Do reflection
            reflectionPh(setup->getRadius());
            //Update the position of the photon
            position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
            
            std::cout << "-> New photon position  : (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ")" << std::endl;
            std::cout << "-> New shift projections: (" << proj_x << ", " << proj_y<< ", " << proj_z << ")" << std::endl;
        
        }else { //reflection false
            
            std::cout << "-> No reflection, the photon is out of the box!"<< std::endl;
            
            position->push_back( new Vector( x->getX(), x->getY(), x->getZ() ) );
            //Determine the angles of the photon at the exit (useful to plot at the angular distribution);
            theta_ph_out = proj_z/norm_proj; //I use step length instead of norm projection
            phi_ph_out   = proj_x/(sqrt(proj_x*proj_x+proj_y*proj_y));
            
            //Check whether the photon exited from the bottom wall or not. Important to count the number of photons reaching the detector.
            if ( x->getZ() > 8.00 ) position_out = 1;
            else position_out = 0;
            
            printSummary();
            
        }
    }
}



void Photon::rotateProjections( double theta_1, double phi_1 ) {
    
    if (p_id == 22) { //controllo di sicurezza, non necessario.
        
        long double temp_x;
        long double temp_y;
        long double temp_z;
        
        temp_x = step_length*sin(theta)*cos(phi);
        temp_y = step_length*sin(theta)*sin(phi);
        temp_z = step_length*cos(theta);
        
        std::cout << "-> The original step length was: " << step_length << std::endl;
        
        //Update the private variables: component of the shift in the global frame
        //This procedure could introduce an aproximation error.
        proj_x = temp_x*cos(theta_1)*cos(phi_1) - temp_y*cos(theta_1)*sin(phi_1) + temp_z*sin(theta_1)*cos(phi_1);
        proj_y = temp_x*cos(theta_1)*sin(phi_1) + temp_y*cos(theta_1)*cos(phi_1) + temp_z*sin(theta_1)*sin(phi_1);
        proj_z = -temp_x*sin(theta_1) - temp_y*sin(theta_1) + temp_z*cos(theta_1);
        
        
        //std::cout << "sum squares " << (proj_x*proj_x + proj_y*proj_y + proj_z*proj_z) << std::endl;
        norm_proj = sqrt((proj_x*proj_x + proj_y*proj_y + proj_z*proj_z));
        //std::cout << "difference " << abs((proj_x*proj_x + proj_y*proj_y + proj_z*proj_z)-norm_proj*norm_proj) << std::endl;
        //std::cout << "squared:    " << norm_proj*norm_proj << std::endl;
        std::cout << "-> Now the step length is      : " << norm_proj << std::endl;
    }
                                          
    
}

void Photon::reflectionPh( double r ) {//input: the radius of the cylinder taken from setup
    
    double x0 = x->getX();
    double y0 = x->getY();
    double z0 = x->getZ();
    
    std::cout << "-> Step length: " << step_length << std::endl;
    
    //x->shift(proj_x/2, proj_y/2, proj_z/2);
    //std::cout << x->getX() << " " << x->getY() << " " << x->getZ() << std::endl;
    
    
    //I take this point as an approximation of the intersection point. TO IMPROVE
    double x1 = x0 + proj_x/2;
    //std::cout << "x1 " << x1 << std::endl;
    //double y1 = 0.0;
    double y1 = y0 + proj_y/2;
    /*if ( y_control >= 0 ) { y1 = sqrt(r*r - x1*x1);}
    else { y1 = -1.0*sqrt(r*r - x1*x1);}*/
    //std::cout << "y1 " << y1 << std::endl;
    double z1 = z0 + proj_z/2;
    //std::cout << "z1 " << z1 << std::endl;
    
    std::cout << "Difference with the radius" << sqrt(x1*x1+y1*y1) - r << std::endl;
    
    //double norm_shift   = sqrt(proj_x*proj_x + proj_y*proj_y + proj_z*proj_z);
    //std::cout << "norm_shift " << norm_shift << std::endl;
    
    //The three components of the versor u of the shift in order: necessary to computer the reflection angles.
    double vers_shift_x = proj_x/norm_proj; //I could use also step length
    //std::cout << "vers_shift_x " << vers_shift_x << std::endl;
    double vers_shift_y = proj_y/norm_proj;
    //std::cout << "vers_shift_y " << vers_shift_y << std::endl;
    double vers_shift_z = proj_z/norm_proj;
    //std::cout << "vers_shift_z " << vers_shift_z << std::endl;
    
    //Compute the angle between the normal to the plane and the versor of the shift as the internal product between them. The normal to the plan is always the radius. Compute the versor of the radius. The coordinates of the radius are those of the intersection point.
    
    double vers_r_x = x1/r;//this is cos of the angle between the radius and the x-axes
    //std::cout << "vers_r_x " << vers_r_x << std::endl;
    double vers_r_y = y1/r;//this is cos of the angle between the radius and the y-axes
    //std::cout << "vers_r_y " << vers_r_y << std::endl;
    double vers_r_z = 0;
    
    
    double cos_theta_0 = (vers_r_x*vers_shift_x) + (vers_r_y*vers_shift_y);//Derived by means of tringonometry
    //std::cout << "theta_0 " << theta_0 << std::endl;
    double cos_phi_0   = sqrt(1 - cos_theta_0*cos_theta_0);
    //std::cout << "phi_0 " << phi_0 << std::endl;
    
    x->shift(proj_x, proj_y, proj_z); //temporary update
    
    //If statement to determine whether the reflection was on the lateral or bottom/top wall.
    if ( sqrt(x->getX()*x->getX() + x->getY()*x->getY()) >= r) {//REFLECTION ON THE LATERAL WALL.
        
        x->shift(-proj_x, -proj_y, -proj_z); //remove the temporary update.
        
        //Find the component of the shift w.r. to the plan and it's normal vector.
        double shift_normal = step_length* cos_theta_0;
        //std::cout << "Norm shift prima: " << norm_shift << std::endl;
        //std::cout << "Norm shift dopo: " << shift_normal/cos(theta_0) << std::endl;
        //std::cout << "shift_normal " << shift_normal << std::endl;
        double shift_plan   = step_length * cos_phi_0 ; //è la y
        //std::cout << "shift_plan " << shift_plan << std::endl;
        
        //Find the new components of the shift versor after the reflection (w.r. to the plan)
        double shift_normal_new = shift_normal;
        //std::cout << "shift_normal_new " << shift_normal_new << std::endl;
        double shift_plan_new = shift_plan;
        //std::cout << "shift_plan_new " << shift_plan_new << std::endl;
        
        
        //Find the new component of the shift versor after the reflection (w.r. to the g.f.)
        
        //double temp_xy = sqrt(abs((shift_plan_new*shift_plan_new) - (proj_z*proj_z)));
        //std::cout << "temp_xy " << temp_xy << std::endl;
        std::cout << "+++++++++++++Reflection on the lateral wall!++++++++++++++++++" << std::endl;
        
        proj_x = shift_normal_new*(-1.0*vers_r_x) - shift_plan*vers_r_y;
        proj_y  = -1.0*shift_normal_new*vers_r_y + shift_plan*vers_r_x;
        
        std::cout << "-> New shift projections: (" << proj_x << ", " << proj_y << ", " << proj_z << ")" << std::endl;
        norm_proj =  sqrt((proj_x*proj_x + proj_y*proj_y + proj_z*proj_z));
        std::cout << "-> Norm projection      : " << norm_proj << std::endl;
        std::cout << "-> Original step length : " << step_length << std::endl;
        
        
        //x->shift(proj_x/2, proj_y/2, proj_z/2);
        
    }
    
    
    else if (x->getZ() >= 8.00 || x->getZ() <= 0.0) {//REFLECTION ON THE BOTTOM/TOP WALL
        
        x->shift(-proj_x, -proj_y, -proj_z);
        
        proj_z = -1.0*proj_z; //update only the z direction
        
        std::cout << "Reflection on the bottom/top wall!" << std::endl;
        x->shift(proj_x/2, proj_y/2, proj_z/2);
        std::cout << "-> New photon position  : (" << x->getX() << ", " << x->getY() << ", " << x->getZ() << ")" << std::endl;
        std::cout << "-> New shift projection : (" << proj_x << ", " << proj_y << ", " << proj_z << ")" << std::endl;
        norm_proj =  norm_proj = sqrt((proj_x*proj_x + proj_y*proj_y + proj_z*proj_z));
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

double Photon::gettheta_out_ph() {
    return theta_ph_out;
}

double Photon::getphi_out_ph() {
    return phi_ph_out;
}


int Photon::getnReflections() {
    return nReflections;
}

bool Photon::getcheckInside() {
    return checkInside;
    
}
