/*
Author: Johan Fotsing
Class: ECE6122
Last Date Modified: 12/06/2019
Description: 
Here are defined all the functions and the main program.
*/

#include <iostream>
//#include <GL/glew.h>
#include <GL/glut.h> // Include the GLUT header file
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h> // standard definitions
#include <thread>
#include <time.h>
#include "Vec3D.h"
#include "mpi.h"

#define N_PROC 16
#define F_MAX 20
#define PI 3.14

/////////////////////////////////////////////////////////
// Functions prototypes
/////////////////////////////////////////////////////////
double yardToMeter(const double&); 
void displayFootballField(); 
void drawUAVs(); 
void calculateUAVsLocation(int); 
void changeSize(int, int); 
void renderScene(); 
void mainOpenGL(int, char**); 
void mainTest(); 
void timerFunction(int); 

// Send location and velocity vector in each direction
const int numElements = 6; // x, y, z, vx, vy, vz
const int rcvSize = N_PROC * numElements; // (Main task + 15 UAVs) * numElements
double* rcvbuffer = new double[rcvSize];
double sendBuffer[numElements];

// Camera
double x_camera = yardToMeter(50); 
double y_camera = 0; 
double z_camera = 80; 
double focus_x = 0; 
double focus_y = 0; 
double focus_z = 0; 
int angle = 0; 

// Football Field and UAV appearance
const double fieldLength = yardToMeter(120); 
const double fieldWidth = yardToMeter(53.0+1.0/3.0); 
const double lineWidth = 0.5; 
int colorCounter = 0; 

// Control variables
Vec3D goal(0, 0, 50), vgoal; 
Vec3D uavPos, uavVel, uavAcc, uavForce; 
double k = 2e-1; 
double kRot = 8e-1; 
double uavMass = 1.0; 
Vec3D u, fSpring, fDrag, fRot;
//double vCircular, Dt, theta;   


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Main entry point determines rank of the process and follows the 
// correct program path
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])

{
    int numTasks, rank;

    int rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS) 
    {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // ----------------------------------------------------------------
    // Initialize positions and velocities
    // ----------------------------------------------------------------
    if (rank == 0)
    {
        for (int i=0; i<numElements; ++i) sendBuffer[i] = 0; 
    }
    else
    {
        int idx = (rank-1)/5; 
        int idy = (rank-1)%5;
        sendBuffer[0] = -fieldWidth/2 + idx*fieldWidth/2;  
        sendBuffer[1] = -fieldLength/2 + yardToMeter(10 + idy*25);  
        sendBuffer[2] = 0;
        sendBuffer[3] = 0;
        sendBuffer[4] = 0;
        sendBuffer[5] = 0;
        uavPos.setX(sendBuffer[0]); 
        uavPos.setY(sendBuffer[1]);
        uavPos.setZ(sendBuffer[2]);
    }
    MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE, rcvbuffer, numElements, MPI_DOUBLE, MPI_COMM_WORLD);
    u = (goal-uavPos)/((goal-uavPos).norm()); 
    srand(rank); 
    double vCircular = 2 + double(rand()%200)*8./200.0; //2 + (double(rand()%200)/200.0)*8.0; 
    Vec3D randDir(double(rand()%200-100)/200.0, double(rand()%200-100)/200.0, double(rand()%200-100)/200.0); 
    randDir = randDir-randDir.projectOn(u); 
    randDir = randDir/randDir.norm(); 
    vgoal = vCircular*randDir; 
    // -----------------------------------------------------------------

    // -----------------------------------------------------------------
    // Main Loop
    // -----------------------------------------------------------------
    if (rank == 0) 
    {
        mainOpenGL(argc, argv);
    }
    else
    {
        // Sleep for 5 seconds
        std::this_thread::sleep_for(std::chrono::seconds(5));
        for (int ii = 0; ii < 600 ; ii++)
        {
            calculateUAVsLocation(rank); 
        }
    }
    // -----------------------------------------------------------------

    return 0;
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//////////////// Function definitions
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------
// Some useful functions
//----------------------------------------------------------------------
double yardToMeter(const double &y)
{
    // Converts yard to meter
    return 0.9144*y; 
}

void displayFootballField()
{

    // Draws the football field
    glPushMatrix(); 

    glColor3d(1, 1, 1); //white
    // Draw outter rectangle
    glBegin(GL_QUADS); 
        glVertex3d(-(fieldWidth+2)/2, -(fieldLength+2)/2, 0); 
        glVertex3d(-(fieldWidth+2)/2, (fieldLength+2)/2, 0); 
        glVertex3d((fieldWidth+2)/2, (fieldLength+2)/2, 0); 
        glVertex3d((fieldWidth+2)/2, -(fieldLength+2)/2, 0);  
    glEnd();

    // Draw actual field
    glColor3f(0.0, 1.0, 0.0); //green
    glBegin(GL_QUADS); 
        glVertex3d(-fieldWidth/2, -fieldLength/2, 0); 
        glVertex3d(-fieldWidth/2, fieldLength/2, 0); 
        glVertex3d(fieldWidth/2, fieldLength/2, 0); 
        glVertex3d(fieldWidth/2, -fieldLength/2, 0); 
    glEnd(); 

    // Draw lines on the field
    glColor3f(1.0, 1.0, 1.0); //white
    double yLine; 
    for (int i=0; i<21; ++i)
    {
        yLine = -fieldLength/2 + yardToMeter(10 + i*5); 
        glBegin(GL_QUADS); 
            glVertex3d(-fieldWidth/2, yLine - lineWidth/2, 0); 
            glVertex3d(-fieldWidth/2, yLine + lineWidth/2, 0); 
            glVertex3d(fieldWidth/2, yLine + lineWidth/2, 0); 
            glVertex3d(fieldWidth/2, yLine - lineWidth/2, 0); 
        glEnd(); 
    }

    glPopMatrix();
}

void drawUAVs()
{
    // Draws the UAVs
    glColor3f(double(128+colorCounter/20)/255.0, double(128+colorCounter/20)/255.0, 0); 
    for (int i=1; i<N_PROC; ++i)
    {
        glPushMatrix();
            glTranslated(rcvbuffer[6*i], rcvbuffer[6*i+1], rcvbuffer[6*i+2]); 
            glutSolidTeapot(0.6); 
        glPopMatrix();
    }
}

void calculateUAVsLocation(int id)
{
    // Update position and speed
    //-------------------------------------------------
    uavPos += uavVel*0.1 + 0.5*uavAcc*0.01; 
    uavVel += uavAcc*0.1; 

    // Check for collisions
    //-------------------------------------------------
    int nCollisions = 0; 
    Vec3D velCollisions; 
    for (int i=1; i<N_PROC; ++i)
    {
        if (i != id)
        {
            Vec3D uavNeighborPos(rcvbuffer[numElements*i], rcvbuffer[numElements*i+1], rcvbuffer[numElements*i+2]); 
            if ((uavPos-uavNeighborPos).norm() < 1.1)
            {
                Vec3D uavNeighborVel(rcvbuffer[numElements*i+3], rcvbuffer[numElements*i+4], rcvbuffer[numElements*i+5]);
                nCollisions++; 
                velCollisions += uavNeighborVel; 
            }
        }
    }
    if (nCollisions > 0) uavVel = velCollisions/nCollisions; 

    // Update force
    //-------------------------------------------------
    double deltaX = (goal-uavPos).norm();
    u = (goal-uavPos)/deltaX; 
    fSpring = k*deltaX*u; 
    fDrag = -2*sqrt(k)*uavVel.projectOn(u); 
    if (deltaX < 5) fRot = kRot*(vgoal-uavVel); 
    uavForce = fSpring + fDrag + fRot; 
    // Check the limits
    if (uavForce.getX() > F_MAX) uavForce.setX(F_MAX); 
    if (uavForce.getX() < -F_MAX) uavForce.setX(-F_MAX); 
    if (uavForce.getY() > F_MAX) uavForce.setY(F_MAX); 
    if (uavForce.getY() < -F_MAX) uavForce.setY(-F_MAX); 
    if (uavForce.getZ() > F_MAX) uavForce.setZ(F_MAX); 
    if (uavForce.getZ() < -F_MAX) uavForce.setZ(-F_MAX); 

    // Update acceleration
    //-------------------------------------------------
    uavAcc = uavForce/uavMass; 

    // Update send buffers
    sendBuffer[0] = uavPos.getX(); 
    sendBuffer[1] = uavPos.getY(); 
    sendBuffer[2] = uavPos.getZ();
    sendBuffer[3] = uavVel.getX(); 
    sendBuffer[4] = uavVel.getY(); 
    sendBuffer[5] = uavVel.getZ(); 

    // Share position and speed
    //-------------------------------------------------
    MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE, rcvbuffer, numElements, MPI_DOUBLE, MPI_COMM_WORLD);
}

//----------------------------------------------------------------------
// Reshape callback
//
// Window size has been set/changed to w by h pixels. Set the camera
// perspective to 45 degree vertical field of view, a window aspect
// ratio of w/h, a near clipping plane at depth 1, and a far clipping
// plane at depth 100. The viewport is the entire window.
//
//----------------------------------------------------------------------
void changeSize(int w, int h)
{
    float ratio = ((float)w) / ((float)h); // window aspect ratio
    glMatrixMode(GL_PROJECTION); // projection matrix is active
    glLoadIdentity(); // reset the projection
    gluPerspective(60.0, ratio, 0.1, 1000.0); // perspective transformation
    glMatrixMode(GL_MODELVIEW); // return to modelview mode
    glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

//----------------------------------------------------------------------
// Draw the entire scene
//----------------------------------------------------------------------
void renderScene()
{
    // This function is called when OpenGL draws the entire scene

    // Clear color and depth buffers
    glClearColor(0.0, 0.0, 0.0, 1.0); // background color to green??
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();


    gluLookAt(x_camera, y_camera, z_camera, 
              focus_x, focus_y, focus_z,
              0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix(); 
    glRotatef(-angle, 0.0, 0.0, 1.0);

    displayFootballField();

    drawUAVs();

    glPopMatrix(); 

    glutSwapBuffers(); // Make it all visible

    MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE, rcvbuffer, numElements, MPI_DOUBLE, MPI_COMM_WORLD);

    // Print positions and velocities
    for (int i=1; i<N_PROC; ++i)
    {
        Vec3D pos(rcvbuffer[numElements*i], rcvbuffer[numElements*i+1], rcvbuffer[numElements*i+2]); 
        Vec3D vel(rcvbuffer[numElements*i+3], rcvbuffer[numElements*i+4], rcvbuffer[numElements*i+5]); 
        cout << "Process " << i; 
        if (i<10) cout << "  || Velocity : " ; 
        else cout << " || Velocity : " ;
        cout << vel.norm() << " || Distance : " << (goal - pos).norm() << endl;
    }
    cout << endl;

    // Update camera focus : Look at the center of gravity of the UAVs
    focus_x = 0; 
    focus_y = 0; 
    focus_z = 0; 
    for (int i=1; i<N_PROC; ++i)
    {
        focus_x += rcvbuffer[6*i]/double(N_PROC-1); 
        focus_y += rcvbuffer[6*i+1]/double(N_PROC-1); 
        focus_z += rcvbuffer[6*i+2]/double(N_PROC-1); 
    }

    // Change color intensity
    colorCounter = (colorCounter+1)%128*20; 
}

//---------------------------------------------------------------------
void processSpecialKeys(int key, int x, int y)
{
    // Use the keyboard arrows to rotate the scene 
    // and move the camera up and down
    switch (key) 
    {
    case GLUT_KEY_RIGHT: angle = (angle + 2) % 360; break;
    case GLUT_KEY_LEFT: angle = (angle - 2) % 360; break;
    case GLUT_KEY_UP: z_camera += 1; break;
    case GLUT_KEY_DOWN: z_camera -= 1; break;
    }
}

//----------------------------------------------------------------------
// mainOpenGL  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
void mainOpenGL(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);
    glutCreateWindow("Football Field");

    glutReshapeFunc(changeSize);
    glutDisplayFunc(renderScene);
    glutTimerFunc(100, timerFunction, 0);
    glutSpecialFunc(processSpecialKeys); 
    glutMainLoop();
}

//----------------------------------------------------------------------
// timerFunction  - called whenever the timer fires
//----------------------------------------------------------------------
void timerFunction(int id)
{
    glutPostRedisplay();
    glutTimerFunc(100, timerFunction, 0);
}