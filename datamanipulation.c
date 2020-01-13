#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "datamanipulation.h"

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
   char *pch;
   pch = strtok (content," ");
   for(int i=0; pch != NULL && i < count; i++, pch = strtok (NULL, " ")) {
      array[i] = strtol(pch, NULL, 16);
   }
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

