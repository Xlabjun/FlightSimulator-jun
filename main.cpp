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
#include "terrain.h"
#include <vector>

using namespace std;

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

terrain heightmap = terrain(500,500);

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
bool lightingEnabled=true, fixedCamera=false;

int gWidth=1000, gHeight=1000;
vector<Particle> particleArr(100);//2000-3000 particle capacity  rand()%1000+3000
Point3D mousePos;
State state = NEUTRAL;
Airplane airplane=Airplane(state);

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
void drawPlane(){
    for (int x = 0; x < heightmap.gridWid-1; x++) {
        glBegin(GL_QUADS);
            for (int z = 0; z < heightmap.gridLen-1; z++) {
                glColor3f((float)heightmap.hgt[x][z]/30,heightmap.hgt[x][z]/30,(float)heightmap.hgt[x][z]/30);
                glNormal3f(heightmap.normals[x][z].x,heightmap.normals[x][z].y,heightmap.normals[x][z].z);
                glVertex3f(x, heightmap.hgt[x][z], z);

                glColor3f((float)heightmap.hgt[x][z+1]/30,heightmap.hgt[x][z+1]/30,(float)heightmap.hgt[x][z+1]/30);
                glNormal3f(heightmap.normals[x][z+1].x,heightmap.normals[x][z+1].y,heightmap.normals[x][z+1].z);
                glVertex3f(x, heightmap.hgt[x][z+1], z+1);
                
                glColor3f((float)heightmap.hgt[x+1][z+1]/30,heightmap.hgt[x+1][z+1]/30,(float)heightmap.hgt[x+1][z+1]/30);
                glNormal3f(heightmap.normals[x+1][z+1].x,heightmap.normals[x+1][z+1].y,heightmap.normals[x+1][z+1].z);
                glVertex3f(x+1, heightmap.hgt[x+1][z+1], z+1);
                
                glColor3f((float)heightmap.hgt[x+1][z]/30,heightmap.hgt[x+1][z]/30,(float)heightmap.hgt[x+1][z]/30);
                glNormal3f(heightmap.normals[x+1][z].x,heightmap.normals[x+1][z].y,heightmap.normals[x+1][z].z);
                glVertex3f(x+1, heightmap.hgt[x+1][z], z);
            }
        glEnd();
    }
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
        glColor3ub( 150, 150, 150 );
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
        for (int x = 0; x < heightmap.gridWid; x++) {
            for (int z = 0; z < heightmap.gridLen; z++) {
            if (airplane.mPos.mY < heightmap.hgt[x][z] + 0.5)       {//bounce off ground
                airplane.mVel.mY=-airplane.mVel.mY/2;airplane.mAcc.mY=airplane.mAcc.mY/8;
                if (airplane.mPos.mY < heightmap.hgt[x][z] + 0.5){
                    if (airplane.frictionEnabled)airplane.mVel.multiply(airplane.friction);
                    airplane.mPos.mY=heightmap.hgt[x][z]*1.1;
            } 
        }
            }
        }
    }
}

GLdouble camOffset[] = { 0, 0.1, 0.3 }; 
GLdouble centerOffset[] = { 0, 0, 0 };


GLfloat lightPos[4] = {
     2, 300, 5, 0 
};
GLfloat lightDiffuses[4] = {
     1, 1, 1, 0.5 
};
GLfloat lightSpeculars[4] = {
    1, 1, 1, 0.5 
};
GLfloat lightAmbients[4] = {
     1, 1, 1, 0.1 
};


void configureLighting(){
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,   lightDiffuses);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpeculars);
    glLightfv(GL_LIGHT0, GL_AMBIENT,   lightAmbients);
}

void display(void) {
    configureLighting();
    airplane.update();
    collision();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glLoadIdentity();

	//glMatrixMode( GL_MODELVIEW );
	if (!fixedCamera){
    Vec3D offsetVec=airplane.mVel.normalize();
        gluLookAt(airplane.mPos.mX-offsetVec.mX,airplane.mPos.mY-offsetVec.mY,airplane.mPos.mZ-offsetVec.mZ, airplane.mPos.mX,airplane.mPos.mY+centerOffset[1],airplane.mPos.mZ, 0,1,0);
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
        glTranslatef(-heightmap.gridWid/2,0,0);
		drawPlane();
	//pop the matrix back to what it was prior to the rotation
	glPopMatrix();


	glutSwapBuffers();//swap buffers for smooth animation
	glutPostRedisplay();
}

void handleKeyboard(unsigned char key, int _x, int _y) {
    if (key == 27) {//escape key
        exit(0);
    
	} else if (key == '.') {
        camOffset[0] +=15;
    } else if (key == 'm') {
        camOffset[0] -=15;
    } else if (key == 'k') {
        camOffset[1] +=10;
    } else if (key == ',') {
        camOffset[1] -=10;
    } else if (key == 'i') {
        camOffset[2] -=10;
    } else if (key == 'o') {
        camOffset[2] +=10;

	} else if (key == 'd') {
        airplane.mAcc.mX+=0.0001;
    } else if (key == 'a') {
        airplane.mAcc.mX-=0.0001;
    } else if (key == 'w') {
        cout<<airplane.mVel.mY<<"\n";
		airplane.mAcc.mY-=0.00005;
    } else if (key == 's') {
        airplane.mAcc.mY+=0.00005;
    } else if (key == 'x') {
        airplane.mAcc.mZ-=.00005;
    } else if (key == 'z') {
        airplane.mAcc.mZ+=.00005;

	} else if (key == 'j') {
        //lightPos[1][0] +=15;
    } else if (key == 'g') {
        //lightPos[1][0] -=15;
    } else if (key == 'y') {
        //lightPos[1][1] +=10;
    } else if (key == 'h') {
        //lightPos[1][1] -=10;
    } else if (key == 't') {
        //lightPos[1][2] -=10;
    } else if (key == 'u') {
        //lightPos[1][2] +=10;

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
    glutTimerFunc(30, FPS, 0); // 1sec = 1000, 60fps = 1000/60 = ~17ms per frame
}

void callBackInit(){
	glutKeyboardFunc(handleKeyboard);
	glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(display);	//registers "display" as the display callback function
	glutTimerFunc(0, FPS, 0);
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	    glPolygonMode( GL_FRONT, GL_FILL );
    glPolygonMode( GL_BACK, GL_LINE );
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
	%-32s   %-18s   %-18s\n\
	%-32s   %-18s   %-18s\n\n\
	%-32s   %-18s   %-18s\n\n"

	, "FUNCTION",  "Do\\Increase Key", "Decrease Key"

	, "Exit Program", "ESC", ""


	, "Yaw left/right", "A", "D"
	, "Pitch up/down", "S", "W"
	, "Thrust up/down", "X", "Z"

    , "Fixed/3rd Person Camera", "C", ""
	, "Camera left/right", "M", "."
	, "Camera up/down", "K", ","
	, "Camera in/out of scene", "I", "O"

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



int main(int argc, char** argv){
    //configureLighting();


    heightmap.circleAlgo(100);
    //heightmap.faultAlgo(100);

    heightmap.compNorms();

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