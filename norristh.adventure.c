#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>


main() 
{
    //set vars
    //the code block to get most recent modified directory acquired from
    //https://stackoverflow.com/questions/42170824/use-stat-to-get-most-recently-modified-directory
    //only their dName was a char*, causing a segfault
    DIR *dirp;
    dirp = opendir(".");
    struct stat dStat;
    time_t latest;
    latest = 0;
    struct dirent *dp;
    char dName[20];
    //Loop through current directory
    while ((dp = readdir(dirp)) != NULL) 
    {
            //Wipe dStat
            memset(&dStat, 0, sizeof(dStat));
            //Run stat function call, check if error
            if (stat(dp->d_name, &dStat) < 0) {
                    continue;
            }
            // If not a directory skip 
            if (S_ISDIR(dStat.st_mode) == 0) {
                    continue;
            }
            // check with the latest timestamp
            if (difftime(latest, dStat.st_mtime) < 0) {
                    // On finding a more recent file switch that to latest
                    strcpy(dName, dp->d_name);
                    latest = dStat.st_mtime;
            }
    }
    closedir(dirp);

    printf("starting part 2\n");
    //Check for start room
    //Change dirp to room directory
    printf("rooms directory is: %s\n", dName);
    dirp = opendir(dName);
    //make file var
    FILE *fp;
    char* line = NULL;
    size_t len = 0;
    char* start = "START";
    char filepath[50];
    char startRoom[20];
    while ((dp = readdir(dirp)) != NULL)
    {
        //dp->d_name is the name of the file
        //Build path to file
        strcpy(filepath, dName);
        strcat(filepath, "/");
        strcat(filepath, dp->d_name);
        //open file for reading
        fp = fopen(filepath, "r");
        //read every line of the file, line = string of text
        while ((getline(&line, &len, fp)) != -1)
        {
            //check if substring START is within line
            if(strstr(line, start) != NULL)
            {
                //if so, set startRoom to name of this room
                strcpy(startRoom, filepath);
            }
        }
        //close the file
        fclose(fp);
    }
    printf("The start room is: %s\n", startRoom);

    //set vars for game loop
    int i = 0;
    int j = 0;
    char cur_name[20] = "";
    char cur_connections[6][50] = {"", "", "", "", "", ""};
    char cur_type[20] = "";
    //set current room file
    char cur_room[50];
    strcpy(cur_room, startRoom);
    //create pseudo bool for while loop
    // if over == 0, = false, game is not over
    // if over == 1, = true, game is over
    int over = 0;
    while(over == 0)
    {
        // -- BLOCK 1: FILE INFO GATHERING -- //
        //open the room file
        fp = fopen(cur_room, "r");
        //read every line of the file
        while ((getline(&line, &len, fp)) != -1)
        {
            //check for NAME line
            if (strstr(line, "NAME") != NULL)
            {
                //start for loop at 10th character (which is the start of the name)
                //and read until a newline
                for (i = 10; line[i] != '\n'; i++)
                {
                    //set cur_name chars to equal room name chars
                    cur_name[i-10] = line[i];
                }
                printf("current room: %s\n", cur_name);
            }
            //check for CONNECTION line
            if (strstr(line, "CONNECTION") != NULL)
            {
                //start for loop at 13th character
                //read until newline
                for (i = 13; line[i] != '\n'; i++)
                {
                    //set cur_connections[j] to equal connection name
                    cur_connections[j][i-13] = line[i];
                }
                //increment j for connections array
                j++;
                printf("connection %d added: %s\n", j-1, cur_connections[j-1]);
            }
            //check for TYPE line
            if (strstr(line, "TYPE") != NULL)
            {
                //start for loop at 10th character
                for (i = 10; line[i] != '\n'; i++)
                {
                    //set current room type
                    cur_type[i-10] = line[i];
                }
                printf("current type: %s\n", cur_type);
            }
        }
        //reset j
        j = 0;
        //close the file
        fclose(fp);
        // -- END BLOCK 1 --//
    }
}