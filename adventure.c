//Name: Andrew Bagwell
//Course: CS344
//Assignment: Assignment 2 Adventure
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>


struct room {

    const char* name;
    const char* roomType;
    int connectionsAmount;
    struct room* roomConnections[6];

};

const char* roomNames[10] = {"red", "blue", "yellow", "purple", "green", "orange", "white", "black", "grey", "brown"};
const char* roomTypes[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
struct room gameRooms[7];

//function prototypes
bool makeConnection (int, int);
void printRoom(struct room);
void createGameFile(char*, int, struct room);

int main(void) {

srand(time(NULL));

//Create the directory for the room files

pid_t processId = getpid();
char dirName[35];
sprintf(dirName, "./bagwella.rooms.%d", processId);
int result = mkdir(dirName, 0700);
printf("Makdir is %d\n", result);
if (result == 0)
	printf("Directory created.\n");
if (result == -1)
	perror("Error in creating directory: ");

//SET UP THE ROOMS

srand(time(NULL));

    int roomTypeIndex[7] = {0, 2, 2, 2, 2, 2, 1}; //it's random as to which room gets these
    int i, j, k;

    int roomNameIndex[10];
    for (i = 0; i < 10; i++)
        roomNameIndex[i] = i;
    for (i = 0; i < 10; i++) {
        int val = rand() % 10;
        int holder = roomNameIndex[i];
        roomNameIndex[i] = roomNameIndex[val];
        roomNameIndex[val] = holder;
    }

    for (i = 0; i < 7; i++) {

        gameRooms[i].name = roomNames[roomNameIndex[i]];
        gameRooms[i].roomType = roomTypes[roomTypeIndex[i]];
        gameRooms[i].connectionsAmount = 0;
    }

    for (i = 0; i < 7; i++) { //testing

        printf("%s\n",gameRooms[i].name);
        printf("%s\n", gameRooms[i].roomType);
        printf("%d\n", gameRooms[i].connectionsAmount);
        printf("\n");
    }

    for (i = 0; i < 7; i++) {


        while (gameRooms[i].connectionsAmount < 3) {
            printf("generating connection value\n");    //this generates the random number of connections
            int randConn = rand() % 7;
            printf("Randcon is: %d\n", randConn);
            makeConnection(i, randConn);
        }
    }

    for (i=0; i < 7; i++) {
        createGameFile(dirName, i, gameRooms[i]);
    }

//read from files




return 0;
}

bool makeConnection(int num1, int num2) {

    if (num1 == num2) {
        printf("same room! duh!\n");
        return false;
    }

    if (gameRooms[num1].connectionsAmount > 6) {

        printf("Connection limit reached for linker");
        return true;
    }


    if (gameRooms[num2].connectionsAmount > 6) {

        printf("Connection limit reached for linkee");
        return false;
    }

    int i;
    for (i = 0; i < 6; i++) {

        if ((gameRooms[num1].roomConnections[i] != NULL) && (gameRooms[num1].roomConnections[i]==&gameRooms[num2])) {
            printf("rooms are already connected\n");
            return false;
        }
    }

    gameRooms[num1].roomConnections[gameRooms[num1].connectionsAmount] = &gameRooms[num2];
    gameRooms[num1].connectionsAmount = gameRooms[num1].connectionsAmount + 1;
    gameRooms[num2].roomConnections[gameRooms[num2].connectionsAmount] = &gameRooms[num1];
    gameRooms[num2].connectionsAmount = gameRooms[num2].connectionsAmount + 1;
    printf("connection made\n");
    return true;
   }

   void printRoom(struct room myRoom) {
    printf("\nname: %s\n", myRoom.name);
    printf("type: %s\n", myRoom.roomType);
    printf("num of Connections: %d\n", myRoom.connectionsAmount);
    int i;
    for (i = 0; i < myRoom.connectionsAmount; i++) {
        printf("Connected to %s\n", myRoom.roomConnections[i]->name);
    }
}

    void createGameFile(char* drName, int fileNumber, struct room roomPassed) {

        //create the file names. the file names will be the indexes of the gameRooms array
        char fileName[sizeof "room8.tmp"];
        sprintf(fileName, "%s/room%d.tmp", drName, fileNumber);
        printf("Writing...%s\n", fileName); //debugging

        //create the actual file

        FILE* filePtr;         //file descriptor, you could use a FILE pointer here too if you prefer...
        filePtr = fopen(fileName, "w");

        if(filePtr==NULL) {

            exit(1);
            fclose(filePtr);
        }

        //write the room name, the connections, and room type to the file

        fprintf(filePtr, "ROOM NAME: %s\n", roomPassed.name);

        int i;
        for (i = 0; i < roomPassed.connectionsAmount; i++)
            fprintf(filePtr, "CONNECTION %d: %s\n", i, roomPassed.roomConnections[i]->name);

        fprintf(filePtr, "ROOM TYPE: %s\n", roomPassed.roomType);

        fclose(filePtr);
        return;

    }
