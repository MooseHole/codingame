#include <iostream>
#include <climits>
#include <map>

using namespace std;

int main()
{
    int N;
    int minX = INT_MAX;
    int maxX = INT_MIN;
    ulong length;

    map<int, int> houseYUnique;
    cin >> N; cin.ignore();
    for (int i = 0; i < N; i++)
    {
        int X;
        int Y;
        cin >> X >> Y; cin.ignore();
        
        houseYUnique[Y] += 1;

        if (X < minX)
        {
            minX = X;
        }
            
        if (X > maxX)
        {
            maxX = X;
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


    for (map<int, int>::iterator nodeY = houseYUnique.begin(); nodeY != houseYUnique.end(); ++nodeY)
    {
        ulong dedicated = abs(trunkY - (*nodeY).first) * (*nodeY).second;
        length += dedicated;
    }
        
    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << length << endl;
}
