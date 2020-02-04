#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


main() 
{
    //set var for pid
    int pid;
    //get pid from unistd.h function
    pid = getpid();
    //norristh.rooms.{pid}
    //Create first part of path
    char path[16]= "norristh.rooms.";
    //Create string to put pid into
    char pidString[20];
    //Set string equal to int(pid)
    sprintf(pidString, "%d", pid);
    //Concatenate first part of path with pid string
    strcat(path, pidString);
    //Create directory
    mkdir(path, 0777);

    //create array of room names
    char* roomNames[10] = {"Kitchen", "Cave", "Tree", "Drain", "Snow", "Hearth", "Pantry", "Ritual", "Moon", "Sun"};

    /*
    int done = 0;
    int indexCheck = 0;
    while(done == 0)
    {
        int indexAra[7];
        //set the seed for a random number
        srandom(time(NULL));
        //get a random number between 0 and 9
        int rindex = random() % 9;
        int i;
        for(i = 0; i < 8; i++) {
            if (rindex == indexAra[i]) {
                indexCheck = 1;
            }
            else {
                indexCheck = 0;
            }
        }

        if (indexCheck == 0) {

        }
    }
}
*/