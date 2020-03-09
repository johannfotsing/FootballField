/*
Author: Johan Fotsing
Class: ECE6122
Last Date Modified: 12/06/2019
Description: 
This is the code for the member functions of the Vec3D class
*/

#include "Vec3D.h"
#include <iostream>
#include "math.h"

using namespace std; 

Vec3D::Vec3D()
{
    x = 0; 
    y = 0; 
    z = 0; 
}

Vec3D::Vec3D(double in_x, double in_y, double in_z) : x(in_x), y(in_y), z(in_z) {}

Vec3D::Vec3D(const Vec3D & vec) : x(vec.x), y(vec.y), z(vec.z) {}

// Member operations
Vec3D& Vec3D::operator= (const Vec3D & vec)
{
    this->x = vec.x; 
    this->y = vec.y; 
    this->z = vec.z; 
    return *this; 
}

Vec3D Vec3D::operator+(const Vec3D & vec) const
{
    return Vec3D(this->x + vec.x, this->y + vec.y, this->z + vec.z); 
}

Vec3D& Vec3D::operator+= (const Vec3D & vec)
{
    this->x += vec.x;
    this->y += vec.y;
    this->z += vec.z;
    return *this; 
}

Vec3D Vec3D::operator-() const
{
    return Vec3D(-this->x, -this->y, -this->z);
}

Vec3D Vec3D::operator-(const Vec3D & vec) const
{
    return *this + (-vec);
}

Vec3D& Vec3D::operator-= (const Vec3D & vec)
{
    return *this += (-vec); 
}

Vec3D Vec3D::operator*(const double & a) const
{
    return Vec3D(a*this->x, a*this->y, a*this->z); 
}

Vec3D Vec3D::operator/(const double & a) const
{
    return Vec3D(this->x/a, this->y/a, this->z/a); 
}

// ---------------------------------------------------------
// Getters, setters and special functions
// ---------------------------------------------------------
double Vec3D::getX() const
{
    return this->x; 
}

double Vec3D::getY() const
{
    return this->y; 
}
double Vec3D::getZ() const
{
    return this->z; 
}

void Vec3D::setX(const double & in_x)
{
    this->x = in_x; 
}

void Vec3D::setY(const double & in_y)
{
    this->y = in_y; 
}

void Vec3D::setZ(const double & in_z)
{
    this->z = in_z; 
}

double Vec3D::norm() const
{
    return sqrt(this->x*this->x + this->y*this->y + this->z*this->z); 
}

double Vec3D::dotProduct(const Vec3D &vec) const
{
    return this->x*vec.x + this->y*vec.y + this->z*vec.z; 
}

Vec3D Vec3D::projectOn(const Vec3D &vec) const
{
    Vec3D u = vec/vec.norm(); 
    return (*this).dotProduct(u)*u; 
}


// Friend functions
//friend Vec3D operator+(const double &, const Vec3D &); 
//friend Vec3D operator-(const double &, const Vec3D &);
Vec3D operator*(const double & a, const Vec3D & vec)
{
    return vec*a; 
}

ostream& operator<<(ostream& os, const Vec3D & vec)
{
    os << "[" << vec.x << "; " << vec.y << "; " << vec.z << "]"; 
    return os; 
}
//~Vec3D();