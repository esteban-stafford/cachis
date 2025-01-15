#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "datamanipulation.h"

#define N_TRUES 3
char* str_true[]= {"1", "yes", "true"};
#define N_FALSES 3
char* str_false[]= {"0","no","false"};

#define N_REPLACEMENT 4
char* str_replacementPolicy[]= {"lru", "lfu", "rand", "fifo"};
char* replacementPolicyStr(enum replacement replacement) { return str_replacementPolicy[replacement]; }

#define N_WRITE 2
char* str_writePolicy[]= {"wt", "wb"};
char* writePolicyStr(enum write_policy write_policy) { return str_writePolicy[write_policy]; }

// Used to keep the same random value on the same cycle
int cycle_rand_value;
int cycle_rand_updated = -1;

/*
 * convert string into long. It can have a multiplier G for 10^9, M for 10^6 o K for 10^3. Any other char will result in error.
 * @param  String to be converted into long
 * @return long with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
long parseLongK1000(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }
    // Obtain the multiplier k, M, or G. Else error
    long len=strlen(cadena);
    long multiplicador=1;
    if(cadena[len-1]=='K'||cadena[len-1]=='k') {
        multiplicador=1000;
    } else if(cadena[len-1]=='M'||cadena[len-1]=='m') {
        multiplicador=1000000;
    } else if(cadena[len-1]=='G'||cadena[len-1]=='g') {
        multiplicador=1000000000;
    } else if(cadena[len-1]>'9'||cadena[len-1]<'0') {
        return -1;

    }

    // if something not numeric or multiplier. error return -1
    for(long i=0; i<len-1; i++) {

        if(cadena[i]>'9'||cadena[i]<'0') {
            return -1;
        }
    }

    if(len==1){
         return multiplicador;

    }

    return atoi(cadena)*multiplicador;
}

/*
 * Convert string into long. It can have a multiplier G for 2^30, M for 2^20 o K for 2^10. Any othe char will result in error.
 * @param  String to be converted into long
 * @return long with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
long parseLongK1024(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }

    // Obtain the multiplier k, M, or G. Else error
    long len=strlen(cadena);
    long multiplicador=1;
    if(cadena[len-1]=='K'||cadena[len-1]=='k') {
        multiplicador=1024;
    } else if(cadena[len-1]=='M'||cadena[len-1]=='m') {
        multiplicador=1048576;
    } else if(cadena[len-1]=='G'||cadena[len-1]=='g') {
        multiplicador=1073741824;
    } else if(cadena[len-1]>'9'||cadena[len-1]<'0') {
        return -1;

    }

    // if something not numeric or multiplier. error return -1
    for(long i=0; i<len-1; i++) {

        if(cadena[i]>'9'||cadena[i]<'0') {
            return -1;
        }
    }

    if(len==1){
         return multiplicador;

    }

    return atoi(cadena)*multiplicador;
}

/*
 * convert string into int.
 * @param  String to be converted into int
 * @return int with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
int parseInt(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }
    // Obtain the multiplier k, M, or G. Else error.
    int len=strlen(cadena);

    // if something not numeric or multiplier. error return -1
    for(long i=0; i<len; i++) {

        if(cadena[i]>'9'||cadena[i]<'0') {
            return -1;
        }
    }

    return atoi(cadena);
}

/*
 * Convert string into boolean.
 * @param  String to be converted into boolean. Possible strings: yes, no, true, false, 0, 1
 * @return int with converted value. 1 for true, 0 for false, -1 for wrong value error. -2 for null pointer error.
 */
int parseBoolean(const char * cadena) {

    // if null return error -2
    if(cadena==NULL) {
        return -2;
    }

    // turn into lower case
    char cadenaMin[10];
    int i;
    for( i= 0; cadena[i]; i++) {
        cadenaMin[i] = tolower(cadena[i]);
    }
    cadenaMin[i]='\0';



    // check string content and retun equivalent boolean value.
    for(int i=0; i<N_TRUES; i++) {
        if(strcmp(str_true[i], cadenaMin)==0) {
            return 1;
        }

    }

    for(int i=0; i<N_FALSES; i++) {
        if(strcmp(str_false[i], cadenaMin)==0) {
            return 0;
        }

    }

    return -1;


}

/*
 * Convert string into enum which represent replacement policy.
 * @param  String to be converted into enum. Possible strings defined in str_replacementPolicy
 * @return enum  value or error. -2 for null pointer. -1 for wrong value error
 */
int parseReplacementPolicy(const char * cadena) {
    if(cadena==NULL) {
        return -2;
    }
    for(int i=0; i<N_REPLACEMENT; i++) {
        if(strcmp(cadena, str_replacementPolicy[i])==0) {
            return i;
        }
    }
    return -1;
}

/*
 * Convert string into enum which represent write policy.
 * @param  String to be converted into enum. Possible strings defined in str_replacementPolicy
 * @return enum  value or error. -2 for null pointer error. -1 for wrong value error
 */
int parseWritePolicy(const char * cadena) {

    // if null pointer retun error -2
    if(cadena==NULL) {
        return -2;
    }

    for(int i=0; i<N_WRITE; i++) {
        if(strcmp(cadena, str_writePolicy[i])==0) {
            return i;
        }
    }

    return -1;

}

/*
 * Check if a number is power of 2
 * @param number to check
 * @return boolean
 */
int isPowerOf2(long number) {

    return number && !(number & (number - 1));

}

/*
 * Check if a number is a multiple of 8
 * @param number to check
 * @return boolean
 */
int isAMultipleOf8(long number) {
    return !number%8;
}

/*
 * Checks if a string has binary value inside.
 * @param  cadena String to be checked
 * @return 1 if it is binary. -1 if it is not binary. -2 if NULL char* param
 */
int isCorrectBinary(const char * cadena) {

    // if null return error -2
    if(cadena==NULL) {
        return -2;
    }
    
    for(int i= 0; cadena[i]; i++) {
        if(cadena[i]!='0'&&cadena[i]!='1'){
        	return -1;
	}
    }
    return 1;
}

/*
 * convert string into double. It can have a multiplier p for 1e-12, n for 1e-9, u for 1e-6, m for 1e-3. Other char will result in error.
 * @param  String to be converted into double
 * @return long with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
double parseDouble(const char * cadena) {

    if(cadena==NULL) {
        return -2;
    }
    // Obtain the multiplier p, n, u, or m. Else error
    long len=strlen(cadena);
    double multiplicador=1;
    if(cadena[len-1]=='m') {
        multiplicador=1.0/1000.0;
    } else if(cadena[len-1]=='u') {
        multiplicador=1.0/1000000.0;
    } else if(cadena[len-1]=='n') {
        multiplicador=1.0/1000000000.0;
    } else if(cadena[len-1]=='p') {
        multiplicador=1.0/1000000000000.0;
    } else if(cadena[len-1]>'9'||cadena[len-1]<'0') {
        return -1;

    }

    // if something not numeric or multiplier. error return -1
    for(long i=0; i<len-1; i++) {

        if(cadena[i]>'9'||cadena[i]<'0') {
            return -1;
        }
    }

    return atoi(cadena)*multiplicador;
}

/*
 * convert string hex address into long. It can have a the format 0x...... 
 * Other char will result in error.
 * @param  page_base_address to be converted into long
 * @return long with converted value or error. -1 for wrong value error. -2 for null pointer error.
 */
long parseAddress(const char* page_base_address){

        if(page_base_address==NULL){
		return -2;
	}

        long toReturn= strtol(page_base_address, NULL, 16);
	return toReturn;
        // provisional TODO
	//faltan comprobaciones sobre tamano y rango de la direccion
}

/**
 * function for checking if hex format is correct
 * @param number hex number with string format to be checked
 */
int isCorrectHexadecimal(char * number){
   if(strlen(number)<2){
      return 0;
   }
   if(number[0]!='0'){
      return 0;
   }
   if(number[1]!='x'&&number[1]!='X'){
      return 0;
   }

   for(int i=2; i<strlen(number); i++){
      if(number[i]<'0'||(number[i]>'9'&&number[i]<'A')||(number[i]>'F'&&number[i]<'a')||number[i]>'f'){
         return 0;
      }
   }
   return 1;
}

/*
 * function for checking if dec format is correct
 * @param number dec number with string format to be checked
 */
int isCorrectDecimal(char * number){
   for(int i=0; number[i]!='\0'; i++){
      if(number[i]<'0'||number[i]>'9'){
         return 0;
      }
   }
   return 1;
}


/*
 * Convert an array of integers to a string
 */
void contentArrayToString(unsigned* array, char* content, int count, int width){
   char num[50];
   content[0]='\0';

   for(int i=0; i<count; i++) {
      sprintf(num, "%s%0*x", i>0 ? " " : "", width, array[i]);
      strcat(content, num);
   }
}

/*
 * Convert a space separated string of hex numbers into an array of longs
 */
void contentStringToArray(unsigned* array, char* content, int count){
	if (content == NULL) {
		return;
	}

   // A copy of the original array gets created so that strtok does not modify the original array
   char* contentCopy = malloc(sizeof(char) * 9 * count);
   strcpy(contentCopy, content);

    char *pch = strtok(contentCopy, " ");
    for (int i = 0; pch != NULL && i < count; i++, pch = strtok(NULL, " ")) {
        array[i] = strtol(pch, NULL, 16);
    }

    free(contentCopy);
}


/**
 * @brief Opens the specified DRAMSys file and returns the last memory access stored in the file
 * @param filename The name or path to the file.
 * @param f A pointer to the file that will get opened.
 * @return The number of the last memory access stored in the open file.
 */
int open_dramsys_file(const char *filename, FILE **f){
	// The file gets opened
	*f = fopen(filename, "r");

	// If the file could not be opened, return -1
    if (*f == NULL) {
        perror("Error opening file\n");
        return -1;
    }

    int number;
    char text[256];

    // While there is data on the file, iterate until the end is reached.
    while (fscanf(*f, "%d:%255[^\n]", &number, text) == 2) {}

    // If the file is empty, the timestamp should begin in 0 or 1, so -1 gets returned (It will get incremented afterwards)
    if (ftell(*f) == 0) {
		number = -1;
	}

    // The file is closed and reopened in append mode
    fclose(*f);
	*f = fopen(filename, "a");

	return number;
}


/**
 * @brief Appends the specified memory address to the specified filename.
 * @param f Pointer to the end of the file to append the content (open_dramsys_file should be used).
 * @param lastNumber The number of the last memory access.
 * @param address The address to append to the file.
 * @param readOrWrite 0 If read, 1 if write.
 */
void write_to_dramsys_file(FILE **f, int lastNumber, int readOrWrite, int address) {
	// The pointer gets checked before writing
    if (*f == NULL) {
        perror("File pointer is null\n");
        return;
    }

    // The line gets written to the file
    if (readOrWrite == 0) {
		fprintf(*f, "%d:\tread\t0x%x\n",lastNumber + 1, address);
	} else {
		fprintf(*f, "%d:\twrite\t0x%x\n",lastNumber + 1, address);
	}
}

/**
 * @brief Closes the DRAMSys trace file.
 * @param f The file to be closed.
 */
void close_dramsys_file(FILE **f) {
    fclose(*f);
}

/**
 * @brief Returns the same random value on the same cycle. This should be used on the rand replacement policy as it might be called
 * multiple times per cycle (When a collision happens for instance). The result should always be the same on the same cycle.
 */
int cycle_rand() {
	// A random value gets calculated for this cycle if it has not been done yet
	if (!(cycle_rand_updated == cycle)) {
		cycle_rand_value = rand();
		cycle_rand_updated = cycle;
	}

	return cycle_rand_value;
}




/*void contentArrayToString(long* array, char* content, int count, int width){

        content[0]='\0';        

        for(int i=0; i<count; i++){

          char num[50];
          sprintf(num, "%0*lx", width, array[i]);
	  strcat(content, num);
          strcat(content, " ");
	}
}*/

/*
void contentStringToArray(long* array, char* content, int count){
        

	for(int i=0; i<count; i++){
		array[i]=0;
	}

        int len=strlen(content);
        int lineIndex=0;
        char num[100];
	int j=0;
        for(int i=0; i<len; i++){
	     if(content[i]!=' '){
                  num[j]=toupper(content[i]);
             	  j++;
             }else{
	          j=0;
		  array[lineIndex]=strtol(num, NULL, 16);
         	  num[0]='\0';
        	  lineIndex++;
             }

        }
}*/

