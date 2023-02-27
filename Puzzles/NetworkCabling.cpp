#include <iostream>
#include <string>
#include <algorithm>
#include <climits>
#include <map>

using namespace std;

int main()
{
    int N;
    int mainY;
    int minY;
    int maxY;
    int minX;
    int maxX;
    unsigned long long int length = ULLONG_MAX;
    map<int, int> houseYUnique;
    cin >> N; cin.ignore();
    for (int i = 0; i < N; i++) {
        int X;
        int Y;
        cin >> X >> Y; cin.ignore();
        cerr << X << " " << Y << endl;
        
        houseYUnique[Y] += 1;
        
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
    
    for (map<int, int>::iterator mainY = houseYUnique.begin(); mainY != houseYUnique.end(); ++mainY)
    {
        cerr << "Iterating at " << (*mainY).first << endl;
        {
            int placement = (*mainY).first;

            ulong checkLength = maxX - minX;
            cerr << "Main at Y=" << placement << " main length " << checkLength << endl;

            for (map<int, int>::iterator nodeY = houseYUnique.begin(); nodeY != houseYUnique.end(); ++nodeY)
            {
                ulong dedicated = abs(placement - (*nodeY).first) * (*nodeY).second;
                checkLength += dedicated;
                cerr << "add " << dedicated << " for house at Y=" << (*nodeY).first << " total " << checkLength << endl;
            }
        
            if (length > checkLength)
            {
                length = checkLength;
                cerr << "!SET! at length " << length << endl;
            }
        }
    }

    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << length << endl;
}
