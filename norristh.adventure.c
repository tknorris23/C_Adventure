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

    //Check for start room
    //Change dirp to room directory
    //debug line, used to not have to run buildrooms every time I need to test this
    //strcpy(dName, "norristh.rooms.3261");
    dirp = opendir(dName);
    //make file var
    FILE *fp;
    char* line = NULL;
    size_t len = 0;
    char filepath[75];
    char startRoom[50];
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
            if(strstr(line, "START_ROOM") != NULL)
            {
                //if so, set startRoom to name of this room
                strcpy(startRoom, filepath);
            }
        }
        //close the file
        fclose(fp);
        strcpy(filepath, "");
    }

    //set vars for game loop
    int i = 0;
    int j = 0;
    char cur_name[20] = "";
    char cur_connections[6][50] = {"", "", "", "", "", ""};
    int steps = 0;
    char step_path[100][50];
    for(i = 0; i < 100; i++)
    {
        memset(step_path[i], 0, sizeof(step_path[i]));
    }
    char cur_type[20] = "";
    //set current room file
    char cur_room[50];
    char possible_connections[50];
    strcpy(cur_room, startRoom);
    //create pseudo bool for while loop
    // if over == 0, = false, game is not over
    // if over == 1, = true, game is over
    int over = 0;
    int change = 1;
    int first = 1;
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
                memset(cur_name,0,sizeof(cur_name));
                //start for loop at 10th character (which is the start of the name)
                //and read until a newline
                for (i = 11; line[i] != '\n'; i++)
                {
                    //set cur_name chars to equal room name chars
                    cur_name[i-11] = line[i];
                }
            }
            //check for CONNECTION line
            if (strstr(line, "CONNECTION") != NULL && change == 1)
            {
                //start for loop at 13th character
                //read until newline
                memset(cur_connections[j],0,sizeof(cur_connections[j]));
                for (i = 14; line[i] != '\n'; i++)
                {
                    //set cur_connections[j] to equal connection name
                    cur_connections[j][i-14] = line[i];
                }
                //increment j for connections array
                j++;
            }
            //check for TYPE line
            if (strstr(line, "END_ROOM") != NULL)
            {
                over = 1;
            }
            
        }
        //reset j... later
        //j = 0;
        //close the file
        fclose(fp);
        // -- END BLOCK 1 --//

        // -- BLOCK 1.5: STEP -- //
        if(change == 1 && first != 1)
        {
            strcpy(step_path[steps], cur_name);
            steps++;
        }
        // -- BLOCK 2: THE PROMPT --//
        if(over == 0)
        {
            printf("CURRENT LOCATION: %s\n", cur_name);
            //make the string for current connections

        
            if(change == 1)
            {
                memset(possible_connections,0,sizeof(possible_connections));
                for(i = 0; i < 6; i++)
                {
                    if(i < j-1)
                    {
                    strcat(possible_connections, cur_connections[i]);
                    strcat(possible_connections, ", ");
                    }

                    if (i == j-1)
                    {
                        strcat(possible_connections, cur_connections[i]);
                        strcat(possible_connections, ".\n");
                    }
                    
                }
            }
            change = 0;
            printf("POSSIBLE CONNECTIONS: %s\n", possible_connections);

            printf("WHERE TO? >");
            char input[20];
            memset(input, 0,sizeof(input));
            fgets(input, 20, stdin);
            printf("\n");
            int connect_lock = 0;
            //get input from user
            //check if input is one of the connections
            for(i = 0; i < 6; i++)
            {
                if(strstr(input, cur_connections[i]) != NULL && connect_lock == 0)
                {
                    connect_lock = 1;
                    //build filepath and set cur_room;
                    change = 1;
                    memset(filepath,0,sizeof(filepath));
                    strcpy(filepath, dName);
                    strcat(filepath, "/");
                    strcat(filepath, cur_connections[i]);
                    strcat(filepath, "_room\0");
                    strcpy(cur_room, filepath);
                }
            }
            if(change == 0)
            {
                printf("HUH? I DON'T KNOW THAT ROOM. TRY AGAIN.\n\n");
            }
            j = 0;
        }
        if(over == 1)
        {
            printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
            printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
            for (i = 0; strcmp(step_path[i], "") != 0; i++)
            {
                printf("%s\n", step_path[i]);
            }
            exit(0);
        }
        first = 0;
        //if not print HUH??? statement
        //if yes set that room to cur_room (have to rebuild the filepath...)
        //  increment step count and add room name to path array
        //  should path array just be an arbitrarily large array? no reqs on assignment
        //hint for time thing on the assignment page
    } // end of game while loop
    //print end stuff
}