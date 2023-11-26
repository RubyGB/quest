#include "doctest.h"
#include "../sequence.h"
#include "../oeis.h"
#include "../squarefree.h"

#include <iostream>

class squarefree_counter_sequence : public quest::sequence<int64_t> {
IMPLEMENTS_SEQUENCE_CLONE(squarefree_counter_sequence, int64_t)
protected:
	int64_t term() override {
		return sfc.num_sf_below(current++);
	}
private:
	int64_t current = 0;
	quest::squarefree_counter<int64_t> sfc = quest::squarefree_counter<int64_t>(1000000);
};

class squarefree_counter_cache_sequence : public quest::sequence<int64_t> {
IMPLEMENTS_SEQUENCE_CLONE(squarefree_counter_cache_sequence, int64_t)
protected:
	int64_t term() override {
		if(latest_sqrt * latest_sqrt == current) {
			squares.push_back(current);
			++latest_sqrt;
		}
		if(is_squarefree(current)) { ++num_squarefree; }
		++current;
		return num_squarefree;
	}
private:
	int64_t current = 0, num_squarefree = 0, latest_sqrt = 2;
	std::vector<int64_t> squares;
	
	bool is_squarefree(int64_t x) const {
		if(x == 0) return false;
		for(auto s2 : squares) {
			if(x % s2 == 0) return false;
		}
		return true;
	}
};

TEST_CASE("A013928 / Number of squarefree numbers up to n") {
	quest::oeis_sequence oeis(13928);
	squarefree_counter_sequence sfc_sequence;
	squarefree_counter_cache_sequence sfc_cache;
	sfc_sequence.limit.max_abs_value = 1000000;
	
	//quest::print_sequence_differences(sfc_sequence, sfc_cache, std::cout);
	//std::cout << quest::verify_sequences(sfc_sequence, sfc_cache) << std::endl;
	CHECK(quest::sequences_agree(oeis, sfc_sequence));
	CHECK(quest::sequences_agree(sfc_sequence, sfc_cache));

}
