#include "page_table.h"
#include "disk.h"
#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct node{
	int frame;
	struct node * next;
	struct node * prev;
} node ;

int * frame_table;
int * memory_counter;
unsigned char *physmem;
struct disk *disk;
char * algorithm;
node * head = NULL;
node * tail = NULL ;
int nframes;
int disk_reads;
int disk_writes;

node * createNode(int frame){
	node * newNode = (node *) malloc(sizeof(node));
	newNode->frame = frame;
	newNode->next = NULL;
	newNode->prev = NULL;
	return newNode;
}

void appendNode(int frame) {
    node * newNode = createNode(frame);
    if (tail == NULL) { // empty list
        head = tail = newNode;
    } else { // non-empty list
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
}

void prependNode(int frame) {
	node * newNode = createNode(frame);
	if (head == NULL) { // empty list
		head = tail = newNode;
	} else { // non-empty list
		head->prev = newNode;
		newNode->next = head;
		head = newNode;
	}
}

void printList() {
	node * current = head;
	while (current != NULL) {
		printf("%d ", current->frame);
		current = current->next;
	}
	printf("\n");
}

void dequeue() {
	if (head == NULL) { // empty list
		return;
	} else if (head == tail) { // one element
		head = tail = NULL;
	} else { // more than one element
		head = head->next;
		head->prev = NULL;
	}
}

void pop(){
	if (head == NULL) { // empty list
		return;
	} else if (head == tail) { // one element
		head = tail = NULL;
	} else { // more than one element
		tail = tail->prev;
		tail->next = NULL;
	}
}

void print_frame_table(){
	printf("Frame Table: \n");
	for(int i = 0; i < nframes; i++){
		printf("Frame %d: %d\n", i, frame_table[i]);
	}
}

void page_replacement(struct page_table *pt, struct disk * d, int page, int evictedPage, int frame, int bits){
	//If statement that check if it's dirty
	if(bits != 2 && bits != 3 && bits != 6 && bits != 7){
		disk_write(d, evictedPage, &physmem[frame * PAGE_SIZE]);
		disk_writes++;
	}
    disk_read(d, page, &physmem[frame * PAGE_SIZE]);
	memory_counter[page]++;
    disk_reads++;
    page_table_set_entry(pt, page, frame, PROT_READ);
    page_table_set_entry(pt, evictedPage, frame, 0);
}

void page_fault_handler(struct page_table *pt, int page)
{	

    // Create an array to store them
	int availableFrames[nframes], count = 0;

	// Loop through the frame_table and record the available frames
	for (int i = 0; i < nframes; i++) {
		if (frame_table[i] == -1) {
			availableFrames[count++] = i;
		}
	}

	// Randomly select a frame from the available frames and initialize bits to be used later
    int frame;
	int bits;


    // CASE WHERE THERE ARE NO FREE FRAMES
    if (count == 0) {
        // Implement page replacement algorithm
		if (!strcmp(algorithm, "rand")) {
			// Random page replacement

			// Choose a random frame
			frame = rand() % page_table_get_nframes(pt);

			//Make disk adjustments
			page_replacement(pt, disk, page, frame_table[frame], frame, bits);

			// Update the frame table
			frame_table[frame] = page;
		} else if (!strcmp(algorithm, "fifo")) {
			// Get the frame from the head of the queue
			frame = head->frame;

			// FIFO page replacement

			// Send it to the back of the queue
			dequeue();
			appendNode(frame);

			// Make disk adjustments
			page_replacement(pt, disk, page, frame_table[frame], frame, bits);

			// Update the frame table
			frame_table[frame] = page;

            
		} else if (!strcmp(algorithm, "lifo")) {
			// Get the frame from the tail of the list
			frame = tail->frame;

			// Custom page replacement

			// Send it to the front of the list
			pop();
			prependNode(frame);

			// Make disk adjustments
			page_replacement(pt, disk, page, frame_table[frame], frame, bits);

			// Update the frame table
			frame_table[frame] = page;
		} else{
			//Store the index of the least accessed page from frame_table
			int leastUsedPage = 0;
			for (int i = 0; i < nframes; i++) {
				if (memory_counter[frame_table[i]] < memory_counter[leastUsedPage]) {
					leastUsedPage = frame_table[i];
					frame = i;
				}
			}

			// Least used page replacement

			// Make disk adjustments
			page_replacement(pt, disk, page, frame_table[frame], frame, bits);

			// Update the frame table
			frame_table[frame] = page;
		}
    }
	//CASE WHERE THERE ARE FREE FRAMES
    else{ 
        //Choose random frame amongst the available ones
        frame= availableFrames[rand() % count];

		// Set the page table entry for the page to the frame
		page_table_set_entry(pt, page, frame, PROT_READ);

		// Read the page from the disk into the frame
		disk_read(disk, page, &physmem[frame * PAGE_SIZE]);
        disk_reads++;

		// Update the frame table
        frame_table[frame] = page;
		appendNode(frame);

    }

	// Get the bits for the page
    page_table_get_entry(pt, page, &frame, &bits);

	//If bits are not writeable, set them to writeable
	if(bits != 2 && bits != 3 && bits != 6 && bits != 7){
		page_table_set_entry(pt, page, frame, bits | PROT_WRITE);
	}
	
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        printf("use: virtmem <npages> <nframes> <rand|fifo|custom> <alpha|beta|gamma|delta>\n");
        return 1;
    }

    int npages = atoi(argv[1]);
    nframes = atoi(argv[2]);
    algorithm = argv[3];
    const char *program = argv[4];

    // Create the frame_table
    frame_table = malloc(sizeof(int) * nframes);
    memset(frame_table, -1, sizeof(int) * nframes); // initialize all elements to -1

	// Create memory counter
	memory_counter = malloc(sizeof(int) * npages);
    
    disk = disk_open("myvirtualdisk", npages);
    if (!disk) {
        fprintf(stderr, "couldn't create virtual disk: %s\n", strerror(errno));
        return 1;
    }

    struct page_table *pt = page_table_create(npages, nframes, page_fault_handler);
    if (!pt) {
        fprintf(stderr, "couldn't create page table: %s\n", strerror(errno));
        return 1;
    }

    unsigned char *virtmem = page_table_get_virtmem(pt);
    physmem = page_table_get_physmem(pt);

    if (!strcmp(program, "alpha")) {
        alpha_program(virtmem, npages * PAGE_SIZE);

    } else if (!strcmp(program, "beta")) {
        beta_program(virtmem, npages * PAGE_SIZE);

    } else if (!strcmp(program, "gamma")) {
        gamma_program(virtmem, npages * PAGE_SIZE);

    } else if (!strcmp(program, "delta")) {
        delta_program(virtmem, npages * PAGE_SIZE);

    } else {
        fprintf(stderr, "unknown program: %s\n", argv[4]);
        return 1;
    }

    page_table_delete(pt);
    disk_close(disk);

    free(frame_table);
	free(head);
	free(tail);

	printf("Page Faults: %d\n", disk_reads+disk_writes);
	printf("Disk Reads:  %d\n", disk_reads);
	printf("Disk Writes: %d\n", disk_writes);
	printf("\n");
    return 0;
}