#include<iostream>
using namespace std;
const int k = 4; //Number of colors

int smallestinrow (int mat[1][k]){
	int min = mat[0][0];
	for (int i=1;i<k;i++){
		if (mat[0][i]<min){
			min = mat[0][i];
		}
	}
	//cout<<"Least number is:"<<"  "<<min<<"   ";
	return min;
}

int indexconsidered (int mat[1][k]){
	int min = mat[0][0];
	int minindex = 0;
	for (int i=1;i<k;i++){
		if (mat[0][i]<min){
			min = mat[0][i];
			minindex = i;
		}
	}
	//cout<<"Index considered is:"<<"  "<<minindex;
	return minindex;
}

int main(){
	int n = 3;
	int matrix[n][k] = {{400,300,200,900},{600,400,300,700},{800,900,600,500}};
	//int matrix[1][k] = {2,3,1,5};
	int max_possible = 10000;
	int temp_array[1][k];
	int sum = 0;
	for (int i=0;i<n;i++){
		for (int j=0;j<k;j++){
			temp_array[0][j] = matrix[i][j];
		}
		sum+= smallestinrow(temp_array);
		matrix[i+1][indexconsidered(temp_array)] = max_possible;
	}
	cout<<"Least possible sum is:"<<"   "<<sum;
	return 0;	
}
