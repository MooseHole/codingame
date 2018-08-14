#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string>

int main(int argc, const char * argv[])
{
    char psBuffer[128];
    FILE *pPipe;
    if ((pPipe = _popen("java -jar ..\\..\\LegendsOfCodeAndMagic_Artifacts\\cg-brutaltester.jar -r \"java -jar -Dleague.level=4 ..\\..\\LegendsOfCodeAndMagic_Artifacts\\LegendsOfCodeAndMagic-1.0.jar\" -p1 \"..\\x64\\Debug\\LegendsOfCodeAndMagic.exe\" -p2 \"..\\x64\\Debug\\LegendsOfCodeAndMagic.exe 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" -t 2 -n 4 -s", "rt")) == NULL)
        exit(1);
    while (fgets(psBuffer, 128, pPipe)) {
        if (!((std::string)psBuffer).compare(0, 10, "| Player 1"))
        {
            printf(psBuffer);
        }
    }

    if (feof(pPipe))
        printf("\nProcess returned %d\n", _pclose(pPipe));
    getchar();
}
