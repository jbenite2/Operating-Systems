#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

//Initialize a linked list that acts as a queue
typedef struct node {
    int jobId;
    char *filename;
    int priority;
    struct node *next;
    char status[10];
    int turnaround;
    size_t size;
    time_t startTime;
} job_t;

job_t *head = NULL;
job_t *tail = NULL;

//Globals
int numJobs = 1;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

//Function prototypes
void submit(char *filename);
void pushQ(job_t *node);
void list();
void hurry(int);
void removeJob(int);
void printScreen(char * filename); //Main
void help();
void algo(int chosenAlgorithm);

//Main function
int main(int argc, char * argv[]){
    //Declaration of local variables
    int nThreads = 0;

    // Parsing command line arguments for correct input 
    if(argc > 1 && isdigit(argv[1][0])){
        nThreads = atoi(argv[1]); //Number of printers to simulate
    } else {
        printf("           ^  Command line argument #1 is supposed to be a number. Please enter input in the correct format. \n");
        return 1;
    }

    //Welcome message
    printf("\nWELCOME TO THE PRINTER SCHEDULE PROGRAM! It'll leverage %d printers. \n\n", nThreads);
    
    //Switch statement
    while(1){
        char input[20];
        scanf("%[^\n]%*c", input);
        char * token = strtok(input, " ");

        if (token == NULL) {
            printf("Invalid input. Please enter a valid command. \n");
            continue;
        }

        if (strcmp(token, "submit") == 0){
            token = strtok(NULL, " ");
            if((access(token, F_OK) != 0 )|| (strstr(token, ".gcode") == NULL) || (strcmp(strstr(token, ".gcode"), ".gcode") != 0)){
                printf("Invalid file entered. Try again.\n");
                break;
            }
            submit(token);
        } else if (strcmp(token, "list") == 0){
            list();
        } else if (strcmp(token, "wait") == 0){
            token = strtok(NULL, " ");
            printf("wait\n");
        } else if (strcmp(token, "drain") == 0){
            printf("drain\n");
        } else if (strcmp(token, "remove") == 0){
            token = strtok(NULL, " ");
            int id = atoi(token);
            if(id <= numJobs){
                removeJob(id);
            }
        } else if (strcmp(token, "hurry") == 0){
            token = strtok(NULL, " ");
            int id = atoi(token);
            if(id <= numJobs){
                hurry(id);
            }
        } else if (strcmp(token, "algo") == 0) {
            token = strtok(NULL, " ");
            if strcmp(token, "fcfs") == 0 {
                algo(1);
            } else if strcmp(token, "sjf") == 0 {
                algo(2);
            } else if strcmp(token, "priority") == 0 {
                algo(3);
            } else {
                printf("Invalid input. Please enter a valid command. \n");
            }
        } else if (strcmp(token, "quit") == 0){
            return 0;
        } else if (strcmp(token, "help") == 0){
            help();
        } else {
            printf("Invalid input. Please enter a valid command. \n");
        }
    }
}

//Function definitions

void printScreen(char *filename){
    char buffer[50];
    sprintf(buffer, "./printersim %s", filename);
    system(buffer);
}

void pushQ(job_t *node) {
    //pushQ
    pthread_mutex_lock(&queue_mutex);
    if (head == NULL) {
        head = node;
        tail = node;
    } else { //Adding it to the end
        tail->next = node;
        tail = node;
    }
    pthread_mutex_unlock(&queue_mutex);
    pthread_cond_signal(&queue_cond);
}

void submit(char *filename) {
    // Status and jobId
    int jobId = numJobs;
    numJobs++;

    //Create a new node
    job_t *newJob = (job_t *) malloc(sizeof(job_t));
    newJob->filename = strdup(filename);
    newJob->priority = 0;
    newJob->jobId = jobId;
    newJob->next = NULL;
    newJob->startTime= time(NULL);
    strcpy(newJob->status, "waiting");

    //Get the size of the file
    struct stat st;
    if(stat(filename, &st) == 0) {
        newJob->size = st.st_size;
    } else {
        newJob->size = -1; //if there is an error getting the size
    }

    //Add the new node to the queue
    pushQ(newJob);

    //Testing printScreen
    printScreen(filename);

    //Print the job ID
    printf("Job %d has been submitted.\n", jobId);
}

void list() {
    //Print the queue
    job_t *temp = head;
    while (temp != NULL) {
        printf("Job Id: %d  File Name:%s  Status:%s\n", temp->jobId, temp->filename, temp->status);
        temp = temp->next;
    }
}

void removeJob(int jobId) {
    pthread_mutex_lock(&queue_mutex);
    job_t *prev = NULL;
    job_t *curr = head;
    while (curr != NULL && curr->jobId != jobId) {
        prev = curr;
        curr = curr->next;
    }
    if (curr == NULL || strcmp(curr->status, "PRINTING")) {
        // job ID not found in the queue
        pthread_mutex_unlock(&queue_mutex);
        return;
    }
    printf("Job %d has been removed.\n", jobId);
    if (prev == NULL) {
        head = curr->next;
    } else {
        prev->next = curr->next;
    }
    if (tail == curr) {
        tail = prev;
    }
    pthread_mutex_unlock(&queue_mutex);
    free(curr->filename);
    free(curr);
}

void hurry(int jobId) {
    //Find the job in the queue
    job_t *temp = head;
    while (temp != NULL) {
        if (temp->jobId == jobId) {
            temp->priority = 1;
            printf("Job %d has been set to high priority.\n", jobId);
            return;
        }
        temp = temp->next;
    }
    printf("Job %d does not exist.\n", jobId);
}

void algo(int chosenAlgorithm) {
    switch(chosenAlgorithm){
        case 0:
            printf("FIFO\n");
            break;
        case 1:
            printf("SJF\n");
            break;
        case 2:
            printf("Balanced\n");
            break;
        default:
            printf("Invalid algorithm entered. Try again.\n");
            break;
    }
}

void help(){
    printf("Commands:\n");
    printf("submit <filename> - submit a job to the queue\n");
    printf("list - list all jobs in the queue\n");
    printf("wait <jobId> - pauses until this job is done\n ");
    printf("drain - wait until all jobs are done\n");
    printf("hurry <jobId> - set a job to high priority\n");
    printf("remove <jobId> - remove a job from the queue\n");
    printf("help - print this message\n");
    printf("quit - exit the program\n");
}
