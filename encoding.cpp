#include "encoding.h"
#include "stdio.h"
#include "stdlib.h"
#include <assert.h>
#include <iostream>
#include <QString>
#include <QTextStream>

QTextStream out(stdout);

encoding::encoding()
{

}

unsigned int encoding::getFileSize(FILE *src)
{
    unsigned int fileSize = 0;
    unsigned int c;

    while((c = fgetc(src)) != EOF)
    {
        fileSize++;
    }
    rewind(src);

    return fileSize;

}

unsigned int encoding::calcNumOfFreq(unsigned int *freqList)
{
    unsigned int i;
    unsigned int numOfFreq = 0;

    for(i=0; i<CHARS_LIMIT; i++)
    {
        if(freqList[i] > 0)
        {
            numOfFreq++;
        }
    }

    return numOfFreq;
}

void encoding::outputFilePath(const char *path, char *outputPath, const char *fileExtension)
{
    int i;
    const int pathLength = strlen(path);

    for(i=0; i<pathLength-4; i++)
    {
        outputPath[i] = path[i];
    }
    outputPath[i] = 0;

    strcat(outputPath, fileExtension);
}

void encoding::calcCharFreq(FILE *src, unsigned int *freqList)
{
    unsigned int c;

    while((c = fgetc(src)) != EOF)
    {
        freqList[c]++;
    }
    rewind(src);
}

void encoding::buildNodeList(encoding::HuffNode **nodeList, unsigned int *freqList)
{
    unsigned int i;
    HuffNode * newNode;

    for(i=0; i<CHARS_LIMIT; i++)
    {
        if(freqList[i] > 0)
        {
            newNode = (HuffNode *)calloc(1, sizeof(HuffNode));
            newNode->charCode = i;
            newNode->freq = freqList[i];
            newNode->next = NULL;
            newNode->left = NULL;
            newNode->right = NULL;
            newNode->leaf = true;

            addToNodeList(nodeList, newNode);
        }
    }
}

void encoding::addToNodeList(encoding::HuffNode **nodeList, encoding::HuffNode *newNode)
{
    HuffNode * prevNode = NULL;
    HuffNode * currNode = *nodeList;

    while(currNode != NULL && currNode->freq < newNode->freq)
    {
        prevNode = currNode;
        currNode = prevNode->next;
    }

    newNode->next = currNode;

    if(prevNode == NULL)
    {
        *nodeList = newNode;
    }
    else
    {
        prevNode->next = newNode;
    }
}

void encoding::buildHuffTree(encoding::HuffNode **nodeList)
{
    HuffNode * leftNode, * rightNode, * currNode;
    HuffNode * newNode;

    while((*nodeList)->next)
    {
        leftNode = *nodeList;
        *nodeList = leftNode->next;

        rightNode = *nodeList;
        *nodeList = rightNode->next;

        newNode = (HuffNode *)calloc(1, sizeof(HuffNode));
        newNode->charCode = 0;
        newNode->freq = leftNode->freq + rightNode->freq;
        newNode->next = NULL;
        newNode->left = leftNode;
        newNode->right = rightNode;
        newNode->leaf = false;

        addToNodeList(nodeList, newNode);
    }
}

bool encoding::buildHuffCode(encoding::HuffNode *treeRoot, encoding::HuffCode *hCode, unsigned char goalChar)
{
    if(treeRoot->charCode == goalChar && treeRoot->leaf)
    {
        return true;
    }

    if(treeRoot->left)
    {
        hCode->code[hCode->length] = '0';
        hCode->length++;

        if(hCode->length == MAX_CODE_SIZE)
        {
            printf("Code size exceeds limit!");
            return false;
        }

        if(buildHuffCode(treeRoot->left, hCode, goalChar))
        {
            hCode->code[hCode->length] = 0;
            return true;
        }
        else
        {
            hCode->length--;
            hCode->code[hCode->length] = 0;
        }
    }

    if(treeRoot->right)
    {
        hCode->code[hCode->length] = '1';
        hCode->length++;

        if(buildHuffCode(treeRoot->right, hCode, goalChar))
        {
            return true;
        }
        else
        {
            hCode->length--;
            hCode->code[hCode->length] = 0;
        }
    }

    return false;
}

void encoding::writeHeader(FILE *dest, encoding::HuffHeader hHeader, unsigned int numOfFreq, unsigned int fileSize)
{
    char * ptr=(char *)&hHeader;

    hHeader.numOfFreq = numOfFreq;
    hHeader.fileSize = fileSize;

    fwrite(&hHeader, sizeof(hHeader), 1, dest);
    for(int i =0;i<sizeof(hHeader);i++,ptr++)
      {
        putchar(*ptr);
      }
}

void encoding::writeFreq(FILE *dest, unsigned int *freqList, encoding::HuffFreq hFreq)
{
    unsigned int i;
    char * ptr=(char *)&hFreq;

    for(i=0; i<CHARS_LIMIT; i++)
    {
        if(freqList[i] > 0)
        {
            hFreq.charCode = i;
            hFreq.freq = freqList[i];
            ptr=(char *)&hFreq;
            fwrite(&hFreq, sizeof(HuffFreq), 1, dest);
            for(int j=0;j<sizeof(HuffFreq);j++)
            {
               putchar(*ptr);
               ptr++;
            }
        }
    }
}

void encoding::writeEncodedData(FILE *src, FILE *dest, encoding::HuffCode *huffCodeTable, unsigned int fileSize)
{
    unsigned int i, c;
    unsigned int bits = 0;
    char currChar = 0;
    HuffCode currCode;
    unsigned int bytes = 0;

    while((c = fgetc(src)) != EOF)
    {
        bytes++;
        currCode = huffCodeTable[c];

        for(i=0; i<currCode.length; i++)
        {
            currChar = (currChar << 1) + (currCode.code[i] == '1' ? 1 : 0);
            bits++;

            if(bits == 8)
            {
                fputc(currChar, dest);
                putchar(currChar);
                currChar = 0;
                bits = 0;
            }
        }
    }

    if(bits > 0)
    {
        currChar = currChar << (8 - bits);
        fputc(currChar, dest);
        putchar(currChar);
    }
}

void encoding::freeHuffTree(encoding::HuffNode *treeRoot)
{
    if(treeRoot)
    {
        freeHuffTree(treeRoot->left);
        freeHuffTree(treeRoot->right);

        free(treeRoot);
    }
}

void encoding::huffmanEncode(const char *inputFile)
{
    /** Read source file **/
    FILE * src = fopen(inputFile, "rb");

    /** Open destination file **/
    char outputPath[1000];
    const char * fileExtension = ".zip";
    outputFilePath(inputFile, outputPath, fileExtension);
    FILE * dest = fopen(outputPath, "wb");

    /** Check if source file exists **/
    if (src == NULL || dest == NULL)
    {
        printf("Could not find the input file!");
        exit(EXIT_FAILURE);
    }

    /** Get size of source file **/
    unsigned int fileSize;
    fileSize = getFileSize(src);

    /** Produce chars freqency list **/
    unsigned int * freqList;
    freqList = (unsigned int *)calloc(CHARS_LIMIT, sizeof(unsigned int));
    calcCharFreq(src, freqList);

    /** Calculate number of frequencies **/
    unsigned int numOfFreq;
    numOfFreq = calcNumOfFreq(freqList);

    /** Produce Huffman node list (frequencies sorted in ascending order)**/
    HuffNode * nodeList = NULL;
    buildNodeList(&nodeList, freqList);

    /** Build Huffman tree **/
    buildHuffTree(&nodeList);
    HuffNode * treeRoot = nodeList;

    /** Build Huffman code lookup table **/
    unsigned int i;
    HuffCode newCode;
    HuffCode * huffCodeTable;
    huffCodeTable = (HuffCode *)calloc(CHARS_LIMIT, sizeof(HuffCode));
    for(i=0; i<CHARS_LIMIT; i++)
    {
        if(freqList[i] > 0)
        {
            newCode.length = 0;
            buildHuffCode(treeRoot, &newCode, i);
            huffCodeTable[i] = newCode;
        }
    }

    /** Write Hufffman header to destination file **/
    HuffHeader hHeader;
    writeHeader(dest, hHeader, numOfFreq, fileSize);

    /** Write Huffman freq to destination file **/
    HuffFreq hFreq;
    writeFreq(dest, freqList, hFreq);

    /** Write encoded data to destination file **/
    writeEncodedData(src, dest, huffCodeTable, fileSize);

    /** Free up memory **/
    freeHuffTree(treeRoot);
    treeRoot = NULL;
    free(huffCodeTable);
    free(freqList);

    /** Close files **/
    fclose(src);
    fclose(dest);

}
