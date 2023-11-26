#pragma once
#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
using simple_duration = std::chrono::duration<double, std::ratio<1, 1000>>;
const simple_duration DEFAULT_TIMEOUT(1);

#include <boost/optional.hpp>

namespace quest {

template<typename T>
boost::optional<T> min(boost::optional<T> x, boost::optional<T> y) {
	if(!x && !y) return boost::none;
	if(!x && y) return y;
	if(x && !y) return x;
	return x < y ? x : y;
}

// This macro should be written as the first line in the class definition of classes extending sequence<T> to implement clone(). See vector_sequence<T> for an example.
#define IMPLEMENTS_SEQUENCE_CLONE(DERIVED_TYPE, ENTRY_TYPE) public: virtual std::unique_ptr<sequence<ENTRY_TYPE>> clone() const { return std::make_unique<DERIVED_TYPE>(static_cast<const DERIVED_TYPE &>(*this)); }

// Template class for lazy-evaluated infinite sequences.
// This base class represents the 'zero sequence' (i.e. infinitely many default-constructed T{} )
// Extending classes should override sequence::term() and, if they run out of elements, set this->done = true
template<typename T>
class sequence {
IMPLEMENTS_SEQUENCE_CLONE(sequence<T>, T)
public:
	struct entry {
		size_t index;
		boost::optional<T> value;
	};
	struct verification_limit {
		boost::optional<T> max_abs_value;
		boost::optional<size_t> max_index;
		boost::optional<simple_duration> timeout;
	};

	boost::optional<T> next_term() {
		boost::optional<T> next = boost::none;
		if(!done) next = term();
		++index;
		return next;
	}
	entry next_entry() {
		return {get_index(), next_term()};
	}
	
	// index is that of the 'current term' i.e., next to be generated
	size_t get_index() const { return index; }
	bool is_empty() const { return done; }
	
	// Logical limits (e.g. for sequence verification).
	// Static limits can be set to apply to all sequences of type T.
	verification_limit limit;
	static inline verification_limit s_limit;

	boost::optional<T> effective_max_abs_value() const {
		return min(limit.max_abs_value, s_limit.max_abs_value);
	}
	boost::optional<size_t> effective_max_index() const {
		return min(limit.max_index, s_limit.max_index);
	}
	
	void add_computing_time(simple_duration time_spent) {
		total_computing_time += time_spent;
	}
	bool is_timed_out() const {
		auto to = min(limit.timeout, s_limit.timeout);
		simple_duration effective_timeout = to ? *to : DEFAULT_TIMEOUT;
		return total_computing_time >= effective_timeout;
	}
protected:
	bool done = false;
	virtual T term() { return T{}; }
private:
	size_t index = 0;
	simple_duration total_computing_time {};
};

// Specialization of sequence<T> for creating sequences from vectors
template<typename T>
class vector_sequence : public sequence<T> {
IMPLEMENTS_SEQUENCE_CLONE(vector_sequence<T>, T)
public:
	vector_sequence(std::vector<T> vals) : vals(vals) {
		if(vals.size() == 0) this->done = true;
	}
protected:
	virtual T term() override {
		if(this->get_index() == vals.size() - 1) this->done = true;
		return vals[this->get_index()];
	}
private:
	std::vector<T> vals;
};

enum class verification_side { BOTH = 0, LEFT, RIGHT };
enum class verification_status {
	DISAGREE = 0, AGREE_TO_VALUE, AGREE_TO_INDEX, TIMEOUT, AGREE_TO_EMPTY
};
const std::string VERIFICATION_SIDE_NAMES[] {"BOTH", "LEFT", "RIGHT"};
const std::string VERIFICATION_STATUS_NAMES[] {"DISAGREE", "AGREE_TO_VALUE", "AGREE_TO_INDEX", "TIMEOUT", "AGREE_TO_EMPTY"};
template<typename S, typename T>
struct verification {
	verification_side which_finished;
	verification_status status;
	typename sequence<S>::entry left;
	typename sequence<T>::entry right;
	
	friend std::ostream &operator <<(std::ostream &os, const verification &v) {
		os << VERIFICATION_SIDE_NAMES[static_cast<size_t>(v.which_finished)] << " " << VERIFICATION_STATUS_NAMES[static_cast<size_t>(v.status)] << ": A[" << v.left.index << "] = ";
		if(v.left.value) os << *(v.left.value);
		else os << "_";
		os << ", B[" << v.right.index << "] = ";
		if(v.right.value) os << *(v.right.value);
		else os << "_";
		return os;
	}
};

namespace sequence_impl {

template<typename S, typename T>
verification<S, T> verify_and_modify_sequences(sequence<S> &left, sequence<T> &right) {
	using namespace std::chrono;
	boost::optional<S> max_value_left = left.effective_max_abs_value();
	boost::optional<T> max_value_right = right.effective_max_abs_value();
	boost::optional<size_t> max_index = min(left.effective_max_index(), right.effective_max_index());
	auto exceeds_mav_left = [&](S x) { return max_value_left && (*max_value_left < x || *max_value_left < -x); };
	auto exceeds_mav_right = [&](T x) { return max_value_right && (*max_value_right < x || *max_value_right < -x); };
	auto exceeds_max_index = [&]() { return max_index && (left.get_index() > max_index || right.get_index() > max_index); };
	steady_clock::time_point timer = steady_clock::now();
	auto get_computing_time = [&]() { return duration_cast<simple_duration>(steady_clock::now() - timer); };
	auto sl = left.next_entry(), sr = right.next_entry();
	left.add_computing_time(get_computing_time());
	right.add_computing_time(get_computing_time());
	while(!(left.is_timed_out() || right.is_timed_out())) {
		if(!sl.value || !sr.value) {
			auto which_fin = verification_side::BOTH;
			if(sr.value) which_fin = verification_side::LEFT;
			if(sl.value) which_fin = verification_side::RIGHT;
			return {which_fin, verification_status::AGREE_TO_EMPTY, sl, sr};
		}
		if(exceeds_mav_left(*sl.value)) {
			return {verification_side::LEFT, verification_status::AGREE_TO_VALUE, sl, sr};
		}
		if(exceeds_mav_right(*sr.value)) {
			return {verification_side::RIGHT, verification_status::AGREE_TO_VALUE, sl, sr};
		}
		if(*sl.value != *sr.value) {
			return {verification_side::BOTH, verification_status::DISAGREE, sl, sr};
		}
		timer = steady_clock::now();
		sl = left.next_entry();
		sr = right.next_entry();
		left.add_computing_time(get_computing_time());
		right.add_computing_time(get_computing_time());
	}
	return {verification_side::BOTH, verification_status::TIMEOUT, sl, sr};
}

} // end namespace quest::sequence_impl

template<typename S, typename T>
verification<S, T> verify_sequences(const sequence<S> &left, const sequence<T> &right) {
	return sequence_impl::verify_and_modify_sequences(*left.clone(), *right.clone());
}

template<typename S, typename T>
bool sequences_agree(const sequence<S> &left, const sequence<T> &right) {
	return verify_sequences(left, right).status != verification_status::DISAGREE;
}

template<typename S, typename T>
void print_sequence_differences(const sequence<S> &left, const sequence<T> &right, std::ostream &os) {
	auto cleft = left.clone(), cright = right.clone();
	auto get_verification = [&]() { return sequence_impl::verify_and_modify_sequences(*cleft, *cright); };
	verification<S, T> ver;
	for(ver = get_verification(); ver.status == verification_status::DISAGREE; ver = get_verification()) {
		os << ver << std::endl;
	}
	os << ver << std::endl;
}

} // end namespace quest

#endif
