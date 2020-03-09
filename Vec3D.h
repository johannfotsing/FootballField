/*
Author: Johan Fotsing
Class: ECE6122
Last Date Modified: 12/06/2019
Description: 
Here is the header file for the class Vec3D that is used to facilitate vector manipulation
*/

#ifndef VEC_3D
#define VEC_3D

#include <iostream>

using namespace std; 

class Vec3D
{
private:
    double x, y, z; 
public:
    Vec3D();
    Vec3D(double, double, double); 
    Vec3D(const Vec3D &);
    // Member operations
    Vec3D& operator= (const Vec3D &);
    Vec3D operator+(const Vec3D &) const;
    Vec3D& operator+= (const Vec3D &);
    Vec3D operator-() const; 
    Vec3D operator-(const Vec3D &) const;
    Vec3D& operator-= (const Vec3D &);
    Vec3D operator*(const double &) const; 
    Vec3D operator/(const double &) const; 
    // Getters and special functions
    double getX() const;
    double getY() const;
    double getZ() const;
    void setX(const double &); 
    void setY(const double &); 
    void setZ(const double &); 
    double norm() const; 
    double dotProduct(const Vec3D &) const; 
    Vec3D projectOn(const Vec3D &) const; 
    // Friend functions
    friend Vec3D operator*(const double &, const Vec3D &);
    friend ostream& operator<<(ostream&, const Vec3D &); 
    //~Vec3D();
};

#endif
