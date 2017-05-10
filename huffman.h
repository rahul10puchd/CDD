#include <memory.h>     
#include <stdlib.h>
typedef unsigned char BYTE;
class simple_Huffman{
private:
	class node{
	public:
		node *left, *right;
		int count;      
		int code;       
		int codelength; 
		BYTE chr;       
		node(void)  {memset(this, 0, sizeof(node));} 
		~node() {
			if (left) {delete left; left = NULL;}
			if (right) {delete right; right = NULL;}
		}
	};
	node *trees[256]; 
	node *leaves[256];
	node *trees_backup[256];
	int STEPS[256]; 
	int stepscount;
	node nodes[256];

	void MakeTree();
	void MakeTreeD();
	int treescount;
	BYTE *allocatedoutput;
	void InitializeLeaves(node *, int, int);
	static int comparefreq(const void *A, const void *B);
	void moveTreesRight(node **toTree);
	void moveTreesRight2(node **toTree);
	void tryToPop();
	void moveToTop();
	void performInternalCleanup();
	void initialize();
	int LastError;
public:
	simple_Huffman();
	int Compress(BYTE *input, int inputlength);
	int Decompress(BYTE *input, int inputlength);
	int CompressFile(char *InFile, char *OutFile);
	int DecompressFile(char *InFile, char *OutFile);
	void Finalize();
	BYTE *getOutput();
	int getLastError();
};
