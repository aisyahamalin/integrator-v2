#include<iostream> 
#include<fstream>   
#include<string>
#include<functional>
#include<cmath>
#include<random>
#include<thread>
#include<mutex>
#include<algorithm>
#include <math.h>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

#include "potential.h" //to import the header file where all the definitions live
#include "star.h" //to import the header file where all the definitions live

#define DIMENSION 3
#define GM 10.0   //G=1
//Mass of galaxy M = 10 solar_mass

using namespace std;
using std::setprecision;  //for decimal precision...

//=========================================================================
//definitions for potential class
//========================================================================
//--------------------------------------------------------------------------
//defining the getter functions
//a function returning the Kepler potential, depending on r
double potential::getpot(double *pos) {
        double r = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);
        return (-GM/(r));
}
//--------------------------------------------------------------------------
//a function giving the force, for each dimension x, y, z
void potential::getforce(double *pos, double *force) {
    double r = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);
    double rm3 = 1.0/(pow(r, 3.0));

    for (int i = 0; i < DIMENSION; i++) {
        force[i] = -GM*pos[i]*rm3;
    }
}


//========================================================================
//definitions for star class
//========================================================================
void star::setstar(double *x, double *v) {
    for (int i = 0; i < DIMENSION; i++) {
        q[i] = x[i];   //assigning the position
        p[i] = v[i];   // assigning the velocity
    }
}//----------------------------------------------------------------------------------------------------
//function for varying the time-step h, depending on r and v values of particle
double star::settime(double e){
    double mod_R = sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2]);
    double mod_V = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
    double dt;
    return dt = e*mod_R/mod_V;
}//----------------------------------------------------------------------------------------------------
void star::getforce(double *force, potential *Phi) {
    Phi->getforce(q, force);  //calling getforce with position q and force from potential class Phi
}                             // -> is the arrow operator, to reference individual members of classes
//----------------------------------------------------------------------------------------------------
//calculating the kinetic energy and potential, to get total energy--------------------------------------
double star::getE(potential *Phi) {
    double Ekin = 0.0;
    for (int i = 0; i < 3; i++) {
        Ekin += 0.5*p[i]*p[i]; //0.5*velocity^2
    }
    return (Ekin + Phi->getpot(q));
}//----------------------------------------------------------------------------------------------------
//void function for printing the coordinates to screen
void star::printcoords() {
    for (int i = 0; i < DIMENSION; i++) {
        cout << q[i] << " " << p[i] << " ";
    }   //the first column gives the position, the second one gives velocity
    cout << "\n";
}
//----------------------------------------------------------------------------------------------------
void star::printfile(ofstream& fileout){
    fileout << q[0] << " " << q[1] << " " << q[2] << " " << p[0] << " " << p[1] << " " << p[2]  << " " << E << " " << L[0] << " " << L[1] << " " << L[2] << endl;
}

//====================================================================
//=========================================================================
//Leapfrog method
//void function for the drift [updating the position of x, y, z each time]
void star::drift(double h) {
        for (int i = 0; i < DIMENSION; i++) {
            q[i] += (h*p[i]);
        } //position += h*velocity
}
//void function for the kick [updating the velocity using force/acceleration]
void star::kick(double h, double *force) {
    for (int i = 0; i < DIMENSION; i++) {
        p[i] += h*force[i];
    } //velocity += h*force  (force is the acceleration with point mass m)
}
//void function for the leapfrog method!!!
void star::leapfrog(double h, potential *Phi) {
    double h2 = h*0.5;
    double *force = new double[3];  //'new' operator requesting to allocate memory dynamically, here there are 3 elements...
    getforce(force, Phi);
    drift(h2);        //drift
    kick(h, force);   //kick
    drift(h2);        //drift
    
    for (int i = 0; i < DIMENSION; i++){
        L[i] += p[i]*q[i]; //velocity*position
    }
    
    delete [] force; //deleting memory so it won't take up space allocated by the 'new' operator
    E = getE(Phi);
}
//====================================================================
//=========================================================================


//====================================================================
//===========================================================
//RK4 method
void star::runge_kutta(double h, potential *Phi){
    
    double *force = new double[3]; //creating memory from the pointer
    getforce(force, Phi);
    stepA(h,force);
    stepB(h,force);
    stepC(h,force);
    stepD(h,force);
    
    for (int i = 0; i < DIMENSION; i++){
        //position q update
        q[i] += h/6.0 *(k1r[i] + k2r[i] + k3r[i] + k4r[i]);
        //velocity p update
        p[i] += h/6.0 *(k1v[i] + k2v[i] + k3v[i] + k4v[i]);
        
        L[i] += p[i]*q[i]; //velocity*position
    }
    
    E = getE(Phi);
    //E*=1.0e11;
    
    delete [] force;
}
//====================================================================
//in each step, loop over each array element
//the derivative in the k1 direction
void star::stepA(double h, double *force){
    for (int i = 0; i < DIMENSION; i++){
        k1r[i] = p[i];     //k1r = velocity
        k1v[i] = force[i];     //k1v = acceleration
    }
}
//the derivative in the k2 direction
void star::stepB(double h, double *force){
    for (int i = 0; i < DIMENSION; i++){
        k2r[i] = p[i] + k1r[i]*h/2.0;   //velocity + k1r h/2
        k2v[i] = force[i] + k1v[i]*h/2.0;   //accel + k1v h/2
    }
}
//the derivative in the k3 direction
void star::stepC(double h, double *force){
    for (int i = 0; i < DIMENSION; i++){
        k3r[i] = p[i] + k2r[i]*h/2.0;//velocity + k2r h/2
        k3v[i] = force[i] + k2v[i]*h/2.0;  //accel + k2v h/2
    }
}
//the derivative in the k4 direction
void star::stepD(double h, double *force){
    for (int i = 0; i < DIMENSION; i++){
        k4r[i] = p[i] + k3r[i]*h;     //velocity + k3r h
        k4v[i] = force[i] + k3v[i]*h;     //accel + k3v h
    }
}
//====================================================================
//=========================================================================
