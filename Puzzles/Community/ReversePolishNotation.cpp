#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <stdexcept>

using namespace std;

string printStack(deque<int>* myStack)
{
	string output = "";
	for (deque<int>::iterator it = myStack->begin(); it != myStack->end(); ++it)
	{
		if (it != myStack->begin())
		{
			output += " ";
		}

		output += to_string(*it);
	}
	
	return output;
}

void popXY(int &x, int &y, deque<int>* myStack)
{
	if (myStack->size() < 2)
	{
    	cout << "ERROR" << endl;
		exit(0);
	}

    x = myStack->back();
    myStack->pop_back();
    y = myStack->back();
    myStack->pop_back();
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    deque<int> rpnStack;

    int N;
    cin >> N; cin.ignore();
    for (int i = 0; i < N; i++) {
        string instruction;
        cin >> instruction; cin.ignore();
        
        try
        {
            int x = stoi(instruction);
            rpnStack.push_back(x);
        }
        catch (invalid_argument)
        {
            int x, y;

            if (instruction == "ADD")
            {
                popXY(x, y, &rpnStack);
                rpnStack.push_back(y + x);
            }
            else if (instruction == "SUB")
            {
                popXY(x, y, &rpnStack);
                rpnStack.push_back(y - x);
            }
            else if (instruction == "MUL")
            {
                popXY(x, y, &rpnStack);
                rpnStack.push_back(y * x);
            }
            else if (instruction == "DIV")
            {
                popXY(x, y, &rpnStack);
                if (x == 0)
                {
                	cout << "ERROR" << endl;
                    exit(2);
                }
                rpnStack.push_back(y / x);
            }
            else if (instruction == "MOD")
            {
                popXY(x, y, &rpnStack);
                if (x == 0)
                {
                	cout << "ERROR" << endl;
                    exit(2);
                }
                rpnStack.push_back(y % x);
            }
            else if (instruction == "POP")
            {
                rpnStack.pop_back();
            }
            else if (instruction == "DUP")
            {
				x = rpnStack.back();
                rpnStack.push_back(x);
            }
            else if (instruction == "SWP")
            {
                popXY(x, y, &rpnStack);
                rpnStack.push_back(x);
                rpnStack.push_back(y);
            }
            else if (instruction == "ROL")
            {
                rpnStack.pop_back();
				deque<int>::iterator it = rpnStack.end()-3;
				x = (*it);
				rpnStack.erase(it);
				rpnStack.push_back(x);
            }
        }
    }
    
	cout << printStack(&rpnStack) << endl;
}