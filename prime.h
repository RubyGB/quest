#pragma once
#ifndef PRIME_H
#define PRIME_H

#include "sequence.h"
#include "modular.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <numeric>

namespace quest {

// TODO: Provide asymptotic time-complexity of primes<T>

// template class to generate primes of numeric type T
// T must support the operators T+T, T*T, ++T, T++
// It must be legal to assign 2 to T
template<typename T>
class primes : public sequence<T> {
IMPLEMENTS_SEQUENCE_CLONE(primes<T>, T)
protected:
	T term() override {
		if(D.count(q) == 0) {
			D[q*q] = {q};
			return q++;
		}
		// q is composite, D[q] is the list of primes that divide it
		for(T p : D[q]) {
			T ppq = p+q;
			if(D.count(ppq) == 0) D[ppq] = {};
			D[ppq].push_back(p);
		}
		D.erase(q);
		++q;
		return term();
	}
private:
	T q = 2;
	std::unordered_map<T, std::vector<T>> D;
};

// Returns true if x is a strong probable prime to base b (Miller Rabin).
// It is a logical error to call this function with even x <= 2, otherwise even x, or with a non-coprime base b.
template<typename T>
bool is_sprp_to_base(T x, T b) {
	if(x <= 2) throw std::runtime_error("Call to quest::is_sprp_to_base() with x <= 2");
	if(x % 2 == 0) throw std::runtime_error("Call to quest::is_sprp_to_base() with odd x");
	if(std::gcd(x, b) != 1) throw std::runtime_error("Call to quest::is_sprp_to_base() with non-coprime base");
	T d = x - 1;
	size_t s = 0;
	while(d % 2 == 0) {
		++s;
		d >>= 1;
	}
	T bpow = expmod_safe(b, d, x);
	if(bpow == 1) return true;
	for(size_t r = 0; r < s; ++r) {
		if(bpow == x - 1) return true;
		bpow = (bpow * bpow) % x;
	}
	return false;
}

// deterministic Miller-Rabin test up to 2^32
bool is_prime_32(uint32_t n) {
	if(n <= 61) return (n == 2 || n == 3 || n == 5 || n == 7 || n == 11 || n == 13 || n == 17 || n == 19 || n == 23 || n == 29 || n == 31 || n == 37 || n == 41 || n == 43 || n == 47 || n == 53 || n == 59);
	if(n % 2 == 0 || n % 7 == 0 || n % 61 == 0) return false;
	if(!is_sprp_to_base<uint32_t>(n, 2)) return false;
	if(!is_sprp_to_base<uint32_t>(n, 7)) return false;
	if(!is_sprp_to_base<uint32_t>(n, 61)) return false;
	return true;
}

// deterministic Miller-Rabin test up to 2^64
bool is_prime_64(uint64_t n) {
	if(n <= 37) return (n == 2 || n == 3 || n == 5 || n == 7 || n == 11 || n == 13 || n == 17 || n == 19 || n == 23 || n == 29 || n == 31 || n == 37);
	if(n % 2 == 0 || n % 3 == 0 || n % 5 == 0 || n % 7 == 0 || n % 11 == 0 || n % 13 == 0 || n % 17 == 0 || n % 19 == 0 || n % 23 == 0 || n % 29 == 0 || n % 31 == 0 || n % 37 == 0) return false;
	if(!is_sprp_to_base<uint64_t>(n, 2)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 3)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 5)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 7)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 11)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 13)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 17)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 19)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 23)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 29)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 31)) return false;
	if(!is_sprp_to_base<uint64_t>(n, 37)) return false;
	return true;
}

// Returns the first prime p > n.
uint32_t next_prime_32(uint32_t n) {
	while(!is_prime_32(++n));
	return n;
}
uint64_t next_prime_64(uint64_t n) {
	while(!is_prime_64(++n));
	return n;
}

} // end namespace quest

#endif
