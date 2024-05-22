#ifndef ENCODING_H
#define ENCODING_H

#include <QString>
#include <stdio.h>

/* Constants */
#define MAX_CODE_SIZE 32
#define CHARS_LIMIT 256

class encoding
{
public:
    encoding();

    /** Input file name getter **/
    QString getInputFileName();

    /** Huffman encoding **/
    void huffmanEncode(const char* inputFile);

    /** Get size of file (in bytes) **/
    unsigned int getFileSize(FILE * src);

    /** Calculate number of frequencies **/
    unsigned int calcNumOfFreq(unsigned int * freqList);
private:

    /** Input file name string **/
    QString inputFileName;

    /** Output file path **/
    void outputFilePath(const char * path, char * outputPath, const char * fileExtension);

    /** Huffman node **/
    struct HuffNode
    {
        unsigned int freq;
        unsigned char charCode;
        bool leaf;
        HuffNode * next;
        HuffNode * left;
        HuffNode * right;
    };

    /** Hufman code **/
    struct HuffCode
    {
        unsigned char code[MAX_CODE_SIZE];
        unsigned int length;
    };

    /** Huffman header node **/
    struct HuffHeader
    {
        unsigned int numOfFreq;
        unsigned int fileSize;
    };

    /** Huffman freq node **/
    struct HuffFreq
    {
        unsigned int freq;
        unsigned char charCode;
    };

    /** Calculate & store freq of each char in list **/
    void calcCharFreq(FILE * src, unsigned int * freqList);

    /** Build Huffman node list (frequencies sorted in ascending order) **/
    void buildNodeList(HuffNode ** nodeList, unsigned int * freqList);

    /** Add new node to list (in ascending order) **/
    void addToNodeList(HuffNode ** nodeList, HuffNode * newNode);

    /** Build Huffman tree from nodeList **/
    void buildHuffTree(HuffNode ** nodeList);

    /** Build Huffman code lookup table **/
    bool buildHuffCode(HuffNode * treeRoot, HuffCode * hCode, unsigned char goalChar);

    /** Write Huffman header to output file **/
    void writeHeader(FILE * dest, HuffHeader hHeader, unsigned int numOfFreq, unsigned int fileSize);

    /** Write char & freq data to output file **/
    void writeFreq(FILE * dest, unsigned int * freqList, HuffFreq hFreq);

    /** Write Huffman encoded data to output file **/
    void writeEncodedData(FILE * src, FILE * dest, HuffCode * huffCodeTable, unsigned int fileSize);

    /** Free Huffman tree memory **/
    void freeHuffTree(HuffNode * treeRoot);

//public slots:

    /** Browse input file **/
//    void browseInputFile();

    /** Compress file **/
 //   void compressFile();
};

#endif // ENCODING_H
