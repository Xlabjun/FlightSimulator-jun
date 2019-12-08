#include <time.h>
#include <math.h>
#include "terrain.h"
#include <stdlib.h>
#include <cstring>

using namespace std;

terrain::terrain() {
    // set default terrain dimensions
    gridLen = 50;
    gridWid = 50;

    hgt = new float* [gridWid];

    // set default height map for flat terrain
    for (int x = 0; x < gridWid; x++) {
        hgt[x] = new float[gridLen];
        memset(hgt[x], 0, gridLen*sizeof(float));
    }

    normals = new norm* [gridWid];

    // set default face normals for all vertices of terrain
    for (int x = 0; x < gridWid; x++) {
        normals[x] = new norm[gridLen];
        memset(normals[x], {0,0,1}, gridLen*sizeof(norm));
    }
}

terrain::terrain(int wid, int len) {
    // set terrain dimensions
    gridLen = len;
    gridWid = wid;

    hgt = new float* [gridWid];

    // set default height map for flat terrain
    for (int x = 0; x < gridWid; x++) {
        hgt[x] = new float[gridLen];
        memset(hgt[x], 0, gridLen*sizeof(float));
    }

    normals = new norm* [gridWid];

    // set default face normals for all vertices of terrain
    for (int x = 0; x < gridWid; x++) {
        normals[x] = new norm[gridLen];
        memset(normals[x], {0,0,1}, gridLen*sizeof(norm));
    }
}

void terrain::circleAlgo(int iteration) {
	srand(time(NULL));
	float pd,dispAux;
    // populates terrain with randomly place, randomly size, and random disperstion of circles
	for (int k = 0; k < iteration;k++) {
		float z = (rand() % gridLen);
		float x = (rand() % gridWid);
		float r = (rand() % (gridWid/2)) + 3;
		float disp = (rand() % 10);
		for (int i = 0;i < gridWid; i++)
			for(int j = 0; j < gridLen; j++) {
					pd = (sqrt(((i-x)*(i-x) + (j-z)*(j-z)))*2)/r;
					dispAux =  (disp/3) + (cos(pd*3.14))*(disp/3);
				if (fabs(pd) <= 1.0) {
					hgt[i][j] += dispAux;
				}
			}
	}
}

void terrain::faultAlgo(int num) {
    int maxHeight = 0;
    int minHeight = 0;

    //run the algorithm for a 'num' amount of iterations
    for(int i = 0; i < num; i++){
        float v = static_cast <float> (rand()); // a random float between 0-1
        float a = sin(v); //sin of the number for the x co-effcient 
        float b = cos(v); //take the cos of the number for the y co-effcient 
        float d = sqrt(gridWid * gridWid + gridLen * gridLen);   //distance from the furthest point 
        // random c value smaller then the distance from the furthest point of the terrain
        float c = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * d - d / 2;
        // for each x,z point on the map determine if increase or decrease height
        for(int x = 0; x < gridWid; x++){
            for(int z = 0; z < gridLen; z++){
                if (a * x + b * z - c > 0){ //increase height
                    hgt[x][z] += 1;
                    if(hgt[x][z] > maxHeight){
                        maxHeight = hgt[x][z]; //calculate new max-height value
                    }
                }
                else {
                    hgt[x][z] -= 1; //decrease height
                    if(hgt[x][z] < minHeight){
                        minHeight = hgt[x][z]; //calculate new min-height value
                    }
                }
            }
        }
    }
}

void terrain::compNorms() {
    norm v1,v2,v;

    for (int x = 0; x < gridWid-1; x++) {
        for (int z = 0; z < gridLen-1; z++) {
            v1.x = x + 1;
            v1.y = hgt[x+1][z] - hgt[x][z];
            v1.z = z;
            v2.x = x + 1;
            v2.y = hgt[x+1][z+1] - hgt[x][z];
            v2.z = z + 1;

            v.x = (v1.y*v2.z) - (v1.z*v2.y);
            v.y = (v1.z*v2.x) - (v1.x*v2.z);
            v.z = (v1.z*v2.y) - (v1.x*v2.x);
            float len = sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
            v.x = v.x/len;
            v.y = v.y/len;
            v.z = v.z/len;

            normals[x][z] = v;
        }
    }
}