#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gmp.h>
#include <stdlib.h>
#include <stdint.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

//gmp_randstate_t state;


#define OPTIONS "i:o:n:vh"


// parse through each input character
int main(int argc, char **argv) {
	// initialize the default values
	//time_t set;
	//set = time(NULL);
	// set mpz_t temporary variables
	mpz_t n, d;
	mpz_inits(n, d, NULL);
	// default the seed to time(NULL) and set the files to their default values
	randstate_init(time(NULL));
	bool verbose = false;
	char *pvfile = "rsa.priv";
	FILE *encinp = stdin;
	FILE *encout = stdout;
	// bool print_bool = false;
	int opt = 0;
	// loop, checking which option was used and if it was in the list of options
	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
		// allows a variable to be tested for equality against a list of values
		switch (opt) {
			case 'i':
				// check if the input file exists so there is at least something to decrypt
				if (fopen(optarg, "r") == NULL){
                                        fprintf(stderr, "decrypt-dist: Couldn't open %s to read ciphertext: No such file or directory\n", optarg);
                                        return 1;
                                }
				// set the file to decrypt to the option passed in through the command line
				encinp = freopen(optarg, "r", stdin);
                                break;
			case 'o':
				// set the file to output to the option passed in through the command line
				encout = freopen(optarg, "w", stdout);
                                break;
			case 'n':
				// set the private key file to the option passed in through the command line
				pvfile = optarg;
				break;
			case 'v':
				// set the flag for the verbose output to true
				verbose = true;
				break;
			case 'h':
				// print the help message
				fprintf(stderr, "Usage: ./decrypt [options]\n");
				fprintf(stderr, "  ./decrypt decrypts an input file using the specified private key file,\n");
				fprintf(stderr, "  writing the result to the specified output file.\n");
				fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
				fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
				fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n");
				fprintf(stderr, "    -v          : Enable verbose output.\n");
				fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 0;
			default:
				// if an option is used than isn't stated above, return a non-zero error
				// print out helper option (given)
				fprintf(stderr, "Usage: ./decrypt [options]\n");
                                fprintf(stderr, "  ./decrypt decrypts an input file using the specified private key file,\n");
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 1;
		}


	}
	// open the private key file with reading permissions and see if there is an error opening it
	FILE *privfile;
	privfile = fopen(pvfile, "r");
	if (privfile == NULL){
		fprintf(stderr, "Error when attempting to open the public file\n");
		return 1;
	}
	//char *name;
	//name = getenv("USER");
	//mpz_set_str(username, name, 62);
	
	//char name[strlen(getenv("USER"))];
	// read the private key from the private key file
	rsa_read_priv(n, d, privfile);
	// decrypt the file desired (encinp), printing the plaintext to encout
	rsa_decrypt_file(encinp, encout, n, d);
	// check if the verbose flag was ever turned on with the -v option, if so, print out information about each variable
	if (verbose){
		uint64_t d_bits = mpz_sizeinbase(d, 2);
		uint64_t n_bits = mpz_sizeinbase(n, 2);
		gmp_fprintf(stderr, "n - modulus (%i bits): %Zu\n",n_bits, n);
		gmp_fprintf(stderr, "d - private exponent (%i bits): %Zu\n", d_bits, d);
	}
	// close the private key file, clear the randstate, and clear the mpz_t variables
	fclose(privfile);
	fclose(encinp);
	fclose(encout);
	randstate_clear();
	mpz_clears(n, d, NULL);
	return 0;

}
