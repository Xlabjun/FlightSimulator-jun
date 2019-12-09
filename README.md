#  Welcome to Erik, Connor, and Akram's Flight (and Particle) Simulator 

By Erik Kredatus, Connor Hewick, and Akram Elwazani
Final Project for 3GC3

* Student #: 400062982, name: Erik Kredatus
        
* Student #: 400137397, name: Akram Elwazani
        
* Student #: 400129482, name: Connor Hewick

## CONTROLS
```
        FUNCTION                           Do\Increase Key      Decrease Key

        Exit Program                       ESC

        Yaw left/right                     A                    D                 
        Pitch up/down                      S                    W                 
        Thrust up/down                     X                    Z          

        Move Cam left/right                M                    .                 
        Move Cam up/down                   K                    ,                
        Move Cam forwar/back               I                    O               

        Move Light left/right              F                    H                 
        Move Light up/down                 T                    G                
        Move Light forwar/back             E                    Y 

        Lighting enable/disable            L
        Textures enable/disable            V
        Toggle Fixed/Following Cam         C           
```
## FEATURES
* Camera
   * Controls
   * Following View
   * Fixed View
* Lighting
   * Compute and use vertex normals
* Shading
   * Implement Gouraud (smooth) shading
* Textures
   * Stored and read from PPM File
   * Toggle textured terrain
* Terrain
   * Generation done using circles algorithm
* Physics
   * Collision detection
   * Plane movement uses speed, direction, acceleration, and velocity
   * Gravity constant
