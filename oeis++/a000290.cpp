#include "doctest.h"
#include "../sequence.h"
#include "../oeis.h"

//#include <iostream>

// Generates squares up to N in O(sqrt N) time.
class square_sequence_direct : public quest::sequence<int> {
IMPLEMENTS_SEQUENCE_CLONE(square_sequence_direct, int)
protected:
	int term() override {
		return get_index() * get_index();
	}
};
bool is_square(int x) {
	int sqrtx = int(sqrt(x));
	return x == sqrtx * sqrtx;
}
// Generates squares up to N in O(N) time.
std::vector<int> get_squares_to_limit(int limit) {
	std::vector<int> squares;
	for(int x = 0; x <= limit; ++x) {
		if(!is_square(x)) continue;
		squares.push_back(x);
	}
	return squares;
}

TEST_CASE("A000290 / Square numbers") {
	square_sequence_direct direct_sequence;
	quest::oeis_sequence oeis(290);
	quest::vector_sequence<int> isqrt_sequence(get_squares_to_limit(10000));
	quest::vector_sequence<long long> wrong_sequence_1({0, 1, 4, 8, 16, 25});

	CHECK(quest::sequences_agree(oeis, direct_sequence));
	CHECK(quest::sequences_agree(direct_sequence, isqrt_sequence));
	CHECK(!quest::sequences_agree(isqrt_sequence, wrong_sequence_1));
}
