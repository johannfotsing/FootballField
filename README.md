# FootballField
Flying robots on a football field. 

This program is a simulation of robots flying above a football field.
The simulation starts with robots (represented by teacups) evenly spaced on the field. After five seconds, they start flying towards the center of the field, at 50m of height. When they get close to that point in space, they start moving circularly in a random direction around the point. 
The goal of the exercise is to let the robots fly within 10m of that point located 50m above the field's center, with a velocity between 2 and 10 m/s. If there is a collision, the robots switch directions. 
This simulation stops when all robots are with 10m of the spot above the field's center or when it has been running for 60s. 

# How to ?
In order to compile this program, [OpenMPI](http://lsi.ugr.es/jmantas/pdp/ayuda/datos/instalaciones/Install_OpenMPI_en.pdf) and [OpenGL](https://askubuntu.com/questions/96087/how-to-install-opengl-glut-libraries) are needed. The following command is used for compilation : ```mpic++ main.cpp Vec3D.cpp Vec3D.h -lGLU -lglut -lGL -std=c++11```
The program uses an MPI with 16 processes (one main process for the computations and displays, the others for the 15 robots on the field). Run the program with : ```mpirun -np 16 ./a.out```
