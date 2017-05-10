# Huffman Compressor
 
 Huffman tree coding is based on prefixes. So there cannot be two different leaves with exact same paths, even more, there must be a path, that ends up only in one leaf (no intersection leaves). Huffman tree coding, furthermore, is based on a frequency or probability of appearance of each one particular symbol. Given this idea, we work with a priority queue, that is initially filled with small trees, each with its symbol/char and probability of appearance/ref.count. We sort our queue/array descending, so most frequent symbols are topmost, while least common are the last. We then merge trees, starting from least one end, going topmost, while merging two trees into one new. After each merging, sorting is performed, so the queue/array is ordered descending again. We do merging, sorting until there is no more than 1 tree in the queue. Then we can finish with tree construction and go to compression, where each symbol is replaced by its path in bits from tree

### Using the Code
The code contain one **public class**  **_simple_Huffman_** in  **_simple_huffman.cpp_**.

##### Main Class

``` cpp
#include <memory.h>     // memset
#include <stdlib.h>     // qsort
typedef unsigned char BYTE;
class simple_Huffman{
private:
    class node{
    public:
        node *left, *right;
        int count;       // ref. count

        int code;        // code in bits

        int codelength;  // code length

        BYTE chr;        // ASCII char, serves in compression and then in writing of frequencies

        // constructor, memset is faster than individ.

        node(void)  {memset(this, 0, sizeof(node));}
        ~node() {
            if (left) {delete left; left = NULL;}
            if (right) {delete right; right = NULL;}
        }
    };
    // Array of trees, here will be only one tree at pos.

    // 0 when finished with merging 

    node *trees[256];
    node *leaves[256]; // array of leaves, each contains ASCII char,

         // ref. count, later code and codelength too. It is a reference to original trees

    node *trees_backup[256];
    // as the tree is constructed, this stores

    // exact steps (movings) of the last tree

    int STEPS[256];
    int stepscount;
    node nodes[256];

    void MakeTree();
    void MakeTreeD(); // maketree for decompression (no searching)

    int treescount;
    BYTE *allocatedoutput;
    // initialization of leaves with codes and code lengths

    void InitializeLeaves(node *, int, int);
    // sorting, we want to sort descending

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
    void Finalize(); // call after each stream compress/decompress to deallocate

    BYTE *getOutput(); // call after each stream compress/decompress to obtain output

    int getLastError(); // get code of last error (-1 input file failed

                        // to open, -2 output failed, -3 output allocation failed)

};
```

### Using the Code

```cpp
int Compress(BYTE *input, int inputlength);   // compress streams, return value is output size

int Decompress(BYTE *input, int inputlength); // decompress streams, return value is output size

int CompressFile(char *InFile, char *OutFile);  // compress file directly, return value is output size

int DecompressFile(char *InFile, char *OutFile); // decompress file directly return value is output size
```
