#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

struct Page
{
    int pageId;
    int accessTime;
    int accessType; // 0 For R and 1 For M
};

static const struct Page EmptyPage;

// METHODS DEFINITION
void printPagesArray(struct Page pages[], int size);
void printPagesArray2(struct Page pages[], int size);
struct Page handleFault(struct Page newPage);
// bool loadPlayer(struct Wheel* wheel);
// struct Player removeIndex(int index);

//ATTRIBUTES
int countOfPages; // total number of pages
int countOfCurrentPages = 0; // number of pages in pagesList
struct Page pages[100]; // input file will be saved here
struct Page pagesList[5]; // this is the linked list that represents the page frames
char path[] = "pages.txt";
int time = 0;

int main()
{

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int a = 0;
    fp = fopen(path, "r");
    if (fp) //Store input file in pages array
    {
        while ((read = getline(&line, &len, fp)) != -1)
        {
            char *pch;
            pch = strtok(line, ",");
            int j = 0;
            struct Page page;
            while (pch != NULL)
            {
                if (j == 0)
                {
                    pages[a].pageId = atoi(pch);
                }
                else if (j == 1)
                {
                    pages[a].accessTime = atoi(pch);
                }
                else if (j == 2)
                {
                    if (strstr(pch, "R") != NULL)
                    {
                        pages[a].accessType = 0;
                    }
                    if (strstr(pch, "W") != NULL)
                    {
                        pages[a].accessType = 1;
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
            countOfPages++;
        }
        fclose(fp);
    }
    int i;
    for (i = 0; i < countOfPages; i += 1)
    {
        printf("Page to be inserted: %i\n",pages[i].pageId);
        handleFault(pages[i]);
        printPagesArray(pagesList, countOfCurrentPages);
    }

}

struct Page handleFault(struct Page newPage){

    int i;
    struct Page page;
    page = pagesList[0];
    if(countOfCurrentPages==5){
        for (i = 0; i < countOfCurrentPages-1; i += 1)
        {
            pagesList[i] = pagesList[i+1];

        }
        countOfCurrentPages--;
    }
    pagesList[countOfCurrentPages] = newPage;
    countOfCurrentPages++;

    return page;
}

void printPagesArray(struct Page pages[], int size)
{
    struct Page page;
    page = pages[0];
    int i;
    printf("Pages currently in arrayList:\n");
    for (i = 0; i < size; i += 1)
    {
        printf("Page ID: %i ,", pages[i].pageId);
        printf("access time: %i ,", pages[i].accessTime);
        printf("access type: %i\n", pages[i].accessType);
    }
    printf("********************************************\n");
}

void printPagesArray2(struct Page pages[], int size)
{
    struct Page page;
    page = pages[0];
    int i;
    for (i = 0; i < size; i += 1)
    {
        printf("%i, ", pages[i].pageId);
    }
    printf("\n");
}