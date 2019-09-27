#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main() {
	int days_month[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	int month, day;
	cout << "\n\nPlease enter month followed by date to get the day in the year:\n" << endl;
	cin >> month >> day;
	int month_loop = month - 1;
	int sum = 0;
	for (int i = 0; i < month_loop; i++) {
		sum += days_month[i];
		//cout << sum << endl;
	}
	sum += day;
	cout << "\n\nIt is the #" << sum << " day of the year" << endl;
	return 0;
}
