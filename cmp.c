#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 1024


int check_equal(char *name_of_file1 , char *name_of_file2, int want_i_operation){

    FILE *f1;
    FILE *f2;

    f1 = fopen(name_of_file1,"r");
    if(f1 == NULL){
        perror(name_of_file1);
        exit(1);
    }

    f2 = fopen(name_of_file2,"r");
    if(f2 == NULL){
        perror(name_of_file2);
        exit(1);
    }

    char c1;
    char c2;

    
    while(!feof(f1) || !feof(f2)){
        c1 = fgetc(f1);
        c2 = fgetc(f2);
        if(want_i_operation == 1){
            c1 = tolower(c1);
            c2 = tolower(c2);
            if(c1 != c2){
                return 1;
            }
        }
        else{
            if(c1 != c2){
                return 1;
            }
        }
    }

    if((!feof(f1) && feof(f2)) || (feof(f2) && !feof(f2))){
     
        return 1;
    
    }

    return 0;
}


int main(int argc, char **argv){

    char *name_of_file1 = NULL;
    char *name_of_file2 = NULL;

    int want_v_operation = 0;
    int want_i_operation = 0;

    for(int i=1; i<argc; i++){
        if(strcmp(argv[i], "-v") == 0){
            want_v_operation = 1;
        }
        else if(strcmp(argv[i], "-i") == 0){
            want_i_operation = 1;
        }
        else if(name_of_file1 == NULL){
            name_of_file1 = argv[i];
        }
        else if(name_of_file2 == NULL){
            name_of_file2 = argv[i];
        }
        else{
            printf("Invaild Input! Need to be in this format : ./cmp <file1> <file2> [-v] [-i]\n");
            return -1;
        }
    }

    if(name_of_file1 == NULL || name_of_file2 == NULL){
        printf("Error: You must specify two file names.\n");
        return 1;
    }

    else if(want_v_operation == 1){
        int result = check_equal(name_of_file1, name_of_file2, want_i_operation);
        
        if(result == 0){
            printf("equal\n");
        }
        else{
            printf("distinct\n");
        }
    }

    return 0;
}