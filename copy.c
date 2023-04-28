#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define SIZE 1024


int copy_file(char *name_of_file1 , char *name_of_file2, int want_f_operation){

    FILE *f1;
    FILE *f2;

    // checking if source file is exists
    f1 = fopen(name_of_file1 , "rb");
    if(f1 == NULL){
        printf("There is no file to copy from!\n");
        return 1;
    }

    // checking if destantion file is already exists that will pr
    f2 = fopen(name_of_file2 , "rb");
    if(f2 != NULL){
     // file2 is exists
        if(want_f_operation != 1){
            printf("target file exists\n");
            fclose(f1);
            fclose(f2);
            return 2;
        }
        
    }

    f2 = fopen(name_of_file2, "w");
    if(f2 == NULL){
        printf("Cant create destination file\n");
        fclose(f1);
        return 1;
    }

    // copy to file 2 from file 1
    
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, 1024, f1)) > 0) {
        size_t bytes_written =  fwrite(buffer, 1, bytes_read, f2);
        if (bytes_written == -1) {
            printf("Error: cannot write to destination file\n");
            fclose(f2);
            fclose(f1);
            return 1;
        }
    }

    fclose(f1);
    fclose(f2);

    return 0;


}


int main(int argc, char **argv){ 
    
    char *name_of_file1 = NULL;
    char *name_of_file2 = NULL;

    int want_v_operation = 0;
    int want_f_operation = 0;

    for(int i=1; i<argc; i++){
        if(strcmp(argv[i], "-v") == 0){
            want_v_operation = 1;
        }
        else if(strcmp(argv[i], "-f") == 0){
            want_f_operation = 1;
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
        int result = copy_file(name_of_file1, name_of_file2, want_f_operation);
        
        if(result == 0){
            printf("Success\n");
        }
        else if(result == 1){
            printf("general failure\n");
        }
    }

    return 0;
}
