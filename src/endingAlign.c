#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int endingAppendSpace ( char *fileName, char* col );
int endingRemoveSpace ( char* fileName );
int file_exists ( char *fileName );
int isDigitalStr ( char *str );
void help ( void );

int main ( int argc, char **argv ) {

    char ch = '\0';
    char* fileName = NULL;
    char* column = NULL;
    int removeFlag = 0;
    while( ( ch = getopt( argc, argv, "c:f:hr" ) ) != EOF ) { 
        switch(ch) { 
        case 'c': 
            column = optarg;
            break; 
        case 'f': 
            fileName = optarg; 
            break; 
        case 'r': 
            removeFlag = 1;
            break; 
        case 'h': 
            help();
            break; 
        case '?': 
            help();
            break; 
        } 
    }

    if ( fileName != NULL ){

        if ( !file_exists ( fileName ) ) {
            printf("file is not exsit.\n");
            return -1;
        }

        if ( column != NULL && removeFlag != 1 ) {

            if ( !isDigitalStr(column) ) {
                printf("column is not a digital string.\n");
                return -1;
            }

            endingAppendSpace ( fileName, column );
            return 0;

        } else if ( column == NULL && removeFlag == 1 ) {

            endingRemoveSpace( fileName );
            return 0;

        } else {

            help();

        }
    } else {

        help();

    }

}

int endingAppendSpace ( char *fileName, char* col ) {

    int    ret                      = 0;
    int    column                   = 0;
    char  *tempFile                 = "generateFile.txt";
    char  *readBuffer               = NULL;
    size_t bufferSize               = 512;
    int    readBufferIndex          = 0;
    int    writeBufferIndex         = 0;
    char   writeBuffer[bufferSize];

    column = atoi ( col );

    FILE *readfile  = fopen ( fileName, "r" );                         // only read
    FILE *writefile = fopen ( tempFile, "w" );                        // only write

    /**
     * man datasheet:
     *    If *lineptr is NULL, then getline() will allocate a buffer for storing the line, 
     *    which should be freed by the user program. (In this case, the value in *n is ignored.)
     */
    while ( ( ret = getline( &readBuffer, &bufferSize, readfile ) ) != -1 ) {

        readBufferIndex  = 0;
        writeBufferIndex = 0;

        while ( readBufferIndex < ret ) {
            if ( readBuffer [ readBufferIndex ] != '\t' )             // kick out '\t'
                writeBuffer [ writeBufferIndex++ ] = readBuffer [ readBufferIndex++ ];
            else {
                memset ( writeBuffer + writeBufferIndex, ' ', 4 );    // every tab key was 4 space
                writeBufferIndex += 4;    
                readBufferIndex++;
            }
        }

        writeBufferIndex--;                                           // back to real index

        if ( ( column - writeBufferIndex ) > 0 ) {                    // may be real column longer than we set
            // '\n' at end of a line, need to kick it out
            memset ( writeBuffer + writeBufferIndex, ' ', column - writeBufferIndex );
            writeBuffer [ column ] = '\n';                            // for end of a line 
            writeBuffer [ column + 1 ] = '\0';                        // for end of a string
        } else 
            writeBuffer [ writeBufferIndex + 1 ] = '\0';              // make sure end of a string

        fputs ( writeBuffer, writefile );                             // write to file 

        bzero ( readBuffer,  bufferSize );                            // clean buffer
        bzero ( writeBuffer, bufferSize );

    }

    free ( readBuffer );                                              // referrence getline()

    fclose ( readfile );
    fclose ( writefile );

    remove ( fileName );                                               // delete source file
    rename ( tempFile, fileName );                                     // tempfile rename to source file
}


int endingRemoveSpace ( char* fileName ) {

    int    ret                      = 0;
    char  *tempFile                 = "generateFile.txt";
    char  *readBuffer               = NULL;
    size_t bufferSize               = 512;
    int    readBufferIndex          = 0;

    FILE *readfile  = fopen ( fileName, "r" );                         // only read
    FILE *writefile = fopen ( tempFile, "w" );                        // only write

    /**
     * man datasheet:
     *    If *lineptr is NULL, then getline() will allocate a buffer for storing the line, 
     *    which should be freed by the user program. (In this case, the value in *n is ignored.)
     */
    while ( ( ret = getline( &readBuffer, &bufferSize, readfile ) ) != -1 ) {

        readBufferIndex  = ret - 1; // index for real position

        while ( ( readBuffer [ readBufferIndex ] == ' ' ) ||          // del ' ', '\t', '\n' character
                ( readBuffer [ readBufferIndex ] == '\n' ) || ( readBuffer [ readBufferIndex ] == '\t' ) )  
            readBuffer [ readBufferIndex-- ] = '\0';
        
        readBuffer [ readBufferIndex + 1 ] = '\n';

        fputs ( readBuffer, writefile );                              // write to file 

        bzero ( readBuffer,  ret );                                   // clean buffer
    }

    free ( readBuffer );                                              // referrence getline()

    fclose ( readfile );
    fclose ( writefile );

    remove ( fileName );                                               // delete source file
    rename ( tempFile, fileName );                                     // tempfile rename to source file
}

int file_exists ( char *fileName ) { 
    if ( access(fileName, F_OK) == 0 ) {
        return 1;
    } else {
        //printf("%s is not exist.\n", fileName);
        return 0;
    }
}

int isDigitalStr ( char *str ) {   
    int len = strlen(str);
    char *s = str;
    int i = 0;
 
    while ( '0' <= *s && *s <= '9' && i < len ){
        s++;
        i++;
    }

    if(i == len)
        return 1;
    else 
        return 0;       
}

void help( void ) {
    printf ( "usage:\n" );
    printf ( "     endingAlign -f <filename> -c <column>\n" );
    printf ( "     endingAlign -r -f <filename>\n" );
}
