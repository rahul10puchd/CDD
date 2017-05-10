#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "huffman.h"

int main(int argc, char *argv[])
{
    if (argc < 4) { printf("syntax: program <INPUT> <OUTPUT> <c/d>\n"); system("PAUSE"); return 0; }
	
    FILE *finput, *foutput;
    BYTE *input, *output = NULL;
    BYTE op = (BYTE) argv[3][0];
	int delka;
    simple_Huffman *huf = new simple_Huffman();
    // open files
	
    finput = fopen(argv[1], "rb");
    if (!finput) { printf("error upon opening %s\n", argv[1]); system("PAUSE"); return 0; }
    foutput = fopen(argv[2], "wb");
    if (!foutput) { printf("error upon opening %s\n", argv[2]); system("PAUSE"); fclose(finput); return 0; }

    // get the length of input 
    fseek(finput, 0L, SEEK_END);
    delka = ftell(finput);
    fseek(finput, 0L, SEEK_SET);
    
    // read input 
    input = new BYTE[delka];
    fread(input, 1, delka, finput);    
    clock_t before,after;
	
	
    if (op == 'c')
    {
    before = clock();       
    int outsize = huf->Compress(input, delka);
	output = huf->getOutput();
    printf("time: %.2lfs\n", ((double)clock()-before) / 1000.0);
       fwrite(output, outsize, 1, foutput);
       huf->Finalize();
    }
    if (op == 'd')
    {      
    before = clock();
    int outsize = huf->Decompress(input, delka);
	output = huf->getOutput();
    printf("time: %.2lfs\n", ((double)clock()-before) / 1000.0);
       fwrite(output, outsize, 1, foutput);
       huf->Finalize();
    }
    fclose(finput);
    fclose(foutput);
    delete huf;
    delete input;
	system("PAUSE");
    return EXIT_SUCCESS;
}
