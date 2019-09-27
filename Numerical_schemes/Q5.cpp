#include<iostream>
#include<fstream>
#include<math.h>

using namespace std;

int FR(double t, double dt, double h) // theta = 0
{
	int ts = t/dt + 1; //No.of time steps
	int nx = 2/h + 1; //space discretization
	double x_cord[nx];
	x_cord[0] = -1;
	for(int i=1;i<nx;i++)
	{
		x_cord[i]=x_cord[i-1]+h;
	}
	double v[ts][nx] = {0}; //first row; derivative wrt time
	double s[ts][nx] = {0}; //2nd row; derivative wrt x 
	
	// implementing boundary condition
	for(int i=0;i<nx;i++)
	{
		s[0][i] = -50*x_cord[i]*exp(-25*x_cord[i]*x_cord[i]);
		
	}
	//solving for v & s
	for(int i=1;i<ts;i++)
	{
		s[i][0] = (dt/h)*(v[i-1][1]-v[i-1][0]) + s[i-1][1];
		for(int j=1;j<nx;j++)
		{
			v[i][j] = (dt/(2*h))*(s[i-1][j+1] - s[i-1][j-1]) + 0.5*(v[i-1][j+1]+v[i-1][j-1]);
			s[i][j] = (dt/(2*h))*(v[i-1][j+1] - v[i-1][j-1]) + 0.5*(s[i-1][j+1]+s[i-1][j-1]);
		}
	}
	double u[ts][nx]={0}; //vector containing v & s
	
	//solving for u
	for(int i=0;i<nx;i++)
	{
		u[0][i] = exp(-25*x_cord[i]*x_cord[i])-exp(-25);
	}
	for(int i=1; i<ts;i++)
	{
		for(int j=0;j<nx;j++)
		{
			u[i][j] = u[i-1][j] + 0.5*dt*(v[i][j]+v[i-1][j]);
		}
	}
	ofstream file1;
	file1.open("MT1Q5.csv",ios::out);
	
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
	FR(2,0.1,0.1); 
	return 0;
}

