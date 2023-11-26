#include "doctest.h"
#include "../sequence.h"
#include "../oeis.h"
#include "../prime.h"

//#include <iostream>

class prime_sequence_naive : public quest::sequence<int> {
IMPLEMENTS_SEQUENCE_CLONE(prime_sequence_naive, int)
protected:
	int term() override {
		while(!is_prime_naive(++current_prime)); 
		return current_prime;
	}
private:
	int current_prime = 1;
	bool is_prime_naive(int x) const {
		for(int d = 2; d <= x/2; ++d) {
			if(x % d == 0) return false;
		}
		return true;
	}
};

TEST_CASE("A000040 / Prime numbers") {
	quest::oeis_sequence oeis(40);
	quest::primes<uint64_t> prime_sequence;
	prime_sequence_naive naive;
	quest::vector_sequence<long long> wrong_sequence_1({2, 3, 5, 7, 9, 11, 13, 17});
	
	CHECK(quest::sequences_agree(oeis, prime_sequence));
	CHECK(quest::sequences_agree(prime_sequence, naive));
	CHECK(!quest::sequences_agree(prime_sequence, wrong_sequence_1));
}
