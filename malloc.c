#include<stdlib.h>
#include<stdio.h>
#include"malloc.h"

//Book-keeping structure - 16 bytes
typedef struct book
{
	int alloc; //Boolean flag to indicate whether the block is allocated or free
	int mem_size; //The size of the next memory block which the book-keeping structure is pointing to
	char *prev; //Pointer to the previous book-keeping structure
	char *memory; //Pointer to the memeory block to which the book-keeping structure is referring to
}book;

//char *p;

void allocate(int n)
{
	p = (char*)malloc(n*sizeof(char));
	//Initialising the first book-keeping structure after allocating memory to the global character array p.
	book* head = (book*)p;
	head->alloc = 0;
	head->mem_size = n - sizeof(book);
	head->memory = NULL; //The last book structure's 'next' always points to NULL
	head->prev = NULL; //The first book structure's 'prev' always points to NULL
}

void* mymalloc(int size)
{
	book* cur = (book*)p;

	while(cur->memory!=NULL)
	{
		if(cur->alloc==0 && cur->mem_size>=size) //Break the loop if a memory block is free and if its memory size is greater than or equal to the memory being asked
		{
			break;
		}	
		cur = (book*)(cur + sizeof(book) + cur->mem_size); //Changing 'cur' to point to the next book-keeping structure
	}

	if(cur->memory==NULL && cur->mem_size<size) //Reached the end of the memory, and insufficient memory block to allocate.
	{	
		return NULL;
	}
    if(cur->memory==NULL && cur->mem_size >= size && cur->mem_size<(size+sizeof(book))) //Reached the last block, and there is no space for a new book-keeping structure
	{
		cur->alloc = 1;
		return (cur+sizeof(book));
	}


	

	int total = cur->mem_size;
	if(cur->memory==NULL && cur->mem_size>=(size+sizeof(book))) //Reached the end of the memory(last memory block), but sufficient memory block to allocate.
	{	
		book* new = (book*)(cur + size + sizeof(book)); //Creating a new book-keeping structure
		cur->alloc = 1; //The current block is allocated 
		cur->mem_size = size;
		cur->memory = (char*)(cur + sizeof(book));
		new->alloc = 0; //The new block is free
		new->memory = NULL; //The new memory block is the last block, hence it should point to NULL
		new->mem_size = total - sizeof(book) - size; //Calculating the size of the new free memory block
		new->prev = (char*)cur; //The 'prev' of the new block pointing back to the current book-keeping structure
		return cur->memory;
	}
	if(cur->mem_size>(size+sizeof(book))) //Reached any other block except the last block, and sufficient memory available.
	{
		book* new = (book*)(cur + size + sizeof(book)); //Creating the new book-keeping structure
		book* cur_next = (book*)(cur + sizeof(book) + total); //Pointer to the next book structure after the current book structure
		cur->alloc = 1; //The current block is allocated
		cur->mem_size = size;
		cur->memory = (char*)(cur + sizeof(book));
		new->alloc = 0; //The new block is free
		new->mem_size = total - sizeof(book) - size; //Free memory size held by the new book-keeping structure
		new->memory = (char*)(new + sizeof(book)); //New book-keeping structure pointing to its free memory block
		new->prev = (char*)cur; // the previous book structure for the new structure will be the current book structure
		cur_next->prev = (char*)new; //The new book structure is added in between the current book structure and its next book structure presently
		return cur->memory;
	}
	else   //Not enough space for a new book-keeping structure. Hence, the remaining memory is allocated to the newly created block itself.
	{
		cur->alloc=1; //The current block is allocated
		cur->memory = (char*)(cur + sizeof(book)); //Pointing to the memory block of the current book-keeping structure
		return cur->memory;
	}


	

}


void display_mem_map()
{
	book* cur = (book*)p;
	int adr = 0;

	while(cur->memory!=NULL)
	{
		printf("%d\t%d\tbook\n",adr, sizeof(book));
		adr = adr + sizeof(book); //Address of the memory block pointed by the current book-keeping structure

		if(cur->alloc == 1)
			printf("%d\t%d\tallocated\n",adr, cur->mem_size); //If the block is allocated
		else
			printf("%d\t%d\tfree\n",adr, cur->mem_size); //If the block is free
		
		adr = adr + cur->mem_size;
		cur = (book*)(cur + sizeof(book) + cur->mem_size); //Traversing to the next book-keeping structure
		
	}
	if(cur->mem_size!=0)
	{
		printf("%d\t%d\tbook\n",adr,sizeof(book)); //Printing details of the last memory block
		if(cur->alloc == 1)
			printf("%d\t%d\tallocated\n",adr, cur->mem_size); //If the block is allocated
		else
			printf("%d\t%d\tfree\n",adr, cur->mem_size); //If the block is free
	}

}

void myfree(void *b)
{
	book* cur = (book*)p;

	while(cur->memory!=b && cur->memory!=NULL) //Traversing until we find the memory pointed by the address in the parameter
	{
		cur = (book*)(cur + sizeof(book) + cur->mem_size);
	}
	
	if(cur->memory==b) //If the memory is found
	{
		if(cur->memory!=NULL) //Checking if it is not the last block
		{
			book* next = (book*)(cur + sizeof(book) + cur->mem_size); //Pointer to the next booking structure
			cur->alloc = 0;
			if(next->alloc==0 && next->memory!=NULL) //If the next block is free and not the last block, merge the current block and the next block
			{
				cur->mem_size = cur->mem_size + sizeof(book) + next->mem_size ;
				next = (book*)(next + sizeof(book) + next->mem_size);
				next->prev = (char*)cur;
			}
			else if(next->alloc==0 && next->memory==NULL) //If the next block is the last block, merge the current block and the next block, together to be the last block in the memory			
			{
				cur->memory = NULL;
				cur->mem_size = cur->mem_size + sizeof(book) + next->mem_size;

			}
		}

		if(cur->prev!=NULL) //Checking if it is not the first block
		{
			cur->alloc = 0;
			book* previous = (book*)(cur->prev);
			if(previous->alloc==0) //If the previous block is free and not the first block, merge the previous and the current block into one block
			{
				previous->mem_size = previous->mem_size + sizeof(book) + cur->mem_size;
				book* next = (book*)(cur + sizeof(book) + cur->mem_size);
				next->prev = (char*)previous;
			}
			else if(previous->alloc==0 && cur->memory==NULL) //If the current block is the last block, merge its previous block and itself to be the last block of the memory
			{
				previous->mem_size = previous->mem_size + sizeof(book) + cur->mem_size;
				previous->memory = NULL;
			}
			// else if(previous->alloc==0&&previous->prev==NULL)
			// {
			// 	previous->mem_size = previous->mem_size + sizeof(book) + cur->mem_size;
			// }
		}
	}

}

void print_book()
{
	printf("Size of book-keeping structure : %d bytes",sizeof(book)); //Printing the size of each book-keeping structure
}

