#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>

struct norm{ float x; float y; float z;}; // struct for representing normal vectors

using namespace std;

class terrain
{
public:
    terrain();
    terrain(int wid, int len);
    vector<vector<float>> hgt; // initialization of grid
    vector<vector<norm>> normals; // initialization of normals of grid
    int gridWid;
    int gridLen;

    void circleAlgo(int num);
    void faultAlgo(int num);
    void compNorms();
};

#endif