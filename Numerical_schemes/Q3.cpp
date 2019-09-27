#include<iostream>
#include<fstream>
#include<math.h>

using namespace std;

int LW(double t, double dt, double h, double c)
{
	int ts = t/dt + 1; //No.of time steps
	int nx = 2/h + 1; //space discretization
	double x_cord[nx];
	x_cord[0] = -1;
	for(int i=1;i<nx;i++)
	{
		x_cord[i]=x_cord[i-1]+h;
	}
	double u[ts][nx]; //u over space and time
	// implementing initial condition
	for(int i=0;i<nx;i++)
	{
		u[0][i] = exp(-25.0*x_cord[i]*x_cord[i])-exp(-25.0);
	}
	// implementing boundary condition
	for(int i=0;i<ts;i++)
	{
		u[i][0] = 0;
	}
	//solving problem
	for(int i=1;i<ts;i++)
	{
		for(int j=1;j<nx;j++)
		{
			u[i][j]= ((c*dt)/(2*h))*(u[i-1][j+1]-u[i-1][j-1]) + ((c*c*dt*dt)/(2*h*h))*(u[i-1][j+1]-2*(u[i-1][j])+u[i-1][j-1]) + u[i-1][j];
		}
	}
	ofstream file1;
	file1.open("MT1Q3.csv",ios::out);
	
	for(int i=0;i<ts;i++)
	{
		for(int j=0;j<nx;j++)
		{
			file1<<u[i][j]<<",";
			file1<<endl; //results are written to file; u at a particular time step over discretized x are enumerated
		} //i.e first nx rows represent u of first time step and so on..
	}
	return 0;
}

int main()
{
	LW(2,0.1,0.1,-1); //dt = 0.1s & h=0.1 with time=2s as given in question
	return 0;
}
