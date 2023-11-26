#pragma once
#ifndef MODULAR_H
#define MODULAR_H

#include <boost/multiprecision/cpp_int.hpp>

namespace quest {

// TODO move this somewhere else
template<typename> struct double_width; // undefined
template<> struct double_width<uint32_t> { using type = uint64_t; };
template<> struct double_width<uint64_t> { using type = boost::multiprecision::uint128_t; };
template<> struct double_width<boost::multiprecision::cpp_int> { using type = boost::multiprecision::cpp_int; };
template<class T> using dw_t = typename double_width<T>::type;

// Compute a * b % m, without any overflows. 
template<typename T>
T mulmod(T a, T b, T m) {
	dw_t<T> dwa = a, dwb = b, dwm = m;
	return T{(dwa * dwb) % dwm};
}

// Compute b^e % m. Unsafe w.r.t. overflows
template<typename T>
T expmod_unsafe(T b, T e, T m) {
	if(e == 0) return 1;
	if(e % 2 == 1) return (b * expmod_unsafe(b, e-1, m)) % m;
	T half = expmod_unsafe(b, e/2, m);
	return (half * half) % m;
}
// Compute b^e % m. Safe w.r.t. overflows
template<typename T>
T expmod_safe(T b, T e, T m) {
	return T{expmod_unsafe<dw_t<T>>(b, e, m)};
}

} // end namespace quest

#endif
