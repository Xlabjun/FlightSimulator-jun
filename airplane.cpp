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
    mPos=Vec3D(0,50,1000);mVel=Vec3D(0,0,-3); 
    mAcc=Vec3D(0,0,0); 
    mOrig=Vec3D(mPos.mX,mPos.mY,mPos.mZ);

    mRot=Vec3D(270,0,0);

    cDull=7; 
    gravity= 0.00001; 
    friction=0.65;frictionEnabled=true;//frictionEnabled=frictionEnable;
    cameraInThisParticle=false;
    
    //position is within 1000x1000 window, direction is stationary
    mColor[0]=(rand()/float(RAND_MAX));mColor[1]=rand()/float(RAND_MAX);mColor[2]=rand()/float(RAND_MAX); //color's rgb streams, each r g and b being between 0-255 randomized
    mSpeed = 5; //speed 1-5
}

void Airplane::update(){
    //cout<<initState;
    if (initState!=PAUSED){

        mRot.mX+=mAcc.mY*100;
		mRot.mY+=mAcc.mX*50;
		mRot.mZ-=mAcc.mX*50;

        //cout<<mPos.mY<<" before updated \n";
        age++;

        mVel.plus(mAcc.mX*mSpeed,mAcc.mY*mSpeed,mAcc.mZ*mSpeed);
        mPos.plus(mVel.mX,mVel.mY,mVel.mZ);
        

        if (mPos.mY<0     )       {//bounce off ground
            mVel.mY=-mVel.mY/2;mAcc.mY=mAcc.mY/8;
            if (mPos.mY<0){
                if (frictionEnabled)mVel.multiply(friction);
                mPos.mY=1;
            } 
        }

        if (mVel.mY>gHeight/4)mVel.mY=gHeight/4;
        else if (mVel.mY<-gHeight/4)mVel.mY=-gHeight/4;
        
        //if moving so fast that jumping outside of screen borders, remake in middle of platform

        slowDown();
        //cout<<"\nmDirY: "<<mDir.mY;
                //cout<<mPos.mY<<" after updated \n";
    }
}

/*float Airplane::forceFunction(float dist){
    return ((dist+1));
}*/

void Airplane::slowDown(){
    mAcc.mY-=gravity;
    mAcc.multiply(0.98);
    mVel.multiply(0.995);
}