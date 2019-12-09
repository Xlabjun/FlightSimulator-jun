//Erik, Connor, Akram

#ifdef __APPLE__
#  define GL_SILENCE_DEPRECATION
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

#include <fstream>
#include <sstream>
#include "glm/gtx/component_wise.hpp"
#include "glm/glm.hpp"
#include <math.h>
#include <iostream>
#include "mathLib3D.h"
#include "particle.h"
#include "airplane.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <string.h>
#include <time.h>       /* time */
#include <algorithm>
#include <vector>

using namespace std;


struct Circle
{
    int x, z;
	int diam, height;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
};

struct VertRef
{
    VertRef( int v, int vt, int vn ) : v(v), vt(vt), vn(vn) { }
    int v, vt, vn;
};

std::vector< Vertex > model;

const int tWidth=300, tLength=300;
int maxHeight=6;
float heightMap[tWidth][tWidth];
vector<Circle> circleArr (tWidth);

//*******************************************************************************wavefront .obj loader code begins
std::vector< Vertex > LoadOBJ( std::istream& in )
{
    std::vector< Vertex > verts;

    std::vector< glm::vec4 > positions( 1, glm::vec4( 0, 0, 0, 0 ) );
    std::vector< glm::vec3 > texcoords( 1, glm::vec3( 0, 0, 0 ) );
    std::vector< glm::vec3 > normals( 1, glm::vec3( 0, 0, 0 ) );
    std::string lineStr;
    while( std::getline( in, lineStr ) )
    {
        std::istringstream lineSS( lineStr );
        std::string lineType;
        lineSS >> lineType;

        // vertex
        if( lineType == "v" )
        {
            float x = 0, y = 0, z = 0, w = 1;
            lineSS >> x >> y >> z >> w;
            positions.push_back( glm::vec4( x, y, z, w ) );
        }
        
        // texture
        if( lineType == "vt" )
        {
            float u = 0, v = 0, w = 0;
            lineSS >> u >> v >> w;
            texcoords.push_back( glm::vec3( u, v, w ) );
        }

        // normal
        if( lineType == "vn" )
        {
            float i = 0, j = 0, k = 0;
            lineSS >> i >> j >> k;
            normals.push_back( glm::normalize( glm::vec3( i, j, k ) ) );
        }

        // polygon
        if( lineType == "f" )
        {
            std::vector< VertRef > refs;
            std::string refStr;
            while( lineSS >> refStr )
            {
                std::istringstream ref( refStr );
                std::string vStr, vtStr, vnStr;
                std::getline( ref, vStr, '/' );
                std::getline( ref, vtStr, '/' );
                std::getline( ref, vnStr, '/' );
                int v = atoi( vStr.c_str() );
                int vt = atoi( vtStr.c_str() );
                int vn = atoi( vnStr.c_str() );
                v  = (  v >= 0 ?  v : positions.size() +  v );
                vt = ( vt >= 0 ? vt : texcoords.size() + vt );
                vn = ( vn >= 0 ? vn : normals.size()   + vn );
                refs.push_back( VertRef( v, vt, vn ) );
            }

            // triangulate, assuming n>3-gons are convex and coplanar
            for( size_t i = 1; i+1 < refs.size(); ++i )
            {
                const VertRef* p[3] = { &refs[0], &refs[i], &refs[i+1] };

                // http://www.opengl.org/wiki/Calculating_a_Surface_Normal
                glm::vec3 U( positions[ p[1]->v ] - positions[ p[0]->v ] );
                glm::vec3 V( positions[ p[2]->v ] - positions[ p[0]->v ] );
                glm::vec3 faceNormal = glm::normalize( glm::cross( U, V ) );

                for( size_t j = 0; j < 3; ++j )
                {
                    Vertex vert;
                    vert.position = glm::vec3( positions[ p[j]->v ] );
                    vert.texcoord = glm::vec2( texcoords[ p[j]->vt ] );
                    vert.normal = ( p[j]->vn != 0 ? normals[ p[j]->vn ] : faceNormal );
                    verts.push_back( vert );
                }
            }
        }
    }

    return verts;
}
//*******************************************************************************wavefront .obj loader code ends here

float xAxisRotation, yAxisRotation;
bool lightingEnabled=false, fixedCamera=false, texMode=true;

int gWidth=1000, gHeight=1000;
vector<Particle> particleArr(1);//2000-3000 particle capacity  rand()%1000+3000
Point3D mousePos;
State state = NEUTRAL;
Airplane airplane=Airplane(state);

//an array for image data
GLubyte* img_data;
GLuint textures;
int width, height, max1;

void drawParticle(Particle p) {
	glPushMatrix();
		glTranslatef(p.mPos.mX,p.mPos.mY,p.mPos.mZ);
		glColor3f(p.mColor[0],p.mColor[1],p.mColor[2]);//change glColor to each particle before we draw

		glutSolidSphere(p.mSize*1,10,10);
		glRotatef(p.mRot.mX,1,0,0);
		glRotatef(p.mRot.mY,0,1,0);
		glRotatef(p.mRot.mZ,0,0,1);
	glPopMatrix();
}

void drawAirplane(){
   
    glTranslatef(airplane.mPos.mX,airplane.mPos.mY,airplane.mPos.mZ);
            
    glScalef(0.01,0.01,0.01);
    //glRotatef(airplane.mRot.mX, 1,0,0);
    
    //glPointSize(3.0);
        //glMatrixMode( GL_MODELVIEW );
    //glLoadIdentity();
    {
        //glRotatef( curRot.x % 360, 0, 1, 0 );
        //glRotatef( -curRot.y % 360, 1, 0, 0 );

        
        // object
        glColor3ub( 130, 115, 110 );
        glRotatef(airplane.mRot.mX,1,0,0);
		glRotatef(airplane.mRot.mY,0,1,0);
		glRotatef(airplane.mRot.mZ,0,0,1);


        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glEnableClientState( GL_NORMAL_ARRAY );
        glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), &model[0].position );
        glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), &model[0].texcoord );
        glNormalPointer( GL_FLOAT, sizeof(Vertex), &model[0].normal );
        glDrawArrays( GL_TRIANGLES, 0, model.size() );
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        glDisableClientState( GL_NORMAL_ARRAY );

        // bounding cube
    }
}
void collision(){
    if(airplane.initState!=PAUSED) {
        for (int x = 0; x < tWidth; x++) {
            for (int z = 0; z < tLength; z++) {
                if ( (abs(airplane.mPos.mX-x)<10) && (abs(airplane.mPos.mZ-z)<10) && (airplane.mPos.mY<heightMap[x][z])) {//bounce off ground
                    airplane.mPos.mY=heightMap[x][z];
                    break;
                }
            }
        }
    }
}

GLdouble camOffset[] = { 0, 0.1, 0.3 }; 
GLdouble centerOffset[] = { 0, 0, 0 };


GLfloat lightPos[4] = {
     250, 300, -tWidth, 1 
};
GLfloat lightDiffuses[4] = {
     1, 1, 1, 1
};
GLfloat lightSpeculars[4] = {
    1, 1, 1, 1
};
GLfloat lightAmbients[4] = {
     1, 1, 1, 0.8 
};


void configureLighting(){

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,   lightDiffuses);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpeculars);
    glLightfv(GL_LIGHT0, GL_AMBIENT,   lightAmbients);
}

Vec3D normalsUsingPoints(Point3D startVecA,Point3D endVecA,Point3D startVecB,Point3D endVecB){
	Vec3D crossProdVecA=Vec3D::createVector(startVecA,endVecA);
	Vec3D crossProdVecB=Vec3D::createVector(startVecB,endVecB);

	//a2b3-a3b2, a3b1-a1b3, a1b2-a2b1 for cross product to calculate normal
	Vec3D normalVector = Vec3D(
	crossProdVecA.mY*crossProdVecB.mZ - crossProdVecA.mZ*crossProdVecB.mY,
	crossProdVecA.mZ*crossProdVecB.mX - crossProdVecA.mX*crossProdVecB.mZ,
	crossProdVecA.mX*crossProdVecB.mY - crossProdVecA.mY*crossProdVecB.mX
	);

	normalVector.normalize();
	return normalVector;
}
Vec3D normMap[tWidth][tWidth];

void drawTerrain(){
    for(int x = 0; x < tWidth-1; x++) {                                                                                                             
		for(int z = 0; z < tWidth-1; z++) {
            
            glBegin(GL_QUADS);
                glColor3f(10/(float)heightMap[x][z],0,(float)heightMap[x][z]/15);
                glTexCoord3f(x, heightMap[x][z], z);
                glNormal3f(normMap[x][z].mX,normMap[x][z].mY,normMap[x][z].mZ);
                glVertex3f(x, 		heightMap[x][z], 		 z);

                glColor3f(10/(float)heightMap[x][z],0,(float)heightMap[x][z]/15);
                glTexCoord3f(x, heightMap[x][z+1], z+1);
                glNormal3f(normMap[x][z+1].mX,normMap[x][z+1].mY,normMap[x][z+1].mZ);
                glVertex3f(x, 		heightMap[x][z+1], 		(z+1));
                
                glColor3f(10/(float)heightMap[x][z],0,(float)heightMap[x][z]/15);
                glTexCoord3f(x+1, heightMap[x+1][z+1], z+1);
                glNormal3f(normMap[x+1][z+1].mX,normMap[x+1][z+1].mY,normMap[x+1][z+1].mZ);
                glVertex3f((x+1), 	heightMap[x+1][z+1], 	(z+1));

                glColor3f(10/(float)heightMap[x][z],0,(float)heightMap[x][z]/15);
                glTexCoord3f(x+1, heightMap[x+1][z], z);
                glNormal3f(normMap[x+1][z].mX,normMap[x+1][z].mY,normMap[x+1][z].mZ);
                glVertex3f((x+1), 	heightMap[x+1][z],		 z);
            glEnd();
        }
    }
}
void display(void) {
    configureLighting();
    collision();
    airplane.update();
    

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glLoadIdentity();

	//glMatrixMode( GL_MODELVIEW );
	if (!fixedCamera){
    Vec3D offsetVec=airplane.mVel.normalize(); offsetVec.multiply(0.5);
        gluLookAt(airplane.mPos.mX-offsetVec.mX+camOffset[0],airplane.mPos.mY-offsetVec.mY+camOffset[1],airplane.mPos.mZ-offsetVec.mZ+camOffset[2], airplane.mPos.mX,airplane.mPos.mY+centerOffset[1],airplane.mPos.mZ, 0,1,0);
	}else {
        gluLookAt(airplane.mOrig.mX+camOffset[0],airplane.mOrig.mY+camOffset[1]+50,airplane.mOrig.mZ+camOffset[2]+100, 0,0,0, 0,1,0);
    }
	/*glTranslatef(-camOffset[0],-camOffset[1],-camOffset[2]);//ensure rotation at 0,0,0
	glRotatef(xAxisRotation,1,0,0);	//about x axis
	glRotatef(yAxisRotation,0,1,0); //about y axis
	glTranslatef(camOffset[0],camOffset[1],camOffset[2]);*/


	//cout<<state;
	for (unsigned int i=0; i<particleArr.size(); i++){ //updateAndRender loop for particles
		particleArr[i].update(mousePos);//update
		if (particleArr[i].age>particleArr[i].ageMax){
			particleArr[i]=Particle(state,rand()%2,rand()%2);
		}
		if (particleArr[i].initState!=PAUSED) drawParticle(particleArr[i]);//render
	}
	
	glPushMatrix();
		drawAirplane();
	glPopMatrix();

		//push the current modelview matrix onto the matrix stack
	glPushMatrix();
        if (!texMode) {
            glDisable(GL_TEXTURE_2D);
        }
        if (texMode) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textures);
        }
        glTranslatef(-tWidth/2,0,0);
		drawTerrain();
	//pop the matrix back to what it was prior to the rotation
	glPopMatrix();


	glutSwapBuffers();//swap buffers for smooth animation
	glutPostRedisplay();
}

void handleKeyboard(unsigned char key, int _x, int _y) {
        //cout <<"\nlightPos: "<<lightPos[0]<<", "<<lightPos[1]<<", "<<lightPos[2];
    if (key == 27) {//escape key
        exit(0);
    

	} else if (key == '.') {
        camOffset[0] +=.5;
    } else if (key == 'm') {
        camOffset[0] -=.5;
    } else if (key == 'k') {
        camOffset[1] +=.5;
    } else if (key == ',') {
        camOffset[1] -=.5;
    } else if (key == 'i') {
        camOffset[2] -=.5;
    } else if (key == 'o') {
        camOffset[2] +=.5;

	} else if (key == 'd') {
        airplane.mAcc.mX+=0.0001;
    } else if (key == 'a') {
        airplane.mAcc.mX-=0.0001;
    } else if (key == 'w') {
        //cout<<airplane.mVel.mY<<"\n";
		airplane.mAcc.mY-=0.00005;
    } else if (key == 's') {
        airplane.mAcc.mY+=0.00005;
    } else if (key == 'x') {
        airplane.mAcc.mZ-=.00005;
    } else if (key == 'z') {
        airplane.mAcc.mZ+=.00005;

	} else if (key == 'h') {
        lightPos[0] +=150;
    } else if (key == 'f') {
        lightPos[0] -=150;
    } else if (key == 't') {
        lightPos[1] +=150;
    } else if (key == 'g') {
        lightPos[1] -=150;
    } else if (key == 'y') {
        lightPos[2] +=150;
    } else if (key == 'r') {
        lightPos[2] -=150;

    } else if (key == 'v') {
        texMode = !texMode;


    } else if (key == ' ') {
        if (state!=PAUSED) state=PAUSED;//if not pause and spacebar pressed, pause
		else {
			state=NEUTRAL;//else unpause
			for (unsigned int i=0; i<particleArr.size(); i++){
				particleArr[i].initState=NEUTRAL;
			}
		}
	
	} else if (key == 'r') {//reset
		particleArr.clear(); particleArr=vector<Particle> (rand()%1000+3000);
		for (int i=0; i<(int)(particleArr.capacity()/1.1); i++) particleArr[i]=Particle(PAUSED,rand()%2,rand()%2);

		if (state!=PAUSED) state=PAUSED;//if not pause and spacebar pressed, pause
		else {
			state=NEUTRAL;//else unreset and fill list again
		}
	} else if (key == 'c') {
		if (!fixedCamera) {
			fixedCamera=true;
            gluLookAt(airplane.mOrig.mX+camOffset[0],airplane.mOrig.mY+camOffset[1]+99999,airplane.mOrig.mZ+camOffset[2],
            0,0,99990, 0,1,0);
            glutPostRedisplay();

		} else {
			fixedCamera=false;
		}	
		
	} else if (key == 'l') {
		if (!lightingEnabled){
			glEnable(GL_LIGHTING); lightingEnabled=true;
		} else {
			glDisable(GL_LIGHTING); lightingEnabled=false;
		}
    } else if (key == 'v') {
        particleArr.push_back(Particle(state,rand()%2,rand()%2));

	} else if (key == 'b') {//delete random particle
		particleArr.erase(particleArr.begin()+rand()%particleArr.size());
	} 
}
void SpecialKeys(int key, int x, int y) {
	switch (key){
		case GLUT_KEY_LEFT:
			yAxisRotation--;
			break;
		case GLUT_KEY_RIGHT:
			yAxisRotation++;
			break;
		case GLUT_KEY_UP:
			xAxisRotation--;
			break;
		case GLUT_KEY_DOWN:
			xAxisRotation++;
			break;
	}
}

void FPS(int val){
    glutPostRedisplay();
    glutTimerFunc(17, FPS, 0); // 1sec = 1000, 60fps = 1000/60 = ~17ms per frame
}

void callBackInit(){
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glutKeyboardFunc(handleKeyboard);
	glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(display);	//registers "display" as the display callback function
	glutTimerFunc(0, FPS, 0);
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glPolygonMode( GL_FRONT, GL_FILL );
    glPolygonMode( GL_BACK, GL_LINE );
    glDisable(GL_LIGHTING);
}

void printIntro(){
		printf("\
	\n%s\n WELCOME TO ERIK, AKRAM AND CONNOR'S FLIGHT (AND PARTICLE) SIMULATOR\n%s\n",
	" ====================================",
	" ====================================");

	printf("\n\
	%-32s   %-18s   %-18s\n\n\
	%-32s   %-18s   %-18s\n\n\
	%-32s   %-18s   %-18s\n\
	%-32s   %-18s   %-18s\n\
	%-32s   %-18s   %-18s\n\n\
	%-32s   %-18s   %-18s\n\
	%-32s   %-18s   %-18s\n\
	%-32s   %-18s   %-18s\n\n\
	%-32s   %-18s   %-18s\n\
	%-32s   %-18s   %-18s\n\
	%-32s   %-18s   %-18s\n\n"

	, "FUNCTION",  "Do\\Increase Key", "Decrease Key"

	, "Exit Program", "ESC", ""

	, "Yaw left/right", "A", "D"
	, "Pitch up/down", "S", "W"
	, "Thrust up/down", "X", "Z"

	, "Camera left/right", "M", "."
	, "Camera up/down", "K", ","
	, "Camera in/out of scene", "I", "O"

    , "Textures enable/disable", "V", ""
    , "Toggle Fixed/Following Cam", "C", ""
	, "Lighting enable/disable", "L", "");

	printf("\nStudent #: 400062982, name: Erik Kredatus\n\
	\nStudent #: 4_____________, name: Akram Elwazani\n\
	\nStudent #: 4_____________, name: Connor Hewick\n\n\
	Final Project for 3GC3\n\n\
	Additions: (check SpecialKeys() and HandleKeyboard() key input methods as well as the controls above): \n\
	- velocity and position for aircraft, combining speed and direction to simulate the effects of inertia and momentum\n\
	");
}

void reshape(int w, int h){
	gWidth = w;
	gHeight = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,1,0.01f ,h*55);
	//glOrtho(-w*2, w*2, -h*2, h*2,   -h*2, h*2);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
	for (int i=0; i<(int)(particleArr.capacity()/1.1); i++){ particleArr[i].gWidth=gWidth; particleArr[i].gHeight=gHeight;}
}



/* LoadPPM -- loads the specified ppm file, and returns the image data as a GLubyte
 *  (unsigned byte) array. Also returns the width and height of the image, and the
 *  maximum colour value by way of arguments
 *  usage: GLubyte myImg = LoadPPM("myImg.ppm", &width, &height, &max);
 */
GLubyte* LoadPPM(char* file, int* wdth, int* hight, int* mx)
{
    GLubyte* img;
    FILE *fd;
    int n, m;
    int  k, nm;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;
    
    fd = fopen(file, "r");
    fscanf(fd,"%[^\n] ",b);
    if(b[0]!='P'|| b[1] != '3')
    {
        printf("%s is not a PPM file!\n",file);
        exit(0);
    }
    printf("%s is a PPM file\n", file);
    fscanf(fd, "%c",&c);
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
    }
    ungetc(c,fd);
    fscanf(fd, "%d %d %d", &n, &m, &k);
    
    printf("%d rows  %d columns  max value= %d\n",n,m,k);
    
    nm = n*m;
    
    img = (GLubyte*)malloc(3*sizeof(GLuint)*nm);
    
    s=255.0/k;
    
    
    for(i=0;i<nm;i++)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        img[3*nm-3*i-3]=red*s;
        img[3*nm-3*i-2]=green*s;
        img[3*nm-3*i-1]=blue*s;
    }
    
    *wdth = n;
    *hight = m;
    *mx = k;
    
    return img;
}

void projection(){
    glMatrixMode(GL_TEXTURE);
    glScalef(1,-1,-1);
    
    img_data = LoadPPM("terrain.ppm", &width, &height, &max1);
        
    glEnable(GL_TEXTURE_2D);
    
    glGenTextures(2, &textures);
    
    glBindTexture(GL_TEXTURE_2D, textures);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glClearColor(0, 0, 0, 0);
    glColor3f(1, 1, 1);    

    /*Let's set up the projection here!*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,1,1,1000);
}
void generateHeightMaps(){
	for(int x = 0; x < tWidth; x++) {                                                                                                             
		for(int z = 0; z < tWidth; z++) {
			heightMap[x][z] = 0;
		}
	}

    circleArr.clear();
	for(int i = 0; i < circleArr.capacity(); i++) {
		Circle tempCirc; tempCirc.x=rand()%tWidth;tempCirc.z=rand()%tWidth;
		
		tempCirc.height=(rand()/float(RAND_MAX))*maxHeight;
		tempCirc.diam=(rand()/float(RAND_MAX))*90+ 10;
		circleArr.push_back(tempCirc);
	}
	
	for (int i = 0; i < circleArr.capacity(); i++) {
        for(int x = 0; x < tWidth-1; x++) {                                                                                                             
            for(int z = 0; z < tWidth-1; z++) {
				    float pd = sqrt(pow((x-circleArr[i].x),2) + pow((z-circleArr[i].z),2))*2/circleArr[i].diam;

                    if (fabs(pd)<=1.0){
                        //cout<<"inIf";
                        heightMap[x][z] +=  maxHeight/2 + cos(pd*3.14)*maxHeight/2;
                    }
            }
        }
    }
    for(int x = 0; x < tWidth-1; x+=2) {                                                                                                             
        for(int z = 0; z < tWidth-1; z+=2) {
                Point3D firstPoint=Point3D( x, 		heightMap[x][z], 		 z);
                Point3D secondPoint=Point3D(x, 		heightMap[x][z+1], 		(z+1));
                Point3D thirdPoint=Point3D((x+1), 	heightMap[x+1][z+1], 	(z+1));
                Point3D fourthPoint=Point3D((x+1), 	heightMap[x+1][z],		 z);

                normMap[x][z]=normalsUsingPoints(firstPoint,secondPoint,firstPoint,fourthPoint),
                normMap[x][z+1]=normalsUsingPoints(secondPoint,thirdPoint,secondPoint,firstPoint),
                normMap[x+1][z+1]=normalsUsingPoints(thirdPoint,fourthPoint,thirdPoint,secondPoint),
                normMap[x+1][z]=normalsUsingPoints(fourthPoint,firstPoint,fourthPoint,thirdPoint);
				//cout<<heightMap[x][z]<<"\n";
		}
	}
}
int main(int argc, char** argv){
    generateHeightMaps();


    std::ifstream ifile( "data/F16C_US_LOD1_v25.obj" );
    model = LoadOBJ( ifile );

    //cout<<(model[0].position==nullptr);

	printIntro();
	srand(time(0));
	//generate an array of particles slightly smaller than capacity to avoid resizing array operation in case we want to add particles
	
	
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	//glEnable(GL_DEPTH_TEST);
	//glFrustum(-2, 2, -1.0, 1.0, 2.0, 100.0);


    glutInitWindowSize(1000,1000);
    glutInitWindowPosition(50,50);
    int window=glutCreateWindow("Flight Simulator");
	callBackInit();
	
	glutReshapeFunc(reshape);
	
	for (int i=0; i<(int)(particleArr.capacity()/1.1); i++) particleArr[i]=Particle(state,rand()%2,rand()%2);
	airplane = Airplane(state);

    // Enable lighting here
	//glEnable(GL_DEPTH_TEST);

    //glEnable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    
    glShadeModel(GL_SMOOTH);

    projection();

    glPolygonMode( GL_FRONT, GL_FILL );
    glPolygonMode( GL_BACK, GL_LINE );

    glutMainLoop();
    return 0;
}
/*   
 // set up "headlamp"-like light
    glShadeModel( GL_SMOOTH );
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE ) ;
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    GLfloat position[] = { 0, 0, 1, 0 };
    glLightfv( GL_LIGHT0, GL_POSITION, position );

*/