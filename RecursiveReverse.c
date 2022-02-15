

/*  *** Program to reverse large text files in memory, recursively and iteratively ***
 * 
 * User selects their desired text file and number of runs, as well as a limit value for the 
 * recursive function. User's .txt file is read into memory, compares the
 * time it takes to reverse the data in memory and outputs the reversed text to two .txt files.
 * Note: to avoid stack overflow with large files, I devised a way to split the recursive function
 * into two mutual recursive functions, a recursive helper nested within the main recursive call.
 * 
 * recursiveReverse()   <--- call made in driver
 *      reverseSegment();  <--- helper function
 *      recursiveReverse();
 * 
 * The helper function contains the acutal reversal logic, but is limited (by the user's input)
 * using a decrementing counter so that the total frames on the stack do not exceed available memory.
 * 
 *   -- Put simply --   4MB text files are too much for C to sort on the stack recursively, so the nested helper 
 * function pops its own stack in each of the caller's stack frame, based on a limit that is equal to 
 * the number of characters (including spaces and escape sequences) that will be sorted by one recursive function at a time.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void recursiveReverse(char*, int, int, char *, int);
void reverseSegment(char*, int, int, int, char*);
void iterativeReverse(char*, char*, int);
int suggestLimit(int);

int main(void){

    //variable declarations, some intitializations
    char c, quit, chosenFile[100];
    int i, rcLimit, trialRuns, recommended = 2011, charCount = 0;
    double rcTotal, itTotal;

    FILE *countF, *bibF, *rcRvBib, *itRvBib;
    clock_t rcTime, itTime;
    
    //Allows user to choose their .txt file
    printf("\n   ---Lab 1 B: Large Text Reversal---   \n\n"); 
    printf("Enter the .txt file name you wish to open (ex: Bible.txt): ");
    scanf("%s", chosenFile);

    //Gets total number of characters from source file
    countF = fopen(chosenFile, "r");
    while((c = fgetc(countF)) != EOF)
        charCount++;
    fclose(countF);

    //allocates heap memory based on size of source file
    char *fwd = malloc(sizeof(char) * charCount);
    char *bwd = malloc(sizeof(char) * charCount); 
    char *itBwd = malloc(sizeof(char) * charCount);

    //loads source file into memory
    bibF = fopen(chosenFile, "r");
    for(i=0;i<charCount;i++){
        c = fgetc(bibF);
        fwd[i] = c;
    }
    fclose(bibF);

    //generates suggested limit for the recursive helper function
    //this value is calculated to be near the most efficient limit value (in terms of space complexity)
    //based on mathematic function that describes the stack frames generated
    //by the recursive functions, then suggests this number to the user
    recommended = suggestLimit(charCount);
    

    //I/O Loop
    while(1){

        //input
        printf("\nEnter the limit for Recursive helper function.\n");
        printf("--> %d <-- is the recommended limit for %s\n", recommended, chosenFile);
        printf("Try different values to test break points!\nEntry: ");
        scanf("%d", &rcLimit);
        printf("Enter the number of trial Runs: ");
        scanf("%d", &trialRuns);
        
        //loop for iterative speed tests
        itTime = clock();
        for(i=0;i<trialRuns;i++)
            iterativeReverse(fwd, itBwd, charCount);
        itTime = clock() - itTime;
        printf("\nProcessing...\n\n");

        //loop for recursive speed tests
        rcTime = clock();
        for(i=0;i<trialRuns;i++)
            recursiveReverse(fwd, charCount, charCount, bwd, rcLimit);
        rcTime = clock() - rcTime;

        //converts data to seconds
        rcTotal = ((double)rcTime)/CLOCKS_PER_SEC;
        itTotal = ((double)itTime/CLOCKS_PER_SEC);

        //output
        printf("******************************************************************\n");
        printf("%s has been reversed iteratively and recursively %d times.\n", chosenFile, trialRuns);
        printf("Iterative Time: %f\nRecursive Time: %f\n", itTotal, rcTotal);
        printf("The Iterative test is %f seconds faster.\n", (rcTotal-itTotal));
        printf("******************************************************************\n");
        printf("\nTry again with a new number of trial runs and recursive limit?\n(y) to try again, (n) to view reversed files: ");
        scanf("%1s", &quit);
        if (quit != 'y')
            break;
    }

    //reversed recursive array written to .txt
    rcRvBib = fopen("rcReverseBible.txt", "w");
    for(i=0;i<charCount;i++)
        fputc(bwd[i], rcRvBib);

    //reversed iterative array written to .txt
    itRvBib = fopen("itReverseBible.txt", "w");
    for (i=0;i<charCount;i++)
        fputc(itBwd[i], itRvBib);

    //frees heap memory
    free(fwd);
    free(bwd);
    free(itBwd);
    return 0;
}

//Limits the stack size by popping the stack of a mutual recursive helper function,
//reverseSegment() at a predetermined base case

//****** Maximum Number of Stacks Mathematical Equation: (n/limit) + limit *******

void recursiveReverse(char * f, int count, int idx, char * b, int limit){
    
    //base case: index counter is 0, end of array is reached
    if (idx>0){

        //calls helper function, lets it stack up to it's limit and unwind
        //Then makes the actual recursive call 
        reverseSegment(f, count, idx, limit, b);
        recursiveReverse(f, count, (idx-limit), b, limit);
    }
}

//mutual recursive function containing the actual reversal logic
//will only stack up to the limit and then unwind, or if the end of the array is reached
void reverseSegment(char * f, int count, int idx, int limit, char * b){

    //base case
    if(limit > 0 && idx > 0){
        b[count-idx] = f[idx-1];
        reverseSegment(f, count, --idx, --limit, b);
    }
}

//sets value of first array index to last value of initial array,
//then iterates upwards where item of new array is always that of 
//the initial array's max index minus the value of the iterator -1 
void iterativeReverse(char * f, char * b, int size){
    int i;
    for (i=0;i<size;i++)
        b[i] = f[size-1-i];
}

//Approximates the local minima of the max stacks equation
//Suggests this number to user to avoid unintentional stack overflow
int suggestLimit(int size){
    long double low=20000, val;
    int i, lowIdx;
    for (i=1;i<20000;i++){
        val = ((double)size/i) + i;
        if (val < low){
            low = val;
            lowIdx = i;
        }
    }
    return lowIdx-1;
}
/*
Output will be two files named rcReverseBible.txt and itReverseBible.txt
placed in local directory
*/
