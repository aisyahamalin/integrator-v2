#include<iostream>  //for cout, cin
#include<fstream>   //for writing and reding from files
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

//THE MAIN FUNCTION
int main() {
    double *x = new double[DIMENSION];  //value of pointer allocated dynamical memory
    double *v = new double[DIMENSION];  //to create a memory (of actual values) in the heap
    //the values of each array
    x[0] = 8.0; x[1] = 0.0; x[2] = 0.0; //note: our sun is about 8kpc from centre
    v[0] = 0.0; v[1] = 1.0; v[2] = 0.2; //velocities in units of km/s

    star pedro; //calling it pedro
    pedro.setstar(x, v); //setting the positions x and velocities v for pedro //instantiating the object
    
    delete [] x; //deleting the allocated memory
    delete [] v; //deleting from the heap
    pedro.printcoords();


    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    //defining the step-length //which is a constant throughout code
    double h = 1.0e-6;  //unit of timeperiod is: 1 Gyr //here, time-step h is 1 000 years
    //note: my original 1.0e-6
    //-----------------------------------------------------------------
    //defining the parameter constant e
    double e = 1.0e-7; //1.0e-7; //my original 1.0e-7
    double dt;
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------


    potential Phi; //calling it Phi //making an instance of potential called phi

    
    //LEAPFROG=========================================================================================
    ofstream myfile ("coor_frog.dat", std::ios_base::app);
    for (int i = 0; i < 1000; i++) {   //one thousand times //how many times you print to file
        for (int ii = 0; ii < 100000; ii++) { //for each one time out of a thousand, do it 100,000 times
                                             //actually implementing the leapfrog //if using h=1 000 yr --> means 100,000,000 years
            dt = pedro.settime(e);
            pedro.leapfrog(dt, &Phi); //the leapfrog for that particular star
            //pedro.runge_kutta(dt, &Phi);
        }
 
        pedro.printcoords();         //printing the new coordinates after applying the leapfrog
        pedro.printfile(myfile);

            }
    myfile.close();
    //=======================================================================================
    //RUNGE-KUTTA===============================================================================
    ofstream myfile2 ("coor_RK.dat", std::ios_base::app);
    for (int i = 0; i < 1000; i++) {   //one thousand times //how many times you print to file
        for (int ii = 0; ii < 100000; ii++) { //for each one time out of a thousand, do it 100,000 times
                                             //actually implementing the integrator
            dt = pedro.settime(e);
           // pedro.leapfrog(dt, &Phi); //the leapfrog for that particular star
            pedro.runge_kutta(h, &Phi);
        }

        pedro.printcoords();         //printing the new coordinates after applying the leapfrog
        pedro.printfile(myfile2);

            }
    myfile2.close();
    //=======================================================================================

    return 0;
}
