#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string>
#include <algorithm>
#include <random>
#include <time.h>
#include <iostream>
#include <fstream>

using namespace std;

#define ITERATIONS_PER_RUN 100
#define TOTAL_RUNS         100

#define SCORE_MIN                     0
#define SCORE_MAX                    10
#define PLAYER_SCORE_MIN              0
#define PLAYER_SCORE_MAX           1000

#define LETHAL_SCORE_DEFAULT         10
#define ATTACK_SCORE_DEFAULT          2
#define BREAKTHROUGH_SCORE_DEFAULT    4
#define CHARGE_SCORE_DEFAULT          2
#define DRAIN_SCORE_DEFAULT           2
#define GUARD_SCORE_DEFAULT           3
#define GUARD_AND_WARD_SCORE_DEFAULT  5
#define WARD_SCORE_DEFAULT            2
#define HEAL_ME_SCORE_DEFAULT         2
#define DAMAGE_THEM_SCORE_DEFAULT     3
#define DRAW_SCORE_DEFAULT            5
#define PLAYER_HEALTH_SCORE_DEFAULT 100
#define PLAYER_GUARD_SCORE_DEFAULT  100
#define PLAYER_ATTACK_SCORE_DEFAULT 100
#define PLAYER_DRAW_SCORE_DEFAULT     1

#define LETHAL_SCORE_MIN         LETHAL_SCORE_DEFAULT
#define LETHAL_SCORE_MAX         LETHAL_SCORE_DEFAULT
#define ATTACK_SCORE_MIN         ATTACK_SCORE_DEFAULT
#define ATTACK_SCORE_MAX         ATTACK_SCORE_DEFAULT
#define BREAKTHROUGH_SCORE_MIN   BREAKTHROUGH_SCORE_DEFAULT
#define BREAKTHROUGH_SCORE_MAX   BREAKTHROUGH_SCORE_DEFAULT
#define CHARGE_SCORE_MIN         CHARGE_SCORE_DEFAULT
#define CHARGE_SCORE_MAX         CHARGE_SCORE_DEFAULT
#define DRAIN_SCORE_MIN          DRAIN_SCORE_DEFAULT
#define DRAIN_SCORE_MAX          DRAIN_SCORE_DEFAULT
#define GUARD_SCORE_MIN          GUARD_SCORE_DEFAULT
#define GUARD_SCORE_MAX          GUARD_SCORE_DEFAULT
#define GUARD_AND_WARD_SCORE_MIN GUARD_AND_WARD_SCORE_DEFAULT
#define GUARD_AND_WARD_SCORE_MAX GUARD_AND_WARD_SCORE_DEFAULT
#define WARD_SCORE_MIN           WARD_SCORE_DEFAULT
#define WARD_SCORE_MAX           WARD_SCORE_DEFAULT
#define HEAL_ME_SCORE_MIN        HEAL_ME_SCORE_DEFAULT
#define HEAL_ME_SCORE_MAX        HEAL_ME_SCORE_DEFAULT
#define DAMAGE_THEM_SCORE_MIN    DAMAGE_THEM_SCORE_DEFAULT
#define DAMAGE_THEM_SCORE_MAX    DAMAGE_THEM_SCORE_DEFAULT
#define DRAW_SCORE_MIN           DRAW_SCORE_DEFAULT
#define DRAW_SCORE_MAX           DRAW_SCORE_DEFAULT
//#define PLAYER_HEALTH_SCORE_MIN  PLAYER_HEALTH_SCORE_DEFAULT
//#define PLAYER_HEALTH_SCORE_MAX  PLAYER_HEALTH_SCORE_DEFAULT
//#define PLAYER_GUARD_SCORE_MIN   PLAYER_GUARD_SCORE_DEFAULT
//#define PLAYER_GUARD_SCORE_MAX   PLAYER_GUARD_SCORE_DEFAULT
//#define PLAYER_ATTACK_SCORE_MIN  PLAYER_ATTACK_SCORE_DEFAULT
//#define PLAYER_ATTACK_SCORE_MAX  PLAYER_ATTACK_SCORE_DEFAULT
#define PLAYER_DRAW_SCORE_MIN    PLAYER_DRAW_SCORE_DEFAULT
#define PLAYER_DRAW_SCORE_MAX    PLAYER_DRAW_SCORE_DEFAULT

//#define LETHAL_SCORE_MIN         SCORE_MIN
//#define LETHAL_SCORE_MAX         SCORE_MAX
//#define ATTACK_SCORE_MIN         SCORE_MIN
//#define ATTACK_SCORE_MAX         SCORE_MAX
//#define BREAKTHROUGH_SCORE_MIN   SCORE_MIN
//#define BREAKTHROUGH_SCORE_MAX   SCORE_MAX
//#define CHARGE_SCORE_MIN         SCORE_MIN
//#define CHARGE_SCORE_MAX         SCORE_MAX
//#define DRAIN_SCORE_MIN          SCORE_MIN
//#define DRAIN_SCORE_MAX          SCORE_MAX
//#define GUARD_SCORE_MIN          SCORE_MIN
//#define GUARD_SCORE_MAX          SCORE_MAX
//#define GUARD_AND_WARD_SCORE_MIN SCORE_MIN
//#define GUARD_AND_WARD_SCORE_MAX SCORE_MAX
//#define WARD_SCORE_MIN           SCORE_MIN
//#define WARD_SCORE_MAX           SCORE_MAX
//#define HEAL_ME_SCORE_MIN        SCORE_MIN
//#define HEAL_ME_SCORE_MAX        SCORE_MAX
//#define DAMAGE_THEM_SCORE_MIN    SCORE_MIN
//#define DAMAGE_THEM_SCORE_MAX    SCORE_MAX
//#define DRAW_SCORE_MIN           SCORE_MIN
//#define DRAW_SCORE_MAX           SCORE_MAX
#define PLAYER_HEALTH_SCORE_MIN  PLAYER_SCORE_MIN
#define PLAYER_HEALTH_SCORE_MAX  PLAYER_SCORE_MAX
#define PLAYER_GUARD_SCORE_MIN   PLAYER_SCORE_MIN
#define PLAYER_GUARD_SCORE_MAX   PLAYER_SCORE_MAX
#define PLAYER_ATTACK_SCORE_MIN  PLAYER_SCORE_MIN
#define PLAYER_ATTACK_SCORE_MAX  PLAYER_SCORE_MAX
//#define PLAYER_DRAW_SCORE_MIN    PLAYER_SCORE_MIN
//#define PLAYER_DRAW_SCORE_MAX    PLAYER_SCORE_MAX


mt19937 rng(time(NULL));
uniform_int_distribution<int> gen_lethal_score(LETHAL_SCORE_MIN, LETHAL_SCORE_MAX);
uniform_int_distribution<int> gen_attack_score(ATTACK_SCORE_MIN, ATTACK_SCORE_MAX);
uniform_int_distribution<int> gen_breakthrough_score(BREAKTHROUGH_SCORE_MIN, BREAKTHROUGH_SCORE_MAX);
uniform_int_distribution<int> gen_charge_score(CHARGE_SCORE_MIN, CHARGE_SCORE_MAX);
uniform_int_distribution<int> gen_drain_score(DRAIN_SCORE_MIN, DRAIN_SCORE_MAX);
uniform_int_distribution<int> gen_guard_score(GUARD_SCORE_MIN, GUARD_SCORE_MAX);
uniform_int_distribution<int> gen_guard_and_ward_score(GUARD_AND_WARD_SCORE_MIN, GUARD_AND_WARD_SCORE_MAX);
uniform_int_distribution<int> gen_ward_score(WARD_SCORE_MIN, WARD_SCORE_MAX);
uniform_int_distribution<int> gen_heal_me_score(HEAL_ME_SCORE_MIN, HEAL_ME_SCORE_MAX);
uniform_int_distribution<int> gen_damage_them_score(DAMAGE_THEM_SCORE_MIN, DAMAGE_THEM_SCORE_MAX);
uniform_int_distribution<int> gen_draw_score(DRAW_SCORE_MIN, DRAW_SCORE_MAX);
uniform_int_distribution<int> gen_player_health_score(PLAYER_HEALTH_SCORE_MIN, PLAYER_HEALTH_SCORE_MAX);
uniform_int_distribution<int> gen_player_guard_score(PLAYER_GUARD_SCORE_MIN, PLAYER_GUARD_SCORE_MAX);
uniform_int_distribution<int> gen_player_attack_score(PLAYER_ATTACK_SCORE_MIN, PLAYER_ATTACK_SCORE_MAX);
uniform_int_distribution<int> gen_player_draw_score(PLAYER_DRAW_SCORE_MIN, PLAYER_DRAW_SCORE_MAX);

int LETHAL_SCORE;
int ATTACK_SCORE;
int BREAKTHROUGH_SCORE;
int CHARGE_SCORE;
int DRAIN_SCORE;
int GUARD_SCORE;
int GUARD_AND_WARD_SCORE;
int WARD_SCORE;
int HEAL_ME_SCORE;
int DAMAGE_THEM_SCORE;
int DRAW_SCORE;
int PLAYER_HEALTH_SCORE;
int PLAYER_GUARD_SCORE;
int PLAYER_ATTACK_SCORE;
int PLAYER_DRAW_SCORE;

ofstream outputFile;

void randomize()
{
    LETHAL_SCORE = gen_lethal_score(rng);
    ATTACK_SCORE = gen_attack_score(rng);
    BREAKTHROUGH_SCORE = gen_breakthrough_score(rng);
    CHARGE_SCORE = gen_charge_score(rng);
    DRAIN_SCORE = gen_drain_score(rng);
    GUARD_SCORE = gen_guard_score(rng);
    GUARD_AND_WARD_SCORE = gen_guard_and_ward_score(rng);
    WARD_SCORE = gen_ward_score(rng);
    HEAL_ME_SCORE = gen_heal_me_score(rng);
    DAMAGE_THEM_SCORE = gen_damage_them_score(rng);
    DRAW_SCORE = gen_draw_score(rng);
    PLAYER_HEALTH_SCORE = gen_player_health_score(rng);
    PLAYER_GUARD_SCORE = gen_player_guard_score(rng);
    PLAYER_ATTACK_SCORE = gen_player_attack_score(rng);
    PLAYER_DRAW_SCORE = gen_player_draw_score(rng);
}

void run()
{
    char psBuffer[128];
    char numberBuffer[50];
    sprintf_s(numberBuffer, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", LETHAL_SCORE, ATTACK_SCORE, BREAKTHROUGH_SCORE, CHARGE_SCORE, DRAIN_SCORE, GUARD_SCORE, GUARD_AND_WARD_SCORE, WARD_SCORE, HEAL_ME_SCORE, DAMAGE_THEM_SCORE, DRAW_SCORE, PLAYER_HEALTH_SCORE, PLAYER_GUARD_SCORE, PLAYER_ATTACK_SCORE, PLAYER_DRAW_SCORE);
    string prog2params = numberBuffer;
    sprintf_s(numberBuffer, "%d", ITERATIONS_PER_RUN);
    string iterations = numberBuffer;
    FILE *pPipe;
    string mainJar = "..\\..\\LegendsOfCodeAndMagic_Artifacts\\cg-brutaltester.jar";
    string subJar = "\"java -jar -Dleague.level=4 ..\\..\\LegendsOfCodeAndMagic_Artifacts\\LegendsOfCodeAndMagic-1.0.jar\"";
    string prog1 = "\"..\\x64\\Debug\\LegendsOfCodeAndMagic.exe";
    string prog1params = "";
    string prog2 = "\"..\\x64\\Debug\\LegendsOfCodeAndMagic.exe";
    string progEnd = "\"";
    string threads = "4";
    string openString = "java -jar " + mainJar + " -r " + subJar + " -p1 " + prog1 + " " + prog1params + progEnd + " -p2 " + prog2 + " " + prog2params + progEnd + " -t " + threads + " -n " + iterations + " -s";
    if ((pPipe = _popen(openString.c_str(), "rt")) == NULL)
        exit(1);
    while (fgets(psBuffer, 128, pPipe)) {
        string buffer = psBuffer;
        if (!buffer.compare(0, 10, "| Player 1"))
        {
            buffer = buffer.substr(10);
            buffer.erase(remove(buffer.begin(), buffer.end(), ' '), buffer.end());
            buffer.erase(remove(buffer.begin(), buffer.end(), '|'), buffer.end());
            buffer.erase(remove(buffer.begin(), buffer.end(), '%'), buffer.end());
            buffer.erase(remove(buffer.begin(), buffer.end(), '\r'), buffer.end());
            buffer.erase(remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
            outputFile.open("runnerOutput.txt", ios::out | ios::app);
            outputFile << buffer << " " << prog2params << endl;
            outputFile.close();
        }
    }
}

void start()
{
    outputFile.open("runnerOutput.txt", ios::out | ios::app);
    outputFile << "Iterations: " << ITERATIONS_PER_RUN << endl;
    outputFile << "Player1WinPercent LETHAL_SCORE ATTACK_SCORE BREAKTHROUGH_SCORE CHARGE_SCORE DRAIN_SCORE GUARD_SCORE GUARD_AND_WARD_SCORE WARD_SCORE HEAL_ME_SCORE DAMAGE_THEM_SCORE DRAW_SCORE PLAYER_HEALTH_SCORE PLAYER_GUARD_SCORE PLAYER_ATTACK_SCORE PLAYER_DRAW_SCORE" << endl;
    outputFile.close();
}

void end()
{
//    printf("Press enter key to end...\n");
//    getchar();
}

int main(int argc, const char * argv[])
{
    start();
    for (auto i = 0; i < TOTAL_RUNS; i++)
    {
        randomize();
        run();
    }
    end();
}
