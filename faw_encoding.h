
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int BITS[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648};

unsigned int encode(unsigned int* arr, unsigned int count){
    int fWordIndex = 0;
    int fWord = arr[0];
    char fWordType = 0;
    unsigned int tWord = 0;
    int dif;
    unsigned int i=1;
    while(i<count){
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
    return fWordIndex;
}
//
void show_items(unsigned int* encoded_items, unsigned int size){
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
    printf("; #items: %d\n", count);
}
//
unsigned int decode(unsigned int* encoded_items, unsigned int size){
    unsigned int* items = (unsigned int* )malloc(size*sizeof(unsigned int));
	unsigned int tmp, j, count=0;
	for(unsigned int si = 0; si < size; si++){
		if((encoded_items[si]&0xC0000000)==0xC0000000){
			// decode FR-word
            if(count+1 >= size){
			    items = (unsigned int* )realloc(items, (count+1)*sizeof(unsigned int));
            }
            items[count++] = encoded_items[si]&0x3fffffff;
			si++;
			j = 0;
			// decode R-word
            if(count+1 >= size){
			    items = (unsigned int* )realloc(items, (count+encoded_items[si])*sizeof(unsigned int));
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
                items = (unsigned int* )realloc(items, (count+1)*sizeof(unsigned int));
            }
            tmp = encoded_items[si]&0x3fffffff;
            items[count++] = tmp;
            tmp++;
			si++;
			// decode M-Word
			for(int j=0; j<32; j++){
				if(encoded_items[si]&BITS[j]){
                    if(count+1 >= size){
                        items = (unsigned int* )realloc(items, (count+1)*sizeof(unsigned int));
                    }
                    items[count++] = tmp+j;
				}
			}
		}
		else {
			// S-Word
            if(count+1 >= size){
			    items = (unsigned int* )realloc(items, (count+1)*sizeof(unsigned int));
            }
            items[count++] = encoded_items[si];
		}
	}
    //printf("; #items: %d\n", count);
    memcpy(&encoded_items[0], &items[0], count*sizeof(unsigned int));
    free(items);
    return count;
}
//
short exists(unsigned int* encoded_items, unsigned int size, unsigned int search_for){
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
unsigned int* intersect(unsigned int* list1, unsigned int list1_size, unsigned int* list2, unsigned int list2_size, unsigned int* length){
    
    list1_size = decode(list1, list1_size);
    list2_size = decode(list2, list2_size);
    //printf("\n#common-items: %u %u\n\n", list1_size, list2_size);
    unsigned int* results = (unsigned int* )malloc(sizeof(unsigned int));
    unsigned int i=0, j=0, result_size = 0;
    while(i<list1_size && j<list2_size){
        while(i<list1_size && list1[i] < list2[j]){
            i++;
        }
        while(j<list2_size && list2[j] < list1[i]){
            j++;
        }
        while(i<list1_size && j<list2_size && list1[i] == list2[j]){
            results = (unsigned int* )realloc(results, (result_size+1)*sizeof(unsigned int));
            results[result_size++] = list1[i];
            i++;
            j++;
        }
        if(list1[list1_size-1] < list2[j] || list2[list2_size-1] < list1[i]){
            break;
        }
    }
    // printf("\n#common-items: %u %u %u\n\n", list1_size, list2_size, result_size);
    *length = result_size;
    return results;
}
//
unsigned int* merge(unsigned int* list1, unsigned int list1_size, unsigned int* list2, unsigned int list2_size, unsigned int* length){
    
    list1_size = decode(list1, list1_size);
    list2_size = decode(list2, list2_size);
    //printf("\n#common-items: %u %u\n\n", list1_size, list2_size);
    unsigned int* results = (unsigned int* )malloc(sizeof(unsigned int));
    unsigned int i=0, j=0, result_size = 0;
    while(i<list1_size && j<list2_size){
        while(i<list1_size && list1[i] < list2[j]){
            results = (unsigned int* )realloc(results, (result_size+1)*sizeof(unsigned int));
            results[result_size++] = list1[i];
            i++;
        }
        while(j<list2_size && list2[j] < list1[i]){
            results = (unsigned int* )realloc(results, (result_size+1)*sizeof(unsigned int));
            results[result_size++] = list2[j];
            j++;
        }
        while(i<list1_size && j<list2_size && list1[i] == list2[j]){
            results = (unsigned int* )realloc(results, (result_size+1)*sizeof(unsigned int));
            results[result_size++] = list1[i];
            i++;
            j++;
        }
    }
    while(i<list1_size){
        results = (unsigned int* )realloc(results, (result_size+1)*sizeof(unsigned int));
        results[result_size++] = list1[i];
        i++;
    }
    while(j<list2_size){
        results = (unsigned int* )realloc(results, (result_size+1)*sizeof(unsigned int));
        results[result_size++] = list2[j];
        j++;
    }
    // printf("\n#common-items: %u %u %u\n\n", list1_size, list2_size, result_size);
    *length = result_size;
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
//
/*
void waf_compress(){

	int BITS[31];
    for(int i=1; i<31; i++){
    	BITS[i] = (1 << i);
    }
    BITS[0] = 1;
    unsigned int FIRST_BIT_HIGH = 0x80000000;

	int numItems = 11;
	int data[11] = {37, 45, 46, 65, 112, 117, 201, 207, 211, 225, 228};
	int flagIndex = 0;
	int mod = data[flagIndex]/31;
	int flagFrom = mod*31;
	int encodedWord = 0;

	for(int i=1; i<numItems; i++){
		if(data[i]/31 == mod){
			encodedWord |= BITS[data[i] - flagFrom];
		} else {
			if(encodedWord == 0){
				data[flagIndex] |= FIRST_BIT_HIGH;
			} else {
				data[++flagIndex] = encodedWord;
			}
			data[++flagIndex] = data[i];
			mod = data[flagIndex]/31;
			flagFrom = mod*31;
			encodedWord = 0;
		}
	}
	if(encodedWord == 0){
		data[flagIndex] |= FIRST_BIT_HIGH;
	} else {
		data[++flagIndex] = encodedWord;
	}

	for(int i=0; i<=flagIndex; i++){
		printf("%d [%X]\n", data[i], data[i]);
	}
}
*/