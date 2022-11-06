#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <stdint.h>
#include "numtheory.h"
#include "randstate.h"

// set the state (extern in randstate.h

// create the rsa_make_pub function to make a public key e
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters){
	// set p bits to a random number in the range of nbits/4 to 3/4(nbits)
	uint64_t p_bits = random() % ((nbits)/2);
	p_bits = ((nbits/4) + p_bits);
	uint64_t q_bits;
	// initialize all mpz_t variables
	mpz_t pprime, qprime, totientn, pq, pminus, qminus, cmn, rand, gcdcheck;
	mpz_inits(pprime, qprime, totientn, pq, pminus, qminus, cmn, rand, gcdcheck, NULL);
	// set q_bits to the remaining amount of bits left over from p
	q_bits = nbits-p_bits;
	// make the large prime numbers with the calculated bits from earlier
	make_prime(p, p_bits, iters);
	make_prime(q, q_bits, iters);
	// find the totient of n (n-(p+q)+1)
	mpz_mul(n, q, p);
	mpz_add(pq, p, q);
	mpz_sub(totientn, n, pq);
	mpz_add_ui(totientn, totientn, 1);
	// get the absolute value of totient of n
	mpz_abs(totientn, totientn);
	// find q-1 and p-1
	mpz_sub_ui(pminus, p, 1);
	mpz_sub_ui(qminus, q, 1);
	// calculate the greatest common denominator of p-1 and q-1
	gcd(cmn, pminus, qminus);
	// divide the absolute value of totient of n by the gcd of p-1 and q-1 for the carmichael's value of n
	mpz_fdiv_q(cmn, totientn, cmn);
	// loop to find a random number that is co-prime with the carmichael's value of n
	do{
		// find the random number to compare against the carmichael's value of n
		mpz_urandomb(rand, state, nbits);
		//gmp_printf("%Zu\n", rand);
		// check if they're co-prime
		gcd(gcdcheck, rand, cmn);
		
	} while((mpz_cmp_ui(gcdcheck, 1) != 0) || (mpz_sizeinbase(rand, 2) < nbits));
	// set the output variable (and public exponent) e to the random number that was co-prime
	mpz_set(e, rand);
	mpz_clears(pprime, qprime, totientn, pq, pminus, qminus, cmn, rand, gcdcheck, NULL);
}

// create the rsa_write pub function to write n (p*q), the public exponent e, the signature s, and the username into a file
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile){
	// print each value into the file specified with a trailing new line after each
	gmp_fprintf(pbfile, "%Zx\n\n", n);
	gmp_fprintf(pbfile, "%Zx\n\n", e);
	gmp_fprintf(pbfile, "%Zx\n\n", s);
	fprintf(pbfile, "%s\n\n", username);
}

// create the rsa_read_pub function to read a desired file for the values of n (p*q), the public exponent e, the signature s, and the username
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile){
	// scan the file to retrieve the value of each variable
	gmp_fscanf(pbfile, "%Zx\n\n", n);
	gmp_fscanf(pbfile, "%Zx\n\n", e);
        gmp_fscanf(pbfile, "%Zx\n\n", s);
        fscanf(pbfile, "%s\n\n", username);
}

// create the rsa_make_priv to generate a private key d
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q){
	// initialize temp variables
	mpz_t cmn, n, totientn, pq, pminus, qminus;
	mpz_inits(n, totientn, pq, pminus, qminus, cmn, NULL);
	// find the totient of n (n-(p+q)+1)
        mpz_mul(n, q, p);
        mpz_add(pq, p, q);
        mpz_sub(totientn, n, pq);
        mpz_add_ui(totientn, totientn, 1);
        // get the absolute value of totient of n
        mpz_abs(totientn, totientn);
        // find q-1 and p-1
        mpz_sub_ui(pminus, p, 1);
        mpz_sub_ui(qminus, q, 1);
        // calculate the greatest common denominator of p-1 and q-1
        gcd(cmn, pminus, qminus);
        // divide the absolute value of totient of n by the gcd of p-1 and q-1 for the carmichael's value of n
        mpz_fdiv_q(cmn, totientn, cmn);
	// find the inverse mod of e and carmichael's of n
	mod_inverse(d, e, cmn);
	// clear the temp variables
	mpz_clears(n, totientn, pq, pminus, qminus, cmn, NULL);
}

// create the rsa_write_priv function to write the private key d and n to a file
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile){
	// print each value into the file specified with a trailing new line after each
        gmp_fprintf(pvfile, "%Zx\n\n", n);
        gmp_fprintf(pvfile, "%Zx\n\n", d);
}

// create rsa_read_priv function to read n and the private key d from a file
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile){
	// scan the private file to retrieve the value of each variable
	gmp_fscanf(pvfile, "%Zx\n\n", n);
	gmp_fscanf(pvfile, "%Zx\n\n", d);
}

// create rsa_encrypt function to calculate the cipher text
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n){
	// use power modulo to calculate the cipher text c = message m^the public key e, modulus n
	pow_mod(c, m, e, n);
}

// create the rsa_encrypt_file to encrypt the input file and print the data in the output file
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e){
	// set k and j
	uint64_t k;
	uint64_t j = 0;
	// initialize mpz_t variables
	mpz_t impout, c;
	mpz_inits(impout, c, NULL);
	// calculate log base 2 of n -1, then divide by 8 for the block size
	k = mpz_sizeinbase(n, 2);
	k -= 1;
	k /= 8;
	do{
		// allocate memory for the block your looking at
		uint8_t *new = (uint8_t*)malloc(k * sizeof(uint8_t));
		// set the 0th index of the block to the workaround byte
		new[0] = 0xFF;
		// set j to the amount of read bytes in the infile
		j = fread(new+1, sizeof(new[0]), k-1, infile);
		// convert the array of bytes to mpz_t
		mpz_import(impout, j+1, 1, sizeof(new[0]), 1, 0, new);
		// encrypt the array that was just filled to turn the mpz_t back to hex
		rsa_encrypt(c, impout, e, n);
		// print the ciphertext to the outfile
		gmp_fprintf(outfile, "%Zx\n\n", c);
		// free the array that was allocated at the top of the loop
		free(new);
	// loop while there is still data to be encrypted in the infile
	}while (feof(infile) == 0);
	// clear the temp mpz_t variables
	mpz_clears(impout, c, NULL);
}

// create the rsa_decrypt function to calculate the message
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n){
	// use the power modulo to calculate the original plain text m = cipher text c^private key d, modulus n
	pow_mod(m, c, d, n);
}

// create the rsa_decrypt_file to decrypt the input file and print the plain text in the output file
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d){
        uint64_t k;
        uint64_t j = 0;
        mpz_t m, c;
        mpz_inits(m, c, NULL);
	// calculate the block size
        k = mpz_sizeinbase(n, 2);
        k -= 1;
        k /= 8;
        do {
		// allocate memory for the block your looking at
                uint8_t *new = (uint8_t*)malloc(k * sizeof(uint8_t));
		// scan the infile for the each ciphertext
		gmp_fscanf(infile, "%Zx\n\n", c);
		// decrypt each ciphertext c, convert it to plain text m
		rsa_decrypt(m, c, d, n);
		// convert the message m to bytes
                mpz_export(new, &j, 1, sizeof(new[0]), 1, 0, m);
		// write the filled array of plain text to the outfile, accounting for the workaround byte
                fwrite(new+1, sizeof(new[0]), j-1, outfile);
		// free the array that was allocated at the top of the loop
                free(new);
	// loop while there is still data to be encrypted in the infile
        } while (feof(infile) == 0);
	// clear the temp mpz_t variables
        mpz_clears(c, m, NULL);
}

// create the rsa_sign function to calculate the signature
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n){
	// use the power modulo function to calculate the signature s = the plaintext m^the private key d, modulo n
	pow_mod(s, m, d, n);
}

// create the rsa_verify function to verify the signature s
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n){
	// calculate the verify mpz_t variable by using the power modulo function for the signature s^the public key e, modulo n
	mpz_t verify;
	mpz_init(verify);
	pow_mod(verify, s, e, n);
	// return true if the verify variable is the same as the plain text
	if (mpz_cmp(verify, m) == 0){
		mpz_clear(verify);
		return true;
	}
	// return false if the signature can't be verified
	mpz_clear(verify);
	return false;
}

/*
int main(void){
	mpz_t p, q, n, e;
	uint64_t iters = 50;
	uint64_t nbits = 10;
	mpz_inits(p, q, n, e, NULL);
	rsa_make_pub(p, q, n, e, nbits, iters);
	gmp_printf("%Zu\n", e);
}
*/
