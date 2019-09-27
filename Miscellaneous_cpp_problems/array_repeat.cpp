#include<iostream>
using namespace std;

int main(){
	int order = 7;
	int arr[order] = {6,10,3,3,3,6,6};
	int count = 0;
	int req_num = 0;
	for (int i=0;i<order;i++){
		int temp_num = arr[i];
		for (int j=0;j<order;j++){
			if (arr[j] == temp_num){
				count+=1;
			}
		}
		if (count == 1){
			req_num = temp_num;
		}
		count = 0;
	}
	cout << req_num;
}
