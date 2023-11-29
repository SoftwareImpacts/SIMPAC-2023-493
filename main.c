#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "faw_encoding.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Following is the python code to generate random numbers: https://www.programiz.com/python-programming/online-compiler/
    // import random
    // n = 1000
    // numLow = 1
    // numHigh = 100000
    // setOfNumbers = set()
    // while len(setOfNumbers) < n:
    //     setOfNumbers.add(random.randint(numLow, numHigh))
    // print(sorted(setOfNumbers))
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int* get_dataset(char* dataset_path, int* size){
    int count = 0;
	FILE* fp = fopen(dataset_path, "r");
	if (!fp){
		printf("Can't open file: %s\n", dataset_path);
    }
	else {
        printf("Loading data from \"%s\" ...\n", dataset_path);
        unsigned int* dataset = (unsigned int*) malloc(4);
        char seq[12];
        char *token;
        int chr;
        token = seq;
        while (1) {
            chr = fgetc(fp);
            if (chr == EOF){
                break;
            }
            switch (chr) {
                case ',':
                    *token = 0;
                    if (token > seq) {
                        count++;
                        dataset = (unsigned int*) realloc(dataset, count*4);
                        dataset[count-1] = atoi(seq);
                        //printf("%s %u\n", seq, dataset[count-1]);
                    }
                    seq[0] = '\0';
                    token = seq;
                    break;
                case ' ':
                    break;
                default:
                    *token++ = chr;
                    break;
            }
        }
        *token = 0;
        if (token > seq) {
            count++;
            dataset = (unsigned int*) realloc(dataset, count*4);
            dataset[count-1] = atoi(seq);
            //printf("%s %u\n", seq, dataset[count-1]);
        }
        fclose(fp);
        *size = count;
        return dataset;
	}
    return NULL;
}


int main(){

    /* Load dataset-1 from the disk */
    int list1_size = 0;
    unsigned int* list1 = get_dataset("dataset1_sorted_positive_integers.dat", &list1_size);
    printf("Dataset-1 is loaded. #integers (size): %d\n\n\n", list1_size);
    /* Load dataset-2 from the disk */
    int list2_size = 0;
    unsigned int* list2 = get_dataset("dataset2_sorted_positive_integers.dat", &list2_size);
    printf("Dataset-2 is loaded. #integers (size): %d\n\n\n", list2_size);


    /* Compress dataset1 by FAW-encoding */
    list1 = encode(list1, &list1_size);
    printf("Dataset-1 is compressed. New size: %d\n\n\n", list1_size);
    /* Compress dataset2 by FAW-encoding */ 
    list2 = encode(list2, &list2_size);
    printf("Dataset-2 is compressed. New size: %d\n\n\n", list2_size);

    /* Show all items in FAW-encoded Dataset-1 */
    // show_items(list1, list1_size);    
    /* Show all items in FAW-encoded Dataset-2 */
    // show_items(list2, list2_size);

     /* Search an item in FAW-encoded Dataset-1 */
    unsigned int needle = 7392;
    short result = exists(list1, list1_size, needle);
    if(result < 0){
        printf("'%d' was not found in the encoded dataset\n\n", needle);
    } else {
        printf("'%d' was found in the encoded dataset\n\n", needle);
    }


    /* Get common values from FAW-encoded Dataset-1 & Dataset-2 */
    int i=0, result_size = 0;
    unsigned int* common_values = intersect(list1, list1_size, list2, list2_size, &result_size);
    printf("Common values are: ");
    while(i < result_size){
        printf("%u, ", common_values[i++]);
    }
    printf(" #Common-values: %d\n\n\n", i);


    /* Get merged values from FAW-encoded Dataset-1 & Dataset-2 */
    result_size = 0;
    unsigned int* merged_values = merge(list1, list1_size, list2, list2_size, &result_size);
    i = 0;
    printf("Merged values are: ");
    while(i < result_size){
        printf("%u, ", merged_values[i++]);
    }
    printf(" #Merged-values: %d\n\n\n", i);



    /* Decompress FAW-encoded Dataset-1 */
    unsigned int* decoded_list1 = decode(list1, &list1_size);
    i = 0;
    printf("Items are: ");
    while(i < list1_size){
        printf("%u, ", decoded_list1[i++]);
    }
    printf("; #items: %d\n\n", i);

    /* Decompress FAW-encoded Dataset-2 */
    unsigned int* decoded_list2 = decode(list2, &list2_size);
    i = 0;
    printf("Items are: ");
    while(i < list2_size){
        printf("%u, ", decoded_list2[i++]);
    }
    printf("; #items: %d\n", i);
    
    return 0;
}
