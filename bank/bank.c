#include "bank.h"
#include "ports.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

/*
    December 2 6:55 pm: Current progress. 
    create-user is done. 
    Working on getting communication done using "balance", function. 
*/


char iv[16]; 
char key[32]; 

void create_user(Bank *bank, char *command, size_t len); 
void deposit(Bank *bank, char *command, size_t len, int local); 
void balance(Bank *bank, char *command, size_t len, int local); 
void invalid_usage_create(); 
void bank_read_file(); 
unsigned char* getAuth(unsigned char* RAN);
void bank_send_begin_session(Bank *bank, char yn); 
void begin_session(char *name, int pin, unsigned char *auth, Bank *bank);

int encrypt(unsigned char* plainText, int plainTextLen, unsigned char* cipherText) {
    int len, ciphertext_len; 
	EVP_CIPHER_CTX *ctx; 
	ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char *)key, (unsigned char*) iv);
	EVP_CipherUpdate(ctx, cipherText, &len,  plainText, plainTextLen);
    ciphertext_len = len; 
	EVP_CipherFinal(ctx, cipherText + len, &len); 
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

 	return ciphertext_len; 
}

int decrypt(unsigned char* cipherText, int cipherTextLen, unsigned char * plainText)  {
	EVP_CIPHER_CTX *ctx;
	int len, plainTextLen;
	ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char *) key, (unsigned char*)iv);
	EVP_DecryptUpdate(ctx, plainText, &len, cipherText, cipherTextLen);
    plainTextLen = len; 
	EVP_DecryptFinal_ex(ctx, plainText + len, &len);
    plainTextLen += len;
	EVP_CIPHER_CTX_free(ctx);
    return plainTextLen;
}

Bank* bank_create(char *bank_path) {
    printf("%s\n", "This is fine0"); 
    Bank *bank = (Bank*) malloc(sizeof(Bank));
    if(bank == NULL) {
        perror("Could not allocate Bank");
        exit(1);
    }

    // Set up the network state

    bank->sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&bank->rtr_addr,sizeof(bank->rtr_addr));
    bank->rtr_addr.sin_family = AF_INET;
    bank->rtr_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    bank->rtr_addr.sin_port=htons(ROUTER_PORT);
    printf("%s\n", "This is fine1"); 
    bzero(&bank->bank_addr, sizeof(bank->bank_addr));
    bank->bank_addr.sin_family = AF_INET;
    bank->bank_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    bank->bank_addr.sin_port = htons(BANK_PORT);
    bank->sessionUser = NULL;
    printf("%s\n", "This is fine2"); 
    bind(bank->sockfd,(struct sockaddr *)&bank->bank_addr,sizeof(bank->bank_addr));
    bank->ht = hash_table_create(100000); 
    bank->hashToName = hash_table_create(100000); 

    printf("%s\n", "This is fine3"); 
    // Set up the protocol state
    // TODO set up more, as needed
    /**
        Open the file here. 
    */
    bank_read_file(bank, bank_path);
    return bank;
}

void bank_read_file(Bank *bank, char *bank_str) {
    char line[32];
    printf("This is the file we want: %s\n", bank_str);
    printf("length of filename: %d\n", (int) strlen(bank_str));
    FILE *fp = fopen(bank_str, "r");
    if (fp == NULL) {
        printf("%s", "Error opening bank initialization file"); 
        exit(64); 
    }

    size_t len = 0;
    fread(key, 32, 1, fp);
    getline(&line, &len, fp);
    strncpy(key, line, 32);
    getline(&line, &len, fp); 
    strncpy(iv, line, 16);
    getline(&line, &len, fp); 
    printf("%s", "This is fine7\n");
    strncpy((char *)&(bank->AuthToken), line, 10);
    
    fclose(fp);
}

void bank_free(Bank *bank) {
    if(bank != NULL) {
        close(bank->sockfd);
        free(bank);
    }
}

ssize_t bank_send(Bank *bank, char *data, size_t data_len) {
    // Returns the number of bytes sent; negative on error
    return sendto(bank->sockfd, data, data_len, 0,
                  (struct sockaddr*) &bank->rtr_addr, sizeof(bank->rtr_addr));
}

ssize_t bank_recv(Bank *bank, char *data, size_t max_data_len) {
    // Returns the number of bytes received; negative on error
    return recvfrom(bank->sockfd, data, max_data_len, 0, NULL, NULL);
}

/**
This function assumes an unencrypted text command. 
*/
// if local == 1, command is local:print
void bank_process_local_command(Bank *bank, char *command, size_t len) {
    // TODO: Implement the bank's local commands
    if (command[0] == 'c') 
        create_user(bank, command, len);
     else if (command[0] == 'd') 
        deposit(bank, command, len, 1); 
     else if (command[0] == 'b') 
        balance(bank, command, len, 1); 
     else 
        printf("%s", "Invalid command\n"); 

}

void bank_process_remote_command(Bank *bank, char *command, size_t len) {
    // TODO: Implement the bank side of the ATM-bank protocol
    
    // Tejan added a comment: Just decrypt the command appropriately and 
    // Call bank_process_local_command.
	/**
	 * The following is a toy example that simply receives a
	 * string from the ATM, prepends "Bank got: " and echoes 
	 * it back to the ATM before printing it to stdout.
	 */

	/*
    char sendline[1000];
    command[len]=0;
    sprintf(sendline, "Bank got: %s", command);
    bank_send(bank, sendline, strlen(sendline));
    printf("Received the following:\n");
    fputs(command, stdout);
	*/

    char plainText[48]; 
    decrypt( (unsigned char *)command, 48, (unsigned char *)plainText); 
    if (plainText[0] == 'b' && plainText[1] == 'e' && plainText[2] == 'g' && plainText[3] == 'i') {
        // We're doing begin session. 
        // parse name, and other stuff
        int i = 6;
        char pin_str[5];
        pin_str[4] = '\0';
        int pin = atoi(pin_str); 
        strncpy(pin_str, plainText + i, 4); 
        i += 5; 
        char nameHash[17]; 
        char *name = hash_table_find(bank->hashToName, nameHash);

        strncpy(nameHash, plainText + i, 16); 
        nameHash[16] = '\0'; 
        i += 17; 
        char auth[11]; 
        strncpy(auth, plainText + i, 10); 
        nameHash[10] = '\0'; 
        begin_session(name, pin, (unsigned char *)auth, bank); 
    } else if (plainText[0] == 'd' && plainText[1] == 'e' && plainText[2] == 'p') {
        // we need to deopsit money
    }
}

void begin_session(char *name, int pin, unsigned char *auth, Bank *bank) {
    // This should verify the username and pin with the database, and also verify the 
    // auth token, and then send a new auth token.
    if (bank->sessionUser != NULL) {
        bank_send_begin_session(bank, 'n');
        return;
    }
    
    if (auth != bank->AuthToken) {
        bank_send_begin_session(bank, 'n'); 
        return; 
    } else {
        int *userData; 
        userData = hash_table_find(bank->ht, name); 
        if (pin != userData[0]) {
            bank_send_begin_session(bank, 'n'); 
            return; 
        }
        else {
            bank->sessionUser = name;
            bank_send_begin_session(bank, 'y');
        }
    }
}


void bank_send_begin_session(Bank *bank, char yn) {
    unsigned char plainText[35]; 
    unsigned char newAuth[10]; 
    getAuth(newAuth); 
    
    unsigned char cipherText[48]; 
    strncpy("start" + yn, (char *)plainText, 6); 
    strncpy((char *) bank->AuthToken, (char *)plainText + 6, 10); 
    strncpy((char *)newAuth, (char *)plainText + 16, 10);
    encrypt(plainText, 35, cipherText); 
    bank->AuthToken = newAuth;
    bank_send(bank, (char *)cipherText, 48); 
}

unsigned char* getAuth(unsigned char* RAN) {
    int i = 0;
    srand(time(NULL));
    for(i =0;i<sizeof(RAN);i++) {
        RAN[i] = rand() % 256;
    }

    return RAN; 
}


// Remove local
void create_user(Bank *bank, char *command, size_t len) {
    // We need to add the user to bank. 
    // Get name of the user from the command: create-user <user-name> <pin> <balance>
    // if local == 1, command is local:print,
    //              otherwise it is remote: send back to network.
    // If user exists then print user exists. 
    // char *token = strtok(command, " ");
    char name[251]; 
    int i = 0;
    while ( i < len && command[i] != ' ' ) {
        i++;
    }

    if (i == len || i > 250) {
        // They passed in "create-user"
        invalid_usage_create();
        return; 
    }
    i++; 
    int j = i; 
    int len_temp = i - j;
    while ( i < len && command[i] != ' ' ) {
        i++;
    }
    
    if (i == len || len_temp > 250) { 
        // They passed in "create-user name"
        invalid_usage_create();
        return;
    }
    strncpy(name, command + j, len_temp); 
    name[len_temp] = '\0';
    // printf("This is the username: %s", name); 
    i++; // we just skipped the space after 
    char pin_temp[5]; 
    pin_temp[4] = '\0';

    strncpy(pin_temp, command + i, 4); 
    int pin = atoi(pin_temp); 
    if (command[i+4] != ' ') {invalid_usage_create(); return;}
    i += 5; // skipping space after pin and reaching the amount

    if (i >= len) {
        invalid_usage_create();
        return; 
    }

    char amount_temp[11]; 
    amount_temp[10] = '\0'; 

    if (strlen(command) - i > 10) {
        invalid_usage_create();
        return; 
    }
    strncpy(amount_temp, command + i, 10); 
    char *eptr;
    long long amount = strtoll(amount_temp, &eptr, 10); 
    if (amount > 2147483647) {
        invalid_usage_create();
        return; 
    }
    printf("name: %s", name);
    if ( hash_table_find(bank->ht, name) == NULL ) {
        // Add username to the hashtable.
        int temp[] = {pin, (int)amount}; 
        hash_table_add(bank->ht, name, temp); 
        unsigned int hashLen; 
        EVP_MD_CTX *mdctx; 
        mdctx = EVP_MD_CTX_new();
        unsigned char cipherText[48];
        EVP_DigestInit_ex(mdctx, EVP_shake128(), NULL);
        EVP_DigestUpdate(mdctx, name, strlen(name)); 
        EVP_DigestFinal_ex(mdctx, hash, &hashLen); 

        hash_table_add(bank->hashToName, (char * ) hash, temp);
        // values will be pin and balance.
        char filename[strlen(name) + 8]; 
        strncpy(filename, "../", 3); 
        strncpy(filename + strlen(name)+3, ".card", 5); 
        strncpy(filename+3, name, strlen(name)); 
        FILE *fp = fopen(filename, 'w'); 
        unsigned char hash[16]; 
        int i = 0; 
        for (i = 0; i < 16; i++) {
            /* write to file using fputc() function */
            fputc(hash[i], fp);
        }
        fclose(fp);        
        printf("Created user %s", name); 
        
    } else { // user already exists.
        printf("Error:  user %s already exists", name); 
    }
}

void invalid_usage_create() {
    // just print the regular invalid_usage_create stuff.
    printf("%s", "Usage:  create-user <user-name> <pin> <balance>"); 
}


void deposit (Bank *bank, char *command, size_t len, int local) {
    int i = 0; 
    while (i < len && command[i] != ' ') i++; 
    i++; 
    // skipping the command "deposit", and whitespace after that.
}

void balance(Bank *bank, char *command, size_t len, int local) {
    // We need to add the user to bank. 
    int i = 0; 
    while (i < len && command[i] != ' ') i++; 
    i++; 
    // skipping the "balance" keyword and the white space 
    if (i >= len) {
        printf("%s", "Usage:  balance <user-name>"); 
    }
    char name[250]; 
    strncpy(name, command + i, len - i); 
    if ( hash_table_find(bank->ht, name) == NULL ) {
        printf("%s", "No such user"); 
    } else {
        if (local == 1) {
            int *data; 
            data = hash_table_find(bank-> ht, name); 
            printf("$%d", data[1]); 
        } else {
            // send to bank. After encrypting, and processing the text.
            
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
