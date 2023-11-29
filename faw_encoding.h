
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int BITS[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648};

unsigned int* encode(unsigned int* arr, int* size){
    printf("Compressing by FAW-encoding....\n");
    int fWordIndex = 0;
    int fWord = arr[0];
    char fWordType = 0;
    unsigned int tWord = 0;
    int dif;
    int i=1;
    int count = *size;
    while(i<count){
        if(arr[i-1]>arr[i] || arr[i] < 0){
            printf("Invalid data. Dataset must contain sorted positive integers.\n");
            exit(0);
        }
        dif = arr[i]-fWord-1;
        if(dif>-1 && dif<32){               // within the range of 32
            tWord |= BITS[dif];
            if(tWord==0xFFFFFFFF){
                fWordType = 2;
                tWord = 32;
            }
        }
        else {
            if(tWord==0){                   // word @flag is a S-Word
                arr[fWordIndex++] = fWord;
                fWord = arr[i];
            }
            else if(fWordType == 2 && dif==1+tWord){// word @flag is a FR-Word
                tWord++;
            }
            else {
                if(fWordType == 2){
                    arr[fWordIndex++] = fWord|0xC0000000;
                }
                else {
                    arr[fWordIndex++] = fWord|0x80000000;
                }
                arr[fWordIndex++] = tWord;
                fWord = arr[i];
                tWord = 0;
            }
        }
        i++;
    }// while
    if(tWord==0){                           // word @flag is a S-Word
        arr[fWordIndex++] = fWord;
    }
    else {
        if(fWordType == 2){
            arr[fWordIndex++] = fWord|0xC0000000;
        }
        else{
            arr[fWordIndex++] = fWord|0x80000000;
        }
        arr[fWordIndex++] = tWord;
    }
    *size = fWordIndex;
    arr = (unsigned int*) realloc(arr, fWordIndex*4);
    return arr;
}
//
void show_items(unsigned int* encoded_items, int size){
    printf("Show all items in encoded dataset:\n");
	unsigned int tmp, j, count=0;
	for(unsigned int si = 0; si < size; si++){
		if((encoded_items[si]&0xC0000000)==0xC0000000){
			// decode FR-word
			tmp = encoded_items[si]&0x3fffffff;
			printf("%d, ", tmp++);  count++;
			si++;
			j = 0;
			// decode R-word
			while(j<encoded_items[si]){
                printf("%d, ", tmp++);  count++;
				j++;
			}
		}
		else if((encoded_items[si]&0xC0000000)==0x80000000){
			// decode FM-word
			tmp = encoded_items[si]&0x3fffffff;
            printf("%d, ", tmp++);  count++;
			si++;
			// decode M-Word
			for(int j=0; j<32; j++){
				if(encoded_items[si]&BITS[j]){
                    printf("%d, ", tmp+j);  count++;
				}
			}
		}
		else {
			// S-Word
            printf("%d, ", encoded_items[si]);  count++;
		}
	}
    printf("; #items: %d\n\n", count);
}
//
unsigned int* decode(unsigned int* encoded_items, int* list_size){
    printf("Decompressing FAW-encoded dataset....\n");
    int size = *list_size;
    unsigned int* items = (unsigned int* )malloc(size*4);
    int count=0;
	unsigned int tmp, j;
	for(int si = 0; si < size; si++){
		if((encoded_items[si]&0xC0000000)==0xC0000000){
			// decode FR-word
            if(count+1 >= size){
			    items = (unsigned int* )realloc(items, (count+1)*4);
            }
            items[count++] = encoded_items[si]&0x3fffffff;
			si++;
			j = 0;
			// decode R-word
            if(count+1 >= size){
			    items = (unsigned int* )realloc(items, (count+encoded_items[si])*4);
            }
			while(j<encoded_items[si]){
                items[count++] = tmp;
                tmp++;
				j++;
			}
		}
		else if((encoded_items[si]&0xC0000000)==0x80000000){
			// decode FM-word
            if(count+1 >= size){
                items = (unsigned int* )realloc(items, (count+1)*4);
            }
            tmp = encoded_items[si]&0x3fffffff;
            items[count++] = tmp;
            tmp++;
			si++;
			// decode M-Word
			for(int j=0; j<32; j++){
				if(encoded_items[si]&BITS[j]){
                    if(count+1 >= size){
                        items = (unsigned int* )realloc(items, (count+1)*4);
                    }
                    items[count++] = tmp+j;
				}
			}
		}
		else {
			// S-Word
            if(count+1 >= size){
			    items = (unsigned int* )realloc(items, (count+1)*4);
            }
            items[count++] = encoded_items[si];
		}
	}
    *list_size = count;
    //printf("; #items: %d\n", count);
    return items;
}
//
short exists(unsigned int* encoded_items, int size, unsigned int search_for){
    printf("Searching '%u' in the compressed dataset...\n", search_for);
	unsigned int tmp, j, count=0;
	for(unsigned int si = 0; si < size; si++){
		if((encoded_items[si]&0xC0000000)==0xC0000000){
			// decode FR-word
			tmp = encoded_items[si]&0x3fffffff;
            if(tmp > search_for) {
                return -1;
            }
            if(tmp == search_for){
                return 1;
            }
            tmp++;
			si++;
			// decode R-word
			if(tmp < search_for && search_for <= tmp+encoded_items[si]){
                return 1;
			}
		}
		else if((encoded_items[si]&0xC0000000)==0x80000000){
			// decode FM-word
			tmp = encoded_items[si]&0x3fffffff;
            if(tmp > search_for) {
                return -1;
            }
            if(tmp == search_for){
                return 1;
            }
            tmp++;
			si++;
			// decode M-Word
			for(int j=0; j<32; j++){
				if(encoded_items[si]&BITS[j]){
                    if(tmp+j > search_for) {
                        return -1;
                    }
                    if(tmp+j == search_for){
                        return 1;
                    }
				}
			}
		}
		else {
			// S-Word
            if(encoded_items[si] > search_for) {
                return -1;
            }
            if(encoded_items[si] == search_for){
                return 1;
            }
		}
	}
    return -1;
}
//
unsigned int* intersect(unsigned int* list1, int list1_size, unsigned int* list2, int list2_size, int* length){
    printf("Finding common values from two FAW-encoded datasets....\n");
    unsigned int* dlist1 = decode(list1, &list1_size);
    unsigned int* dlist2 = decode(list2, &list2_size);
    //printf("\n#common-items: %u %u\n\n", list1_size, list2_size);
    unsigned int* results = (unsigned int* )malloc(4);
    int i=0, j=0, result_size = 0;
    while(i<list1_size && j<list2_size){
        while(i<list1_size && dlist1[i] < dlist2[j]){
            i++;
        }
        while(j<list2_size && dlist2[j] < dlist1[i]){
            j++;
        }
        while(i<list1_size && j<list2_size && dlist1[i] == dlist2[j]){
            results = (unsigned int* )realloc(results, (result_size+1)*4);
            results[result_size++] = dlist1[i];
            i++;
            j++;
        }
        if(dlist1[list1_size-1] < dlist2[j] || dlist2[list2_size-1] < dlist1[i]){
            break;
        }
    }
    // printf("\n#common-items: %u %u %u\n\n", list1_size, list2_size, result_size);
    *length = result_size;
    free(dlist1);
    free(dlist2);
    return results;
}
//
unsigned int* merge(unsigned int* list1, int list1_size, unsigned int* list2, int list2_size, int* length){
    printf("Finding merged values from two FAW-encoded datasets....\n");
    unsigned int* dlist1 = decode(list1, &list1_size);
    unsigned int* dlist2 = decode(list2, &list2_size);
    //printf("\n#common-items: %u %u\n\n", list1_size, list2_size);
    unsigned int* results = (unsigned int* )malloc(4);
    unsigned int i=0, j=0, result_size = 0;
    while(i<list1_size && j<list2_size){
        while(i<list1_size && dlist1[i] < dlist2[j]){
            results = (unsigned int* )realloc(results, (result_size+1)*4);
            results[result_size++] = dlist1[i];
            i++;
        }
        while(j<list2_size && dlist2[j] < dlist1[i]){
            results = (unsigned int* )realloc(results, (result_size+1)*4);
            results[result_size++] = dlist2[j];
            j++;
        }
        while(i<list1_size && j<list2_size && dlist1[i] == dlist2[j]){
            results = (unsigned int* )realloc(results, (result_size+1)*4);
            results[result_size++] = dlist1[i];
            i++;
            j++;
        }
    }
    while(i<list1_size){
        results = (unsigned int* )realloc(results, (result_size+1)*4);
        results[result_size++] = dlist1[i];
        i++;
    }
    while(j<list2_size){
        results = (unsigned int* )realloc(results, (result_size+1)*4);
        results[result_size++] = dlist2[j];
        j++;
    }
    // printf("\n#common-items: %u %u %u\n\n", list1_size, list2_size, result_size);
    *length = result_size;
    free(dlist1);
    free(dlist2);
    return results;
}
//
int wah_encode(unsigned int* arr, int itemCount){
    
    int count = itemCount;
    int j, updatedIndex;
	int* comData = (int*)malloc(4);
    comData[0] = 0;
    updatedIndex = 0;
    int leadingZero, currentWordId = 0;

    for(int j=0; j < count; j++){
        leadingZero = arr[j]/31;
        if(leadingZero==currentWordId){ // merge with previous literal word
            comData[updatedIndex] |= BITS[arr[j]%31];
            if(comData[updatedIndex]==0x7FFFFFFF){ // if all 31-bits are high
                comData[updatedIndex] = (1|0xC0000000);
            }
        }else{
            if(currentWordId==0){ // start from empty
                comData[updatedIndex] = (leadingZero-currentWordId) | 0x80000000; 	// leading 0-FILL
                updatedIndex++;
                comData = (int*) realloc(comData, (1+updatedIndex)*4);				// a new word for a new literal
                comData[updatedIndex] = BITS[arr[j]%31];
            }else if(leadingZero-currentWordId == 1){ 								// already have some data, add a new literal
                updatedIndex++;
                comData = (int*) realloc(comData, (1+updatedIndex)*4);
                comData[updatedIndex] = BITS[arr[j]%31];
            }else{																	// already have some data, 0-FILL exists between words
                updatedIndex += 2;													// add two new words
                comData = (int*) realloc(comData, (1+updatedIndex)*4);
                comData[updatedIndex-1] = (leadingZero-currentWordId-1) | 0x80000000;	// WORD1: 0-FILL word
                comData[updatedIndex] = BITS[arr[j]%31];				// WORD2: 1-FILL word
            }
            currentWordId = leadingZero;
        }
    }
    //printf("\n");
    updatedIndex++;
    //arr = (int*) realloc(arr, updatedIndex*4);
    memcpy(&arr[0], &comData[0], updatedIndex*4);
    free(comData);
    return updatedIndex;
}
