#pragma once
#ifndef OEIS_H
#define OEIS_H

#include <fstream>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/multiprecision/cpp_int.hpp>
using int_mpt = boost::multiprecision::cpp_int;

#include "sequence.h"

namespace quest {

// Specialization of quest::vector_sequence for oeis example terms
class oeis_sequence : public vector_sequence<int_mpt> {
IMPLEMENTS_SEQUENCE_CLONE(oeis_sequence, int_mpt)
public:
	oeis_sequence(int sequence_number) : vector_sequence<int_mpt>(get_sequence_list(sequence_number)) {}
	oeis_sequence(std::string sequence_name) : oeis_sequence(name_to_number(sequence_name)) {}
private:
	static std::vector<int_mpt> get_sequence_list(int sequence_number) {
		std::ifstream oeis("oeis");
		std::string line;
		if(!oeis.is_open()) throw std::runtime_error("file 'oeis' not found.");
		for(int i = 0; i < sequence_number + 4; ++i) std::getline(oeis, line);
		std::vector<std::string> string_terms;
		boost::split(string_terms, line, [](char c) { return c == ','; });
		std::vector<int_mpt> terms;
		for(int i = 1; i < string_terms.size() - 1; ++i) {
			terms.push_back(int_mpt(string_terms[i]));
		}
		return terms;
	}
	static int name_to_number(std::string sequence_name) {
		return std::stoi(sequence_name.substr(1));
	}
};

} // end namespace quest

#endif
