#include<iostream>
#include<fstream>
#include<math.h>

using namespace std;

//Function to calculate dT(alpha)/dx(i) * dT(beta)/dx(i)
double dTdxi(int alpha, int beta, double sip1, double sip2, double x1, double x2, double x3, double x4, double y1, double y2, double y3, double y4){
	double dTdsi[2][4] = {{0.25*(-1+sip2), 0.25*(1-sip2), 0.25*(1+sip2), 0.25-(-1-sip2)},{0.25*(sip1-1), 0.25*(-1-sip1), 0.25*(1+sip1), 0.25*(1-sip1)}};
	double dxdsi1 = 0.25*(x1*(sip2-1) + x2*(1-sip2) + x3*(1+sip2) + x4*(-1-sip2));
	double dxdsi2 = 0.25*(x1*(sip1-1) + x2*(-1-sip1) + x3*(1+sip1) + x4*(1-sip1));
	double dydsi1 = 0.25*(y1*(sip2-1) + y2*(1-sip2) + y3*(1+sip2) + y4*(-1-sip2));
	double dydsi2 = 0.25*(y1*(sip1-1) + y2*(-1-sip1) + y3*(1+sip1) + y4*(1-sip1));
	double J[2][2] = {{dydsi2 , -dxdsi2}, {-dydsi1 , dxdsi1}};//jacobian(sip1,sip2,x1,x2,x3,x4,y1,y2,y3,y4);
	double T_alpha_si[2][1] = {{dTdsi[0][alpha]*J[0][0] + dTdsi[1][alpha]*J[1][0]},{dTdsi[0][alpha]*J[0][1] + dTdsi[1][alpha]*J[1][1]}};
	double T_beta_si[2][1] = {{dTdsi[0][beta]*J[0][0] + dTdsi[1][beta]*J[1][0]},{dTdsi[0][beta]*J[0][1] + dTdsi[1][beta]*J[1][1]}};
	return T_alpha_si[0][0]*T_beta_si[0][0] + T_alpha_si[1][0]*T_beta_si[1][0];
	  
}

int main()
{
	int num_elements = 9;
	int num_nodes = 16;
	int ng = 2; //no.of gauss points
	double x_cord[num_nodes] = {0,0.4,0.4,0,0.8,0.8,0,1.2,1.2,0.4,0,2,1.5,1,0.5,0}; // X-coordinates node wise
	double y_cord[num_nodes] = {0,0,0.2,0.2,0,0.4,0.4,0,0.2,0.6,0.6,0,0.25,0.5,0.75,1}; // Y-coordinates node wise
	int node_connec[num_elements][4] = {{1,2,3,4},{2,5,6,3},{4,3,6,7},{5,8,9,6},{7,6,10,11},{8,12,13,9},{9,13,14,6},{6,14,15,10},{11,10,15,16}}; //Element connectivity
	double K_stiff[num_nodes][num_nodes] = {0};
	double gauss_pts[2] = {-0.57735,0.57735};
	double gauss_wts[2] = {1,1};
	double u[num_nodes] = {0};
	
	//Global stiffness matrix formation
	
	for (int ele=0;ele<num_elements;ele++){ 
		for (int I=0;I<num_nodes;I++){
			for (int J=0;J<num_nodes;J++){
				for (int p=0;p<ng;p++){
					for (int alpha=0;alpha<4;alpha++){
						for (int beta=0;beta<4;beta++){
							if (I==node_connec[ele][alpha]-1){
								if(J==node_connec[ele][beta]-1){
										K_stiff[I][J] += dTdxi(alpha,beta,gauss_pts[p],gauss_pts[p],x_cord[node_connec[ele][0]-1],x_cord[node_connec[ele][1]-1],x_cord[node_connec[ele][2]-1],x_cord[node_connec[ele][3]-1],y_cord[node_connec[ele][0]-1],y_cord[node_connec[ele][1]-1],y_cord[node_connec[ele][2]-1],y_cord[node_connec[ele][3]-1]);										
									
									}
								}
							
							}
						}
					}	 
				}
			}
		}
										//reduced stiffness after applying BCs
	double K_red[4][4] = {{K_stiff[2][2],K_stiff[2][5],K_stiff[2][8],K_stiff[2][9]},{K_stiff[5][2],K_stiff[5][5],K_stiff[5][8],K_stiff[5][9]},{K_stiff[8][2],K_stiff[8][5],K_stiff[8][8],K_stiff[8][9]},{K_stiff[9][2],K_stiff[9][5],K_stiff[9][8],K_stiff[9][9]}};
	double u_n[3] = {0.64, 0.96, 0.96}; //non-zero U's on boundary
										//load vector (solution of known part in linear equation taken to left)
	double load_vector[4] = {K_stiff[2][1]*u_n[0]+K_stiff[2][4]*u_n[1]+K_stiff[2][7]*u_n[2] , K_stiff[5][1]*u_n[0]+K_stiff[5][4]*u_n[1]+K_stiff[5][7]*u_n[2], K_stiff[8][1]*u_n[0]+K_stiff[8][4]*u_n[1]+K_stiff[8][7]*u_n[2], K_stiff[9][1]*u_n[0]+K_stiff[9][4]*u_n[1]+K_stiff[9][7]*u_n[2]};	
	
	//Gauss elemination : Two parts - Forward Elimination & Back substitution
	
	double k_r[4][4] = {{K_stiff[2][2],K_stiff[2][5],K_stiff[2][8],K_stiff[2][9]},{K_stiff[5][2],K_stiff[5][5],K_stiff[5][8],K_stiff[5][9]},{K_stiff[8][2],K_stiff[8][5],K_stiff[8][8],K_stiff[8][9]},{K_stiff[9][2],K_stiff[9][5],K_stiff[9][8],K_stiff[9][9]}};
	double l_v[4] = {K_stiff[2][1]*u_n[0]+K_stiff[2][4]*u_n[1]+K_stiff[2][7]*u_n[2] , K_stiff[5][1]*u_n[0]+K_stiff[5][4]*u_n[1]+K_stiff[5][7]*u_n[2], K_stiff[8][1]*u_n[0]+K_stiff[8][4]*u_n[1]+K_stiff[8][7]*u_n[2], K_stiff[9][1]*u_n[0]+K_stiff[9][4]*u_n[1]+K_stiff[9][7]*u_n[2]}; 
	int index = 0;

	if (k_r[0][0]==0){
		for (int i=1;i<4;i++){
			if (k_r[i][0]!=0){
				index = i;
				break;
			}
		}
	}
	l_v[0] = load_vector[index];
	l_v[index] = load_vector[0];
	for (int j=0;j<4;j++){
		k_r[0][j] = K_red[index][j];
		k_r[index][j] = K_red[0][j];
	}
	
										// Augmented matrix preparation
	double aug_mat[4][5] = {0};
	for (int i=0;i<4;i++){
		for (int j=0;j<4;j++){
			aug_mat[i][j] = k_r[i][j];
		}
	}
	for (int i=0;i<4;i++){
		aug_mat[i][4] = -l_v[i];
	}
	
									// Preparing pivot
	for (int i=0; i<4;i++){
		for (int k=i+1;k<4;k++){
			if (aug_mat[i][i]<aug_mat[k][i]){
				for (int j=0;j<=4;j++){
					double temp = aug_mat[i][j];
					aug_mat[i][j] = aug_mat[k][j];
					aug_mat[k][j] = temp;
					
				}
			}
		}
	}
								//Forward Elimination
	for (int i=0;i<4-1;i++){
        for (int k=i+1;k<4;k++){
            {
                double t=aug_mat[k][i]/aug_mat[i][i];
                for (int j=0;j<=4;j++)
                    aug_mat[k][j]=aug_mat[k][j]-t*aug_mat[i][j];   
            }
		}
	}	
							//Back substitution
	double u_req[4];
	for (int i=4-1;i>=0;i--)           
    {                        
        u_req[i]=aug_mat[i][4];                
        for (int j=i+1;j<4;j++)
            if (j!=i)            
                u_req[i]=u_req[i]-aug_mat[i][j]*u_req[j];
        u_req[i]=u_req[i]/aug_mat[i][i];            
	}
		
	ofstream file1;
	ofstream file2;
	file1.open("Q1_Global_Stiffness.csv",ios::out);
	file2.open("Q1_u_required.txt",ios::out);		
	
	cout<<"\nU at unknown nodes is (in ascending order of node numbers):\n";
    for (int i=0;i<4;i++){
        cout<<u_req[i]<<endl;
        file2<<u_req[i]<<","<<endl;
	}
	for(int i=0;i<num_nodes;i++){
		for(int j=0;j<num_nodes;j++){
			file1<<K_stiff[i][j]<<",";
			file1<<endl; //results are written to file with every row of global matrix in node numbered columns
		} 				//i.e first 16 rows represent first column and so on..
	}	
	return 0;
}
	

