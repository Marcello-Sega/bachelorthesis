clear

// Calculates the positions (x) and velocities (v) for given particles 
// using the langevin equation and a second order integrator

// Checks if the energy of the simple harmonic oscillator is conserved
// using this method and wheather the probability density of position
// is the same as the analytic expression

// CONSTANTS

k_B = 1

// FUNCTIONS

function f = force(x)
// Returns the value for the force f = -k*x due to the potential V = (1/2)*k*x^2

// Input:   x: vector (coordinates)
// Output:  f: vector
// CONSTANTS: k
    k = 2
    f = -k*x
endfunction

function [x,v] = velocity_verlet(dt,n,T,g,m,x0,v0,forcefield)
// Returns a vector containing all positions obtained using 
// a generalization of the velocity verlet algorithm in order to
// integrate the langevin equation in one dimension

// Input:   dt: number (timestep)
//          n: number (amount of steps)
//          T: number (temperature)
//          g: number (gamma factor: friction coefficient)
//          m: number (mass of particle)
//          x0: rowvector (initial position)
//          v0: rowvector (initial velocity)
//          forcefield: function (a function which returns the values for the force)
// Output:  x: nxd matrix (coordinates where d is the dimension)
//          v: nxd matrix (velocities where d is the dimension)
    sigma = sqrt(2*k_B*T*g*m^(-1))
    d = length(x0)
    x(1,1:d) = x0
    v(1,1:d) = v0
    for i = 1:1:n
        v_half = v(i,1:d) + (1/2)*dt*(1/m)*forcefield(x(i,1:d)) - (1/2)*dt*g*v(i,1:d) + (1/2)*sqrt(dt)*sigma*grand(1,d,"nor",0,1) - (1/8)*dt^2*g*((1/m)*forcefield(x(i,1:d)) - g*v(i,1:d)) - (1/4)*dt^(3/2)*g*sigma*((1/2)*grand(1,d,"nor",0,1) + (1/sqrt(3))*grand(1,d,"nor",0,1))
        x(i+1,1:d) = x(i,1:d) + dt*v_half + dt^(3/2)*sigma*(1/sqrt(12))*grand(1,d,"nor",0,1)
        v(i+1,1:d) = v_half + (1/2)*dt*(1/m)*forcefield(x(i+1,1:d)) - (1/2)*dt*g*v_half + (1/2)*sqrt(dt)*sigma*grand(1,d,"nor",0,1) - (1/8)*dt^2*g*((1/m)*forcefield(x(i+1,1:d)) - g*v_half) - (1/4)*dt^(3/2)*g*sigma*((1/2)*grand(1,d,"nor",0,1) + (1/sqrt(3))*grand(1,d,"nor",0,1))
    end
endfunction

// PARAMETERS

dt = 10^-2
n = 10000
T = 100
g = 0
m = 2 
x0 = 0
v0 = rand()
k = 2

[x,v] = velocity_verlet(dt,n,T,g,m,x0,v0,force)

// Check if energy is conserved and P(x) satisifies the analytic expression

K = (m*v.^2)/2
U = (k*x.^2)/2

E = K(1)+U(1)
x_ana = (-max(x)+0.002:0.001:max(x)-0.002)'
t = (0:dt:n*dt)'
P = (1/%pi)*sqrt(k)*(2*E-k*x_ana.^2)^(-1/2)
subplot(121)
histplot(100,x)
plot(x_ana,P,"r")
legend("Numerical solution","Analytic solution")
title("$P(x) \text{ for } V(x) = \frac{1}{2} x^2 \text{ and } \gamma = 0$")
subplot(122)
plot2d(t,[K,U,K+U],rect=[0,0,10,E*1.1])
legend(["Kinetic Energy","Potential Energy","Total Energy"])


