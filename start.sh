gcc -o raspLib.o -c raspLib.c
gcc rasp.c -o main raspLib.o -lm -lwiringPi && ./main
