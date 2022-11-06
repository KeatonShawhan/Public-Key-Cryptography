How to compile:
To compile this program, simply use the “make” command. This will create an executable file for each test harness, keygen.c, encrypt.c, and decrypt.c. Now each executable can be run with default values like so: “./keygen”. Included in each executable are specific command-line options pertaining to the usage of each harness. 

For keygen.c, you can change the number of bits (n) with the “-b” option in a range of 50-4096 inclusive, the number of Miller-Rabin iterations with the “-i” option in a range of 1-500 inclusive, “-n [filename]” to specify the public key file, “-d [filename]” to specify the private key file, “-s” to specify the random seed for the random state initialization, “-v” to enable verbose output, and “-h” to display program synopsis and usage.

For encrypt.c you can use “-i” to specify the input file to encrypt, “-o” to specify the output file to encrypt, “-n” to specify the file containing the public key, “-v” to enable verbose output, and “-h” to display program synopsis and usage. 

For decrypt.c you can use “-i” to specify the input file to decrypt, “-o” to specify the output file to decrypt, “-n” to specify the file containing the private key, “-v” to enable verbose output, and “-h” to display program synopsis and usage. For the encrypt.c and decrypt.c harnesses, if an input or output file isn’t specified with a command-line option, the command line itself will take the input and output.


File description:
“readme.md” is this file.
“numtheory.c” is a file that contains all of the functions for calculating public and private keys for later use in other files.
“numtheory.h” specifies the interface for the number theory functions.
“randstate.c” contains the implementation of the random state interface for the RSA library
and number theory functions.
“randstate.h” specifies the interface for initializing and clearing the random state.
“rsa.c” contains the implementation of the RSA library.
“rsa.h” specifies the interface for the RSA library.
“keygen.c” contains the implementation and main() function for the keygen program.
“encrypt.c” contains the implementation and main() function for the encrypt program.
“decrypt.c” contains the implementation and main() function for the decrypt program.
“Makefile” is a file that compiles other files to make executables.
“WRITEUP.pdf” is a file that contains a description of how the functions were tested.
“DESIGN.pdf” is a file that contains pseudocode for each function.

