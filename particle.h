#ifndef PARTICLE_H
#define PARTICLE_H

#ifdef __APPLE__
#  define GL_SILENCE_DEPRECATION
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

#include <iostream>
#include "mathLib3D.h"
#include <vector>
using namespace std;

enum State {RESET,NEUTRAL,PAUSED};



class Particle {
public:
    State initState;
    int gHeight, gWidth;
    bool colorHasBeenChangedByMouseProximity, cameraInThisParticle, antiGrav, rubber;
    float friction, gravity, cDull, age, ageMax;
    Particle();
    Particle(State state,bool antiGrav,bool rubber);
    Point3D mPos,mOrig,mRot;
    Vec3D mVel; //velocity used to combine direction and speed and add it to position
    float mColor[3];
    unsigned int mSize;
    float mSpeed;
    void update(Point3D mousePos);
    void slowDown();
    void loadFields(int winWidth, int winHeight);
    float forceFunction(float dist);
    GLfloat materialShiny,*materialAmbient, *materialDiffuse, *materialSpecular;
};

#endif
