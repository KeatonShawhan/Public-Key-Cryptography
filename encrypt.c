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
	mpz_t n, e, s, m, username;
	mpz_inits(n, e, s, m, username, NULL);
	// default the seed to time(NULL) and set the files to their default values
	randstate_init(time(NULL));
	bool verbose = false;
	char *pbfile = "rsa.pub";
	FILE *encinp = stdin;
	FILE *encout = stdout;
	// bool print_bool = false;
	int opt = 0;
	// loop, checking which option was used and if it was in the list of options
	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
		// allows a variable to be tested for equality against a list of values
		switch (opt) {
			case 'i':
				// check if the input file exists so there is at least something to encrypt
				if (fopen(optarg, "r") == NULL){
					fprintf(stderr, "encrypt-dist: Couldn't open %s to read plaintext: No such file or directory\n", optarg);
					return 1;
				}
				// set the file to encrypt to the option passed in through the command line
				encinp = fopen(optarg, "r");
                                break;
			case 'o':
                                // set the file to decrypt to the option passed in through the command line
				encout = fopen(optarg, "w");
                                break;
			case 'n':
                                // set the public key file to the option passed in through the command line
				pbfile = optarg;
				break;
			case 'v':
				// set the flag for the verbose output to true
				verbose = true;
				break;
			case 'h':
				// print the help message
				fprintf(stderr, "Usage: ./encrypt [options]\n");
				fprintf(stderr, "  ./encrypt encrypts an input file using the specified public key file,\n");
				fprintf(stderr, "  writing the result to the specified output file.\n");
				fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
				fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
				fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
				fprintf(stderr, "    -v          : Enable verbose output.\n");
				fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 0;
			default:
				// if an option is used than isn't stated above, return a non-zero error
				// print out helper option (given)
				fprintf(stderr, "Usage: ./encrypt [options]\n");
                                fprintf(stderr, "  ./encrypt encrypts an input file using the specified public key file,\n");
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 1;
		}


	}
	// open the public key file with reading permissions and see if there is an error opening it
	FILE *pubfile;
	pubfile = fopen(pbfile, "r");
	if (pubfile == NULL){
		fprintf(stderr, "Error when attempting to open the public file\n");
		return 1;
	}
	//char *name;
	//name = getenv("USER");
	//mpz_set_str(username, name, 62);
	
	// set a name array to the length of the username from getenv()
	char name[strlen(getenv("USER"))];
	// read the public key from the public file
	rsa_read_pub(n, e, s, name, pubfile);
	// convert the name pulled from rsa_read_pub to mpz_t
	mpz_set_str(m, name, 62);
	// verify the signature, print and error if not able to
	if(rsa_verify(m, s, e, n) == false){
		fprintf(stderr, "Error verifying the signature\n");
		return 1;
	}
	// encrypt the file
	rsa_encrypt_file(encinp, encout, n, e);
	// check if the verbose flag was ever turned on with the -v option, if so, print out information about each variable
	if (verbose){
		uint64_t s_bits = mpz_sizeinbase(s, 2);
		uint64_t n_bits = mpz_sizeinbase(n, 2);
		uint64_t e_bits = mpz_sizeinbase(e, 2);
		gmp_fprintf(stderr, "username: %s\n", name);
		gmp_fprintf(stderr, "user signature (%i bits): %Zu\n", s_bits, s);
		gmp_fprintf(stderr, "n - modulus (%i bits): %Zu\n",n_bits, n);
		gmp_fprintf(stderr, "e - public exponent (%i bits): %Zu\n", e_bits, e);
	}
	// close the public key file, clear the randstate, and clear the mpz_t variables
	fclose(pubfile);
	fclose(encinp);
	fclose(encout);
	randstate_clear();
	mpz_clears(n, e, m, s, NULL);
	return 0;

}
