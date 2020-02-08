#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <pthread.h>

////////////////////////////////////////////////////////////////
//
//  Thomas Kelly Norris
//  CS344 Operating Systems Winter 2020 Oregon State University
//  February 7th, 2020
//  Program 2 - Adventure
//  Adventure file
//  This program reads in data from files made by buildrooms
//      and plays a simple terminal adventure game.
//  !!VERY IMPORTANT NOTE!!
//  adventure will ONLY work if it's run directly after buildrooms!
//      command sequence should be:
//
//  gcc -o buildrooms norristh.buildrooms.c
//  gcc -o adventure norristh.adventure.c
//  buildrooms
//  adventure
//
//  Behavior is undefined if not ran this way, and will probably
//      lead to a segfault.
//
/////////////////////////////////////////////////////////////////

//function for thread that grabs time
void * start_routine(void *mewtex)
{
    //Attempts to lock mewtex, and will stall until able
    pthread_mutex_lock(mewtex);
    //Set vars
    char times[200];
    time_t t;
    struct tm *tmp;
    t = time(NULL);
    tmp = localtime(&t);
    //Get time from strftime
    strftime(times, sizeof(times), "%c", tmp);
    //Open / create file and write time into it
    FILE *fp;
    fp = fopen("currentTime.txt", "w");
    int i;
    for(i = 0; times[i] != '\0'; i++)
    {
        fputc(times[i], fp);
    }
    //close file
    fclose(fp);
    //unlock mewtex
    pthread_mutex_unlock(mewtex);
}

main() 
{
    //create thread and mutex for timekeeping
    pthread_mutex_t mewtex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&mewtex);
    pthread_t thread;
    pthread_create(&thread, NULL, &start_routine, &mewtex);
    
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
    //clear memory of step_path
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
        //close the file
        fclose(fp);
        // -- END BLOCK 1 --//

        // -- BLOCK 1.5: STEP -- //
        //if changing to a new room, add to step list and counter
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
            // if change = 1, get new connections from new room
            if(change == 1)
            {
                //reset connections string
                memset(possible_connections,0,sizeof(possible_connections));
                //go through connections and add to string
                for(i = 0; i < 6; i++)
                {
                    if(i < j-1)
                    {
                    strcat(possible_connections, cur_connections[i]);
                    strcat(possible_connections, ", ");
                    }

                    //if last connection, add period and newline
                    if (i == j-1)
                    {
                        strcat(possible_connections, cur_connections[i]);
                        strcat(possible_connections, ".\n");
                    }
                    
                }
            }
            //reset change and print string
            change = 0;
            printf("POSSIBLE CONNECTIONS: %s\n", possible_connections);

            //get input from user
            printf("WHERE TO? >");
            char input[20];
            memset(input, 0,sizeof(input));
            fgets(input, 20, stdin);
            printf("\n");
            //Check for time command
            char time[50];
            memset(time, 0, sizeof(time));
            if(strstr(input, "time") != NULL)
            {
                //Unlock mewtex and join thread, wait until thread is done
                pthread_mutex_unlock(&mewtex);
                pthread_join(thread, NULL);
                //Lock mewtex and recreate thread
                pthread_mutex_lock(&mewtex);
                pthread_create(&thread, NULL, &start_routine, &mewtex);
                //Read info from file and print string
                FILE *getTime;
                getTime = fopen("currentTime.txt", "r");
                getline(&line, &len, getTime);
                printf(" %s\n\n", line);
                fclose(getTime);
            }
            int connect_lock = 0;
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
            //check for unknown command
            if(change == 0 && strstr(input, "time") == NULL)
            {
                printf("HUH? I DON'T KNOW THAT ROOM. TRY AGAIN.\n\n");
            }
            //reset # of connections in current room, which is j
            j = 0;
        }
        //Check for game over
        if(over == 1)
        {
            //print game over text
            printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
            printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
            for (i = 0; strcmp(step_path[i], "") != 0; i++)
            {
                printf("%s\n", step_path[i]);
            }
            exit(0);
        }
        first = 0;
    } // end of game while loop
}