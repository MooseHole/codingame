#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/

string lower(string input)
{
    int length = input.length();
    string output = "";
    for (int i = 0; i < length; i++)
    {
        char thisChar = input[i];
        if ((thisChar >= 'A') && (thisChar <= 'Z'))
        {
            // Convert to starting with 0
            thisChar -= 'A';

            // Convert to lower case
            thisChar += 'a';
        }

        output += thisChar;
    }

    return output;
}

string getLast(string input, size_t length)
{
    if (length == string::npos)
    {
        return "";
    }

    return input.substr(input.length() - length);
}

class mimeType
{
public:
    string extension;
    string type;
    int length;

    mimeType()
    {
        extension = "";
        type = "";
        length = 0;
    }
};

int mime_sort(mimeType a, mimeType b)
{
    return a.extension < b.extension;
}

int main()
{
    int N; // Number of elements which make up the association table.
    cin >> N; cin.ignore();
    int Q; // Number Q of file names to be analyzed.
    cin >> Q; cin.ignore();

    vector<mimeType> mimeTypes;

    int maxExt = 0;
    for (int i = 0; i < N; i++)
    {
        string EXT; // file extension
        string MT; // MIME type.
        cin >> EXT >> MT; cin.ignore();

        mimeType mime;

        mime.extension = "." + lower(EXT);
        mime.type = MT;
        mime.length = mime.extension.length();
        mimeTypes.push_back(mime);

        if (mime.length > maxExt)
        {
            maxExt = mime.length;
        }

    }

    sort(mimeTypes.begin(), mimeTypes.end(), mime_sort);

    string output = "";
    string names[Q];
    for (int i = 0; i < Q; i++)
    {
        string FNAME; // One file name per line.
        getline(cin, FNAME);

        size_t lastDot = FNAME.rfind('.');
        string extension = "";
        if (lastDot != string::npos)
        {
            extension = lower(FNAME.substr(lastDot));
        }
        int extensionLength = extension.length();

        string type = "UNKNOWN";

        if (extensionLength > 0)
        {
            // Check each extension
            for (int j = 0; j < N; j++)
            {
                // Too short, so skip it
                if (extensionLength < mimeTypes[j].length)
                {
                    continue;
                }

                // Get exact extension
                int comparison = extension.compare(mimeTypes[j].extension);
                if (comparison == 0)
                {
                    type = mimeTypes[j].type;
                    break;
                }
                if (comparison < 0)
                {
                    break;
                }
            }
        }

        output += type + "\n";
    }


    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;
    cout << output << endl;
}