
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cachelab.h"

typedef struct Ways{
    long tag;
    unsigned long address;
    int cycle;


}Ways;
typedef struct Sets{
    Ways* way;
}Sets;

typedef struct Addresses{
    unsigned long tag;
    unsigned long setId;
    unsigned long offset;
    unsigned long address;
    char hitOrMiss;

}Addresses;

int powI(int base,int exponent){
    int b = base;
    int e = exponent;
    int result = 1;
    for(int i = 1; i <= e; i++){
        result = result * b;
    }
    return result;
}

int log_2(int num){
    double x = (double)num;
    double result = log10(x)/log10(2);
    return (int)result;
}
unsigned long tagMask(unsigned long address,int tag,unsigned long bits,unsigned long sBits){
    unsigned long a = address;
    unsigned long b = bits;
    unsigned long s = sBits;
    int t = tag;
    int m = 0;
    for( int i = 0; i<t;i++){
        m = m + powI(2,i);
    }
    int shift = (int)b - t;
    m = m << shift;
    unsigned long result = a & m;
    result = result >> shift;
    return result;
}
unsigned long setMask(unsigned long address,int tag,int set,unsigned long bits){
    unsigned long a = address;
    unsigned long b = bits;
    int t = tag;
    int s = set;
    int m = 0;
    for( int i = 0; i<s;i++){
        m = m + powI(2,i);
    }
    unsigned long shift = b;
    m = m << shift;
    unsigned long result = a & m;
    result = result >> shift;
    return result;

}
unsigned long offSetMask(unsigned long address,int offset){
    unsigned long a = address;
    int o = offset;
    int m = 0;
    for( int i = 0; i<o;i++){
        m = m + powI(2,i);
    }
    unsigned long result = a & m;

    return result;
}
typedef struct Frame{
    int flag;
    int way;
    int matchCount;
}Frame;


int replace_policy(char* policy, Sets* s ,int E, int id,Addresses* a,int aSequence,int size){
    Sets * set = malloc(E * sizeof (Sets));
    set = s;
    int result = 0;
    int max = 0;
    Frame frame[E];

    for (int i = 0; i <E ; ++i) {

        frame[i].flag = 0;
        frame[i].way = -1;
        frame[i].matchCount =0;
    }
    if(strcmp("lru",policy)==0){
        int min = size;
        for (int i = 0; i < E ; ++i) {
            if ( min > set[id].way[i].cycle ) {
                min = set[id].way[i].cycle;
                result = i;
            }
        }
        return result;
    }
    else if(strcmp("fifo",policy) == 0){

        for (int i = 0; i < E; ++i) {
            if(max < set[id].way[i].cycle){
                max = set[id].way[i].cycle;
                result = i;
            }
        }
        return result;
    }
    else if(strcmp("optimal",policy) == 0) {

        int count = aSequence;
        for (int i = aSequence; i < size; ++i) {
            for (int j = 0; j < E; ++j) {
                if(set[id].way[j].tag == a[i].tag) {
                    if (frame[j].flag != 1 ){
                        frame[j].flag = 1;
                        frame[j].way = i;
                    }
                }
                else{
                    if(frame[j].flag != 1) {
                        frame[j].way = (E + j) * (i + size);
                    }
                }
            }
        }
        count = 0;
        for (int i = 0; i < E; ++i) {
            if(frame[i].flag == 1)
                count++;
        }

        if(count == 2){

        }
        for (int i = 0; i < E; ++i) {
            if(max < frame[i].way && frame[i].flag != 1){
                    max = frame[i].way;
                    result =i;
            }
        }
        return result;
    }

}

void printResult(int hits, int misses, int missRate, int runTime) {
    printf("[result] hits: %d misses: %d miss rate: %d%% total running time: %d cycle\n",hits, misses, missRate, runTime);
}

int main(int argc,char* argv[]){


    char * fn1 = malloc(45 * sizeof (char));
    strcpy(fn1,"/home/jscollo/cachelab/");  //set path to address text file.
    char* f = malloc(15 * sizeof (char));
    char fileName[60];
    strcpy(f,argv[10]);
    strcpy(fileName,strcat(fn1,f));
    FILE *fp = fopen(fileName,"r");
    unsigned long m = atol(argv[2]);
    int S = powI(2,atoi(argv[4]));
    int E = powI(2,atoi(argv[6]));
    int B = powI(2,atoi(argv[8]));

    char* type = argv[12];
    printf("Replacement Policy: %s",type);
    int aSize = 0;
    const unsigned MAX_LENGTH = 256;
    char buffer[MAX_LENGTH];
    unsigned long holder;
    int hits = 0;
    int misses = 0;
    double missRate = 0;

    while(fgets(buffer,MAX_LENGTH,fp)){
        aSize++;
    }
    fclose(fp);
    Sets* set = malloc(S * sizeof(Sets));

    for (int i = 0; i < S ; ++i) {

        set[i].way = malloc(E * sizeof(Ways));

        for (int j = 0; j < E ; ++j) {
            set[i].way[j].cycle = -1;
            set[i].way[j].address = -1;
            set[i].way[j].tag = -1;
        }
    }
    Addresses* address= (Addresses*) malloc(aSize * sizeof(Addresses));
    int tag = (int)m - (log_2((int)S) + log_2((int)B));
    int offset = log_2((int)B);
    int setBits = log_2((int)S);
    FILE *fp2 = fopen(fileName,"r");
    char* temp;
    aSize = 0;
    char buffer2[MAX_LENGTH];
    while(fgets(buffer2,MAX_LENGTH,fp2)){
        temp = buffer2;
        holder = strtoul(temp,NULL,16);
        address[aSize].address = holder;
        address[aSize].tag = tagMask(address[aSize].address, tag, m, setBits);
        address[aSize].offset = offSetMask(address[aSize].address, offset);
        address[aSize].setId = setMask(address[aSize].address, tag, setBits, offset);
        aSize++;
    }
    fclose(fp2);
    int setID;
    int n;
    int line = 0;
    int breakFlag = 0;
    int pageFault = -1;

    for (int i = 0; i < aSize; ++i) {
        setID = address[i].setId;
        n = -1;
        for (int j = 0; j < E; ++j) {
            if (set[setID].way[j].tag == address[i].tag) {
                n = j;
                pageFault = 0;
            }
            if (n != -1)
                break;
            }
            if (pageFault != 0) {
                n = -1;
                for (int j = 0; j < E; ++j) {
                    if (set[setID].way[j].tag == -1) {
                        pageFault = 1;
                        n = j;

                    }
                    if (n != -1)
                        break;
                }
            }
            if (pageFault != 1 && pageFault != 0) {
                n = -1;
                for (int j = 0; j < E; ++j) {
                    if (set[setID].way[j].tag != address[i].tag) {
                        pageFault = 2;
                        n = j;
                    }
                    if (n != -1)
                        break;
                }
            }
            switch (pageFault) {
                case 0:
                    address[i].hitOrMiss = 'H';
                    hits++;
                    if(strcmp(type,"fifo")!=0)
                        set[setID].way[n].cycle = i;
                    pageFault = -1;
                    break;
                case 1:
                    address[i].hitOrMiss = 'M';
                    misses++;
                    set[setID].way[n].cycle = i;
                    set[setID].way[n].tag = address[i].tag;
                    pageFault = -1;
                    break;
                case 2:
                    address[i].hitOrMiss = 'M';
                    misses++;
                    int max = 0;
                    int w = -1;
                    w = replace_policy(type, set, E, setID, address, i, aSize);
                    set[setID].way[w].tag = address[i].tag;
                    set[setID].way[w].cycle = i;
                    pageFault = -1;
                    break;
            }
        }






    for (int i = 0; i < aSize ; ++i) {
        printf("\n%lx: %c ",address[i].address,address[i].hitOrMiss);
    }
    printf("\n");
    missRate = 100 - (double)hits / ((double)hits + (double)misses)* MISS_PENALTY;
    int mr = (int)missRate;
    int clockCycle = (misses * MISS_PENALTY)+ (hits + misses);
    int hitRate = 100 - mr;
    printf("\n");
    printResult(hits,misses,mr,clockCycle);
    return 0;
    free(f);
    free(set);
    free(fn1);
    free(address);
    
}





