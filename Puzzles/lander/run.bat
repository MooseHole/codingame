taskkill /IM trainer.exe /IM lander.exe /F
rm *.exe
rm *Output.txt
g++ trainer.cpp -std=c++11 -o trainer
g++ lander.cpp -std=c++11 -o lander
rem start /B lander.exe & start /B trainer.exe test1.txt
start /B trainer.exe test1.txt & start /B lander.exe