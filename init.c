#include <string.h>
#include <stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include <time.h>


int main(int argc, char *argv[]){
    /*Streamline the input, make sure user passes in correct arguments*/

    if(argc != 2){
        printf("Usage: bank <filename>\n");
        return 62;
    } else {
        char *atm_str = (char*) malloc(strlen(argv[1]) * sizeof(char) + 5);
        char *bank_str = (char*) malloc(strlen(argv[1]) * sizeof(char) + 6);
        strcpy(atm_str, argv[1]);
        strcpy(bank_str, argv[1]);
        strcat(atm_str, ".atm");
        strcat(bank_str, ".bank");

        FILE *fp_atm = fopen(atm_str, "r");
        FILE *fp_bank = fopen(bank_str, "r");
        if(fp_atm != NULL || fp_bank != NULL) {
            free(atm_str);
            free(bank_str);
            fclose(fp_atm);
            fclose(fp_bank);
            printf("Error: one of the files already exists\n");
            return 63;
        }

    /* Create files if they don't exist and make them writable*/
        fp_atm = fopen(atm_str, "w");
        fp_bank = fopen(bank_str, "w");
        if(fp_atm == NULL || fp_bank == NULL){
            free(atm_str);
            free(bank_str);
            fclose(fp_atm);
            fclose(fp_bank);
            printf("Error creating initialization files\n");
            return 64;
        }

        /* All error cases prescribed on gitlab should be handled now */
        unsigned char key[32];
        unsigned char iv[16];
        unsigned char RAN[10];
        int i = 0;
        srand(time(NULL) + 3);
        for (i = 0; i < sizeof(key); i++) {
            key[i] = 56 + rand() % 26;
        }

        fwrite(key , 1 , 32 , fp_atm );
        fwrite("\n", sizeof("\n"), 1, fp_atm);
        fwrite(key, 1 , 32 , fp_bank);
        fwrite("\n", sizeof("\n"), 1, fp_bank);

        i = 0;
        srand(time(NULL) + 2);
        for (i = 0;  i < sizeof(iv); i++) {
            iv[i] = 56 + rand() % 26;
        }

        fwrite(iv , 1 , sizeof(iv) , fp_atm );
        fwrite("\n", sizeof("\n"), 1, fp_atm);
        fwrite(iv, 1 , sizeof(iv) , fp_bank);
        fwrite("\n", sizeof("\n"), 1, fp_bank);

        i = 0;
        srand(time(NULL) + 1);
        for(i =0;i<sizeof(RAN);i++){
            RAN[i] = 56 + rand() % 26;
        }

        fwrite(RAN , 1 , sizeof(RAN) , fp_atm );
        fwrite("\n", sizeof("\n"), 1, fp_atm);
        fwrite(RAN, 1 , sizeof(RAN) , fp_bank);
        fwrite("\n", sizeof("\n"), 1, fp_bank);


        printf("Successfully initialized bank state\n");


        /**
        Writes to file in this order: 
            Key first, 
            IV next, 
            RAN next.

        */
        return 0;   
    }

    

}
