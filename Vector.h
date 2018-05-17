#ifndef Vector_h
#define Vector_h

class Vector {
    
public:
    Vector( double x_0, double y_0, double z_0 );
    ~Vector();
    double getX();
    double getY();
    double getZ();
    void   shift( double x_1, double y_1, double z_1 );
    void   rotate( double theta, double phi );
    
private:
    double x;
    double y;
    double z;
    
};

#endif
