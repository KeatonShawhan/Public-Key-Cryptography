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


#define OPTIONS "b:i:n:d:s:vh"


// parse through each input character
int main(int argc, char **argv) {
	// initialize the default values
	//time_t set;
	//set = time(NULL);
	// set mpz_t variables
	mpz_t p, q, n, e, s, d, m, username;
	mpz_inits(m, p, q, n, e, s, d, username, NULL);
	// default the seed time(NULL)
	randstate_init(time(NULL));
	// set default values
	bool verbose = false;
	uint64_t iters = 50;
	uint64_t nbits = 1024;
	char *pbfile = "rsa.pub";
	char *pvfile = "rsa.priv";
	// bool print_bool = false;
	int opt = 0;
	// loop, checking which option was used and if it was in the list of options
	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
		// allows a variable to be tested for equality against a list of values
		switch (opt) {
			case 'b':
				// check if the number of bits inputted is in the range of 50-4096
				if (strtol(optarg, NULL, 10) < 50 || strtol(optarg, NULL, 10) > 4096){
					// if not, print the help message and exit the program with a non-zero return
					fprintf(stderr, "./keygen-dist: Number of bits must be 50-4096, not %s.\n", optarg);
					fprintf(stderr, "Usage: ./keygen-dist [options]\n");
                                        fprintf(stderr, "  ./keygen-dist generates a public / private key pair, placing the keys into the public and private\n");
                                        fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                        fprintf(stderr, "  the program options.\n");
                                        fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                        fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                        fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                                        fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                        fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                                        fprintf(stderr, "    -v          : Enable verbose output.\n");
                                        fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
					return 1;
				}else{
					// if so, set the bits to the long version of the argument after the -b option
					nbits = strtol(optarg, NULL, 10);
				}
				// break the case
				break;
			case 'i':
				// check if the number of iterations is in the range of 1-500
				if (strtol(optarg, NULL, 10) < 1 || strtol(optarg, NULL, 10) > 500){
					// if not, print the help message and exit the program with a non-zero return
					fprintf(stderr, "./keygen-dist: Number of iterations must be 1-500, not %s.\n", optarg);
					fprintf(stderr, "Usage: ./keygen-dist [options]\n");
                                	fprintf(stderr, "  ./keygen-dist generates a public / private key pair, placing the keys into the public and private\n");
                                	fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                	fprintf(stderr, "  the program options.\n");
                                	fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                	fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                	fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                                	fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                	fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                                	fprintf(stderr, "    -v          : Enable verbose output.\n");
                                	fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
					return 1;
                                }else{
					// if so, set the iters to the long version of the argument after the -i option
                                        iters = strtol(optarg, NULL, 10);
                                }
                                break;
			case 'n':
				// set the public file to the string passed in through the command line
				pbfile = optarg;
                                break;
			case 'd':
                                // set the private file to the string passed in through the command line
				pvfile = optarg;
				break;
			case 's':
				// set the seed to the long version of the string passed in through the command line
				randstate_init(strtol(optarg, NULL, 10));
                                break;
			case 'v':
				// turn the verbose flag to true for printing later
				verbose = true;
				break;
			case 'h':
				// help message
				fprintf(stderr, "Usage: ./keygen [options]\n");
				fprintf(stderr, "  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
				fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
				fprintf(stderr, "  the program options.\n");
				fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
				fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
				fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
				fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
				fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
				fprintf(stderr, "    -v          : Enable verbose output.\n");
				fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 0;
			default:
				// if an option is used than isn't stated above, return a non-zero error
				// print out helper option (given)
				fprintf(stderr, "Usage: ./keygen [options]\n");
                                fprintf(stderr, "  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
                                fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                fprintf(stderr, "  the program options.\n");
                                fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                                fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 1;
		}


	}
	// try to open the private key file with writing ability
	FILE *privfile;
	privfile = fopen(pvfile, "w"); 
	// check if there is an error opening it
	if (privfile == NULL){
                fprintf(stderr, "Error when attempting to open the private file\n");
                return 1;
        }
	// use fchmod so only the owner can read or write into the private key file
        int pvfilecheck;
	int chmodfile;
	// use fileno to link the actual privfile to the fchmod function
        pvfilecheck = fileno(privfile);
	chmodfile = fchmod(pvfilecheck, S_IRUSR|S_IWUSR);
	// check if the fchmod has a problem
        if (chmodfile != 0){
                fprintf(stderr, "Error when attempting to use fchmod() on pvfile\n");
        }
	// try to open the public key file with writing ability
        FILE *pubfile;
	pubfile = fopen(pbfile, "w");
	// check if there is an error opening it
	if (pubfile == NULL){
		fprintf(stderr, "Error when attempting to open the public file\n");
		return 1;
	}
	// generate the public and private keys
	rsa_make_pub(p, q, n, e, nbits, iters);
        rsa_make_priv(d, e, p, q);
	// get the username
	char *name;
	name = getenv("USER");
	// set the username to type mpz_t for the rsa_sign function
	mpz_set_str(username, name, 62);
	rsa_sign(s, username, d, n);
	// write the public and private keys to their respective files
	rsa_write_pub(n, e, s, name, pubfile);
	rsa_write_priv(n, d, privfile);
	// check if the verbose flag is true from the command line
	if (verbose){
		// if so, print out each variable and their bits
		uint64_t s_bits = mpz_sizeinbase(s, 2);
		uint64_t p_bits = mpz_sizeinbase(p, 2);
		uint64_t q_bits = mpz_sizeinbase(q, 2);
		uint64_t n_bits = mpz_sizeinbase(n, 2);
		uint64_t e_bits = mpz_sizeinbase(e, 2);
		uint64_t d_bits = mpz_sizeinbase(d, 2);
		gmp_fprintf(stderr, "username: %s\n", name);
		gmp_fprintf(stderr, "user signature (%i bits): %Zu\n", s_bits, s);
		gmp_fprintf(stderr, "p (%i bits): %Zu\n", p_bits, p);
		gmp_fprintf(stderr, "q (%i bits): %Zu\n", q_bits, q);
		gmp_fprintf(stderr, "n - modulus (%i bits): %Zu\n",n_bits, n);
		gmp_fprintf(stderr, "e - public exponent (%i bits): %Zu\n", e_bits, e);
		gmp_fprintf(stderr, "d - private exponent (%i bits): %Zu\n", d_bits, d);
	}
	// close the public/private files, clear the random state, and clear the temporary mpz_t variables
	fclose(pubfile);
	fclose(privfile);
	randstate_clear();
	mpz_clears(p, q, n, e, d, username, s, NULL);
	return 0;

}
/*
int main(void){
	randstate_init(236);
        mpz_t p, q, n, e, s, d, c, m;
	char username[] = "keaton";
        uint64_t iters = 50;
        uint64_t nbits = 20;

	FILE *pubfile;
	pubfile = fopen("pubfile", "w");
        mpz_inits(c, m, p, q, n, e, s, d, NULL);
        rsa_make_pub(p, q, n, e, nbits, iters);

	rsa_write_pub(n, e, s, username, pubfile);
	fclose(pubfile);

	mpz_set_ui(n, 0);
	mpz_set(e, n);
	mpz_set(s, n);
	pubfile = fopen("pubfile", "r");
	rsa_read_pub(n, e, s, username, pubfile);
	//gmp_printf("%Zu\n%Zu\n%Zu\n%s\n", n, e, s, username);
	fclose(pubfile);

	FILE *privfile;
	privfile = fopen("privfile", "w");
	rsa_make_priv(d, e, p, q);
	rsa_write_priv(n, d, privfile);
	fclose(privfile);

	mpz_set_ui(n, 0);
	mpz_set(d, n);
        privfile = fopen("privfile", "r");
	rsa_read_priv(n, d, privfile);
	fclose(privfile);
	gmp_printf("%Zu\n%Zu\n", n, d);

	mpz_init_set_ui(m, 5);
	rsa_encrypt(c, m, e, n);

	gmp_printf("c: %Zu\n", c);
	mpz_set_ui(m, 0);
	rsa_decrypt(m, c, d, n);
	gmp_printf("m: %Zu\n", m);
	
	rsa_sign(s, m, d, n);
	gmp_printf("s: %Zu\n", s);

	bool check = rsa_verify(m, s, e, n);
	if (check == true){
		printf("true\n");
	}else{
		printf("false\n");	
	}

	FILE *encin;
	encin = fopen("encin", "r");
	FILE *encout;
	encout = fopen("encout", "w");
	rsa_encrypt_file(encin, encout, n, e);
	fclose(encout);

	FILE *decin;
	decin = fopen("encout", "r");
	FILE *decout;
        decout = fopen("decout", "w");
	rsa_decrypt_file(decin, decout, n, d);

	fclose(encin);
	fclose(encout);
	fclose(decout);


	randstate_clear();
}
*/
