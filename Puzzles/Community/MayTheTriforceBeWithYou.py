#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int N;
    cin >> N; cin.ignore();
    
	// Top triangle
	bool firstChar = true;
	int numStars = 1;
    for (int y = 0; y < N; y++)
	{
		for (int space = 0; space < N - (y + 1); space++)
		{
			if (firstChar)
			{
				cout << ".";
				firstChar = false;
			}
			else
			{
				cout << " ";
			}
		}
		
		for (int star = 0; star < numStars; star++)
		{
			cout << "*";
		}
		numStars += 2;
		cout << endl;
	}
	
	// Bottom triangles
	numStars = 1;
    for (int y = 0; y < N; y++)
	{
		for (int space = 0; space < N - (y + 1); space++)
		{
			cout << " ";
		}
		
		for (int star = 0; star < numStars; star++)
		{
			cout << "*";
		}

		for (int space = 0; space < N*2 - (y + 1); space++)
		{
			cout << " ";
		}

		for (int star = 0; star < numStars; star++)
		{
			cout << "*";
		}


		numStars += 2;
		cout << endl;
	}

    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

//    cout << "answer" << endl;
}
