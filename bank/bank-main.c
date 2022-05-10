/* 
 * The main program for the Bank.
 *
 * You are free to change this as necessary.
 */

#include <string.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include "bank.h"
#include "ports.h"

static const char prompt[] = "BANK: ";

int main(int argc, char**argv) {
   int n;
   char sendline[1000];
   char recvline[1000];
    if(argc != 2){
        printf("Usage: init <filename>\n");
        return 62;
    }

   char bank_path[strlen(argv[1])+1]; 
   strncpy(bank_path, argv[1], strlen(argv[1])); 
   bank_path[strlen(argv[1])] = '\0'; 
   printf("We're in main, the length of path is: %d\n", (int) strlen(argv[1])); 
   Bank *bank = bank_create(bank_path);
    
   printf("%s", prompt);
   fflush(stdout);

   while(1) {
       fd_set fds;
       FD_ZERO(&fds);
       FD_SET(0, &fds);
       FD_SET(bank->sockfd, &fds);
       select(bank->sockfd+1, &fds, NULL, NULL, NULL);

       if(FD_ISSET(0, &fds))
       {
           fgets(sendline, 1000,stdin);
           bank_process_local_command(bank, sendline, strlen(sendline));
           printf("%s", prompt);
           fflush(stdout);
       }
       else if(FD_ISSET(bank->sockfd, &fds))
       {
           n = bank_recv(bank, recvline, 1000);
           bank_process_remote_command(bank, recvline, n);
       }
   }

   return EXIT_SUCCESS;
}
