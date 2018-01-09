#include <stdio.h>
#include <stdlib.h>

//LCS/SES based FileComp (file comprare)
/*
AUTHOR: Piotr Domanski
LCS is basically impementation of Myers' Diff Algorithm
project realized durng studies @WEITI, Polish Universiti of Technology
v1.0 09.01.2018
*/

struct line{
    int numOfWords;
    int averageWordLenght;
    fpos_t positionOfEnd;
}; //struct line, file will be devided into those

struct Vertex {
	int x;
	int y;
	struct Vertex * previous;
};//i need plenty of those...


enum dir{right, down}; //in LCS, i need to know if to go right or down
enum state {word , other, wait}; //while dividing file infto lines, we need some states of reading buff

void fileDecompose ( FILE * input, struct line lines[]); //divides file into lines structure
FILE * getFile (); //get this file from user!
int getMaxIL (); //simply "getter"
int countLines ( FILE * input ); //nice function, it counts fines of text in file 
int LCS(struct line * one, int oneLenght, FILE * input1, struct line * two, int twoLenght, FILE * input2, int MAX, int * IL1, int * IL2); //implementation of Myers' Diff Algorithm
int linesEqual(struct line * one, FILE * input1, struct line * two, FILE * input2); //we need to oomprare single lines, it ain't easy so i created special function
void displayLine(fpos_t position, FILE * input); //like linesEqual, displaying those lines ain't easy too

int main() //returns 1 if error, 0 if ok
{
	//create FILES
    FILE * input1; 
    FILE * input2; 
	
	//get those files from user
    input1 = getFile();
    input2 = getFile();

    if( input1 == NULL || input2 == NULL)//if one or both files are empty, error
    {
        puts("ERROR BLU SKRIN \n can't load files, check if you put .txt extension");
        return 1;
    }//file emptiness error

    int maxIL = getMaxIL(); //IL stands for incompatibile lines

    int IL[2][maxIL], numbOfIL, lines[2]; //list of those Incompatibile lines for each file, number of them AND number of lines in each file

	//get the number of lines in each file
    lines[0] = countLines(input1);
    lines[1] = countLines(input2);
    if( abs( lines[0] - lines[1] ) > maxIL )//if there are far too many missing lines...
    {
        puts("ERROR BLU SKRIN \n There are too many different lines in files");
        return 1;
    }//lines abs error

    struct line * lines1; //structs for file 1
    lines1 = malloc(sizeof(struct line) * lines[0]);
    struct line * lines2; //structs for file 2
    lines2 = malloc(sizeof(struct line) * lines[1]);
    fileDecompose( input1 , lines1 );
    fileDecompose( input2 , lines2 );

	//where the all magic happends, numbOfIL is now number of IL :))) and IL[0] holds, from now, numbers of IL in first file, which we want to display, simillary IL[1] holds those from second file
	numbOfIL = LCS(lines1, lines[0], input1, lines2, lines[1], input2, maxIL, IL[0], IL[1]);

	if (numbOfIL == -1) //LCS returns -1 there are too many IL
    {
        puts("ERROR BLU SKRIN \n There are too many different lines in files");
        return 1;
    }//too many IL, LCS = -1
	
	//in other case we have those IL, and we can display them
	int i;
	for(i = 0; i < numbOfIL; i++)
    {
		//printf("X: %d Y: %d", IL[0][i], IL[1][i]); testlinje
		
		//from first file
        printf("Difference number %d:\nin first file there's: ", i+1);
        displayLine(lines1[IL[0][i]].positionOfEnd, input1);
		//from second file
        printf("in second file there's: ");
        displayLine(lines2[IL[1][i]].positionOfEnd, input2);
    }//displaying IL
	
	free(lines1);
	free(lines2);
	fclose(input1);
	fclose(input2);

    return 0;
}//main



FILE * getFile ()
{
    char fileName[64];
    FILE * input;
    puts ("Enter file name with extension after dot:");
    scanf ("%s", &fileName);
    input = fopen (fileName, "r");

    return input;

}//get file pointer

int getMaxIL ()
{
    int temp;
    puts( "Enter maximum number of incompatibile lines: " );
    scanf( "%d", &temp );
    return temp;
}//getmaxIL

int countLines( FILE * input )
{
    int lines = 1;
    int c;
    while( ( c = fgetc(input) ) != EOF)
        if(c == '\n') lines++;
    rewind(input);
    return lines;
}//count numb of lines in file

void fileDecompose ( FILE * input, struct line lines[])
{
    enum state state;
    state = wait; //at first, we wait for chars
    int c, i, chars, words; //c will hold chars from file (as ints), i will hold number of current line, chars number of chars in line, so as words hold number of words in line
    i = 0; chars = 0; words = 0; //we need to set them to 0 at first
	fpos_t position; //will hold the position of beggining, names as end to misslead people...

    while( ( c = getc(input) ) != EOF )//while input isn't pointing at file's end
        {
			//at first, if we waited, this is the begging of next line, so we have to save position
			if(state == wait) 
			{
				state == other; //set state to other, we're no longer waiting for next line
				fgetpos( input, & position ); //save position in file
			}//position in beg of line
				
			
			//printf(" %c",c );
            if( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ) //if we read chars
            {
				if(state == other) words++; //if we werent in word before, we jsut found new one!
                state = word; //the state set, so we know we are in middle of word
                chars++; //we found char, so number of them increases
            }//word read
            else
                state = other; //else not in word :)

            if(c == '\n')//if we reached end of line
            {
				//saving new struct
                lines[i].numOfWords = words;
                if(words !=0) lines[i].averageWordLenght = chars / words;
				else lines[i].averageWordLenght = chars;
				lines[i].positionOfEnd = position - 1;

				//prepare for next line!
                i++;
                chars = 0;
                words = 0;
				state = wait;
            }

        }//while

		//after exiting loop, there's last line to save
        lines[i].numOfWords = words;
		if(words !=0) lines[i].averageWordLenght = chars / words;
		else lines[i].averageWordLenght = chars;
		lines[i].positionOfEnd = position - 1;

}//BEST FUNCTION, HELPS SOOOO MUCH!!!

int LCS(struct line * one, int oneLenght, FILE * input1, struct line * two, int twoLenght, FILE * input2, int MAX, int * IL1, int* IL2)
{
	enum dir dir;

	struct Vertex * path[MAX * 2 + 1];
	struct Vertex point [(MAX+1)*(MAX+2)/2];
	struct Vertex V[MAX*2 + 1];
	int i;
	for(i = 0; i <= MAX*2; i++)
	{
		V[i].x = 0;
		V[i].y = 0;
		V[i].previous	= NULL;
		path[i] = NULL;
	}//set vertex's to 0
	for(i = 0; i < (MAX+1)*(MAX+2)/2; i++)
	{
		point[i].x = 0;
		point[i].y = 0;
		point[i].previous	= NULL;
	}//set vertex's to 0

	int d,k;



	i=0;

	for( d = 0; d <= MAX; d++ )
	{
		for( k = -d; k<=d; k+=2)
		{
			dir=right;
			if( k == -d || (k != d && V[MAX - ( k - 1 )].x < V[MAX - ( k + 1 )].x ) ) dir = down;

			if( dir == down ) V[MAX - k].previous = &V[MAX - (k+1)];
			else V[MAX - k].previous = &V[MAX - (k-1)];

			V[MAX - k].x = V[MAX - k].previous->x;
			V[MAX - k].y = V[MAX - k].previous->y;

			//printf("%d %d \n", V[MAX - k].x, V[MAX - k].y);

			V[MAX - k].x += dir==down? 0 : 1;
			V[MAX - k].y = V[MAX - k].x - k;

			//V and path are temporary, so i save them to point list :)
			path[MAX - k] = &point[i];
			point[i].x = V[MAX - k].x;
			point[i].y = V[MAX - k].y;
			point[i].previous = dir==down? path[MAX - (k+1)] : path[MAX - (k-1)];

			//printf("%d %d \n", V[MAX - k].x, V[MAX - k].y);

			//follow diagonals
			while( V[MAX - k].x < oneLenght && V[MAX - k].y < twoLenght )
			{
			    if( linesEqual(&one[V[MAX - k].x + 1], input1, &two[V[MAX - k].y + 1], input2)==1 )
                {
                    V[MAX - k].x++;
                    V[MAX - k].y++;
                }
				else break;
			}

			if(V[MAX - k].x >= oneLenght && V[MAX - k].y >= twoLenght)
            {
                //puts("XD");
                int j;
                j = 0;
                while( 1 )
                {
					//lines used for test
                    //if( linesEqual(&one[point[i].x], input1, &two[point[i].y], input2)==0 )
                    //puts("XD");
                    //printf("%d %d \n", point[i].x, point[i].y);
					
                    IL1[j] = point[i].x;
                    IL2[j] = point[i].y;
                    j++;

                    if( ( point[i].previous )== NULL) break;
                    else point[i] = *point[i].previous;
                }
                return d;
            }
            i++;//next index of point
		}//analize of diagonals
	}//d to max loop, while IL < maxIL simply

	return -1; //returns -1 if there are too many IL

}//LCS

int linesEqual(struct line * one, FILE * input1, struct line * two, FILE * input2)
{
    if(one->numOfWords == one->numOfWords && one->averageWordLenght == one->averageWordLenght) 
    {
        fsetpos( input1, &one->positionOfEnd );
        fsetpos( input2, &two->positionOfEnd );
        int c,d;
        c = getc(input1);
        d = getc(input2);

        while( 1 )
        {
            if(c!='\n' || c!=EOF) c = getc(input1);
            if(d!='\n' || c!=EOF) d = getc(input2);

            if( ( c == '\n' || c == EOF )&& ( d == '\n' || d == EOF ) ) break;
            if( c != d ) return 0;
        }
        return 1;
    }
    return 0; // false, they dont equal :)
}//lines equal

void displayLine(fpos_t position, FILE * input)
{
    fsetpos(input, &position);//set position in file
    int c;
    while( ( c = getc(input) ) != '\n' && c != EOF ) //till the end
        printf("%c", c);
    printf("\n");
}//display line
