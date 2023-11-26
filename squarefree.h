#pragma once
#ifndef SQUAREFREE_H
#define SQUAREFREE_H

#include <math.h>
#include <vector>
#include <algorithm>

namespace quest {

// counts squarefree numbers of signed numeric type T
// Constructor is O(sqrt(N)log(log(sqrt(N)))).
// num_sf_below is O(sqrt(x)).
template<typename T>
class squarefree_counter {
public:
	// mandatory constructor requires a limit
	squarefree_counter(T limit) : LIMIT(limit), SQRT_LIMIT(isqrt(limit)) {
		static_assert(std::is_signed<T>::value, "squarefree_counter<T> must have signed type T.");
		// Compute \mu(x) for 1 <= x <= sqrt(limit).
		T quartic_rt_limit = isqrt(SQRT_LIMIT);
		mu.resize(SQRT_LIMIT + 1);
		std::fill(mu.begin() + 1, mu.end(), 1);
		for(T i = 2; i <= quartic_rt_limit; ++i) {
			if(mu[i] != 1) continue;
			for(T j = i; j <= SQRT_LIMIT; j += i) {
				mu[j] *= -i;
			}
			T isqr = i*i;
			for(T j = isqr; j <= SQRT_LIMIT; j += isqr) {
				mu[j] = 0;
			}
		}
		for(T i = 2; i <= SQRT_LIMIT; ++i) {
			if(mu[i] == i) mu[i] = 1;
			else if(mu[i] == -i) mu[i] = -1;
			else if(mu[i] < 0) mu[i] = 1;
			else if(mu[i] > 0) mu[i] = -1;
		}
	}
	T num_sf_below(T x) const {
		if(x > LIMIT) throw std::out_of_range("call to squarefree_counter<T>::num_sf_below with argument x > LIMIT (" + std::to_string(x) + " > " + std::to_string(LIMIT) + ").");
		T sf = 0;
		for(T d = 1; d <= isqrt(x); ++d) {
			sf += mu[d] * (x / (d * d));
		}
		return sf;
	}
private:
	const T LIMIT;
	const T SQRT_LIMIT;
	std::vector<T> mu;
	
static constexpr auto isqrt = [](T x) { return static_cast<T>(sqrt(x)); };
};

} // end namespace quest

#endif
