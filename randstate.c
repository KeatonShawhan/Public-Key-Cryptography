#include <stdint.h>
#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>

// initialize the global state variable
extern gmp_randstate_t state;
gmp_randstate_t state;

// create the randstate_init function to set the seed
void randstate_init(uint64_t seed){
	// seeds the seed desired
	srandom(seed);
	// initialize the state inside the function
	gmp_randinit_mt(state);
	// sets the state and seed together
	gmp_randseed_ui(state, seed);
}

// create the randstate_clear function
void randstate_clear(void){
	// clear all the memory used by the global variable "state"
	gmp_randclear(state);
}
