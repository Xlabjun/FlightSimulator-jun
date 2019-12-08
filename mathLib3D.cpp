#include <math.h>
#include "mathLib3D.h"


Point3D::Point3D(float inX,float inY,float inZ){
	mX = inX; mY = inY; mZ = inZ;
}
Point3D::Point3D(){
	mX = 0; mY = 0; mZ = 0;
}
void Point3D::multiply(float scalar){
    mX*=scalar;mY*=scalar;mZ*=scalar;
}
void Point3D::plus(float scalar){
    mX+=scalar;mY+=scalar;mZ+=scalar;
}
void Point3D::minus(float scalar){
    mX-=scalar;mY-=scalar;mZ-=scalar;
}
void Point3D::multiply(float x, float y, float z){
    mX*=x;mY*=y;mZ*=z;
}
void Point3D::plus(float x, float y, float z){
    mX+=x;mY+=y;mZ+=z;
}
void Point3D::minus(float x, float y, float z){
    mX-=x;mY-=y;mZ-=z;
}
float Point3D::distanceTo(Point3D other){
    return sqrt(pow(mX - other.mX,2) + pow(mY - other.mY, 2) + pow(mZ - other.mZ, 2));
}
void Point3D::set(float x, float y, float z){
	mX = x; mY = y; mZ = z;
}

Vec3D::Vec3D(float inX,float inY,float inZ){
	mX = inX; mY = inY; mZ = inZ;
}
Vec3D::Vec3D(){
	mX = 0; mY = 0;mZ = 0;
}
void Vec3D::set(float x, float y, float z){
	mX = x; mY = y; mZ = z;
}
float Vec3D::length(){
    return sqrt(pow(mX, 2) + pow(mY, 2) + pow(mZ, 2));
}
Vec3D Vec3D::normalize(){
    return Vec3D(mX/length(), mY/length(), mZ/length());
}
void Vec3D::multiply(float scalar){
    mX*=scalar;mY*=scalar;mZ*=scalar;
}
void Vec3D::plus(float scalar){
    mX+=scalar;mY+=scalar;mZ+=scalar;
}
void Vec3D::minus(float scalar){
    mX-=scalar;mY-=scalar;mZ-=scalar;
}
void Vec3D::multiply(Vec3D vec){
    mX*=vec.mX;mY*=vec.mY;mZ*=vec.mZ;
}
void Vec3D::plus(Vec3D vec){
    mX+=vec.mX;mY+=vec.mY;mZ+=vec.mZ;
}
void Vec3D::minus(Vec3D vec){
    mX-=vec.mX;mY-=vec.mY;mZ-=vec.mZ;
}
Vec3D Vec3D::neg(){
    return Vec3D(-mX,-mY,-mZ);
}
void Vec3D::multiply(float x, float y, float z){
    mX*=x;mY*=y;mZ*=z;
}
void Vec3D::plus(float x, float y, float z){
    mX+=x;mY+=y;mZ+=z;
}
void Vec3D::minus(float x, float y, float z){
    mX-=x;mY-=y;mZ-=z;
}
Point3D Vec3D::movePoint(Point3D source){
	return Point3D(source.mX+mX,source.mY+mY,source.mZ+mZ);
}
Vec3D Vec3D::createVector(Point3D p1, Point3D p2){
    return Vec3D(p2.mX-p1.mX, p2.mY-p1.mY, p2.mZ-p1.mZ);
}