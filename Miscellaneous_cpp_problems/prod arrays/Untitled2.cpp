#include<iostream>
using namespace std;
const int k = 5;

int product (int mat[k],int p){
	int prod = 1;
	int q = mat[p];
	mat[p] = 1;
	for (int i=0;i<k;i++){
		prod = prod*mat[i];
	}
	mat[p] = q;
	return prod;
}

int main(){
	int array[k] = {1,2,3,4,5};
	int result[k] = {0};
	for (int i=0;i<k;i++){
		//array[i] = 1;
		result[i] = product(array,i);
	}
	
	for (int i=0;i<k;i++){
		cout<<result[i]<<"  ";
	}
	return 0;
}
