#include<iostream>
using namespace std;
const int k = 3;

int max(int mat[k]){
	int maximum = mat[0];
	for (int i=1;i<k;i++){
		if (mat[i]>maximum){
			maximum = mat[i];
		}
	}
	cout<<maximum<<"   ";
}

int main(){
	int n = 6;
	int array[n] = {10,5,2,7,8,7};
	int temp_array[k] = {0};
	for (int i=0;i<n+1-k;i++){
		for (int j=0;j<k;j++){
			temp_array[j] = array[i+j];		
		}		
		max(temp_array);
	} 
}


