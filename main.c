
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "bmplib.h"

/*
 * This method enlarges a 24-bit, uncompressed .bmp file
 * that has been read in using readFile()
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the original number of rows
 * cols     - the original number of columns
 *
 * scale    - the multiplier applied to EACH OF the rows and columns, e.g.
 *           if scale=2, then 2* rows and 2*cols
 *
 * new      - the new array containing the PIXELs, allocated within
 * newrows  - the new number of rows (scale*rows)
 * newcols  - the new number of cols (scale*cols)
 */
int enlarge(PIXEL* original, int rows, int cols, int scale,
            PIXEL** new, int* newrows, int* newcols)
{
    int row, col, i, j;
    if ((rows <= 0) || (cols <= 0))
    {
        return -1;
    }
    
    *newrows = scale*rows;
    *newcols = scale*cols;
    *new = (PIXEL*)malloc((*newrows)*(*newcols)*sizeof(PIXEL));
    
    for(row = 0; row < rows; row++)
    {
        for (i =0; i < scale; i++)
        {
            for (col = 0; col < cols; col++)
            {
                for (j = 0; j < scale; j++)
                {
                    PIXEL* o = original + row*cols + col; //points to original of this index
                    PIXEL* n = (*new) + row*cols*scale*scale + cols*scale*i + col*scale + j; //points to an enlarged image
                    *n = *o; //copies original to enlarged image
                }
            }
        }
    }
    return 0;
}

/*
 * This method rotates a 24-bit, uncompressed .bmp file that has been read
 * in using readFile(). The rotation is expressed in degrees and can be
 * positive, negative, or 0 -- but it must be a multiple of 90 degrees
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 * rotation - a positive or negative rotation,
 *
 * new      - the new array containing the PIXELs, allocated within
 * newrows  - the new number of rows
 * newcols  - the new number of cols
 */
int rotate(PIXEL* original, int rows, int cols, int rotation,
           PIXEL** new, int* newrows, int* newcols)
{
    int row, col;
    if(rotation == 360 || rotation == -360 || rotation == 0) //FULL rotation
    {
        *newrows = rows;
        *newcols = cols;
        *new = (PIXEL*)malloc((*newcols)*(*newrows)*sizeof(PIXEL));
        
        for(row = 0; row < rows; row++)
        {
            for(col = 0; col < cols; col++)
            {
                PIXEL* o = original + row*cols + col;
                PIXEL* n = (*new) + row*cols + col;
                *n = *o;
            }
        }
    }
    else if(rotation == 270 || rotation == -90) // 270 rotatation( clockwise or counter)
    {
        *newrows = cols;
        *newcols = rows;
        *new = (PIXEL*)malloc((*newrows)*(*newcols)*sizeof(PIXEL));
        
        for(row = 0; row < rows; row++)
        {
            for(col = 0; col < cols; col++)
            {
                PIXEL* o = ((original) + (row*cols) + col);
                PIXEL* n = (*new)+(col*rows) + (rows-row-1);
                *n = *o;
            }
        }
    }
    else if (rotation == 180 || rotation == -180) //180 rotation (clockwise or counter)
    {
        *newrows = rows;
        *newcols = cols;
        *new = (PIXEL*)malloc((*newrows)*(*newcols)*sizeof(PIXEL));
        
        for (row = 0; row < rows; row++)
        {
            for (col = 0; col < cols; col++)
            {
                PIXEL* o = original + row*cols + col;
                PIXEL* n = (*new) + (rows-row)*cols - (col + 1);
                *n = *o;
            }
        }
    }
    else if(rotation == 90 || rotation == -270) // 90 rotation (clockwise or counter)
    {
        *newrows = cols;
        *newcols = rows;
        *new = (PIXEL*)malloc((*newrows)*(*newcols)*sizeof(PIXEL));
        
        for (row = 0; row < rows; row++)
        {
            for (col = 0; col < cols; col++)
            {
                PIXEL* o = ((original) + (row*cols) + col);
                PIXEL* n = (*new)+((cols-col-1)*rows) + row;
                *n = *o;
            }
        }
    }
    return 0;
}

/*
 * This method Vertically flips a 24-bit, uncompressed bmp file
 * that has been read in using readFile().
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 *
 * new      - the new array containing the PIXELs, allocated within
 */
int verticalflip (PIXEL *original, PIXEL **new, int rows, int cols)
{
    int row, col;
    if ((rows <= 0) || (cols <= 0))
    {
        return -1;
    }
    
    *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));
    
    for (row=0; row < rows; row++)
    {
        for (col=0; col < cols; col++)
        {
            (*new)[(rows-row-1)*cols+col] = original[row*cols+col];
        }
    }
    return 0;
}

/*
 * This method horizontally flips a 24-bit, uncompressed bmp file
 * that has been read in using readFile().
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 *
 * new      - the new array containing the PIXELs, allocated within
 */
int flip (PIXEL *original, PIXEL **new, int rows, int cols)
{
    int row, col;
    if ((rows <= 0) || (cols <= 0)) return -1;
    
    *new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));
    
    for (row=0; row < rows; row++)
    {
        for (col=0; col < cols; col++)
        {
            PIXEL* o = original + row*cols + col;
            PIXEL* n = (*new) + row*cols + (cols-1-col);
            *n = *o;
        }
    }
    
    return 0;
}


int main(int argc, char *argv[])
{
    int r, c, nr, nc;
    PIXEL *b, *nb;
    char* inputFile;
    char* outputFile;
    
    static char usage[] = "usage: bmptool [-s scale | -r degree | -f | -v][-o ouput_file] [input_file]\n";
    
    int horFlip, rot, scale, verFlip, out, numRot; //getopt flags
    int scaleVal, rotateVal;
    
    scaleVal = rotateVal = 0;
    horFlip = rot = scale = verFlip = out = numRot = 0;
    
    while((c = getopt(argc, argv, "s:r:fvo:")) != -1)
    {
        switch(c)
        {
            case 's':
                if(scale) //prompt user with error for entering 2 of same
                {
                    fprintf(stderr, "Duplicate options! Only one value.Usage: %s\n", usage);
                    exit(-1);
                }
                scale = 1;
                scaleVal = atoi(optarg);
                break;
            case 'r':
                if (rot) //prompt user with error for entering 2 of same
                {
                    fprintf(stderr, "Duplicate options! Only one value.Usage: %s\n", usage);
                    exit(-1);
                }
                rot = 1;
                rotateVal = atoi(optarg);
                break;
            case 'f':
                if (horFlip) //prompt user with error for entering 2 of same
                {
                    fprintf(stderr, "Duplicate options! Only one value.Usage: %s\n", usage);
                }
                horFlip = 1;
                break;
            case 'v':
                if (verFlip) //prompt user with error for entering 2 of same
                {
                    fprintf(stderr, "Duplicate options! Only one value.Usage: %s\n", usage);
                }
                verFlip = 1;
                break;
            case 'o':
                if(out) //prompt user with error for entering 2 of same
                {
                    fprintf(stderr, "Duplicate options! Only one value.Usage: %s\n", usage);
                }
                out = 1;
                outputFile = optarg;
                break;
            default:
                printf("Default: \n%s", usage);
        }
    }
    
    inputFile = NULL;
    inputFile = argv[argc - 1];
    if(inputFile && (inputFile != outputFile)) //if inputFile provided
    {
        readFile(inputFile, &r, &c, &b); // reads input file
    }
    
    if(scale == 1)
    {
        if(scaleVal == 0)
        {
            fprintf(stderr, "Need to include a scale value: %s\n", usage);
            exit(-1);
        }
        if(scaleVal < 0)
        {
            fprintf(stderr, "Scale value needs to be a positive integer: %s\n", usage);
            exit(-1);
        }
        enlarge(b, r, c, scaleVal, &nb, &nr, &nc);
        b = nb;
        r = nr;
        c = nc;
        
    }
    
    
    if(rot == 1)
    {
        if(rotateVal % 90 != 0)
        {
            fprintf(stderr, "Rotate value must be multiple of 90: %s\n", usage);
            exit(-1);
        }
        rotate(b, r, c, rotateVal, &nb, &nr, &nc);
        b = nb;
        r = nr;
        c = nc;
       
    }
    
    if(verFlip == 1)
    {
        verticalflip(b, &nb, r, c);
    }
   
    if(horFlip == 1)
    {
        flip(b, &nb, r, c);
    }
    
    if(out)
    {
        writeFile(outputFile, r, c, nb);
        outputFile = NULL;
    }
    return 0;
}
