#ifndef AIRPLANE_H
#define AIRPLANE_H

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
#include "particle.h"
#include <vector>
using namespace std;



class Airplane {
public:
    State initState;
    int gHeight, gWidth;
    bool colorHasBeenChangedByMouseProximity, frictionEnabled, cameraInThisParticle, antiGrav, rubber;
    float friction, gravity, cDull, age, ageMax;
    Airplane();
    Airplane(State state);
    Vec3D mPos,mVel,mAcc,mOrig,mRot;//velocity used to combine direction and speed and add it to position
    float mColor[3];
    unsigned int mSize;
    float mSpeed;
    void update();
    void slowDown();
    void draw();
    void loadFields(int winWidth, int winHeight);
    float forceFunction(float dist);
    GLfloat materialShiny,*materialAmbient, *materialDiffuse, *materialSpecular;
};

#endif
