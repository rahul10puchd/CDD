#include "huffman.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

void simple_Huffman::moveTreesRight2(node **toTree){
	node *a, *b;
	register node **ptr = trees+treescount-1;
	register node **_toTree = toTree;
	while (ptr > _toTree){                
		a = *(ptr-1);        
		b = *(ptr);
		*(ptr-1) = b;
		*(ptr) = a;
		ptr--;
	}  
}
void simple_Huffman::tryToPop(){
	register node **ptr = trees+treescount-2;
	register node **ourtree = trees+treescount-1;
	register node **begin = trees;
	while(ptr >= begin){
		if (ourtree != ptr)
			if (((*ptr)->count > (*ourtree)->count) || (ptr == begin)){
				moveTreesRight(ptr);
				return;
			}      
			ptr--;
	}
}
void simple_Huffman::MakeTree(){
	register node *n;
	register node **endtree = trees+treescount-1;
	memset(nodes, 0, sizeof(node)*256);
	node *nptr = nodes;
	node **backupptr = trees_backup+treescount-1;
	while (treescount > 1){ 
		n = nptr;
		nptr++;
		n->right = *(endtree-1);
		n->left = *endtree;
		n->count = n->right->count + n->left->count;      

		*endtree = *backupptr; 
		*(endtree-1) = n;                              
		treescount--;             
		endtree--;  
		backupptr--;
		*(STEPS+stepscount) = treescount;
		tryToPop();
		stepscount++;
	}  
}
void simple_Huffman::MakeTreeD(){  
	node *n;
	node *nptr2 = nodes;
	node **backupptr;
	node **tptr = trees;  
	stepscount = 0;
	while (treescount > 1)
	{
		n = nptr2;
		nptr2++;
		tptr = trees+treescount;
		backupptr = trees_backup+treescount;

		n->right = *(tptr-2);
		n->left = *(tptr-1);

		*(tptr-1) = *(backupptr-1);
		*(tptr-2) = n;

		treescount--; 

		moveTreesRight2(trees+(*(STEPS+stepscount))); 
		stepscount++;
	}
}
int simple_Huffman::Compress(BYTE *input, int inputlength){

	initialize();
	register BYTE *inptr = input;
	BYTE *stop = input+inputlength;
	allocatedoutput = new BYTE[5*inputlength];
	if (allocatedoutput == NULL) { LastError = 3; return 0; }
	allocatedoutput = allocatedoutput;
	int byteswritten = 0;
	register BYTE *outptrX = allocatedoutput; 
	while (inptr != stop){

		(*(trees+*inptr))->count++;
		(*(trees+*inptr))->chr = (*inptr);

		inptr++;
	}

	qsort(trees, 256, sizeof(node*),  comparefreq);

	for (register int i = 0; i < 256; i++)
		if ((*(trees+i))->count > 0)
			treescount++;      
		else break; 

		(*outptrX) = treescount-1;
		outptrX++;
		for (register int i = 0; i < treescount; i++){
			*outptrX = (*(trees+i))->chr;
			outptrX++;
		}
		MakeTree();
		*outptrX = stepscount;
		outptrX++;
		for (register int i = 0; i < stepscount; i++){
			*outptrX = *(STEPS+i);
			outptrX++;
		}

		*outptrX     =  inputlength >> 24; 
		*(outptrX+1) =  inputlength >> 16;
		*(outptrX+2) =  inputlength >> 8;
		*(outptrX+3) =  inputlength;
		outptrX+=4;
		InitializeLeaves(*trees, 0,0); 
		int bitpath, bitswritten, outbitswritten = 0;
		register int i, bitcount;

		inptr = input;  
		*outptrX = 0;
		while (inptr != stop){
			bitpath = (*(leaves+*inptr))->code;
			bitcount = (*(leaves+*inptr))->codelength;
			bitswritten =0;
			for (i = 0; i < bitcount; i++){
				(*outptrX) ^= ((bitpath >> bitswritten)&1) << outbitswritten;
				bitswritten++;
				(*(outptrX+1)) = '\0';
				outptrX += ((outbitswritten >> 2) & (outbitswritten >> 1) & outbitswritten) &1;
				outbitswritten++; 
				outbitswritten&=7;

			}
			inptr++;
		}

		byteswritten = outptrX - allocatedoutput;
		if (bitswritten > 0) 
			byteswritten++;
		performInternalCleanup();
		return byteswritten;   
}
int simple_Huffman::Decompress(BYTE *input, int inputlength)
{      

	initialize();
	BYTE *stop = input + inputlength;
	register BYTE *inptr = input;
	node *n;
	treescount = *inptr;  
	inptr++;
	treescount++; 
	node **tptr = trees;  

	for (register int i = 0; i < treescount; i++)
	{  
		(*tptr)->chr = *inptr;
		inptr++; 
		tptr++;         
	}

	stepscount = *inptr;
	inptr++;
	int *sptr = STEPS;
	for (register int i = 0; i < stepscount; i++){
		(*sptr) = *inptr;
		inptr++;    
		sptr++;
	}
	int outsize;
	outsize  = *inptr << 24; 
	outsize ^= *(inptr+1) << 16;
	outsize ^= *(inptr+2) << 8;
	outsize ^= *(inptr+3);
	inptr+=4;

	allocatedoutput = new BYTE[outsize+10];
	if (allocatedoutput == NULL) {performInternalCleanup(); LastError = 3; return 0;}

	MakeTreeD();

	InitializeLeaves(*trees, 0,0);

	register BYTE *outptr = allocatedoutput;
	int bit = 0;
	register node *nptr ;
	register int b;
	int outbitswritten = 0;
	while(inptr <= stop){  
		nptr = *trees; 
		while(nptr->codelength == 0){
			b = ((*inptr) >> bit) &1;
			nptr = (b > 0) ? nptr->right :  nptr->left;
			inptr+=( (bit >> 2) & (bit >> 1) & bit) & 1;
			bit++;        
			bit&=7;
		}  
		(*outptr) = nptr->chr;      
		outptr ++;
	}
	performInternalCleanup();
	return outsize;
}   
BYTE *simple_Huffman::getOutput(){
	if (allocatedoutput)
		return allocatedoutput;
	return NULL;
}
int simple_Huffman::CompressFile(char *input, char *output){
	FILE *in = fopen(input, "rb");
	if (!in) { LastError = 1; return 0; }
	FILE *out = fopen(output, "wb");
	if (!out) {fclose(in); LastError = 2; return 0;}
	char *inbuffer = new char[65536];
	char *outbuffer = new char[65536];
	setvbuf(in,inbuffer, _IOFBF, 65536);
	setvbuf(out,outbuffer, _IOFBF, 65536);

	initialize();

	fseek(in, 0L, SEEK_END);
	int insize = ftell(in);
	int c;
	int out_char;
	fseek(in, 0L, SEEK_SET);
	for (register int i = 0; i < insize; i++){
		c = fgetc(in);
		(*(trees+c))->count++;
		(*(trees+c))->chr = c;
	}
	fseek(in, 0L, SEEK_SET);

	qsort(trees, 256, sizeof(node*),  comparefreq);

	for (register int i = 0; i < 256; i++) 
		if ((*(trees+i))->count > 0)
			treescount++;      
		else break; 

		fputc(treescount-1, out);
		for (register int i = 0; i < treescount; i++){
			fputc((*(trees+i))->chr, out);
		}

		MakeTree();

		fputc(stepscount, out);
		for (register int i = 0; i < stepscount; i++){
			fputc(*(STEPS+i), out);
		}

		fputc(insize >> 24, out); 
		fputc(insize >> 16, out);
		fputc(insize >> 8, out);
		fputc(insize, out);
		InitializeLeaves(*trees, 0,0);
		int bitpath, bitswritten, outbitswritten = 0;
		int byteswritten = 0;
		register int i, bitcount;
		out_char = 0;
		for (int X = 0; X < insize; X++){
			c = fgetc(in);
			bitpath = (*(leaves+c))->code;
			bitcount = (*(leaves+c))->codelength;
			bitswritten =0;
			for (i = 0; i < bitcount; i++){
				out_char ^= ((bitpath >> bitswritten)&1) << outbitswritten;
				bitswritten++;
				if (outbitswritten == 7){
					fputc(out_char, out);
					out_char = 0;
				}
				outbitswritten++;
				outbitswritten&=7;

			}
		}

		byteswritten = ftell(out);
		if (bitswritten > 0){ 
			byteswritten++; 
			fputc(out_char, out);
			byteswritten++;
		}
		fclose(in);
		fclose(out);

		performInternalCleanup();
		delete inbuffer;
		delete outbuffer;
		return byteswritten;   

}
int simple_Huffman::DecompressFile(char *input, char *output){
	FILE *in = fopen(input, "rb");
	if (!in) { LastError = 1; return 0;}
	FILE *out = fopen(output, "wb");
	if (!out) {fclose(in); LastError = 2; return 0;}
	char *inbuffer = new char[65536];
	char *outbuffer = new char[65536];
	setvbuf(in,inbuffer, _IOFBF, 65536);
	setvbuf(out,outbuffer, _IOFBF, 65536);

	initialize();

	node *n;
	fseek(in, 0L, SEEK_END);
	int insize = ftell(in);
	fseek(in, 0L, SEEK_SET);
	treescount = fgetc(in);  
	treescount++;
	node **tptr = trees;  

	for (register int i = 0; i < treescount; i++)
	{  
		(*tptr)->chr = fgetc(in);
		tptr++;         
	}

	stepscount = fgetc(in);
	int *sptr = STEPS;
	for (register int i = 0; i < stepscount; i++){
		(*sptr) = fgetc(in);
		sptr++;
	}
	int outsize;
	outsize  = fgetc(in) << 24;
	outsize ^= fgetc(in) << 16;
	outsize ^= fgetc(in) << 8;
	outsize ^= fgetc(in);

	MakeTreeD();
	InitializeLeaves(*trees, 0,0);

	int bit = 0;
	register node *nptr ;
	register int b;
	int outbitswritten = 0;
	BYTE in_chr = fgetc(in);
	int position = 0;
	while (position < outsize){
		nptr = *trees; // root

		while(nptr->codelength == 0){
			b = ((in_chr) >> bit) &1;
			nptr = (b > 0) ? nptr->right :  nptr->left;
			if (bit == 7){
				in_chr = fgetc(in);
			}
			bit++;        
			bit&=7;
		}  
		fputc(nptr->chr, out);
		position++;
	}

	fclose(in);
	fclose(out);
	delete inbuffer;
	delete outbuffer;
	performInternalCleanup();
	return position;
}
void simple_Huffman::performInternalCleanup(){
	for (register int i = 0; i < 256; i++) {
		memset(*(trees_backup+i), 0, sizeof(node));
		*(trees+i) = *(trees_backup+i);
		*(leaves+i) = *(trees+i);

	}

	memset(nodes, 0, 256*sizeof(node));

	memset(STEPS, 0, 256*sizeof(int));
	stepscount = 0;
	treescount = 0;	

}
void simple_Huffman::Finalize()
{
	if (allocatedoutput)
		delete allocatedoutput;
	allocatedoutput = NULL; 
}

void simple_Huffman::initialize() {
	stepscount = 0;      
	treescount = 0;
	LastError = 0;
	for (register int i = 0; i < 256; i++) {
		*(trees_backup+i) = *(trees+i);
		*(leaves+i) = *(trees+i);
		*(STEPS+i)  = 0;
	}
	memset(nodes, 0, 256*sizeof(node));
	if (allocatedoutput) delete allocatedoutput;
	allocatedoutput = NULL;
}

simple_Huffman::simple_Huffman(){
	LastError = 0;
	for (register int i = 0; i < 256; i++) 
		*(trees+i)  = new node();
	allocatedoutput = NULL;
}
int simple_Huffman::getLastError(){
	return LastError;
}
