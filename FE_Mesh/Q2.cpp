#include<iostream>
#include<fstream>
#include<math.h>
using namespace std;

int plot(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, int nx, int ny)
{
	ofstream file1;
	ofstream file2;
	ofstream file3;
	
	file1.open("cords2.txt",ios::out);
	file2.open("conec2.txt",ios::out);
	file3.open("bound2.txt",ios::out);
	
	file1<<"x y"<<endl;
	
	int np = (nx+1)*(ny+1);
	int nn = nx*ny;
	int nb = 2*nx+2*ny;
	double leftx[ny+1],lefty[ny+1],rightx[ny+1],righty[ny+1];
	double coordx[np], coordy[np];
	double connect1[nn],connect2[nn],connect3[nn],connect4[nn];
	
	for (int i=0;i<=ny;i++)
	{
		leftx[i] = x1 + ((x4-x1)/ny)*i;
		lefty[i] = y1 + ((y4 - y1)/ny)*i;
		rightx[i] = x2 + ((x3 - x2)/ny)*i;
		righty[i] = y2 + ((y3-y2)/ny)*i;
	}
	for (int i=0; i<=ny;i++)
	{
		for (int j=0;j<nx;j++)
		{
			coordx[i*(nx+1)+j] = leftx[i] + ((rightx[i]-leftx[i])/nx)*j;
			coordy[i*(nx+1)+j] = lefty[i] + ((righty[i]-lefty[i])/nx)*j;
		}
	}
	for (int i=0;i<ny;i++)
	{
		for (int j=0;j<nx;j++)
		{
			connect1[i*nx+j] = i*(nx+1)+j+1;
			connect2[i*nx+j] = i*(nx+1)+j+2;
			connect3[i*nx+j] = (i+1)*(nx+1)+j+2;
			connect4[i*nx+j] = (i+1)*(nx+1)+j+1;
		}
	}
	for (int a=0;a<np;a++)
	{
		file1<<coordx[a]<<" "<<coordy[a]<<endl; //Coordinates of nodes are generated here
	}
	for (int b=0;b<nn;b++)
	{
		file2<<connect1[b]<<" "<<connect2[b]<<" "<<connect3[b]<<" "<<connect4[b]<<endl;; //Connectivity of nodes is seen here
	}
	int first[nb];
	int second[nb];
	int third [nb];
	for (int z=0;z<(2*nx+2*ny);z++)
	{
		if (z<nx)
		{
			first[z] = z+1;
			second[z] = z+2;
			third[z] = 1;
		}
		else if (z>=nx && z<nx+ny)
		{
			first[z] = second[z-1];
			second[z] = second[z-1]+(nx+1);
			third[z] = 2;
		}
		else if (z>=nx+ny && z<2*nx+ny)
		{
			first[z] = second[z-1];
			second[z] = first[z]-1;
			third[z] = 3;
		}
		else if (z>=2*nx+ny && z<2*nx+2*ny)
		{
			first[z] = second[z-1];
			second[z] = first[z] - (nx+1);
			third[z] = 4;
		}
	}
	for (int c=0;c<2*nx+2*ny;c++)
	{
		file3<<first[c]<<" "<<second[c]<<" "<<third[c]<<endl;; //boundary element nodes along with boundaries
	}
	file1.close();
	file2.close();
	file3.close();
	
	return 0;
}

int main()
{
	plot(0,0,3.5,0,3.5,4,1,3,3,4); //input to be given as plot(x1,y1,x2,y2,x3,y3,x4,y4,nx,ny)
	return 0;
}
