#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

struct PageRequest
{
    int pageId;
    int accessTime;
    int accessType; // 0 For R and 1 For M
};

struct Page
{
    int pageId;
    int accessTime;
    int R;
    int M;
};

typedef struct node {
    struct Page page;
    struct node * next;
} node_t;

static const struct PageRequest EmptyPageRequest;
static const struct Page EmptyPage;

// METHODS DEFINITION
void printPagesArray(struct PageRequest pages[], int size);
void printPagesArray2(struct PageRequest pages[], int size);
struct Page handleFault(struct Page newPage);
void handlNotFault(struct PageRequest newPage);
void handleInterrupt();
void clearR();
int list_includes(int pageId);
int list_size();
void push(struct Page page); //Push at the end (TAIL)
struct Page pop(); //Pop first element (HEAD)
void putLast(int index);
void print_list();

//ATTRIBUTES
int countOfPageRequests; // total number of page requests
int countOfPages = 0; // number of pages in pagesList
struct PageRequest pageRequests[100]; // input file will be saved here
node_t * head = NULL; // this is the head of the linked list that represents the page frames
char path[] = "pages.txt";
int time = 0;
FILE *file;
FILE *file2;
int main()
{
    printf("You will find two files: one detailed and one short\n");
    printf("Please read the detailed file if anything is not clear in the short one\n");
    file = fopen("Detailed-FIFO-Output.txt", "w");// "w" means that we are going to write on this file
    file2 = fopen("FIFO-Output.txt", "w");// "w" means that we are going to write on this file

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int a = 0;
    fp = fopen(path, "r"); // We are going to read this file
    if (fp) //We are going to store the information of the input file in the pageReqeusts array
    {
        while ((read = getline(&line, &len, fp)) != -1)
        {
            char *pch;
            pch = strtok(line, ",");
            int j = 0;
            struct PageRequest page;
            while (pch != NULL)
            {
                if (j == 0)
                {
                    pageRequests[a].accessTime = atoi(pch);
                }
                else if (j == 1)
                {
                    pageRequests[a].pageId = atoi(pch);
                }
                else if (j == 2)
                {
                    if (strstr(pch, "R") != NULL)
                    {
                        pageRequests[a].accessType = 0;
                    }
                    if (strstr(pch, "W") != NULL)
                    {
                        pageRequests[a].accessType = 1;
                    }
                }
                j++;
                pch = strtok(NULL, ",");
            }
            if (j == 1)
            {
                a--;
            }
            a++;
            countOfPageRequests++;
        }
        fclose(fp);
    }

    int i;
    for (i = 0; i < countOfPageRequests; i += 1) //Loop until all page requests are over
    {
        if(time+20<=pageRequests[i].accessTime){ //checks if we should handle time interrupt
            handleInterrupt();
        }
        if(list_includes(pageRequests[i].pageId)==1){ //if the list includes the page then we will just modify it
            fprintf(file,"PAGE FOUND\n");
            fprintf(file,"Time: %ims\n",pageRequests[i].accessTime);
            fprintf(file,"id of modified page: %i\n",pageRequests[i].pageId);
            handlNotFault(pageRequests[i]);
        } else{ // If the list does not include the page we will handle fault
            fprintf(file,"PAGE FAULT\n");
            fprintf(file,"Time: %ims\n",pageRequests[i].accessTime);
            fprintf(file,"id of loaded page: %i\n",pageRequests[i].pageId);
            fprintf(file2,"PAGE FAULT\n");
            fprintf(file2,"Time: %ims, id of loaded page: %i, ",pageRequests[i].accessTime,pageRequests[i].pageId);

            struct Page newPage = EmptyPage; //new Page to be inserted in the linked list
            newPage.pageId = pageRequests[i].pageId;
            newPage.accessTime = pageRequests[i].accessTime;
            if(pageRequests[i].accessType==0){ // setting the R&M flag from access type
                newPage.R = 1;
                newPage.M = 0;
            } else{
                newPage.R = 0;
                newPage.M = 1;            
            }
            handleFault(newPage); //Handle fault in FIFO means remove Head and add new page in Tail(If linked list is full - size 5)
        }
    }

}

struct Page handleFault(struct Page newPage){
    if(list_size()<5){ // if linked list is less than 5 then just add the new page
        push(newPage);
        fprintf(file,"id of evicted page: n/a\n");
        fprintf(file2,"id of evicted page: n/a\n");
        fprintf(file2,"*****************************************\n");
        print_list(head);
    } else{ //If linked list is full then remove head and put new page in tail
        struct Page evictedPage = pop(); // Remove head
        fprintf(file,"id of evicted page:%i\n",evictedPage.pageId);
        fprintf(file2,"id of evicted page:%i\n",evictedPage.pageId);
        if(evictedPage.M ==1){ // if Modified bit is 1 then the page must be written back to disk 
            fprintf(file,"Evicted Page Written Back to disk\n");
            fprintf(file2,"Evicted Page Written Back to disk\n");
        }
        push(newPage);//put new page in tail
        fprintf(file2,"*****************************************\n");
        print_list(head);
    } 
}

void handlNotFault(struct PageRequest pageRequest){
    node_t * current = head;
    while (current != NULL) {
        if(current->page.pageId==pageRequest.pageId){
            if(pageRequest.accessType==0){ //R BIT
                current->page.R = 1;
            } else{
                current->page.M = 1;                
            }
        }
        current = current->next;
    }
    print_list(head);  
}

void handleInterrupt(){
    time = time + 20;
    fprintf(file,"TIME INTERRUPT : %ims\n",time);
    fprintf(file2,"TIME INTERRUPT : %ims\n",time);
    fprintf(file,"*****************************************\n");
    fprintf(file2,"*****************************************\n");
    clearR();
    print_list();
}

void clearR(){
    node_t * current = head;

    while (current != NULL) {
        current->page.R = 0;
        current = current->next;
    } 
}


int list_includes(int pageId){
    node_t * current = head;

    while (current != NULL) {
        if(current->page.pageId==pageId){
            return 1;
        }
        current = current->next;
    } 
    return 0;
}

int list_size(){
    int size = 0;
    node_t * current = head;

    while (current != NULL) {
        current = current->next;
        size++;
    } 
    return size;
}

void push(struct Page page) {
    node_t * current = head;
    if(current==NULL){
        head = malloc(sizeof(node_t));
        head->page = page;
        head->next = NULL;
    } else{
        while (current->next != NULL) {
            current = current->next;
        }

        /* now we can add a new variable */
        current->next = malloc(sizeof(node_t));
        current->next->page = page;
        current->next->next = NULL;
    }
}

struct Page pop() {
    struct Page retval = EmptyPage;
    node_t * next_node = NULL;

    if (head == NULL) {
        return EmptyPage;
    }

    next_node = (head)->next;
    retval = (head)->page;
    free(head);
    head = next_node;

    return retval;
}

void putLast(int index){
    node_t * current = head;
    int i=0;
    int size = list_size();
    struct Page modifiedPage = EmptyPage;
    while (i<size) {
        struct Page page = pop();
        if(i!=index){
            push(page);
        } else{
            modifiedPage = page;
        }
        i++;
    }
    push(modifiedPage); 
}


void print_list() {
    node_t * current = head;
    fprintf(file,"LINKED LIST: \n");
    while (current != NULL) {
        fprintf(file,"pageID:%i, Access Time:%ims, R:%i, W:%i\n", current->page.pageId,current->page.accessTime,current->page.R,current->page.M);
        current = current->next;
    }
    fprintf(file,"************************************\n");
}


void printPagesArray(struct PageRequest pages[], int size)
{
    struct PageRequest page;
    page = pages[0];
    int i;
    printf("Pages currently in arrayList:\n");
    for (i = 0; i < size; i += 1)
    {
        printf("Page ID: %i ,", pages[i].pageId);
        printf("access time: %ims ,", pages[i].accessTime);
        printf("access type: %i\n", pages[i].accessType);
    }
    printf("********************************************\n");
}

void printPagesArray2(struct PageRequest pages[], int size)
{
    struct PageRequest page;
    page = pages[0];
    int i;
    for (i = 0; i < size; i += 1)
    {
        printf("%i, ", pages[i].pageId);
    }
    printf("\n");
}