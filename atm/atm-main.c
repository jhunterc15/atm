/* 
 * The main program for the ATM.
 *
 * You are free to change this as necessary.
 */

#include "atm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static char prompt[259] = "ATM: ";

int main( int argc, char ** argv) {
    char user_input[1000];
	char username[251]; 


   char atm_path[strlen(argv[1])+1]; 
   strncpy(atm_path, argv[1], strlen(argv[1])); 
   atm_path[strlen(argv[1])] = '\0'; 
   printf("We're in main, the length of path is: %d\n", (int) strlen(argv[1])); 

   ATM *atm = atm_create( atm_path );

    printf("%s", prompt);
    fflush(stdout);

    while (fgets(user_input, 1000,stdin) != NULL)
    {
		int result; 
        result = atm_process_command(atm, user_input, username);
		if (result == 2){
			sprintf(prompt, "ATM (%s): ", username);
		}

		else if (result == -1){
			return EXIT_SUCCESS; 
		}
        printf("%s", prompt);
        fflush(stdout);
    }
	return EXIT_SUCCESS;
}
