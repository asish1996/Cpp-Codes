#include<iostream>
#include<fstream>
#include<math.h>
using namespace std;

int grid(double Lx, double Ly, int nx, int ny)
{
	ofstream file1;
	ofstream file2;
	ofstream file3;

	file1.open("cords.txt", ios::out);
	file2.open("conec.txt", ios::out);
	file3.open("bound.txt", ios::out);
	file1 << "x y" << endl;
	
	int np = (nx + 1)*(ny + 1);
	int nn = nx * ny;
	int nb = 2 * nx + 2 * ny;
	double coordx[np], coordy[np];
	double connect1[nn], connect2[nn], connect3[nn], connect4[nn];
	for (int i = 0; i <= ny; i++)
	{
		for (int j = 0; j <= nx; j++)
		{
			coordy[i*(nx + 1) + j] = (-Ly / 2) + i * (Ly / ny);
			coordx[i*(nx + 1) + j] = (-Lx / 2) + j * (Lx / nx);
		}
	}
	for (int i = 0; i < ny; i++)
	{
		for (int j = 0; j < nx; j++)
		{
			connect1[i*nx + j] = i * (nx + 1) + j + 1;
			connect2[i*nx + j] = i * (nx + 1) + j + 2;
			connect3[i*nx + j] = (i + 1)*(nx + 1) + j + 2;
			connect4[i*nx + j] = (i + 1)*(nx + 1) + j + 1;
		}
	}
	for (int a = 0; a < np; a++)
	{
		file1 << coordx[a] << " " << coordy[a] << endl; //Coordinates of nodes are generated here
	}
	for (int b = 0; b < nn; b++)
	{
		file2 << connect1[b] << " " << connect2[b] << " " << connect3[b] << " " << connect4[b] << endl;; //Connectivity of nodes is seen here
	}
	int first[nb];
	int second[nb];
	int third[nb];
	for (int i = 0; i < (2 * nx + 2 * ny); i++)
	{
		if (i < nx)
		{
			first[i] = i + 1;
			second[i] = i + 2;
			third[i] = 1;
		}
		else if (i >= nx && i < nx + ny)
		{
			first[i] = second[i - 1];
			second[i] = second[i - 1] + (nx + 1);
			third[i] = 2;
		}
		else if (i >= nx + ny && i < 2 * nx + ny)
		{
			first[i] = second[i - 1];
			second[i] = first[i] - 1;
			third[i] = 3;
		}
		else if (i < 2 * nx + 2 * ny)
		{
			first[i] = second[i - 1];
			second[i] = first[i] - (nx + 1);
			third[i] = 4;
		}
	}
	for (int c = 0; c < 2 * nx + 2 * ny; c++)
	{
		file3 << first[c] << " " << second[c] << " " << third[c] << endl;; //boundary element nodes along with boundaries
	}
	file1.close();
	file2.close();
	file3.close();

	return 0;
}
int main()
{
	grid(1.5, 1.0, 5, 3); //input to be given as gird(Lx,Ly,nx,ny)
	return 0;
}
