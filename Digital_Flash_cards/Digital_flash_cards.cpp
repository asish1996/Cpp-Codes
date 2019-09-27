#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <cstdlib>
#include <chrono>

using namespace std;

void SwapInt(int& a, int& b)
{
	int c;
	c = b;
	b = a;
	a = c;
}

void shuffling(int& a, int& b) {
	srand(time(NULL));
	a = rand() % 11 + 1;
	b = rand() % 11 + 1;
	SwapInt(a, b);
}


void card(int i, int j, int& k) {
	cout << i << " X " << j << " = ";
	cin >> k;
}

int RightorWrong(int num1, int num2, int& num3, int& num4) {
	if (num1 * num2 != num3) {
		num4 += 1;
		cout << "Your answer is wrong and the correct answer is: " << num1 * num2 << endl;
		return 1;
	}
	return 0;
}

int main() {
	int num_cards;
	cout << "How many flash cards you want to work on ?  ";
	cin >> num_cards;
	if (num_cards < 1 || num_cards > 144) {
		cout << "\n" << "The number of cards must be between 1 and 144" << endl;
		cout << "\n" << "Please enter a number between 1 and 144:  " << endl;
		cin >> num_cards;
	}
	auto start = chrono::steady_clock::now();
	int mistake = 0;
	for (int l = 0; l < num_cards; l++) {
		int num1 = l;
		int num2 = l + 1;
		shuffling(num1, num2);
		int num3 = 0;
		card(num1, num2, num3);
		int check = RightorWrong(num1, num2, num3, mistake);
	}
	auto end = chrono::steady_clock::now();
	auto time = chrono::duration_cast<chrono::seconds>(end - start).count();
	double percent_correct = (double(num_cards - mistake) / double(num_cards)) * 100;
	cout << "\n\nYou answered " << num_cards << " problems in " << time << " seconds" << endl;
	cout << "You answered " << num_cards - mistake << " correctly " << "(" << percent_correct << "%)" << endl;

	return 0;
}

