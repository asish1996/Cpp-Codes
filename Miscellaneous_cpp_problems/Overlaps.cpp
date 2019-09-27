#include <iostream>
using namespace std;

int main(){
	int temp1;
	int temp2;
	int k = 6;
	int arr1[k] = {0,0,15,17};
	int arr2[k] = {40,20,90,36};
	for (int i=0;i<k;i++){
		for (int j=i+1;j<k;j++){
			if (arr1[i]>arr1[j]){
				temp1 = arr1[i];
				temp2 = arr2[i];
				arr1[i] = arr1[j];
				arr2[i] = arr2[j];
				arr1[j] = temp1;
				arr2[j] = temp2;}}}
	
	int rooms = 0;
	int p = 0;
	int overlaps[k] = {0};
	for (int i=0;i<k;i++){
		for (int j=0;j<k;j++){
			if (i != j){			
				if (((arr1[i]<arr1[j]) && (arr1[j]<arr2[i])) || ((arr1[i]<arr2[j]) && (arr2[j]<arr2[i]))){
					overlaps[i] +=1 ;
				}
			}
		}		
	}	
	
	int tempo1;
	for (int i=0;i<k;i++){
		for (int j=i+1;j<k;j++){
			if (overlaps[i]<overlaps[j]){
				tempo1 = overlaps[i];
				overlaps[i] = overlaps[j];
				overlaps[j] = tempo1;
			}
		}
	}		
	cout<<overlaps[0];
	return 0;
}
