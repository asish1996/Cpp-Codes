#include <stdio.h>
#include<stdlib.h>
#include<cstdlib>
#include <iostream>
#include <time.h>
using namespace std;

void PrintBoard(char board[])
{
	for (int r = 0; r < 3; ++r)
	{
		for (int c = 0; c < 3; ++c)
		{
			int i = r * 3 + c;
			cout << board[i];
			if (c != 2)
			{
				cout << "|";
			}
		}
		cout << "\n";
		if (r != 2)
		{
			cout << "-+-+-\n";
		}
	}
	cout << "\n\n" << endl;
}


char CheckWinner2(char board[], char s)
{
	if ((board[0] == s && board[1] == s && board[2] == s) ||
		(board[3] == s && board[4] == s && board[5] == s) ||
		(board[6] == s && board[7] == s && board[8] == s) ||
		(board[0] == s && board[3] == s && board[6] == s) ||
		(board[1] == s && board[4] == s && board[7] == s) ||
		(board[2] == s && board[5] == s && board[8] == s) ||
		(board[0] == s && board[4] == s && board[8] == s) ||
		(board[2] == s && board[4] == s && board[6] == s))
	{
		return s;
	}
	return 0;
}

char CheckWinner(char board[])
{
	if (0 != CheckWinner2(board, 'X'))
	{
		return 'X';
	}
	else if (0 != CheckWinner2(board, 'O'))
	{
		return 'O';
	}
	return 0;
}


void computer_turn(char board[]) {
	//srand(time(NULL));
	int x1 = rand() % 3;
	int y1 = rand() % 3;
	if (board[y1 * 3 + x1] == ' ') {
		board[y1 * 3 + x1] = 'O';
	}
	else {
		computer_turn(board);
	}
}


void human_turn(char board[]) {
	int input1, input2;
	cout << "\nPlease enter column number SPACE followed by row number for selecting your location:" << endl;
	cin >> input1 >> input2;
	if (input1 >= 0 && input1 <= 2 && input2 >= 0 && input2 <= 2) {
		int loc = 3 * input2 + input1;
		if (board[loc] == ' ') {
			board[loc] = 'X';
		}
		else {
			cout << "\nLocation is occupied" << endl;
			human_turn(board);
		}
	}
	else {
		cout << "\nPlease make a valid entry" << endl;
		human_turn(board);
	}
}

int main(void)
{
	char board[9];
	for (int i = 0; i < 9; ++i)
	{
		board[i] = ' ';
	}

	PrintBoard(board);
	cout << "Computer's choice is represented by 'O' and yours is represented by 'X'" << endl;
	human_turn(board);   //1
	PrintBoard(board);
	computer_turn(board); //2
	//cout << "\n\nComputer choice is\n" << endl;
	PrintBoard(board);
	human_turn(board);  //3
	PrintBoard(board);
	computer_turn(board);  //4
	//cout << "\n\nComputer choice is\n" << endl;
	PrintBoard(board);
	human_turn(board);  //5
	PrintBoard(board);
	if (CheckWinner(board) == 'X') {
		cout << "\nGameover You won!" << endl;
	}
	else {
		computer_turn(board);  //6
		//cout << "\n\nComputer choice is\n" << endl;
		PrintBoard(board);
		if (CheckWinner(board) == 'O') {
			cout << "\nComputer Won!" << endl;
		}
		else {
			human_turn(board);  //7
			PrintBoard(board);
			if (CheckWinner(board) == 'X') {
				cout << "\nGameover you won!" << endl;
			}
			else {
				computer_turn(board);  //8
				//cout << "\n\nComputer choice is\n" << endl;
				PrintBoard(board);
				if (CheckWinner(board) == 'O') {
					cout << "\nComputer Won!" << endl;
				}
				else {
					human_turn(board); //9
					PrintBoard(board);
					if (CheckWinner(board) == 'X') {
						cout << "\nGame over you won!" << endl;
					}
					else {
						cout << "\n It is a tough match & tied. Congratulations on you efforts!!" << endl;
					}

				}
			}
		}
	}

	return 0;
}