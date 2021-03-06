# ATM Build-it Project Team 23 
Design and implementation of a prototype ATM/Bank system.

# Protocol
#### Initialize (init.c)
```sh
<path1>init <path2>/<name>  
```
Running the above on terminal produces two files: <path2>/<name>.bank and <path2>/<name>.atm These files store symmetric secret keys and are required to run the Bank and ATM programs. The Bank and ATM both use identical keys to encrypt/decrypt messages sent among themselves. The symmetric secret keys are randomly generated by the init script and are stored in .bank and .atm.
We do not use asymmetric keys here because it may be difficult to distinguish between the two individuals (ATM and Bank) who share the key. Also, 
these keys are usually slower to deconstruct. 



# Public Test Output: <TBD>


# Vulnerability 1: Buffer Overflow
Buffer overflows are a common vulnerability in programming languages such as C or C++ because of the direct access to memory. Even when there is randomized memory allocation, the offset between a saved instruction pointer and the last element of the buffer in memory remains the same. This is the issue we tackled in Project 1 and once again had to deal with here. We mitigate Buffer Overflows in our implementation by explicitly limiting the length of user inputs the ATM receives. Examples can be seen in code where we limit the input in atm-main.c to be 1000 characters. Even input in
init.c has malloc() used on it so that we do not create room for an attacker to insert shellcode and overflow. Hash Table matching can also be used to prevent overflowing. This is because the additional input into the char\[] or buffer\[] will not map to a value in a hashtable, thus rendering an overflow of this type unlikely.
	
	

# Vulnerability 2: Brute Force Cracking
Given the limited character amount in pins used for the ATM, it may be brute force cracked. The math here would be nPr = 10^4 given that we have 10 digits to choose from and just 4 characters in a pin and characters are repeatable (hence 10\*10\*10*10 possibilities). This is a very low number to brute force given most computers can be assumed to be able to handle 10000 total iterations. We mitigated this risk by limiting the number of times someone can enter a pin. In this way, an all out brute force is impossible, and the intruder would have to rely on luck or some extra level of analysis which in a real-life setting gives system managers enough time to inform the real user as to what's happening and take the appropriate action. Consider how some federal website passwords work like FAFSA: You can't enter the wrong password more than about 3-5 times. It locks the account if you try exceeding this limit. This was our objective in the ATM file. 

# Vulnerability 3: Replay Attack 
Replay Attacks are a method by which an attacker may intercept a packet and store them to send later as a "valid token" of sorts in order to access data and steal/modify information under false authentication. In our code, we generate a random token each time and "end" that token for the current session of the Bank/ATM communication to mitigate this risk, thus making the chances of a Replay Attack very unlikely. 


# Vulnerability 4: Packet Modification/Encryption Tampering
An attacker could modify the value to be withdrawn as desired if they intercept and modify the packet while in transit. This causes several problems as it indicates that one could potentially withdraw more than that exists within the account. Using hash values and/or hex values to "pad" the data being sent helps mitigate this risk. The process is also called "salted hashing". If the extra data sent is not matching from one end to the other, then the data has surely been tampered with and hence any query/input is ignored and the program supposed to not run until the correct communication process is taken or the program must be terminated. This is what we attempted to do in our ATM and Bank communication. 

# Vulnerability 5: Packet Dropping
Reception of data on one end of the communication may be infinitely halted if an attacker intercepts a packet and rendering it never sent. The program either crashes or becomes unusable because it doesn't receive this packet that has not been sent. Further effects would include that the ATM in our case will not process any additional requests from the single user in question because it will continue to wait on this matter. Even if one took a multithreaded approach to this project, a packet drop would cause the final thread to wait infinitely for its job to complete, rendering it still unusable even if other tasks get completed. We mitigate this issue in our ATM file by giving a timeout beyond which other processes may resume and proceed smoothly. 

# Vulnerability 6: Concurrent Logins 

The project description clearly states only one user is allowed to log in at one time. This means that we need to mitigate the risk of when an attacker could potentially log in while a user is already active. Concurrent Logins can lead to the use of legitimate/valid certificates by unauthorized/illegitimate individuals to authenticate a network or system. In our code, we return/print an error message when there's an additional attempt to log in while a session is active. This mitigates the threat concurrent logins pose. 


# Vulnerability 7: Integer Overflow
This is not to be confused with Buffer Overflow: Integer Overflow refers to the wrap around of int values (usually with signed ints). By wrap around, we mean that when an integer reaches the INT_MAX value, it'll move on to negative values and this will cause the withdraw function to actually *add* to the bank instead of subtract. We prevented this by explicitly checking for the account requested and making sure it doesn't go over the integer limit, i.e, cap the max request at that value provided that much is actually available for withdrawal.  
