#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
////////////////////////////////////////////////////////////////
//
//  Thomas Kelly Norris
//  CS344 Operating Systems Winter 2020 Oregon State University
//  February 7th, 2020
//  Program 2 - Adventure
//  Buildrooms file
//  This program creates a directory and 7 files for rooms
//      to be used in the "adventure" program.
//
/////////////////////////////////////////////////////////////////

//room struct for storing information
struct room
{
    char name[10];
    char* room_type;
    char* connections[6];
};

//  This function is needed to put non-garbage values into the "conncections" array in room structs
void initializeConnections(struct room rooms[7])
{
    //initialize looping vars
    int i, j;
    //First loop goes through rooms
    for (i = 0; i < 7; i++)
    {
        //Second loop goes through each connection
        for (j=0; j < 6; j++)
        {
            //Set connection to ""
            rooms[i].connections[j] = "";
        }
    }
}

//  This function checks if all rooms have the correct amount of connections
int IsGraphFull(struct room rooms[7])
{
    //initialize looping vars
    int counter;
    int check;
    check = 1;
    int i, j;
    char* no = "";
    //  Pull in the array of room structs.
    //  For each room, go through connections array.
    //  If there is a connection, that index will not be null, add to a counter.
    //  Check that counter is between 3 and 6.
    //  If not, return 0 (false)
    //  If all connections are between 3 and 6, return 1 (true)
    for (i = 0; i < 7; i++) 
    {
        
        counter = 0;
        for (j=0; j < 6; j++) 
        {
            if (strcmp(rooms[i].connections[j], no) != 0) 
            {
                counter++;
            }
        }
        if (counter < 3) 
        {        
            check = 0;
        }
    }
    //Return logic values based on value of check
    if (check == 0) 
    {
        return 0;
    }
    else if (check == 1)
    {
        return 1;
    }
}

//  Function to return a random room from the array of rooms
struct room * GetRandomRoom(struct room rooms[7])
{
    //Seed random with time
    srandom(time(NULL));
    //Get random number between 0 and 6
    int index;
    index = random() % 7;
    //return room
    return &rooms[index];
}

//  Function that checks if a room can have a new connection added
int CanAddConnectionFrom(struct room* x)
{
    //initialize looping vars
    int j, counter;
    counter = 0;
    //loop through connections
    for (j = 0; j < 6; j++) 
    {
        //if connection is not null, add to counter
        if (x->connections[j] != "")
        {
            counter++;
        }
    }
    //if counter is less than 6, return true
    if (counter < 6)
    {
        return 1;
    }
    //if counter is 6, return false
    else 
    {
        return 0;
    }
}

//  Function to check if a connection already exists between two rooms
int ConnectionAlreadyExists(struct room* x, struct room* y) {
    //initialize looping vars
    int j;
    //loop through x's connections
    for (j = 0; j < 6; j++)
    {
        //If a connection is equal to the name of y, return true
        if (x->connections[j] == y->name)
        {
            return 1;
        }
    }
    //else return false
    return 0;
}

//  Adds a connection between two rooms
void ConnectRoom(struct room* x, struct room* y)
{
    //initialize looping vars
    int j, lock;
    //set lock psuedo-bool to false
    lock = 0;
    //Loop through x's connections
    for (j = 0; j < 6; j++)
    {
        //If lock is false
        if (lock == 0) 
        {
            //If x's connections[j] is empty
            if (x->connections[j] == "")
            {
                //Set connection to y
                x->connections[j] = y->name;
                //Set lock to true
                lock = 1;
            }
        }
    }
}

// Checks if two input rooms are the same by checking their name
int IsSameRoom(struct room* x, struct room* y)
{
    //could use strcmp() but if they're the same room, the name location would be the same
    if (x->name == y->name) 
    {
        return 1;
    }
    else {
        return 0;
    }
}

//  Adds a random connection to two rooms, if able. 
void AddRandomConnection(struct room rooms[7])
{
    // declaring two pointers to room structs
    struct room* A;
    struct room* B;

    //setting pseudo-bools for while loop
    int check = 1;
    int back;
    while(check == 1)
    {
        //Get a random room for A
        A = GetRandomRoom(rooms);
        //check if A can add connections
        back = CanAddConnectionFrom(A);
        if (back == 1)
        {
            //if A can add connections, exit while loop.
            //if A cannot add connections, loop and get another room
            check = 0;
        }
    }

    //This do-while loop gets a random room for B given it won't conflict with A
    //  Fairly certain this loop adds the most to run time
    do
    {
        B = GetRandomRoom(rooms);
    }
    while(CanAddConnectionFrom(B) == 0 || IsSameRoom(A, B) == 1 || ConnectionAlreadyExists(A, B) == 1);

    //Add the connections to each room
    ConnectRoom(A, B);
    ConnectRoom(B, A);
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
        rindex = random() % 10;
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
    struct room rooms[7];
    char roomName[20];
    for (i = 0; i < 7; i++)
    {
        //printf("For loop: %d\n", i);
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
        strcpy(rooms[i].name, roomName);
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
        for (j = 0; header[j] != '\0'; j++)
        {
            fputc(header[j], file);
        }
        //close file
        fclose(file);

        //reset roomName
        char* no = "";
        strcpy(roomName, no);
    }
    //PART 1 DONE!



    //PART 2 - ROOM CONNECTIONS

    //Initialize connections with known null values
    initializeConnections(rooms);

    //Fill room graph with connections. This only does the struct, files are next.
    while (IsGraphFull(rooms) == 0)
    {
        AddRandomConnection(rooms);
    }

    //Add connections to files
    //loop through array of rooms
    int k;
    for (i = 0; i < 7; i++)
    {
        //Get room names and make a path to its file
        //printf("For loop: %d\n", i);
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
        strcpy(roomName, roomNames[rindi[i]]);
        //Add _room to room name for file
        //Add file name to path
        //Add _room to room name for file
        strcat(roomName, "_room");
        //Add file name to path
        strcat(path, roomName);
        strcpy(roomName, roomNames[rindi[i]]);
        //create file pointer
        FILE *file;
        //open file from path
        file = fopen(path, "a");
        //Add header to file
        //Loop through connections
        char* no = "";
        for (j = 0; j < 6; j++)
        {
            //if connection = "", skip
            if (strcmp(rooms[i].connections[j], no) != 0)
            {
                //else, add CONNECTION {j}: connection[j]\n to file
                char front[100] = "CONNECTION ";
                char jstring[10] = "";
                sprintf(jstring, "%d", j);
                strcat(front, jstring);
                strcat(front, ": ");
                strcat(front, rooms[i].connections[j]);
                strcat(front, "\n");
                
                for (k = 0; front[k] != '\0'; k++)
                {
                    fputc(front[k], file);
                }
            }
            
        }
        //close file
        fclose(file);
    }

    //Add room types to files
    //  Note that since the array of rooms is randomized, so is room type assignment
    for(i = 0; i < 7; i++)
    {
        char front[100] = "ROOM TYPE: ";
        //Get room names and make a path to its file
        //printf("For loop: %d\n", i);
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
        strcpy(roomName, roomNames[rindi[i]]);
        //Add _room to room name for file
        //Add file name to path
        //Add _room to room name for file
        strcat(roomName, "_room");
        //Add file name to path
        strcat(path, roomName);
        strcpy(roomName, roomNames[rindi[i]]);
        //create file pointer
        FILE *file;
        //open file from path
        file = fopen(path, "a");
        //if first room, give START_ROOM type
        if (i == 0)
        {
            strcat(front, "START_ROOM\n");
            for(j = 0; front[j] != '\0'; j++)
            {
                fputc(front[j], file);
            }
        }
        //if not last room, give MID_ROOM type
        else if (i < 6) {
            strcat(front, "MID_ROOM\n");
            for(j = 0; front[j] != '\0'; j++)
            {
                fputc(front[j], file);
            }
        }
        //if last room, give END_ROOM type
        else if (i == 6) {
            strcat(front, "END_ROOM\n");
            for(j = 0; front[j] != '\0'; j++)
            {
                fputc(front[j], file);
            }
        }
    }
}
