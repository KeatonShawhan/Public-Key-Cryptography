#include <stdio.h>
#include <gmp.h>
#include <stdbool.h>
#include <inttypes.h>
//#include "rsa.h"
#include "randstate.h"
#include <stdlib.h>

// initialize the state to set the seed in other functions
//extern gmp_randstate_t state;
//gmp_randstate_t state;

/*
void lg(mpz_t n){
	mpz_t k;
	mpz_t q;
	mpz_t two;
	mpz_t one;
	mpz_init(k);
	mpz_init(two);
	mpz_init(one);
	mpz_set_si(one, 1);
	//gmp_printf("%Zu\n", one);
	mpz_set_si(two, 2);
	mpz_set_si(k, -1);
	mpz_abs(n, n);
	while (mpz_cmp_si(n, 0) > 0){
		mpz_fdiv_q(n, n, two);
		mpz_add(k, k, one);
	}
	gmp_printf("%Zu\n", k);
	
}
*/

// create gcd function to find the greatest common denominator
void gcd(mpz_t d, mpz_t a, mpz_t b){
	// initialize a temp variable
	mpz_t t, temp_a, temp_b;
        mpz_inits(t, NULL);
	mpz_init_set(temp_a, a);
	mpz_init_set(temp_b, b);
	// loop while b is less than or greater than 0
	while (mpz_cmp_si(temp_b, 0) != 0){
		// set the temp variable to b
		mpz_set(t, temp_b);
		// set b to the value of a modulo b
		mpz_mod(temp_b, temp_a, temp_b);
		// set a to the temp variable
		mpz_set(temp_a, t);
	}
	// update the output variable d
	mpz_set(d, temp_a);
	// clear the temp variable
	mpz_clears(t, temp_a, temp_b, NULL);
}

// create the pow_mod function, passing in an output variable, a base, the exponent, and a modulus variable
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus){
	// initialize temp variables v and p
	mpz_t v, p, exp_temp;
	// set v to 1
	mpz_init_set_si(v, 1);
	// set p to the value of the base variable passed in
	mpz_init_set(p, base);
	mpz_init_set(exp_temp, exponent);
	// loop while the exponent variable is greater than 0
	while (mpz_cmp_si(exp_temp, 0) > 0){
		// check if the exponent variable is even
		if (mpz_odd_p(exp_temp) != 0){
			// multiply (v*p), settng v to the value
			mpz_mul(v,v,p);
			// calculate v % modulus, setting v to the value
			mpz_mod(v, v, modulus);
		}
		// square p, setting p to the value
		mpz_mul(p,p,p);
		// set p to the value to (p % modulus)
		mpz_mod(p, p, modulus);
		// floor div the exponent variable by 2
		mpz_fdiv_q_ui(exp_temp, exp_temp, 2);
	}
	// set the output variable to v so v can be cleared
	mpz_set(out, v);
	// clear temp variables
	mpz_clears(v, p, exp_temp, NULL);
}

// create is_prime function to check if the number n is prime or not
bool is_prime(mpz_t n, uint64_t iters){
	// edge cases for 1, 2, and 3
	if (mpz_cmp_si(n, 1) == 0 || mpz_cmp_si(n, 2) == 0 || mpz_cmp_si(n, 3) == 0){
		return true;
	}
	// create a uint64_t version of the s counter in future while loop
	uint64_t se;
	se = 0;
	// initialize all temp variables
	mpz_t r, s, one, sub, rand, upper, y, j, two, temps;
	mpz_init(j);
	// set sub to the n
	mpz_init_set(sub, n);
	// set variables for constant values
	mpz_init_set_si(two, 2);
	mpz_init_set_si(one, 1);
	// set other temp variables to 0
	mpz_inits(r, s, rand, upper, y, temps, NULL);
	// set sub to n-1
	mpz_sub_ui(sub, n, 1);
	// loop while r is even
	while (mpz_even_p(r)){
		// set r to (1*2^se)
		mpz_mul_2exp(r, one, se);
		// floor divion n-1 by r
		mpz_fdiv_q(r, sub, r);
		// add 1 to the mpz_t version of s
		mpz_add_ui(s, s, 1);
		// add 1 to the uint64_t version of s
		se += 1;
	}
	// subtract 3 from n to change the range of urandomm
	mpz_sub_ui(upper, n, 3);
	// loop from 1 to iters
	for (uint64_t i = 1; i <= iters; i += 1){
		// find a random value from 2 - n-2
		mpz_urandomm(rand, state, upper);
		mpz_add_ui(rand, rand, 2);
		// call pow_mod for the random number, r, and n. set the value to y
		pow_mod(y, rand, r, n);
		// check if y doesnt equal 1 and y doesnt equal n-1
		if (mpz_cmp_si(y, 1) != 0 && mpz_cmp(y, sub) != 0){
			// if so, set a temp variable j to 1
			mpz_set_si(j, 1);
			// subtract 1 from the original s, setting it to a new temps
			mpz_sub_ui(temps, s, 1);
			// loop while j is less than or equal to s-1 and y doesnt equal n-1
			while ((mpz_cmp(j, temps) <= 0) && (mpz_cmp(y, sub) != 0)){
				// call pow_mod for the earlier y, a constant of 2, and the original n passed in. Set the output to y
				pow_mod(y, y, two, n);
				// check if y equals 1
				if (mpz_cmp_si(y, 1) == 0){
					// if so, clear all the temp variables and return false
					mpz_clears(r, s, one, sub, rand, upper, y, j, two, temps, NULL); 
					return false;
				}
				// if not, add 1 to j
				mpz_add_ui(j, j, 1);
			}
			// check if y doesnt equal n-1
			if (mpz_cmp(y, sub) != 0){
				// if so, clear all the temp variables and return false
				mpz_clears(r, s, one, sub, rand, upper, y, j, two, temps, NULL);
				return false;
			}
		}
	}
	// after the for loop runs, clear all the temp variables except j and return true
	mpz_clears(j, r, s, one, sub, rand, upper, y, two, temps, NULL);
	return true;
}	


// create mod_inverse function, computing the inverse i of a modulo n 
void mod_inverse(mpz_t i, mpz_t a, mpz_t n){
	// initialize the temporary variables to not change the values passed in
	mpz_t r, rprime, t, tprime, q, rtemp, ttemp;
	// set some temporary variables to 0
	mpz_inits(q, rtemp, ttemp, NULL);
	// set r to a copy of n
	mpz_init_set(r, n);
	// set rprime to a
	mpz_init_set(rprime, a);
	// set t to 0
	mpz_init_set_ui(t, 0);
	// set tprime to 1
	mpz_init_set_ui(tprime, 1);
	// loop while rprime doesnt equal 0
	while (mpz_cmp_ui(rprime, 0) != 0){
		// floor div r and rprime to find the q
		mpz_fdiv_q(q, r, rprime);
		// set rtemp to r so both r equations can be performed with original values
		mpz_set(rtemp, r);
		// set r to rprime
		mpz_set(r, rprime);
		// multiply (q*rprime), set to rprime
		mpz_mul(rprime, q, rprime);
		// subtract rprime from rtemp (r's value), set to rprime
		mpz_sub(rprime, rtemp, rprime);
		// set ttemp to r so both t equations can be performed with original values
		mpz_set(ttemp, t);
		// set t to tprime
                mpz_set(t, tprime);
		// multiply (q*tprime), set to tprime
                mpz_mul(tprime, q, tprime);
		// subtract tprime from ttemp, set to tprime
                mpz_sub(tprime, ttemp, tprime);
	}
	// check if r is greater than 1
	if (mpz_cmp_ui(r, 1) > 0){
		// if so, set i to 0
		mpz_set_ui(i, 0);
		// clear the temporary variables and return nothing because function type is void
		mpz_clears(r, rprime, t, tprime, q, rtemp, ttemp, NULL);	
		return;
	}
	// check if t is less than 0
	if (mpz_cmp_ui(t, 0) < 0){
		// add n to t
		mpz_add(t, t, n);
	}
	// set output variable to t
	mpz_set(i, t);
	// clear temporary variables
	mpz_clears(r, rprime, t, tprime, q, rtemp, ttemp, NULL);
}

// create make_prime function, passing in a certain amount of bits and iterations, setting p to output value
void make_prime(mpz_t p, uint64_t bits, uint64_t iters){
	// initialize random variable to store the value produced later
        mpz_t rand;
        mpz_init(rand);
	// set a temporary variable tempprime to false to check if the number is prime or not
	//srandom(236);
	/*
	for (uint64_t p = 0; p < bits; p++){
		out = out << 1;
		if (random() % 2 == 1){
			out |= 1;
		}
	}
	*/
	// loop infinitely
	while (true){
		// check if the numnber is prime and the size of the random value is correct
		if ( (mpz_sizeinbase(rand ,2)) < bits || is_prime(rand, iters) == false){
			mpz_urandomb(rand, state, bits);
			// generate the random number, storing in random
                        // set the tempprime boolean to the output of the is_prime, checking if the random value is prime
			// can decrease time complexity by simply subtracting 1 to the random number if its even to make it odd
			if (mpz_even_p(rand) != 0){
				mpz_sub_ui(rand, rand, 1);
        		}
		}else{
			// if so, set the output variable p to the random value and return nothing because the function is void type
			mpz_set(p, rand);
        		mpz_clear(rand);
        		return;
		}
	}
}

/*
int main(void){
	mpz_t p;
	mpz_t n;
	uint64_t bits;
	uint64_t iters;
	mpz_init(p);
	mpz_init_set_si(n, 11);
	bits = 10;
	iters = 50;
	printf("main\n");
	make_prime(p, bits, iters);
	//is_prime(n, iters);
}
*/
