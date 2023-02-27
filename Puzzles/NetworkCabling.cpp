#include <iostream>
#include <string>
#include <algorithm>
#include <climits>
#include <map>

using namespace std;

int main()
{
    int N;
    int minY;
    int maxY;
    int minX;
    int maxX;
    ulong length;

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

    int medianIndex = N/2;
    int trunkY = 0;
    ulong trunkLength = maxX - minX;
    int index = 0;
    for (map<int, int>::iterator houseY = houseYUnique.begin(); houseY != houseYUnique.end(); ++houseY)
    {
        bool isMedian = false;
        if (index <= medianIndex && (index + (*houseY).second) > medianIndex)
        {
            isMedian = true;
        }

        index += (*houseY).second;

        if (isMedian)
        {
            trunkY = (*houseY).first;
            break;
        }
    }

    length = trunkLength;

    cerr << "Trunk at Y=" << trunkY << " Trunk length " << trunkLength << endl;

    for (map<int, int>::iterator nodeY = houseYUnique.begin(); nodeY != houseYUnique.end(); ++nodeY)
    {
        ulong dedicated = abs(trunkY - (*nodeY).first) * (*nodeY).second;
        length += dedicated;
        cerr << "add " << dedicated << " for house at Y=" << (*nodeY).first << " total " << length << endl;
    }
        
    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << length << endl;
}
