#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>
#include <set>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int N;
    int mainY;
    int minY;
    int maxY;
    int minX;
    int maxX;
    unsigned long long int length = ULLONG_MAX;
    set<int> houseYUnique;
    vector<int> houseY;
    cin >> N; cin.ignore();
    for (int i = 0; i < N; i++) {
        int X;
        int Y;
        cin >> X >> Y; cin.ignore();
        cerr << X << " " << Y << endl;
        
        houseYUnique.insert(Y);
        houseY.push_back(Y);
        
        if (i == 0)
        {
            minX = X;
            maxX = X;
            minY = Y;
            maxY = Y;
        }
        else
        {
            if (Y < minY)
            {
                minY = Y;
            }
            
            if (Y > maxY)
            {
                maxY = Y;
            }

            if (X < minX)
            {
                minX = X;
            }
            
            if (X > maxX)
            {
                maxX = X;
            }
        }
    }
    
    int previousMainY = *(houseYUnique.begin());
    for (set<int>::iterator mainY = houseYUnique.begin(); mainY != houseYUnique.end(); ++mainY)
    {
        cerr << "Iterating at " << *mainY << endl;
        for (int i = 0; i < (previousMainY == *mainY ? 1 : 2); i++)
        {
            int placement;
            if (i == 0)
            {
                placement = *mainY;
            }
            else
            {
                placement = previousMainY + (*mainY - previousMainY) / 2;
            }
            ulong checkLength = maxX - minX;
            cerr << "Main at Y=" << placement << " main length " << checkLength << endl;
            for (vector<int>::iterator nodeY = houseY.begin(); nodeY != houseY.end(); ++nodeY)
            {
                ulong dedicated = abs(placement - *nodeY);
                checkLength += dedicated;
                cerr << "add " << dedicated << " for house at Y=" << *nodeY << " total " << checkLength << endl;
            }
        
            if (length > checkLength)
            {
                length = checkLength;
                cerr << "!SET! at length " << length << endl;
            }
        }
        
        previousMainY = *mainY;
    }

    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << length << endl;
}
