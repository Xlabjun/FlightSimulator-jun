#include <math.h>
#include <iostream>
#include "mathLib3D.h"
#include "airplane.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>
using namespace std;

Airplane::Airplane(){};//default constructor, not used

Airplane::Airplane(State state){
    initState=state;
    mPos=Vec3D(0,100,1000);mVel=Vec3D(0,-0.1,-1); 
    mAcc=Vec3D(0,0,0); 
    mOrig=Vec3D(mPos.mX,mPos.mY,mPos.mZ);

    mRot=Vec3D(float(RAND_MAX)*360,float(RAND_MAX)*360,float(RAND_MAX)*360);

    ageMax=rand()%300 + 70;   //age is 1s to 5s
    cDull=7; 
    gravity= 0.14; 
    friction=0.65;//frictionEnabled=frictionEnable;
    cameraInThisParticle=false;
    
    //position is within 1000x1000 window, direction is stationary
    mColor[0]=(rand()/float(RAND_MAX));mColor[1]=rand()/float(RAND_MAX);mColor[2]=rand()/float(RAND_MAX); //color's rgb streams, each r g and b being between 0-255 randomized
    mSize = rand()%100;   if (mSize>=99) mSize = 5; else if (mSize>=95) mSize = 4;  else if (mSize>3) mSize=rand()%3;//size 0-4
    mSpeed = (rand()/float(RAND_MAX))*1; //speed 1-5
}

void Airplane::update(){
    //cout<<initState;
    if (initState!=PAUSED){
        age++;
        mAcc.multiply(0.999);
        mVel.plus(mAcc.mX*mSpeed,mAcc.mY*mSpeed,mAcc.mZ*mSpeed);
        mPos.plus(mVel.mX*mSpeed,mVel.mY*mSpeed,mVel.mZ*mSpeed);
     

        if (mPos.mY<0     || mPos.mY>=500)       {//bounce off ground
            mVel.mY=-mVel.mY/2;
            if (mPos.mY<0){
                if (frictionEnabled)mVel.multiply(friction);
                mPos.mY=0;
            } 
        }
        //lose speed to friction when hitting ground, unless rubber then gain some every time, and a lot if no friction
        if (rubber)mVel.mY*=(1/friction +0.01);

        if (mVel.mY>gHeight/4)mVel.mY=gHeight/4;
        else if (mVel.mY<-gHeight/4)mVel.mY=-gHeight/4;
        

        if (abs(mPos.mY)+abs(mPos.mX)+abs(mPos.mZ)>gWidth*100) {mPos.multiply(0,0,0);} 
        //if moving so fast that jumping outside of screen borders, remake in middle of platform

      
        //cout<<"\nmDirY: "<<mDir.mY;
        
    }
}
void Airplane::draw(){
    glPushMatrix();
    glColor3f (0.3,    0.7,  1);
        glTranslatef(mPos.mX,mPos.mY,mPos.mZ);
        glutSolidCube(5);
    glPopMatrix();
};
/*float Airplane::forceFunction(float dist){
    return ((dist+1));
}*/

void Airplane::slowDown(){
    mSpeed*=gravity;
}