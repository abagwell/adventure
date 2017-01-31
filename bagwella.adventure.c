/*
 Name: Andrew Bagwell
 Course: CS344
 Assignment: Assignment 2 Adventure
 Description: This program is a take on the text based Cave Adventure game developed by Crowther et al. Users move through a set of seven rooms until the end room is found. When this
 happens the user is notified along with a message that display the number of steps taken to reach the end room along with the path the user took to get there.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

/*
	- There are two structs used in this game, room and printedRoom. Room is used to write information
	to files while printedRoom is used to read information from files. You could use the same struct with some modification but I prefer to use two to keep them separate.
	Note, the printedRoom struct only contains an array of the names of the connection, not pointers to the connect structs themselves, as room does.
 */

struct room {
    
    const char* name;
    const char* roomType;
    int connectionsAmount;
    struct room* roomConnections[6];
    
};

struct printedRoom {
    
    const char* name;
    const char* roomType;
    int connectionsAmount;
    char* connectionNames[6];
    
};

//This array of room structs will hold the rooms for the game BEFORE they are written to file.

struct room gameRooms[7];

//function prototypes

bool makeConnection (int, int);
void printRoom(struct room);
void createGameFile(char*, int, struct room);
void readGameFile(char*, int, struct printedRoom*);


int main() {
    
    //This is needed for random assignment of values to rooms...
    
    srand(time(NULL));
    
    //Here, we create an array of room names that will be used to select the room names for the gameRooms.
   	const char* roomNames[10] = {"red", "blue", "yellow", "purple", "green", "orange", "white", "black", "grey", "brown"};
    
   	// Similar to above, this is an array that holds the room types
    const char* roomTypes[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};
    
    // this is primarily used for meeting the "read from files" requirement from the assignment
    struct printedRoom newGameRooms[7];
    
    //Per the assignment requirments - this creates a directory that includes the process id as part of its name
    
    pid_t processId = getpid(); //gets process id
    char dirName[35];	//holds the name of the directory
    sprintf(dirName, "./bagwella.rooms.%d", processId);	//combines the directory name with the process id
    
    int result = mkdir(dirName, 0700); //mkdir creates the directory
    if (result == -1) {		//check for errors, and exit if there are errors..
        
        perror("Error in creating directory: ");
        exit(1);
    }
    
    
    //The rooms for the game are actually "built" here
    
    //Create an array that will hold the indexes of the array of room types..
    
    int roomTypeIndex[7] = {0, 2, 2, 2, 2, 2, 1}; //it will be random as to which room really gets these
    int i, j, k;
    
    //Create an array that will hold the indexes of the array of room names
    int roomNameIndex[10];
    //initialize that array
    for (i = 0; i < 10; i++)
        roomNameIndex[i] = i;
    //randomize the array contents via calls to rand() and then swapping the contents of array
    for (i = 0; i < 10; i++) {
        int val = rand() % 10;
        int holder = roomNameIndex[i];
        roomNameIndex[i] = roomNameIndex[val];
        roomNameIndex[val] = holder;
    }
    
    //Fill the gameRooms array
    
    for (i = 0; i < 7; i++) {
        
        //set the name of the gameRoom with the roomName array at the index value of roomNameIndex array
        gameRooms[i].name = roomNames[roomNameIndex[i]];
        //set the room type of the gameroom with the roomType array at the index value of roomTypeIndex array
        gameRooms[i].roomType = roomTypes[roomTypeIndex[i]];
        //set the connection amoun to zero for now
        gameRooms[i].connectionsAmount = 0;
    }
    
    //Generate the connections for the rooms
    
    for (i = 0; i < 7; i++) {
        
        //for each gameRoom, continue to make connections until there are at least three connections for each room
        while (gameRooms[i].connectionsAmount < 3) {
            //randomnly generate the room to connect to
            int randConn = rand() % 7;
            //make connection to the room
            makeConnection(i, randConn);	//see function definition below
        }
    }
    
    //This where the files are written
    
    //generate a file for each of the seven rooms
    for (i=0; i < 7; i++) {
        createGameFile(dirName, i, gameRooms[i]);	//see function definition below
    }
    
    //read rooms files
    
    for (i=0; i < 7; i++) {
        readGameFile(dirName, i, &newGameRooms[i]);		//see function definition below
    }
    
    //This where the game is actually played
    
    printf("\n--------------------------------");
    printf("	  ROOM ADVENTURE	");
    printf("---------------------------------\n\n");
    
    //create location, stepcount and a flag variable and set them to 0;
    int location, stepCount, flag;
    location = 0;
    stepCount = 0;
    flag = 0;
    //create a buffer to hold the room name of the user's selection
    char next[30];
    //create an array of integers that will represent the index of gameRooms array that the user has visited
    int path[7];
    
    //play game until an exit condition is reached
    while (1) {
        
        //set the first room visited to the current location
        path[stepCount] = location;
        
        //check to see if the current room is the end room
        if (strcmp(newGameRooms[location].roomType, roomTypes[1]) == 0) {
            
            //display end room message
            printf("CONGRATULATIONS! YOU HAVE FOUND THE ROOM.\n");
            printf("YOU TOOK %d STEPS. ", stepCount);
            printf("YOUR PATH TO VICTORY WAS:\n");
            for (i =0; i < stepCount; i++) {
                printf("%s\n", newGameRooms[path[i]].name);
            }
            
            exit(0);
        }
        
        //print the current location's name and connections
        printf("CURRENT LOCATION: %s\n", newGameRooms[location].name);
        printf("CONNECTIONS: ");
        for (i = 0; i < newGameRooms[location].connectionsAmount; i++) {
            
            printf("%s", newGameRooms[location].connectionNames[i]);
            if (i == (newGameRooms[location].connectionsAmount - 1))
                printf(".\n");
            else
                printf(", ");
        }
        //get the user's choice here..
        printf("WHERE TO?>");
        fgets(next, 30, stdin);
        //strip newline from string and print new line
        next[strlen(next)-1] = '\0';
        printf("\n");
        //check to see if this is a valid connection from location
        for (k = 0; k < newGameRooms[location].connectionsAmount; k++) {
            
            if (strcmp(next, newGameRooms[location].connectionNames[k]) == 0) {
                
                //if user input matches a valid choice, find that room among newGameRooms
                for (j=0; j<7; j++) {
                    
                    if (strcmp(next, newGameRooms[j].name) == 0) {
                        
                        location = j;
                        stepCount++;
                        flag = 1;
                    }
                }
            }
			     }
        //if the user input was not a valid choice, display this message
        if (flag == 0)
            printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN\n\n");
        
    }
    
    return 0;
}
/*
 Function Name: makeConnection
 Parameters: two integers that represent indexes in the gameRooms array
 Description: after checking if a connection is viable, this function then connects two room structs reciprocally
 */

bool makeConnection(int num1, int num2) {
    
    //if the two rooms are equal to each other, don't connect them
    
    if (num1 == num2) {
        
        return false;
    }
    
    //if the room to be connected has reached its connection limit, do nothing
    
    if (gameRooms[num1].connectionsAmount > 6) {
        
        return true;
    }
    
    //if the room being connected to has reached its connection limit, don't connect the rooms
    
    if (gameRooms[num2].connectionsAmount > 6) {
        
        return false;
    }
    
    int i;
    //loop through the struct's connection array and see if the room to be connected is present. if it is, don't connect the rooms
    for (i = 0; i < 6; i++) {
        
        if ((gameRooms[num1].roomConnections[i] != NULL) && (gameRooms[num1].roomConnections[i]==&gameRooms[num2])) {
            
            return false;
        }
    }
    
    
    //otherwise, connect the rooms to each other. The connections are reciprocal
    gameRooms[num1].roomConnections[gameRooms[num1].connectionsAmount] = &gameRooms[num2];
    gameRooms[num1].connectionsAmount = gameRooms[num1].connectionsAmount + 1;
    gameRooms[num2].roomConnections[gameRooms[num2].connectionsAmount] = &gameRooms[num1];
    gameRooms[num2].connectionsAmount = gameRooms[num2].connectionsAmount + 1;
    return true;
}

/*
 Function Name: printRoom
 Parameters: This function takes a room struct as a parameter
 Description: This function displays the fields of a room struct. I mainly used it for debugging.
 */

void printRoom(struct room myRoom) {
    printf("\nname: %s\n", myRoom.name);
    printf("type: %s\n", myRoom.roomType);
    printf("num of Connections: %d\n", myRoom.connectionsAmount);
    int i;
    for (i = 0; i < myRoom.connectionsAmount; i++) {
        printf("Connected to %s\n", myRoom.roomConnections[i]->name);
    }
}

/*
 Function Name: createGameFile
 Parameters: This function takes a directory name, a file number to be used in the file name, and a room struct as parameters
 Description: This function creates a file to hold a room by combining the directory name and the file number together. It then writes the content
 of the room struct's fields into the file
 */

void createGameFile(char* drName, int fileNumber, struct room roomPassed) {
    
    //create the file names. the file name numbers will be the same as indexes of the gameRooms array
    
    int drNameSize = sizeof(drName);
    int fileNumberSize = sizeof(fileNumber);
    int sumSizes = drNameSize + fileNumberSize;
    //char fileName[sizeof ("room8.tmp")];
    char fileName[sumSizes];
    sprintf(fileName, "%s/room%d.tmp", drName, fileNumber);
    
    //create the actual file
    
    FILE* filePtr;         //you could use a file descriptor here if you wished
    filePtr = fopen(fileName, "w");
    
    if(filePtr==NULL) {
        
        fclose(filePtr);
        exit(1);
    }
    
    //write the room name, the connections, and room type to the file
    
    fprintf(filePtr, "ROOM NAME: %s\n", roomPassed.name);
    
    int i;
    for (i = 0; i < roomPassed.connectionsAmount; i++)
        fprintf(filePtr, "CONNECTION %d: %s\n", i+1, roomPassed.roomConnections[i]->name);
    
    fprintf(filePtr, "ROOM TYPE: %s\n", roomPassed.roomType);
    
    fclose(filePtr);
    return;
    
}

/*
 Function Name: readGameFile
 Parameters: This function takes a directory name, a file number to be used in the file name, and a pointer to a printedRoom struct as parameters
 Description: This function reads the content of the file into the struct's fields.
 */

void readGameFile (char* drName, int fileNumber, struct printedRoom* roomPassed) {
    
    //create the file name
    int drNameSize = sizeof(drName);
    int fileNumberSize = sizeof(fileNumber);
    int sumSizes = drNameSize + fileNumberSize;
    //char fileName[sizeof "room8.tmp"];
    char fileName[sumSizes];
    sprintf(fileName, "%s/room%d.tmp", drName, fileNumber);
    
    //open the file
    FILE* fptr;
    fptr = fopen(fileName, "r");
    if (fptr == NULL) {
        perror("Error: file open error\n");
        exit(1);
    }
    
    //set up a buffer to read the file
    char buff[50];
    //set up a variable to iterate through the connection array of struct
    int i = 0;
    //set the amount of connections to 0
    roomPassed->connectionsAmount = 0;
    
    //read the file line by line until there are no more lines
    
    while (fgets(buff, 50, fptr) != NULL) {
        
        //compare the line,  offseting the buffer based on the format specified, to see if it is a room name line
        if (strncmp(buff, "ROOM NAME", 9) == 0) {
            //create container to hold the contents of the buffer
            char structName[20];
            //copy the buffer, offsetting it to get the actual value, into that container
            strcpy(structName, buff + 11);
            //remove the new line symbol and repalce with /0
            strtok(structName, "\n");
            //allocate memory for the field
            roomPassed->name = malloc(sizeof(structName));
            //copy the container to the field
            strcpy(roomPassed->name, structName);
        }
        
        //compare the line,  offseting the buffer based on the format specified , to see if it is a connection line
        if (strncmp(buff, "CONNECTION", 9 ) == 0) {
            //create container to hold the contents of the buffer
            char structName[20];
            //copy the buffer, offsetting it to get the actual value, into that container
            strcpy(structName, buff + 14);
            strtok(structName, "\n"); //removing newline
            //allocate memory for the field, here we only copy the connection's name into the field (compare the two structs at the beginning of the file to see the difference)
            roomPassed->connectionNames[i] = malloc(sizeof(structName)); 
            //copy the container to the field
            strcpy(roomPassed->connectionNames[i], structName);
            //increment the amount of connections
            roomPassed->connectionsAmount++; 
            //increment the connect iterator
            i++;
            
        }
        //compare the line,  offseting the buffer based on the format specified , to see if it is a connection line
        if (strncmp(buff, "ROOM TYPE", 9) == 0) { 
            
            //create container to hold the contents of the buffer
            char structName[20]; 
            //copy the buffer, offsetting it to get the actual value, into that container
            strcpy(structName, buff + 11);
            //remove the new line symbol and repalce with /0
            strtok(structName, "\n");
            //allocate memory for the field
            roomPassed->roomType = malloc(sizeof(structName));
            //copy the container to the field
            strcpy(roomPassed->roomType, structName);
        }	
    }
    
    fclose(fptr);
    return;
}
