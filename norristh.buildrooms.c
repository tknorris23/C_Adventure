#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct room
{
    char* name;
    char* room_type;
    struct room connections[6];
};

int IsGraphFull(struct room rooms[7])
{
    int counter;
    int check;
    check = 0;
    int i, j;
    //  Pull in the array of room structs.
    //  For each room, go through connections array.
    //  If there is a connection, that index will not be null, add to a counter.
    //  Check that counter is between 3 and 6.
    //  If not, return 0 (false)
    //  If all connections are between 3 and 6, return 1 (true)
    for (i = 0; i < 7; i++) {
        counter = 0;
        for (j=0; j < 6; j++) {
            if (rooms[i].connections[j].name != "") {
                counter++;
            }
        }
        if (3 <= counter <= 6) {
            check = 0;
        }
        else {
            check = 1;
        }
    }
    if (check == 1) {
        return 0;
    }
    else {
        return 1;
    }
}

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

    //Getting random names requires two nested for loops:
    //The first gets a random #, uses second for loop to check if it's in rindi, and adds rindex to rindi
    //Second loops through rindi to check if rindex is already in there
    //This can take a few seconds to finish because random is seeded every second by time, 
    //making it so a lot of loops have to happen before all are added
    //Setting vars for looping:
    int i;
    int j;
    int rindex;
    //since rindex will never be 10, array is filled with 10's as garbage values since 0's won't work
    int rindi[7] = {10, 10, 10, 10, 10, 10, 10};
    int check;
    for (i = 0; i < 7; i++)
    {
        //get random #
        srandom(time(NULL));
        rindex = random() % 9;
        //check if it's in rindi:
        check = 0;
        for (j=0; j < 7; j++)
        {
            //check rindex against rindi[j] value
            if (rindex == rindi[j])
            {
                //set check to true, decrease iterator
                check = 1;
                i--;
            }
        }
        //If check is false, add rindex to rindi[i]
        if (check == 0) 
        {
            rindi[i] = rindex;
        }
    }
    
    //Next step is to create room structs and files from name indexes acquired
    printf("Beginning file creation\n");
    struct room rooms[7];
    char roomName[20];
    printf("Starting for loop:\n");
    for (i = 0; i < 7; i++)
    {
        //get pid from unistd.h function
        pid = getpid();
        //norristh.rooms.{pid}
        //Create first part of path
        char path[20]= "norristh.rooms.";
        //Create string to put pid into
        char pidString[20];
        //Set string equal to int(pid)
        sprintf(pidString, "%d", pid);
        //Concatenate first part of path with pid string
        strcat(path, pidString);
        //Add slash to path
        strcat(path, "/");

        //Get room name from list of names
        strcpy(roomName, roomNames[rindi[i]]);
        //Set struct name
        rooms[i].name = roomName;
        //Start creating header for file
        char header[] = "ROOM NAME: ";
        //Add room name to header
        strcat(header, roomName);
        //Add newline to header
        strcat(header, "\n");

        //Add _room to room name for file
        strcat(roomName, "_room");
        //Add file name to path
        strcat(path, roomName);

        //create file pointer
        FILE *file;
        //open / create file from path
        file = fopen(path, "w");
        //Add header to file
        for (j = 0; header[j] != '\n'; j++)
        {
            fputc(header[j], file);
        }
        //close file
        fclose(file);
    }
    //PART 1 DONE!
    //PART 2 - ROOM CONNECTIONS
    int boi;
    boi = IsGraphFull(rooms);
    printf("%d", boi);
}
