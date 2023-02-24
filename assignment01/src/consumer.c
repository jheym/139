/*
CSC139
Spring 2023
First Assignment
Heyman, Justin
Section 2
OSs Tested on: 	Ubuntu-22.04, GCC 11.3.0, -std=gnu89
				RHEL-7.9, GCC 4.8.5, -std=gnu89 [ecs-pa-coding1] 
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

// Size of shared memory block
// Pass this to ftruncate and mmap
#define SHM_SIZE 4096

// Global pointer to the shared memory block
// This should receive the return value of mmap
// Don't change this pointer in any function
void *gShmPtr;

// You won't necessarily need all the functions below
void SetIn(int);
void SetOut(int);
void SetHeaderVal(int, int);
int GetBufSize();
int GetItemCnt();
int GetIn();
int GetOut();
int GetHeaderVal(int);
void WriteAtBufIndex(int, int);
int ReadAtBufIndex(int);

int main()
{
	const char *name = "OS_HW1_heymanJustin"; // Name of shared memory block to be passed to shm_open
	int bufSize;													// Bounded buffer size
	int itemCnt;													// Number of items to be consumed
	int in;															// Index of next item to produce
	int out;														// Index of next item to consume

	// Write code here to create a shared memory block and map it to gShmPtr
	// Use the above name
	// **Extremely Important: map the shared memory block for both reading and writing
	// Use PROT_READ | PROT_WRITE

	int shm_fd = shm_open(name, O_RDWR, 0666);
	
	// Error checking
	if (shm_fd == -1)
	{
		printf("Error opening %s\n", name);
		exit(-1);
	}

	gShmPtr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);


	// Write code here to read the four integers from the header of the shared memory block
	// These are: bufSize, itemCnt, in, out
	// Just call the functions provided below like this:
	bufSize = GetBufSize();
	itemCnt = GetItemCnt();
	in = GetIn();
	out = GetOut();
	int i;

	// Write code here to check that the consumer has read the right values:
	// printf("Consumer reading: bufSize = %d\n", bufSize);
	// printf("Consumer reading: itemCnt = %d\n", itemCnt);
	// printf("Consumer reading: in = %d\n", in);
	// printf("Consumer reading: out = %d\n", out);
	
	// Write code here to consume all the items produced by the producer
	// Use the functions provided below to get/set the values of shared variables in, out, bufSize
	// Use the provided function ReadAtBufIndex() to read from the bounded buffer
	// **Extremely Important: Remember to set the value of any shared variable you change locally
	// Use the following print statement to report the consumption of an item:
	// printf("Consuming Item %d with value %d at Index %d\n", i, val, out);
	// where i is the item number, val is the item value, out is its index in the bounded buffer
	
	for (i = 1; i <= itemCnt; i++) {
		while (GetIn() == out); // Wait until there is an item to consume
		int val = ReadAtBufIndex(out);
		printf("Consuming Item %d with value %d at Index %d\n", i, val, out);
		out = (out + 1) % bufSize; // Increment out
		SetOut(out); // Update out
	}

	// remove the shared memory segment
	if (shm_unlink(name) == -1)
	{
		printf("Error removing %s\n", name);
		exit(-1);
	}

	return 0;
}

// Set the value of shared variable "in"
void SetIn(int val)
{
	SetHeaderVal(2, val);
}

// Set the value of shared variable "out"
void SetOut(int val)
{
	SetHeaderVal(3, val);
}

// Get the ith value in the header
int GetHeaderVal(int i)
{
	int val;
	void *ptr = gShmPtr + i * sizeof(int);
	memcpy(&val, ptr, sizeof(int));
	return val;
}

// Set the ith value in the header
void SetHeaderVal(int i, int val)
{
	void *ptr = gShmPtr + i * sizeof(int);
	memcpy(ptr, &val, sizeof(int));
}

// Get the value of shared variable "bufSize"
int GetBufSize()
{
	return GetHeaderVal(0);
}

// Get the value of shared variable "itemCnt"
int GetItemCnt()
{
	return GetHeaderVal(1);
}

// Get the value of shared variable "in"
int GetIn()
{
	return GetHeaderVal(2);
}

// Get the value of shared variable "out"
int GetOut()
{
	return GetHeaderVal(3);
}

// Write the given val at the given index in the bounded buffer
void WriteAtBufIndex(int indx, int val)
{
	// Skip the four-integer header and go to the given index
	void *ptr = gShmPtr + 4 * sizeof(int) + indx * sizeof(int);
	memcpy(ptr, &val, sizeof(int));
}

// Read the val at the given index in the bounded buffer
int ReadAtBufIndex(int indx)
{
	int val;
	// Skip the four-integer header and go to the given index
	void *ptr = gShmPtr + 4 * sizeof(int) + indx * sizeof(int);
	memcpy(&val, ptr, sizeof(int));
	return val;
}
