#include <math.h>
#include <iostream>
#include "mathLib3D.h"
#include "particle.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>
using namespace std;

Particle::Particle(){};//default constructor, not used

Particle::Particle(State state,bool frictionEnable,bool setAntiGrav,bool setRubber){
    antiGrav=setAntiGrav;rubber=setRubber;

    initState=state;
    mPos=Point3D(0,0,0); 
    mOrig=Point3D(mPos.mX,mPos.mY,mPos.mZ);
    mVel=Vec3D((rand()/float(RAND_MAX))*1 -.5,(rand()/float(RAND_MAX))*3 +2,(rand()/float(RAND_MAX))*1 -.5); 
    mRot=Point3D(float(RAND_MAX)*360,float(RAND_MAX)*360,float(RAND_MAX)*360);

    ageMax=rand()%300 + 70;   //age is 1s to 5s
    cDull=7; 
    gravity= 0.14; 
    friction=0.65;frictionEnabled=frictionEnable;
    cameraInThisParticle=false;
    
    //position is within 1000x1000 window, direction is stationary
    mColor[0]=(rand()/float(RAND_MAX));mColor[1]=rand()/float(RAND_MAX);mColor[2]=rand()/float(RAND_MAX); //color's rgb streams, each r g and b being between 0-255 randomized
    mSize = rand()%100;   if (mSize>=99) mSize = 5; else if (mSize>=95) mSize = 4;  else if (mSize>3) mSize=rand()%3;//size 0-4
    mSpeed = (rand()/float(RAND_MAX))*1+1; //speed 1-5
}

void Particle::update(Point3D mousePos){
    cout<<mVel.mY*mSpeed<<"\n";
    if (initState!=PAUSED){
        age++;

        mPos.plus(mVel.mX*mSpeed,mVel.mY*mSpeed,mVel.mZ*mSpeed);
        
        if (mPos.mZ<=-500 || mPos.mZ>=500)       {mVel.mZ=-mVel.mZ;}
        if (mPos.mX<=-500 || mPos.mX>=500)        {mVel.mX=-mVel.mX;}//when hitting borders of animation, bounce off
        if (mPos.mY<0     || mPos.mY>=500)       {
            mVel.mY=-mVel.mY;
            if (mPos.mY<0){
                if (frictionEnabled)mVel.multiply(friction);
                mPos.mY=0;
            } else if (mPos.mY>500){
                mPos.mY=0;
            }
        }
        //lose speed to friction when hitting ground, unless rubber then gain some every time, and a lot if no friction
        if (rubber)mVel.mY*=(1/friction +0.01);

        if (mVel.mY>gHeight/4)mVel.mY=gHeight/4;
        else if (mVel.mY<-gHeight/4)mVel.mY=-gHeight/4;
        

        if (abs(mPos.mY)+abs(mPos.mX)+abs(mPos.mZ)>10000) {mPos.multiply(0,0,0);} 
        //if moving so fast that jumping outside of screen borders, remake in middle of platform
        
        if (!antiGrav) mVel.mY-=gravity;    //if antigrav, gravity doesnt affect
      
        //cout<<"\nmDirY: "<<mDir.mY;
        
    }
}

/*float Particle::forceFunction(float dist){
    return ((dist+1));
}*/

void Particle::slowDown(){
    mSpeed*=gravity;
}