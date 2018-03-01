/*
 * test.cpp
 *
 *  Created on: Jan 19, 2018
 *      Author: erlkoenig
 */

#include <iomanip>
#include <cstdint>
#include <iostream>
#include <vector>
#include <array>
#include <utility>
#include <cstddef>
#include <algorithm>
#include <tuple>
#include <cstdio>
#include <uSer.hh>

static_assert (std::is_same_v<uSer::SWalk::SumRetMaxDyn<uSer::MaxSize<3>, uSer::MaxSize<7>>, uSer::MaxSize<10>>);
static_assert (std::is_same_v<uSer::SWalk::SumRetMaxDyn<uSer::MaxSize<3>, uSer::NoDyn>, uSer::MaxSize<3>>);
static_assert (std::is_same_v<uSer::SWalk::SumRetMaxDyn<uSer::MaxSize<3>, uSer::Unlimited>, uSer::Unlimited>);
static_assert (std::is_same_v<uSer::SWalk::SumRetMaxDyn<uSer::NoDyn, uSer::NoDyn>, uSer::NoDyn>);

#define verify2(cond,file,l) do { if (!(cond)) { std::puts("Assertion \"" #cond "\" failed in " file ":" l "\n"); std::abort ();  }  } while (0)
#define STRINGIFY(x) #x
#define verify(cond) verify2(cond,__FILE__,USER_PPM_EVAL0(STRINGIFY USER_LPAREN __LINE__ USER_RPAREN))

template <typename Raw, typename Data>
struct TestCase {
	Raw raw;
	Data data;
	bool shouldthrow;
	std::size_t line;
	const char* file;
};

template <typename Raw, typename Data>
constexpr TestCase<Raw, Data> tc (const char* file, std::size_t line, bool shouldthrow, Raw&& r, Data&& d) {
	return {std::forward<Raw> (r), std::forward<Data> (d), shouldthrow, line, file};
}

#define TC(...) tc(__FILE__,__LINE__,false,__VA_ARGS__)
#define TCT(...) tc(__FILE__,__LINE__,true,__VA_ARGS__)

using MaxSWord = uint64_t;

struct T1 {
	USER_STRUCT(T1, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	uint32_t a, b, c;

	USER_MEM_ANNOT(a, uSer::ByteOrder::BE)
	USER_MEM_ANNOT(c, uSer::ByteOrder::PDP)

	USER_ENUM_MEM(a, b, c)

	static constexpr uSer::Bit bits = 96;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,12> {{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x23, 0x01, 0x67, 0x45 }},
				T1 {0x01234567, 0xEFCDAB89, 0x01234567 }
			),
			TCT (
				std::array<uint8_t,8> {{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF }},
				T1 {0x01234567, 0xEFCDAB89, 0x01234567 }
			)
		);
	}
};

struct T2 {
	USER_STRUCT(T2, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	uint32_t a, b, c;
	USER_MEM_ANNOT(b, uSer::Width<5>)

	USER_ENUM_MEM(a, b, c)

	static constexpr uSer::Bit bits = 69;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,9> {{ 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0 }},
				T2 { 0xFFFFFFFF, 0, 0 }
			),
			TC (
				std::array<uint8_t,9> {{ 0, 0, 0, 0, 0x1F, 0, 0, 0, 0 }},
				T2 { 0, 0x1F, 0 }
			),
			TC (
				std::array<uint8_t,9> {{ 0, 0, 0, 0, 0x7 << 5, 0xFF, 0xFF, 0xFF, 0x1F }},
				T2 { 0, 0, 0xFFFFFFFF }
			) /*,
			TC (
				std::array<uint8_t,9> {{ 0, 0, 0, 0, 0, 0, 0, 0, 7 << 5 }},
				T2 { 0, 0, 0 }
			) */
		);
	}
};

struct T3 {
	USER_STRUCT(T3, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	uint32_t a;
	USER_MEM(uint32_t, b, uSer::Width<5>)
	uint32_t c;

	USER_ENUM_MEM(a, b, c)

	static constexpr uSer::Bit bits = 69;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,9> {{ 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0 }},
				T3 { 0xFFFFFFFF, 0, 0 }
			),
			TC (
				std::array<uint8_t,9> {{ 0, 0, 0, 0, 0x1F, 0, 0, 0, 0 }},
				T3 { 0, 0x1F, 0 }
			),
			TC (
				std::array<uint8_t,9> {{ 0, 0, 0, 0, 0x7 << 5, 0xFF, 0xFF, 0xFF, 0x1F }},
				T3 { 0, 0, 0xFFFFFFFF }
			)
		);
	}
};

struct T4 {
	USER_STRUCT(T4, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	USER_DEF_MEM(
		(uint32_t,a, uSer::AttrNone),
		(uint32_t,b, uSer::Width<5>),
		(uint32_t,c, uSer::AttrNone)
	)

	static constexpr uSer::Bit bits = 69;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,9> {{ 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0 }},
				T4 { 0xFFFFFFFF, 0, 0 }
			),
			TC (
				std::array<uint8_t,9> {{ 0, 0, 0, 0, 0x1F, 0, 0, 0, 0 }},
				T4 { 0, 0x1F, 0 }
			),
			TC (
				std::array<uint8_t,9> {{ 0, 0, 0, 0, 0x7 << 5, 0xFF, 0xFF, 0xFF, 0x1F }},
				T4 { 0, 0, 0xFFFFFFFF }
			)
		);
	}
};

struct T5 {
	using MaxSWord = ::MaxSWord;

	uint8_t a, b, c;

	static constexpr uSer::Bit bits = 9;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,2> {{ 3, 0 }},
				T5 { 3, 0, 0 }
			),
			TC (
				std::array<uint8_t,2> {{ 3 << 2, 0 }},
				T5 { 0, 3, 0 }
			),
			TC (
				std::array<uint8_t,2> {{ 0xF0, 1 }},
				T5 { 0, 0, 0x1F }
			) /* ,
			TC (
				std::array<uint8_t,2> {{ 0, 0xE }},
				T5 { 0, 0, 0 }
			) */
		);
	}
};
USER_EXT_ANNOT(T5, uSer::AttrNone)
USER_EXT_DEF_MEM(T5,
		(a, uSer::Width<2>),
		(b, uSer::Width<2>),
		(c, uSer::Width<5>)
)

struct T6 {
	using MaxSWord = ::MaxSWord;

	uint8_t a, b, c;

	static constexpr uSer::Bit bits = 15;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,2> {{ 0x1F, 0 }},
				T6 { 0x1F, 0, 0 }
			),
			TC (
				std::array<uint8_t,2> {{ 7 << 5, 0 }},
				T6 { 0, 7, 0 }
			),
			TC (
				std::array<uint8_t,2> {{ 0, 0x7F }},
				T6 { 0, 0, 0x7F }
			) /*,
			TC (
				std::array<uint8_t,2> {{ 0, 0x80 }},
				T6 { 0, 0, 0 }
			) */
		);
	}
};
USER_EXT_ANNOT(T6, uSer::AttrNone)
USER_EXT_MEM_ANNOT(T6, a, uSer::Width<5>)
USER_EXT_MEM_ANNOT(T6, b, uSer::Width<3>)
USER_EXT_MEM_ANNOT(T6, c, uSer::Width<7>)
USER_EXT_ENUM_MEM(T6, a, b, c)

struct T7 {
	USER_STRUCT(T7, uSer::AttrNone)
	using MaxSWord = uint16_t;

	uint8_t a, b, c;
	USER_MEM_ANNOT(a, uSer::Width<5>, uSer::Padding::Fixed<15>)
	USER_MEM_ANNOT(b, uSer::Width<3>)
	USER_MEM_ANNOT(c, uSer::Width<7>)

	USER_ENUM_MEM(a, b, c)
	static constexpr uSer::Bit bits = 30;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,4> {{ 0x1F, 0, 0, 0 }},
				T7 { 0x1F, 0, 0 }
			),
			TC (
				std::array<uint8_t,4> {{ 0, 0, 0x70, 0  }},
				T7 { 0, 7, 0 }
			),
			TC (
				std::array<uint8_t,4> {{ 0, 0, 0x80, 0x3F }},
				T7 { 0, 0, 0x7F }
			) /* ,
			TC (
				std::array<uint8_t,4> {{ 0xE0, 0xFF, 0x0F, 0 }},
				T7 { 0, 0, 0 }
			) */,
			TCT (
				std::array<uint8_t, 2> {{ 0, 0 }},
				T7 { 0, 0, 0 }
			)

		);
	}
};

struct T8 {
	USER_STRUCT(T8, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	uint8_t a, b, c;
	USER_MEM_ANNOT(a, uSer::Width<5>, uSer::Padding::Fixed<3>)
	USER_MEM_ANNOT(b, uSer::Width<3>)
	USER_MEM_ANNOT(c, uSer::Width<7>)

	USER_ENUM_MEM(a, b, c)

	static constexpr uSer::Bit bits = 18;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,3> {{ 0x1F, 0, 0 }},
				T8 { 0x1F, 0, 0 }
			),
			TC (
				std::array<uint8_t,3> {{ 0, 7, 0 }},
				T8 { 0, 7, 0 }
			),
			TC (
				std::array<uint8_t,3> {{ 0, 0xF8, 3 }},
				T8 { 0, 0, 0x7F }
			) /* ,
			TC (
				std::array<uint8_t,3> {{ 0x60, 0, 0 }},
				T7 { 0, 0, 0 }
			) */
		);
	}
};

struct T9 {
	USER_STRUCT(T9, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	enum class E : uint8_t { E1 = 0, E2, E3 };

	std::array<E, 3> e;

	USER_ENUM_MEM(e)

	static constexpr uSer::Bit bits = 24;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,3> {{ 0, 1, 2 }},
				T9 {{{ E::E1, E::E2, E::E3 }}}
			)
		);
	}
};


struct T10 {
	USER_STRUCT(T10, uSer::AttrNone)
	using MaxSWord = uint32_t;

	uint32_t N;
	uint32_t arr [3];
	USER_MEM_ANNOT(arr, uSer::Dyn::Size<&T10::N>)
	uint32_t x;

	USER_ENUM_MEM(N, arr, x)

	static constexpr uSer::Bit bits = 64;
	template <uSer::Bit wSize>
	using MaxDyn = uSer::MaxSize<96 / wSize>;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,20> {{ 3, 0, 0, 0, 0x67, 0x45, 0x23, 0x01, 0x67, 0x45, 0x23, 0x01, 0x67, 0x45, 0x23, 0x01, 0xAA, 0x55, 0xAA, 0x55 }},
				T10 { 3, { 0x01234567, 0x01234567, 0x01234567 }, 0x55AA55AA }
			),
			TC (
				std::array<uint8_t,16> {{ 2, 0, 0, 0, 0x67, 0x45, 0x23, 0x01, 0x67, 0x45, 0x23, 0x01, 0xAA, 0x55, 0xAA, 0x55 }},
				T10 { 2, { 0x01234567, 0x01234567 }, 0x55AA55AA }
			),
			TC (
				std::array<uint8_t,8> {{ 0, 0, 0, 0, 0xAA, 0x55, 0xAA, 0x55 }},
				T10 { 0, {  }, 0x55AA55AA }
			),
			TCT (
				std::array<uint8_t,4> {{ 0, 0, 0, 0 }},
				T10 { 0, {  }, 0x55AA55AA }
			),
			TCT (
				std::array<uint8_t,10> {{ 2, 0, 0, 0, 0xAA, 0x55, 0xAA, 0x55, 0, 0 }},
				T10 { 2, {  }, 0x55AA55AA }
			),
			TCT (
				std::array<uint8_t,8> {{ 100, 0, 0, 0, 0xAA, 0x55, 0xAA, 0x55 }},
				T10 { 100, {}, 42 }
			)
		);
	}
};

struct T11 {
	USER_STRUCT(T11, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	uint8_t N;
	USER_MEM_ANNOT(N, uSer::Width<4>)
	uint8_t arr [3];
	USER_MEM_ANNOT(arr, uSer::Width<5>)

	USER_ENUM_MEM(N, arr)

	static constexpr uSer::Bit bits = 19;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,3> {{ 3 | (5 << 4), 1 | 0x22 | (3 << 6), 1 }},
				T11 { 3, { 21, 17, 7 } }
			)
		);
	}
};

struct T12 {
	USER_STRUCT(T12, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	T11 a [2];

	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 2*19;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,5> {{ 3 | (5 << 4), 1 | 0x22 | (3 << 6), 1 | (14<<3) | (1 << 7), 3 | (8 << 4),  9 << 1 }},
				T12 { { { 3, { 21, 17, 7 } }, { 14, { 7, 8, 9 }} }}
			)
		);
	}
};

struct T13;
uint32_t T13_getSize (const T13& str);

struct T13 {
	USER_STRUCT(T13, uSer::AttrNone)
	using MaxSWord = uint32_t;

	uint32_t N;
	T10 a [3];
	USER_MEM_ANNOT(a, uSer::Dyn::Size<T13_getSize>)
	USER_ENUM_MEM(N, a)

	static constexpr uSer::Bit bits = 32;
	template <uSer::Bit wSize>
	using MaxDyn = uSer::MaxSize<480 / wSize>;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,28> {{ 2, 0, 0, 0, 2, 0, 0, 0, 0x67, 0x45, 0x23, 0x01, 0x10, 0x32, 0x54, 0x76, 0xAA, 0x55, 0xAA, 0x55, 0, 0, 0, 0, 0xEF, 0xBE, 0xAD, 0xDE }},
				T13 { 2, { T10 { 2, { 0x01234567, 0x76543210 }, 0x55AA55AA }, T10 { 0, {}, 0xDEADBEEF } } }
			),
			TCT (
				std::array<uint8_t,28> {{ 2, 0, 0, 0, 2, 0, 0, 0, 0x67, 0x45, 0x23, 0x01, 0x10, 0x32, 0x54, 0x76, 0xAA, 0x55, 0xAA, 0x55, 0, 0, 0, 4, 0xEF, 0xBE, 0xAD, 0xDE }},
				T13 { 2, { T10 { 2, { 0x01234567, 0x76543210 }, 0x55AA55AA }, T10 { 4, {}, 0xDEADBEEF } } }
			),
			TCT (
				std::array<uint8_t,28> {{ 2, 0, 0, 0, 2, 0, 0, 0, 0x67, 0x45, 0x23, 0x01, 0x10, 0x32, 0x54, 0x76, 0xAA, 0x55, 0xAA, 0x55, 2, 0, 0, 0, 0xEF, 0xBE, 0xAD, 0xDE }},
				T13 { 2, { T10 { 2, { 0x01234567, 0x76543210 }, 0x55AA55AA }, T10 { 2, {}, 0xDEADBEEF } } }
			)
		);
	}
};
uint32_t T13_getSize (const T13& str) {
	return str.N;
}

struct T14 {
	USER_STRUCT(T14, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	uint8_t a [4];

	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 32;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,4> {{ 1, 2, 3, 4 }},
				T14 { { 1, 2, 3, 4} }
			)
		);
	}
};

struct T15 {
	USER_STRUCT(T15, uSer::AttrNone)
	using MaxSWord = ::uint8_t;

	std::vector<uint8_t> a;

	T15 (std::initializer_list<uint8_t> il) : a(std::move (il)) {}
	T15 () : a (3) {}

	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 0;
	template <uSer::Bit>
	using MaxDyn = uSer::Unlimited;
	static auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,3> {{ 1, 2, 3 }},
				T15 { { 1, 2, 3 } }
			)
		);
	}
};

struct T16 {
	USER_STRUCT(T16, uSer::AttrNone)
	using MaxSWord = ::uint8_t;

	uint8_t N;
	std::vector<uint8_t> a;

	T16 (uint8_t n, std::initializer_list<uint8_t> il) : N(n), a(std::move (il)) {}
	T16 () : N(0), a (3) {}

	USER_ENUM_MEM(N, a)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::Unlimited;
	static auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,4> {{ 3, 1, 2, 3 }},
				T16 {3, { 1, 2, 3 } }
			)
		);
	}
};

struct T17 {
	USER_STRUCT(T17, uSer::AttrNone)
	using MaxSWord = ::uint8_t;

	uint8_t N;
	std::unique_ptr<uint8_t[]> a;
	USER_MEM_ANNOT(a, uSer::Dyn::Size<&T17::N>)

	T17 (uint8_t n, uint8_t a1, uint8_t a2, uint8_t a3) : N(n), a(new uint8_t [3] { a1, a2, a3 }) {}
	T17 () : N(0), a (new uint8_t [3]) {}

	USER_ENUM_MEM(N, a)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::Unlimited;
	static auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,4> {{ 3, 1, 2, 3 }},
				T17 {3, 1, 2, 3 }
			)
		);
	}
};

struct T20 {
	USER_STRUCT(T20, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	int32_t a, b, c;

	USER_MEM_ANNOT(a, uSer::SignFormat::TwosComplement)
	USER_MEM_ANNOT(b, uSer::SignFormat::SignedMagnitude)
	USER_MEM_ANNOT(c, uSer::SignFormat::OnesComplement)

	USER_ENUM_MEM(a, b, c)

	static constexpr uSer::Bit bits = 96;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,12> {{ 0x67, 0x45, 0x23, 0x01, 0x67, 0x45, 0x23, 0x01, 0x67, 0x45, 0x23, 0x01}},
				T20 {0x01234567, 0x01234567, 0x01234567 }
			),
			TC (
				std::array<uint8_t,12> {{ 0x99, 0xBA, 0xDC, 0xFE, 0x67, 0x45, 0x23, 0x81, 0x98, 0xBA, 0xDC, 0xFE }},
				T20 {-0x01234567, -0x01234567, -0x01234567 }
			),
			TC (
				std::array<uint8_t,12> {{ 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F }},
				T20 { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF }
			),
			TC (
				std::array<uint8_t,12> {{ 0, 0, 0, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0x80 }},
				T20 { -2147483648, -0x7FFFFFFF, -0x7FFFFFFF }
			),
			TC (
				std::array<uint8_t,12> {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }},
				T20 { 0, 0, 0 }
			) /*,
			TC (	// Check negative Zero - works only for deserialization
				std::array<uint8_t,12> {{ 0, 0, 0, 0, 0, 0, 0, 0x80, 0xFF, 0xFF, 0xFF, 0xFF }},
				T20 { 0, 0, 0 }
			) */
		);
	}
};

struct T21 {
	USER_STRUCT(T21, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	int32_t a, b, c;

	USER_MEM_ANNOT(a, uSer::ByteOrder::BE, uSer::SignFormat::TwosComplement)
	USER_MEM_ANNOT(b, uSer::ByteOrder::BE, uSer::SignFormat::SignedMagnitude)
	USER_MEM_ANNOT(c, uSer::ByteOrder::BE, uSer::SignFormat::OnesComplement)

	USER_ENUM_MEM(a, b, c)

	static constexpr uSer::Bit bits = 96;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,12> {{ 0x01, 0x23, 0x45, 0x67, 0x01, 0x23, 0x45, 0x67, 0x01, 0x23, 0x45, 0x67 }},
				T21 {0x01234567, 0x01234567, 0x01234567 }
			),
			TC (
				std::array<uint8_t,12> {{ 0xFE, 0xDC, 0xBA, 0x99, 0x81, 0x23, 0x45, 0x67, 0xFE, 0xDC, 0xBA, 0x98 }},
				T21 {-0x01234567, -0x01234567, -0x01234567 }
			),
			TC (
				std::array<uint8_t,12> {{ 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF }},
				T21 { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF }
			),
			TC (
				std::array<uint8_t,12> {{ 0x80, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0, 0, 0 }},
				T21 { -2147483648, -0x7FFFFFFF, -0x7FFFFFFF }
			),
			TC (
				std::array<uint8_t,12> {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }},
				T21 { 0, 0, 0 }
			) /*,
			TC (	// Check negative Zero - works only for deserialization, as serialization always produces positive zeros
				std::array<uint8_t,12> {{ 0, 0, 0, 0, 0x80, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF }},
				T21 { 0, 0, 0 }
			) */
		);
	}
};

struct T22 {
	USER_STRUCT(T22, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	int8_t a;
	int16_t b;
	int32_t c;

	USER_MEM_ANNOT(a, uSer::SignFormat::TwosComplement, uSer::Width<8>)
	USER_MEM_ANNOT(b, uSer::SignFormat::SignedMagnitude, uSer::Width<8>)
	USER_MEM_ANNOT(c, uSer::SignFormat::OnesComplement, uSer::Width<8>)

	USER_ENUM_MEM(a, b, c)

	static constexpr uSer::Bit bits = 24;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple(
			TC (
				std::array<uint8_t,3> {{ 0x2A, 0x2A, 0x2A }},
				T22 {42, 42, 42 }
			),
			TC (
				std::array<uint8_t,3> {{ 0xD6, 0xAA, 0xD5 }},
				T22 {-42, -42, -42 }
			),
			TC (
				std::array<uint8_t,3> {{ 0x7F, 0x7F, 0x7F }},
				T22 { 127, 127, 127 }
			),
			TC (
				std::array<uint8_t,3> {{ 0x80, 0xFF, 0x80 }},
				T22 { -128, -127, -127 }
			),
			TC (
				std::array<uint8_t,3> {{ 0, 0, 0 }},
				T22 { 0, 0, 0 }
			) /*,
			TC (	// Check negative Zero - works only for deserialization
				std::array<uint8_t,12> {{ 0, 0, 0, 0, 0, 0, 0, 0x80, 0xFF, 0xFF, 0xFF, 0xFF }},
				T20 { 0, 0, 0 }
			) */
		);
	}
};

struct T30;
struct T30_Functional {
	bool operator () (const T30& str);
} t30_Functional;

struct T30 {
	USER_STRUCT(T30, uSer::AttrNone)
	using MaxSWord = uint32_t;

	bool flag;
	uint32_t data;
	uint32_t x;

	USER_MEM_ANNOT(data, uSer::Dyn::Optional<&t30_Functional>)

	USER_ENUM_MEM(flag, data, x)

	static constexpr uSer::Bit bits = 33;
	template <uSer::Bit wSize>
	using MaxDyn = uSer::MaxSize<32 / wSize>;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,5> {{0xFE,0xFF,0xFF,0xFF,1}},
					T30 { 0, 0, 0xFFFFFFFF }
				),
				TC (
					std::array<uint8_t, 9> {{ 1, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 1 }},
					T30 { 1, 0x80000000, 0xFFFFFFFF }
				),
				TCT (
					std::array<uint8_t, 8> {{ 1 }},
					T30 { 1, 0x01234567, 0xFFFFFFFF }
				)
			);
	}
};

bool T30_Functional::operator () (const T30& str) {
	return str.flag;
}

struct T31 {
	USER_STRUCT(T31, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	bool b [12];

	USER_ENUM_MEM(b)

	static constexpr uSer::Bit bits = 12;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,2> {{0xFF, 0x0F}},
					T31 {{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }}
				),
				TC (
					std::array<uint8_t,2> {{0x55, 0x05}},
					T31 {{ 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }}
				),
				TC (
					std::array<uint8_t,2> {{0xAA, 0x0A}},
					T31 {{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 }}
				)
			);
	}
};
struct T32 {
	USER_STRUCT(T32, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	bool b1, b2, b3, b4, b5;
	USER_MEM_ANNOT(b1, uSer::Padding::Fixed<3>)
	USER_MEM_ANNOT(b2, uSer::Padding::Fixed<2>)
	USER_MEM_ANNOT(b4, uSer::Padding::Fixed<3>)

	USER_ENUM_MEM(b1, b2, b3, b4, b5)

	static constexpr uSer::Bit bits = 13;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,2> {{0x91, 0x11}},
					T32 { 1, 1, 1, 1, 1 }
				),
				/* TC ( // Only for serialization
					std::array<uint8_t,2> {{0x6E, 0xEE}},
					T32 { 0, 0, 0, 0, 0 }
				), */
				TC (
					std::array<uint8_t,2> {{0x80, 0}},
					T32 { 0, 0, 1, 0, 0 }
				),
				TC (
					std::array<uint8_t,2> {{0, 1}},
					T32 { 0, 0, 0, 1, 0 }
				)
			);
	}
};

struct T33 {
	USER_STRUCT(T33, uSer::AttrNone)
	using MaxSWord = uint32_t;

	constexpr T33 (bool f, uint32_t a_) : flag (f), a (a_) {}
	constexpr T33 (bool f, int32_t b_) : flag (f), b (b_) {}
	constexpr T33 () : flag (), a () {}

	bool flag;
	union {
		uint32_t a;
		int32_t b;
	};

	bool checkA () const { return flag; }
	bool checkB () const { return !flag; }

	USER_MEM_ANNOT(a, uSer::Dyn::Optional<&T33::checkA>)
	USER_MEM_ANNOT(b, uSer::Dyn::Optional<&T33::checkB>)

	USER_ENUM_MEM(flag, a, b)

	static constexpr uSer::Bit bits = 1;
	template <uSer::Bit wSize>
	using MaxDyn = uSer::MaxSize<64 / wSize>;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,5> {{ 1 | 0xCE, 0x8A, 0x46, 2}},
					T33 { 1, uint32_t { 0x01234567 } }
				),
				TC (
					std::array<uint8_t,5> {{ 0x32, 0x75, 0xB9, 0xFD, 1 }},
					T33 { 0, int32_t { -0x01234567 } }
				)
			);
	}
};

struct T40 {
	USER_STRUCT(T40, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	std::tuple<uint8_t, std::array<uint8_t, 3>> x;

	USER_ENUM_MEM(x)

	static constexpr uSer::Bit bits = 32;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,4> {{ 1, 2, 3, 4 }},
					T40 {{ 1, {{ 2, 3, 4 }}}}
				)
			);
	}
};

struct T41 {
	USER_STRUCT(T41, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	std::pair<uint8_t, std::array<uint8_t, 3>> x;

	USER_ENUM_MEM(x)

	static constexpr uSer::Bit bits = 32;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,4> {{ 1, 2, 3, 4 }},
					T41 {{ 1, {{ 2, 3, 4 }}}}
				)
			);
	}
};

struct T50 {
	USER_STRUCT(T50, uSer::Padding::Fixed<8>)
	using MaxSWord = ::MaxSWord;

	uint32_t x;
	USER_MEM_ANNOT(x, uSer::Width<16>)
	USER_ENUM_MEM(x)

	static constexpr uSer::Bit bits = 16;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,2> {{ 0xCD, 0xAB }},
					T50 { 0xABCD }
				)
			);
	}
};

struct T51 {
	USER_STRUCT(T51, uSer::ByteOrder::BE)
	using MaxSWord = ::MaxSWord;

	uint32_t a;
	USER_MEM_ANNOT(a, uSer::Width<16>)
	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 16;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,2> {{ 0xAB, 0xCD }},
					T51 { 0xABCD }
				)
			);
	}
};

struct T52 {
	USER_STRUCT(T52, uSer::ByteOrder::BE)
	using MaxSWord = ::MaxSWord;

	uint32_t a;
	USER_MEM_ANNOT(a, uSer::Width<16>, uSer::ByteOrder::LE)
	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 16;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,2> {{ 0xCD, 0xAB }},
					T52 { 0xABCD }
				)
			);
	}
};

struct T53 {
	USER_STRUCT(T53, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	uint16_t a;
	USER_ENUM_MEM(a)
};

struct T54 {
	USER_STRUCT(T54, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	T53 a;
	USER_MEM_ANNOT(a, uSer::ByteOrder::BE)
	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 16;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,2> {{ 0xAB, 0xCD }},
					T54 { { 0xABCD } }
				)
			);
	}
};

struct T55 {
	USER_STRUCT(T55, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	T53 a;
	uint8_t b;
	USER_MEM_ANNOT(a, uSer::Padding::Fixed<8>)
	USER_ENUM_MEM(a, b)

	static constexpr uSer::Bit bits = 24;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,3> {{ 0xCD, 0xAB, 42 }},
					T55 { { 0xABCD }, 42 }
				)
			);
	}
};

struct T60 {
	static uint8_t testState;

	USER_STRUCT(T60, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	void serPre1 (const uint8_t&) const {
//		std::cout << "serPre1\n";
		verify (testState == 0);
		testState = 1;
	}
	void serPost1 (const uint8_t&) const {
//		std::cout << "serPost1\n";
		verify (testState == 1);
		testState = 2;
	}
	void deSerPre1 (uint8_t&) {
//		std::cout << "deSerPre1\n";
		verify (testState == 0);
		testState = 1;
	}
	void deSerPost1 (uint8_t&) {
//		std::cout << "deSerPost1\n";
		verify (testState == 1);
		testState = 2;
	}

	uint8_t a;
	USER_MEM_ANNOT(a, uSer::Hook::SerPre<&T60::serPre1>, uSer::Hook::SerPost<&T60::serPost1>, uSer::Hook::DeSerPre<&T60::deSerPre1>, uSer::Hook::DeSerPost<&T60::deSerPost1>)
	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,1> {{ 42 }},
					T60 { 42 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
uint8_t T60::testState = 0;

struct T61 {
	static uint8_t testState;

	USER_STRUCT(T61, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	uSer_ErrorCode serPre1 (const uint8_t& obj) const {
//		std::cout << "serPre1\n";
		verify (testState == 0);
		verify (&obj == &b);
		testState = 1;
		return (obj != 42) ? uSer_EOK : uSer_EHOOK;
	}
	uSer_ErrorCode serPost1 (const uint8_t& obj) const {
//		std::cout << "serPost1\n";
		verify (testState == 1);
		verify (&obj == &b);
		testState = 2;
		return (obj != 43) ? uSer_EOK : uSer_EHOOK;
	}
	uSer_ErrorCode deSerPre1 (uint8_t& obj) {
//		std::cout << "deSerPre1\n";
		verify (testState == 0);
		verify (&obj == &b);
		testState = 1;
		return (a != 42) ? uSer_EOK : uSer_EHOOK;
	}
	uSer_ErrorCode deSerPost1 (uint8_t& obj) {
//		std::cout << "deSerPost1\n";
		verify (testState == 1);
		verify (&obj == &b);
		testState = 2;
		return (b != 43) ? uSer_EOK : uSer_EHOOK;
	}

	uint8_t a, b;
	USER_MEM_ANNOT(b, uSer::Hook::SerPre<&T61::serPre1>, uSer::Hook::SerPost<&T61::serPost1>, uSer::Hook::DeSerPre<&T61::deSerPre1>, uSer::Hook::DeSerPost<&T61::deSerPost1>)
	USER_ENUM_MEM(a, b)

	static constexpr uSer::Bit bits = 16;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TCT (
					std::array<uint8_t,2> {{ 42, 42 }},
					T61 { 42, 42 }
				),
				TCT (
					std::array<uint8_t,2> {{ 43, 43 }},
					T61 { 43, 43 }
				),
				TCT (
					std::array<uint8_t,2> {{ 42, 42 }},
					T61 { 42, 42 }
				),
				TCT (
					std::array<uint8_t,2> {{ 43, 43 }},
					T61 { 43, 43 }
				),
				TC (
					std::array<uint8_t,2> {{ 44, 44 }},
					T61 { 44, 44 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
uint8_t T61::testState = 0;

struct T62 {
	static uint8_t testState;

	USER_STRUCT(T62, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	static void serPre1 (const uint8_t&, const T62&) {
//		std::cout << "serPre1\n";
		verify (testState == 0);
		testState = 1;
	}
	static void serPost1 (const uint8_t&, const T62&) {
//		std::cout << "serPost1\n";
		verify (testState == 1);
		testState = 2;
	}
	static void deSerPre1 (uint8_t&, T62&) {
//		std::cout << "deSerPre1\n";
		verify (testState == 0);
		testState = 1;
	}
	static void deSerPost1 (uint8_t&, T62&) {
//		std::cout << "deSerPost1\n";
		verify (testState == 1);
		testState = 2;
	}

	uint8_t a;
	USER_MEM_ANNOT(a, uSer::Hook::SerPre<&T62::serPre1>, uSer::Hook::SerPost<&T62::serPost1>, uSer::Hook::DeSerPre<&T62::deSerPre1>, uSer::Hook::DeSerPost<&T62::deSerPost1>)
	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,1> {{ 42 }},
					T62 { 42 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
uint8_t T62::testState = 0;

struct T63 {
	static uint8_t testState;

	USER_STRUCT(T63, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	static uSer_ErrorCode serPre1 (const uint8_t& obj, const T63& s) {
//		std::cout << "serPre1\n";
		verify (testState == 0);
		verify (&obj == &s.b);
		testState = 1;
		return (obj != 42) ? uSer_EOK : uSer_EHOOK;
	}
	static uSer_ErrorCode serPost1 (const uint8_t& obj, const T63& s) {
//		std::cout << "serPost1\n";
		verify (testState == 1);
		verify (&obj == &s.b);
		testState = 2;
		return (obj != 43) ? uSer_EOK : uSer_EHOOK;
	}
	static uSer_ErrorCode deSerPre1 (uint8_t& obj, T63& s) {
//		std::cout << "deSerPre1\n";
		verify (testState == 0);
		verify (&obj == &s.b);
		testState = 1;
		return (s.a != 42) ? uSer_EOK : uSer_EHOOK;
	}
	static uSer_ErrorCode deSerPost1 (uint8_t& obj, T63& s) {
//		std::cout << "deSerPost1\n";
		verify (testState == 1);
		verify (&obj == &s.b);
		testState = 2;
		return (s.b != 43) ? uSer_EOK : uSer_EHOOK;
	}

	uint8_t a, b;
	USER_MEM_ANNOT(b, uSer::Hook::SerPre<&T63::serPre1>, uSer::Hook::SerPost<&T63::serPost1>, uSer::Hook::DeSerPre<&T63::deSerPre1>, uSer::Hook::DeSerPost<&T63::deSerPost1>)
	USER_ENUM_MEM(a, b)

	static constexpr uSer::Bit bits = 16;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TCT (
					std::array<uint8_t,2> {{ 42, 42 }},
					T63 { 42, 42 }
				),
				TCT (
					std::array<uint8_t,2> {{ 43, 43 }},
					T63 { 43, 43 }
				),
				TCT (
					std::array<uint8_t,2> {{ 42, 42 }},
					T63 { 42, 42 }
				),
				TCT (
					std::array<uint8_t,2> {{ 43, 43 }},
					T63 { 43, 43 }
				),
				TC (
					std::array<uint8_t,2> {{ 44, 44 }},
					T63 { 44, 44 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
uint8_t T63::testState = 0;




struct T64 {
	static uint8_t testState;

	USER_STRUCT(T64, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	struct SerPre1 {
		void operator () (const uint8_t&, const T64&) {
//		std::cout << "serPre1\n";
			verify (testState == 0);
			testState = 1;
		}
	} static serPre1;
	struct SerPost1 {
		void operator () (const uint8_t&, const T64&) {
	//		std::cout << "serPost1\n";
			verify (testState == 1);
			testState = 2;
		}
	} static serPost1;
	struct DeSerPre1 {
		void operator () (uint8_t&, T64&) {
	//		std::cout << "deSerPre1\n";
			verify (testState == 0);
			testState = 1;
		}
	} static deSerPre1;
	struct DeSerPost1 {
		void operator () (uint8_t&, T64&) {
	//		std::cout << "deSerPost1\n";
			verify (testState == 1);
			testState = 2;
		}
	} static deSerPost1;

	uint8_t a;
	USER_MEM_ANNOT(a, uSer::Hook::SerPre<&T64::serPre1>, uSer::Hook::SerPost<&T64::serPost1>, uSer::Hook::DeSerPre<&T64::deSerPre1>, uSer::Hook::DeSerPost<&T64::deSerPost1>)
	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,1> {{ 42 }},
					T64 { 42 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
uint8_t T64::testState = 0;
T64::SerPre1 T64::serPre1 {};
T64::SerPost1 T64::serPost1 {};
T64::DeSerPre1 T64::deSerPre1 {};
T64::DeSerPost1 T64::deSerPost1 {};

struct T65 {
	static uint8_t testState;

	USER_STRUCT(T65, uSer::AttrNone)
	using MaxSWord = ::MaxSWord;

	struct SerPre1 {
		uSer_ErrorCode operator () (const uint8_t& obj, const T65& s) {
	//		std::cout << "serPre1\n";
			verify (testState == 0);
			verify (&obj == &s.b);
			testState = 1;
			return (obj != 42) ? uSer_EOK : uSer_EHOOK;
		}
	} static serPre1;
	struct SerPost1 {
		uSer_ErrorCode operator ()(const uint8_t& obj, const T65& s) {
	//		std::cout << "serPost1\n";
			verify (testState == 1);
			verify (&obj == &s.b);
			testState = 2;
			return (obj != 43) ? uSer_EOK : uSer_EHOOK;
		}
	} static serPost1;
	struct DeSerPre1 {
		uSer_ErrorCode operator () (uint8_t& obj, T65& s) {
	//		std::cout << "deSerPre1\n";
			verify (testState == 0);
			verify (&obj == &s.b);
			testState = 1;
			return (s.a != 42) ? uSer_EOK : uSer_EHOOK;
		}
	} static deSerPre1;
	struct DeSerPost1 {
		uSer_ErrorCode operator () (uint8_t& obj, T65& s) {
	//		std::cout << "deSerPost1\n";T65
			verify (testState == 1);
			verify (&obj == &s.b);
			testState = 2;
			return (s.b != 43) ? uSer_EOK : uSer_EHOOK;
		}
	} static deSerPost1;

	uint8_t a, b;
	USER_MEM_ANNOT(b, uSer::Hook::SerPre<&T65::serPre1>, uSer::Hook::SerPost<&T65::serPost1>, uSer::Hook::DeSerPre<&T65::deSerPre1>, uSer::Hook::DeSerPost<&T65::deSerPost1>)
	USER_ENUM_MEM(a, b)

	static constexpr uSer::Bit bits = 16;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TCT (
					std::array<uint8_t,2> {{ 42, 42 }},
					T65 { 42, 42 }
				),
				TCT (
					std::array<uint8_t,2> {{ 43, 43 }},
					T65 { 43, 43 }
				),
				TCT (
					std::array<uint8_t,2> {{ 42, 42 }},
					T65 { 42, 42 }
				),
				TCT (
					std::array<uint8_t,2> {{ 43, 43 }},
					T65 { 43, 43 }
				),
				TC (
					std::array<uint8_t,2> {{ 44, 44 }},
					T65 { 44, 44 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
uint8_t T65::testState = 0;
T65::SerPre1 T65::serPre1 {};
T65::SerPost1 T65::serPost1 {};
T65::DeSerPre1 T65::deSerPre1 {};
T65::DeSerPost1 T65::deSerPost1 {};

struct T66 {
	static uint8_t testState;
	using MaxSWord = ::MaxSWord;

	static void serPre1 (const T66&) {
//		std::cout << "serPre1\n";
		verify (testState == 0);
		testState = 1;
	}
	static void serPost1 (const T66&) {
//		std::cout << "serPost1\n";
		verify (testState == 1);
		testState = 2;
	}
	static void deSerPre1 (T66&) {
//		std::cout << "deSerPre1\n";
		verify (testState == 0);
		testState = 1;
	}
	static void deSerPost1 (T66&) {
//		std::cout << "deSerPost1\n";
		verify (testState == 1);
		testState = 2;
	}
	USER_STRUCT(T66, uSer::Hook::SerPre<&T66::serPre1>, uSer::Hook::SerPost<&T66::serPost1>, uSer::Hook::DeSerPre<&T66::deSerPre1>, uSer::Hook::DeSerPost<&T66::deSerPost1>)


	uint8_t a;
	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,1> {{ 42 }},
					T66 { 42 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
uint8_t T66::testState = 0;
template <uint8_t Init = 0>
struct T67 {
	static uint8_t testState;
	using MaxSWord = ::MaxSWord;

	static uSer_ErrorCode serPre1 (const T67& s) {
//		std::cout << "serPre1\n";
		verify (testState == 0);
		testState = 1;
		return (s.b != 42) ? uSer_EOK : uSer_EHOOK;
	}
	static uSer_ErrorCode serPost1 (const T67& s) {
//		std::cout << "serPost1\n";
		verify (testState == 1);
		testState = 2;
		return (s.b != 43) ? uSer_EOK : uSer_EHOOK;
	}
	static uSer_ErrorCode deSerPre1 (T67& s) {
//		std::cout << "deSerPre1\n";
		verify (testState == 0);
		testState = 1;
		return (s.b != 42) ? uSer_EOK : uSer_EHOOK;
	}
	static uSer_ErrorCode deSerPost1 (T67& s) {
//		std::cout << "deSerPost1\n";
		verify (testState == 1);
		testState = 2;
		return (s.b != 43) ? uSer_EOK : uSer_EHOOK;
	}
	USER_STRUCT(T67, uSer::Hook::SerPre<&T67::serPre1>, uSer::Hook::SerPost<&T67::serPost1>, uSer::Hook::DeSerPre<&T67::deSerPre1>, uSer::Hook::DeSerPost<&T67::deSerPost1>)

	uint8_t b = Init;
	USER_ENUM_MEM(b)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TCT (
					std::array<uint8_t,1> {{ 42 }},
					T67<42> { 42 }
				),
				TCT (
					std::array<uint8_t,1> {{ 43 }},
					T67<0> { 43 }
				),
				TCT (
					std::array<uint8_t,1> {{ 42 }},
					T67<42> { 42 }
				),
				TCT (
					std::array<uint8_t,1> {{ 43 }},
					T67<0> { 43 }
				),
				TC (
					std::array<uint8_t,1> {{ 44 }},
					T67<0> { 44 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
template <uint8_t Init>
uint8_t T67<Init>::testState = 0;


struct T68 {
	static uint8_t testState;

	using MaxSWord = ::MaxSWord;

	struct SerPre1 {
		void operator () (const T68&) {
//		std::cout << "serPre1\n";
			verify (testState == 0);
			testState = 1;
		}
	} static serPre1;
	struct SerPost1 {
		void operator () (const T68&) {
	//		std::cout << "serPost1\n";
			verify (testState == 1);
			testState = 2;
		}
	} static serPost1;
	struct DeSerPre1 {
		void operator () (T68&) {
	//		std::cout << "deSerPre1\n";
			verify (testState == 0);
			testState = 1;
		}
	} static deSerPre1;
	struct DeSerPost1 {
		void operator () (T68&) {
	//		std::cout << "deSerPost1\n";
			verify (testState == 1);
			testState = 2;
		}
	} static deSerPost1;

	USER_STRUCT(T68, uSer::Hook::SerPre<&T68::serPre1>, uSer::Hook::SerPost<&T68::serPost1>, uSer::Hook::DeSerPre<&T68::deSerPre1>, uSer::Hook::DeSerPost<&T68::deSerPost1>)

	uint8_t a;
	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,1> {{ 42 }},
					T68 { 42 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
uint8_t T68::testState = 0;
T68::SerPre1 T68::serPre1 {};
T68::SerPost1 T68::serPost1 {};
T68::DeSerPre1 T68::deSerPre1 {};
T68::DeSerPost1 T68::deSerPost1 {};

template <uint8_t Init = 0>
struct T69 {
	static uint8_t testState;
	using MaxSWord = ::MaxSWord;

	struct SerPre1 {
		uSer_ErrorCode operator () (const T69& s) {
	//		std::cout << "serPre1\n";
			verify (testState == 0);
			testState = 1;
			return (s.b != 42) ? uSer_EOK : uSer_EHOOK;
		}
	} static serPre1;
	struct SerPost1 {
		uSer_ErrorCode operator ()(const T69& s) {
	//		std::cout << "serPost1\n";
			verify (testState == 1);
			testState = 2;
			return (s.b != 43) ? uSer_EOK : uSer_EHOOK;
		}
	} static serPost1;
	struct DeSerPre1 {
		uSer_ErrorCode operator () (T69& s) {
	//		std::cout << "deSerPre1\n";
			verify (testState == 0);
			testState = 1;
			return (s.b != 42) ? uSer_EOK : uSer_EHOOK;
		}
	} static deSerPre1;
	struct DeSerPost1 {
		uSer_ErrorCode operator () (T69& s) {
	//		std::cout << "deSerPost1\n";T65
			verify (testState == 1);
			testState = 2;
			return (s.b != 43) ? uSer_EOK : uSer_EHOOK;
		}
	} static deSerPost1;
	USER_STRUCT(T69, uSer::Hook::SerPre<&T69::serPre1>, uSer::Hook::SerPost<&T69::serPost1>, uSer::Hook::DeSerPre<&T69::deSerPre1>, uSer::Hook::DeSerPost<&T69::deSerPost1>)

	uint8_t b = Init;
	USER_ENUM_MEM(b)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TCT (
					std::array<uint8_t,1> {{ 42 }},
					T69<42> { 42 }
				),
				TCT (
					std::array<uint8_t,1> {{ 43 }},
					T69<0> { 43 }
				),
				TCT (
					std::array<uint8_t,1> {{ 42 }},
					T69<42> { 42 }
				),
				TCT (
					std::array<uint8_t,1> {{ 43 }},
					T69<0> { 43 }
				),
				TC (
					std::array<uint8_t,1> {{ 44 }},
					T69<0> { 44 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
template <uint8_t Init>
uint8_t T69<Init>::testState = 0;
template <uint8_t Init>
typename T69<Init>::SerPre1 T69<Init>::serPre1 {};
template <uint8_t Init>
typename T69<Init>::SerPost1 T69<Init>::serPost1 {};
template <uint8_t Init>
typename T69<Init>::DeSerPre1 T69<Init>::deSerPre1 {};
template <uint8_t Init>
typename T69<Init>::DeSerPost1 T69<Init>::deSerPost1 {};

struct T70 {
	static uint8_t testState;
	using MaxSWord = ::MaxSWord;

	void serPre1 () const {
//		std::cout << "serPre1\n";
		verify (testState == 0);
		testState = 1;
	}
	void serPost1 () const {
//		std::cout << "serPost1\n";
		verify (testState == 1);
		testState = 2;
	}
	void deSerPre1 () {
//		std::cout << "deSerPre1\n";
		verify (testState == 0);
		testState = 1;
	}
	void deSerPost1 () {
//		std::cout << "deSerPost1\n";
		verify (testState == 1);
		testState = 2;
	}
	USER_STRUCT(T70, uSer::Hook::SerPre<&T70::serPre1>, uSer::Hook::SerPost<&T70::serPost1>, uSer::Hook::DeSerPre<&T70::deSerPre1>, uSer::Hook::DeSerPost<&T70::deSerPost1>)


	uint8_t a;
	USER_ENUM_MEM(a)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TC (
					std::array<uint8_t,1> {{ 42 }},
					T70 { 42 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
uint8_t T70::testState = 0;

template <uint8_t Init = 0>
struct T71 {
	static uint8_t testState;
	using MaxSWord = ::MaxSWord;

	uSer_ErrorCode serPre1 () const {
//		std::cout << "serPre1\n";
		verify (testState == 0);
		testState = 1;
		return (b != 42) ? uSer_EOK : uSer_EHOOK;
	}
	uSer_ErrorCode serPost1 () const {
//		std::cout << "serPost1\n";
		verify (testState == 1);
		testState = 2;
		return (b != 43) ? uSer_EOK : uSer_EHOOK;
	}
	uSer_ErrorCode deSerPre1 () {
//		std::cout << "deSerPre1\n";
		verify (testState == 0);
		testState = 1;
		return (b != 42) ? uSer_EOK : uSer_EHOOK;
	}
	uSer_ErrorCode deSerPost1 () {
//		std::cout << "deSerPost1\n";
		verify (testState == 1);
		testState = 2;
		return (b != 43) ? uSer_EOK : uSer_EHOOK;
	}
	USER_STRUCT(T71, uSer::Hook::SerPre<&T71::serPre1>, uSer::Hook::SerPost<&T71::serPost1>, uSer::Hook::DeSerPre<&T71::deSerPre1>, uSer::Hook::DeSerPost<&T71::deSerPost1>)

	uint8_t b = Init;
	USER_ENUM_MEM(b)

	static constexpr uSer::Bit bits = 8;
	template <uSer::Bit>
	using MaxDyn = uSer::NoDyn;
	static constexpr auto testcases () {
		return std::make_tuple (
				TCT (
					std::array<uint8_t,1> {{ 42 }},
					T71<42> { 42 }
				),
				TCT (
					std::array<uint8_t,1> {{ 43 }},
					T71<0> { 43 }
				),
				TCT (
					std::array<uint8_t,1> {{ 42 }},
					T71<42> { 42 }
				),
				TCT (
					std::array<uint8_t,1> {{ 43 }},
					T71<0> { 43 }
				),
				TC (
					std::array<uint8_t,1> {{ 44 }},
					T71<0> { 44 }
				)
			);
	}
	void testBegin () const {
//		std::cout << "testBegin ()\n";
		testState = 0;
	}
	void testEnd () const {
//		std::cout << "testEnd ()\n";
		verify (testState == 2);
	}
};
template <uint8_t Init>
uint8_t T71<Init>::testState = 0;

template <typename Base>
class OutIterSingle {
	public:
		using value_type = typename std::iterator_traits<Base>::value_type;
		using difference_type = typename std::iterator_traits<Base>::difference_type;
		using pointer = typename std::iterator_traits<Base>::pointer;
		using reference = typename std::iterator_traits<Base>::reference;
		using iterator_category = std::output_iterator_tag;

		OutIterSingle (Base b) : m_base (std::move (b)), m_mode (false), m_valid (true) {}
		OutIterSingle (const OutIterSingle&) = delete;
		OutIterSingle& operator = (const OutIterSingle&) = delete;

		OutIterSingle (OutIterSingle&& src) : m_base (std::move (src.m_base)), m_mode (std::move (src.m_mode)), m_valid (std::move (src.m_valid)) { src.m_valid = false; }
		OutIterSingle& operator = (OutIterSingle&& src) { m_base = std::move (src.m_base); m_mode = std::move (src.m_mode); m_valid = std::move (src.m_valid); src.m_valid = false; return *this; }

		decltype(auto) operator * () { assert (m_valid && !m_mode); m_mode = true; return *m_base; }
		OutIterSingle& operator++ () {
			assert (m_valid && m_mode);
			m_mode = false;
			++m_base;
			return *this;
		}
	private:
		Base m_base;
		bool m_mode, m_valid;
};

template <typename Base>
class OutIterForward {
	public:
		using value_type = typename std::iterator_traits<Base>::value_type;
		using difference_type = typename std::iterator_traits<Base>::difference_type;
		using pointer = typename std::iterator_traits<Base>::pointer;
		using reference = typename std::iterator_traits<Base>::reference;
		using iterator_category = std::forward_iterator_tag;

		OutIterForward (Base b) : m_base (std::move (b)), m_mode (false), m_valid (true) {}
		OutIterForward (const OutIterForward&) = delete;
		OutIterForward& operator = (const OutIterForward&) = delete;

		OutIterForward (OutIterForward&& src) : m_base (std::move (src.m_base)), m_mode (std::move (src.m_mode)), m_valid (std::move (src.m_valid)) { src.m_valid = false; }
		OutIterForward& operator = (OutIterForward&& src) { m_base = std::move (src.m_base); m_mode = std::move (src.m_mode); m_valid = std::move (src.m_valid); src.m_valid = false; return *this; }

		decltype(auto) operator * () { assert (m_valid && !m_mode); m_mode = true; return *m_base; }
		OutIterForward& operator++ () {
			assert (m_valid);
			m_mode = false;
			++m_base;
			return *this;
		}
	private:
		Base m_base;
		bool m_mode, m_valid;
};

template <typename Base>
class OutIterForwardSkip {
	public:
		using value_type = typename std::iterator_traits<Base>::value_type;
		using difference_type = typename std::iterator_traits<Base>::difference_type;
		using pointer = typename std::iterator_traits<Base>::pointer;
		using reference = typename std::iterator_traits<Base>::reference;
		using iterator_category = std::forward_iterator_tag;

		OutIterForwardSkip (Base b) : m_base (std::move (b)), m_mode (false), m_valid (true) {}
		OutIterForwardSkip (const OutIterForwardSkip&) = delete;
		OutIterForwardSkip& operator = (const OutIterForwardSkip&) = delete;

		OutIterForwardSkip (OutIterForwardSkip&& src) : m_base (std::move (src.m_base)), m_mode (std::move (src.m_mode)), m_valid (std::move (src.m_valid)) { src.m_valid = false; }
		OutIterForwardSkip& operator = (OutIterForwardSkip&& src) { m_base = std::move (src.m_base); m_mode = std::move (src.m_mode); m_valid = std::move (src.m_valid); src.m_valid = false; return *this; }

		decltype(auto) operator * () { assert (m_valid && !m_mode); m_mode = true; return *m_base; }
		OutIterForwardSkip& operator++ () {
			assert (m_valid && m_mode);
			m_mode = false;
			++m_base;
			return *this;
		}
		OutIterForwardSkip& operator += (difference_type count) {
			assert (m_valid && !m_mode);
			m_base += count;
			return *this;
		}
	private:
		Base m_base;
		bool m_mode, m_valid;
};

template <typename Base>
class InpIter {
	public:
		using value_type = typename std::iterator_traits<Base>::value_type;
		using difference_type = typename std::iterator_traits<Base>::difference_type;
		using pointer = typename std::iterator_traits<Base>::pointer;
		using reference = typename std::iterator_traits<Base>::reference;
		using iterator_category = typename std::iterator_traits<Base>::iterator_category;

		InpIter (Base b) : m_base (std::move (b)), m_mode (false), m_valid (true) {}
		InpIter (const InpIter&) = delete;
		InpIter& operator = (const InpIter&) = delete;

		InpIter (InpIter&& src) : m_base (std::move (src.m_base)), m_mode (std::move (src.m_mode)), m_valid (std::move (src.m_valid)) { src.m_valid = false; }
		InpIter& operator = (InpIter&& src) { m_base = std::move (src.m_base); m_mode = std::move (src.m_mode); m_valid = std::move (src.m_valid); src.m_valid = false; return *this; }

		decltype(auto) operator * () { assert (m_valid && !m_mode); m_mode = true; return *m_base; }
		InpIter& operator++ () {
			assert (m_valid);
			m_mode = false;
			++m_base;
			return *this;
		}
	private:
		Base m_base;
		bool m_mode, m_valid;
};

template <typename Base>
class InpIterSkip {
	public:
		using value_type = typename std::iterator_traits<Base>::value_type;
		using difference_type = typename std::iterator_traits<Base>::difference_type;
		using pointer = typename std::iterator_traits<Base>::pointer;
		using reference = typename std::iterator_traits<Base>::reference;
		using iterator_category = typename std::iterator_traits<Base>::iterator_category;

		InpIterSkip (Base b) : m_base (std::move (b)), m_mode (false), m_valid (true) {}
		InpIterSkip (const InpIterSkip&) = delete;
		InpIterSkip& operator = (const InpIterSkip&) = delete;

		InpIterSkip (InpIterSkip&& src) : m_base (std::move (src.m_base)), m_mode (std::move (src.m_mode)), m_valid (std::move (src.m_valid)) { src.m_valid = false; }
		InpIterSkip& operator = (InpIterSkip&& src) { m_base = std::move (src.m_base); m_mode = std::move (src.m_mode); m_valid = std::move (src.m_valid); src.m_valid = false; return *this; }

		decltype(auto) operator * () { assert (m_valid && !m_mode); m_mode = true; return *m_base; }
		InpIterSkip& operator++ () {
			assert (m_valid && m_mode);
			m_mode = false;
			++m_base;
			return *this;
		}
		InpIterSkip& operator += (difference_type count) {
			assert (m_valid && !m_mode);
			m_base += count;
			return *this;
		}
	private:
		Base m_base;
		bool m_mode, m_valid;
};

template <typename T>
auto callTestBegin (T* obj) -> decltype (obj->testBegin ()) {
	obj->testBegin ();
}
void callTestBegin (...) {}

template <typename T>
auto callTestEnd (T* obj) -> decltype (obj->testEnd ()) {
	obj->testEnd ();
}
void callTestEnd (...) {}

std::ostream& printElem (std::ostream& os, const char& x) {return os << static_cast<int> (x); }
std::ostream& printElem (std::ostream& os, const signed char& x) {return os << static_cast<int> (x); }
std::ostream& printElem (std::ostream& os, const unsigned char& x) {return os << static_cast<int> (x); }
template <typename T> std::ostream& printElem (std::ostream& os, const T& x) {return os << x; }

template <typename Iter>
void printArray (std::ostream& os, Iter b, Iter e) {
	using V = typename std::iterator_traits<Iter>::value_type;

	std::ios  state (nullptr);
	state.copyfmt (os);

	os << "{";
	bool first = true;
	while (b != e) {
		if (!first) os << ", ";
		else first = false;
		os << std::hex << "0x" << std::setw(std::numeric_limits<V>::digits / 4) << std::setfill('0');
		printElem (os, *b);
		++b;
	}
	os << "}";

	os.copyfmt(state);
}

template <typename F, typename F_Unexp, typename F_Excp, typename F_Comp>
bool expect (bool shouldthrow, F f, F_Unexp u, F_Excp e, F_Comp fc) {
	if constexpr (std::is_same_v<decltype(f()), void>) {
#ifdef USER_EXCEPTIONS
		try {
#endif
			f ();
#ifdef USER_EXCEPTIONS
			if (shouldthrow) {
				e ();
				return false;
			} else {
				return fc ();
			}
		} catch (const uSer::Exception& ex) {
			if (!shouldthrow) {
				u (ex.what ());
				return false;
			} else
				return true;
		}
#else
		static_cast<void> (shouldthrow);
		static_cast<void> (u);
		static_cast<void> (e);
		verify (!shouldthrow);
		return fc ();
#endif
	} else {
		uSer_ErrorCode c = f ();
		if (c == uSer_EOK) {
			if (shouldthrow) {
				e ();
				return false;
			} else return fc ();
		} else {
			if (!shouldthrow) {
				u (uSer_getErrorMessage (c));
				return false;
			} else
				return true;
		}
	}
}

template <typename SWord, typename Iterator, uSer::Bit wSize, typename Raw, typename Data>
bool runTestcaseWI_Out (const std::vector<SWord>& rawRef, const TestCase<Raw, Data>& tc) {
	std::vector<SWord> r (rawRef.size ());

	callTestBegin (&tc.data);
	size_t sizeUsed = 0xDEADBEEF;
	return expect (tc.shouldthrow,
				[&] () { return uSer::serialize (Iterator { std::begin(r) }, tc.data, r.size (), &sizeUsed); },
				[&] (const char* errStr) { std::cout << "\nTestcase " << tc.file << ":" << tc.line << " word size = " << wSize << ": Unexpected exception during Serialization:\n" << errStr << std::endl; },
				[&] () { std::cout << "\nTestcase " << tc.file << ":" << tc.line << " word size = " << wSize << ": Expected exception was not thrown during Serialization" << std::endl; },
				[&] () {
		callTestEnd (&tc.data);
	if (sizeUsed != rawRef.size ()) {
		std::cout << "\nTestcase " << tc.file << ":" << tc.line << " word size = " << wSize << ": sizeUsed wrong during Serialization. Expected: " << rawRef.size () << ", Got: " << sizeUsed << std::endl;
		return false;
	}
	if (!std::equal (std::begin (rawRef), std::end(rawRef), std::begin(r))) {
		std::cout << "\nTestcase " << tc.file << ":" << tc.line << " word size = " << wSize << ": Serialization failed. Expected result:\n";
		printArray (std::cout, std::begin(rawRef), std::end(rawRef));
		std::cout << "\nGot:\n";
		printArray (std::cout, std::begin(r), std::begin(r)+static_cast <typename decltype (r)::difference_type> (std::size(rawRef)));
		std::cout << std::endl;
		return false;
	} else
		return true;
	});
}

template <typename SWord, typename Iterator, uSer::Bit wSize, typename Raw, typename Data>
bool runTestcaseWI_In (const std::vector<SWord>& rawRef, const TestCase<Raw, Data>& tc) {
	Data d {};
	size_t sizeUsed = 0xDEADBEEF;
	callTestBegin (&tc.data);
	return expect (tc.shouldthrow,
				[&] () { return uSer::deserialize (Iterator { std::begin(rawRef) }, d, rawRef.size (), &sizeUsed); },
				[&] (const char* errStr) { std::cout << "\nTestcase " << tc.file << ":" << tc.line << " word size = " << wSize << ": Unexpected exception during Deserialization:\n" << errStr << std::endl; },
				[&] () { std::cout << "\nTestcase " << tc.file << ":" << tc.line << " word size = " << wSize << ": Expected exception was not thrown during Deserialization" << std::endl; },
				[&] () {
					callTestEnd (&tc.data);

	if (sizeUsed != rawRef.size ()) {
		std::cout << "\nTestcase " << tc.file << ":" << tc.line << " word size = " << wSize << ": sizeUsed wrong during Deserialization. Expected: " << rawRef.size () << ", Got: " << sizeUsed << std::endl;
		return false;
	}
	if (!uSer::compare(d, tc.data)) {
		std::cout << "\nTestcase " << tc.file << ":" << tc.line << " word size = " << wSize << ": Deserialization failed. Expected result:\n";
		uSer::print (std::cout, tc.data);
		std::cout << "\nGot:\n";
		uSer::print (std::cout, d);
		std::cout << std::endl;
		return false;
	} else
		return true;
	});
}

template <typename SWord, uSer::Bit wSize, typename Raw, typename Data, typename... Iterators>
bool runTestcaseW_Out (const std::vector<SWord>& rawRef, const TestCase<Raw, Data>& tc) {
	return (runTestcaseWI_Out<SWord, Iterators, wSize, Raw, Data> (rawRef, tc) && ...);
}

template <typename SWord, uSer::Bit wSize, typename Raw, typename Data, typename... Iterators>
bool runTestcaseW_In (const std::vector<SWord>& rawRef, const TestCase<Raw, Data>& tc) {
	return (runTestcaseWI_In<SWord, Iterators, wSize, Raw, Data> (rawRef, tc) && ...);
}

template <typename SWord, typename Raw, typename Data>
std::enable_if_t<(sizeof (typename Data::MaxSWord) % sizeof(SWord)) == 0, bool> runTestcaseW (const TestCase<Raw, Data>& tc) {
	static constexpr uSer::Bit wSize = uSer::Helper::wordSize<SWord>;
	static constexpr uSer::Bit inpWSize = uSer::Helper::wordSize<typename Raw::value_type>;
	std::vector<SWord> rawRef ((tc.raw.size ()*inpWSize + wSize - 1) / wSize);
	static_assert (wSize % inpWSize == 0);

	static_assert (Data::bits == uSer::rawStaticBits<SWord*, Data>, "Invalid rawStaticBits value!");
	static_assert (std::is_same_v<typename Data::template MaxDyn<wSize>, uSer::RawMaxDynamic<SWord*, Data>>, "Invalid MaxDyn value!");

	std::size_t wFact = wSize / inpWSize;

	std::cout << "Running testcase " << tc.file << ":" << std::setw(4) << tc.line << " with word size " << std::setw(2) << wSize << "... ";

//		uSer::serialize (std::begin (rawRef), tc.raw);
	for (std::size_t i = 0; i < rawRef.size (); ++i) {
		SWord collect = 0;
		std::size_t count;
		if (i == rawRef.size () - 1 && tc.raw.size () % wFact != 0) {
			count = tc.raw.size () % wFact;
		} else
			count = wFact;
		for (std::size_t j = 0; j < count; ++j) {
			collect = static_cast<SWord> ((SWord { tc.raw [i*wFact + j] } << (j * inpWSize)) | collect);
		}
		rawRef [i] = collect;
	}

	using BaseIter = typename std::vector<SWord>::iterator;
	using CBaseIter = typename std::vector<SWord>::const_iterator;
	bool okOut =	runTestcaseW_Out<SWord, wSize, Raw, Data, OutIterSingle<BaseIter>
#ifndef USER_TEST_QUICK
	, OutIterForward<BaseIter>, OutIterForwardSkip<BaseIter>
#endif
	> (rawRef, tc);
	bool okIn =		runTestcaseW_In<SWord, wSize, Raw, Data, InpIter<CBaseIter>
#ifndef USER_TEST_QUICK
	, InpIterSkip<CBaseIter>
#endif
	> (rawRef, tc);
	bool ok = okOut && okIn;

	if (ok) std::cout << "OK" << std::endl;
	return ok;
}

template <typename SWord, typename Raw, typename Data>
std::enable_if_t<(sizeof (typename Data::MaxSWord) % sizeof(SWord)) != 0, bool> runTestcaseW (const TestCase<Raw, Data>&) {
	return true;
}

template <typename... SWords, typename Raw, typename Data>
bool runTestcase (const TestCase<Raw, Data>& tc) {
	return (runTestcaseW<SWords> (tc) && ...);
}

struct RunTestcases {
	template <typename... TC>
	std::size_t operator () (const TC&... tc) {
		return (std::size_t { runTestcase<uint8_t
#ifndef USER_TEST_QUICK
			, uint16_t, uint32_t, uint64_t
#endif
			> (tc) } + ... );
	}
};

template <typename... T> constexpr auto getTestcases () { return std::tuple_cat (T::testcases () ...); }
#ifndef USER_TEST_QUICK
static auto allTestcases = getTestcases <T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T20, T21, T22, T30, T31, T32, T33, T40, T41, T60, T61, T62, T63, T64, T65, T66, T67<0>, T68, T69<0>, T70, T71<0>> ();
#else
static auto allTestcases = getTestcases <T1, T2, T5, T7, T8, T10, T15, T20, T30, T31, T32, T33, T40, T60> ();
#endif

static constexpr std::size_t numTestcases = std::tuple_size_v<decltype(allTestcases)>;

void testComparison () {
	T13 x { 2,  {{3, {4, 5, 6}, 3}, {2, { 4, 5 }, 0}}};
	T13 y { 2,  {{3, {4, 5, 6}, 3}, {2, { 4, 5 }, 0}}};
	verify (uSer::compare (x, y));
	y.a[1].arr[1] = 42;
	verify (!uSer::compare (x, y));
}

void testInfSize () {
	{
		T1 t {0x01234567, 0xEFCDAB89, 0x01234567 };
		std::array<uint8_t,12> rawRef {{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x23, 0x01, 0x67, 0x45 }};
		std::array<uint8_t, 12> raw;

		std::size_t sizeUsed;
		uSer::serialize(raw, t, uSer::infSize, &sizeUsed) ;
		verify (sizeUsed == 12);
		uSer::deserialize(rawRef, t, uSer::infSize, &sizeUsed);
		verify (sizeUsed == 12);

	}
	{
		std::array<uint8_t, 20> rawRef {{3, 0, 0, 0, 0x67, 0x45, 0x23, 0x01, 0x67, 0x45, 0x23, 0x01, 0x67, 0x45, 0x23, 0x01, 0xAA, 0x55, 0xAA, 0x55}};
		std::array<uint8_t, 20> raw;
		T10 t { 3, { 0x01234567, 0x01234567, 0x01234567 }, 0x55AA55AA };

		std::size_t sizeUsed;
#ifndef USER_EXCEPTIONS
		verify (uSer::serialize (raw, t, uSer::infSize, &sizeUsed) == uSer_EOK && sizeUsed == 20);
		verify (uSer::deserialize (rawRef, t, uSer::infSize, &sizeUsed) == uSer_EOK && sizeUsed == 20);
#else
		uSer::serialize (raw, t, uSer::infSize, &sizeUsed);
		verify (sizeUsed == 20);
		uSer::deserialize (rawRef, t, uSer::infSize, &sizeUsed);
		verify (sizeUsed == 20);
#endif
	}
}

void testFixedSize () {
	{
		T1 t {0x01234567, 0xEFCDAB89, 0x01234567 };
		std::array<uint8_t,12> rawRef {{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x23, 0x01, 0x67, 0x45 }};
		std::array<uint8_t, 12> raw;

		std::size_t sizeUsed;

		uSer::serialize(raw, t, uSer::fixedSize<12>, &sizeUsed);
		verify (sizeUsed == 12);
		uSer::deserialize(rawRef, t, uSer::fixedSize<12>, &sizeUsed);
		verify (sizeUsed == 12);

	}

	{
		std::array<uint8_t,20> rawRef {{ 3, 0, 0, 0, 0x67, 0x45, 0x23, 0x01, 0x67, 0x45, 0x23, 0x01, 0x67, 0x45, 0x23, 0x01, 0xAA, 0x55, 0xAA, 0x55 }};
		T10 t { 3, { 0x01234567, 0x01234567, 0x01234567 }, 0x55AA55AA };
		std::array<uint8_t, 20> rawOk;
		std::array<uint8_t, 19> rawShort;

		std::size_t sizeUsed;

#ifdef USER_EXCEPTIONS
		try {
			uSer::serialize (rawShort, t, uSer::fixedSize<10>, &sizeUsed);
			verify (false);
		} catch (const uSer::Exception& e) {
			assert (e.errorCode ==  uSer_EBUFSIZE);
		}
		try {
			uSer::deserialize (rawShort, t, uSer::fixedSize<10>, &sizeUsed);
			verify (false);
		} catch (const uSer::Exception& e) {
			assert (e.errorCode ==  uSer_EBUFSIZE);
		}
		uSer::serialize (rawOk, t, uSer::fixedSize<20>, &sizeUsed);
		verify (sizeUsed == 20);
		uSer::deserialize (rawRef, t, uSer::fixedSize<20>, &sizeUsed);
		verify (sizeUsed == 20);
#else
		verify (uSer::serialize (rawShort, t, uSer::fixedSize<10>, &sizeUsed) == uSer_EBUFSIZE);
		verify (uSer::deserialize (rawShort, t, uSer::fixedSize<10>, &sizeUsed) == uSer_EBUFSIZE);

		verify (uSer::serialize (rawOk, t, uSer::fixedSize<20>, &sizeUsed) == uSer_EOK && sizeUsed == 20);
		verify (uSer::deserialize (rawRef, t, uSer::fixedSize<20>, &sizeUsed) == uSer_EOK && sizeUsed == 20);
#endif
	}
}

void testDirectAttr () {
	uint8_t raw [2] {};
	uint16_t x = 0xABCD;
	uSer::serialize<uSer::ByteOrder::BE> (raw, x);
	verify (raw[0] == 0xAB && raw[1] == 0xCD);
	raw [0] = 0; raw [1] = 0;

	uSer::serialize<uSer::ByteOrder::BE> (raw, T53 { 0xABCD });
	verify (raw[0] == 0xAB && raw[1] == 0xCD);

	uSer::serialize<uSer::Padding::Fixed<8>> (raw, T53 { 0xABCD });
	verify (raw[0] == 0xCD && raw[1] == 0xAB);
}

void testRawInfo () {
	{
		uint16_t raw [2] = { 0xFFAA, 0xFFBB };
		uint8_t x [2];
		uSer::deserialize<uSer::RawInfo<uint16_t, 8>> (raw, x);
		verify (x [0] == 0xAA && x [1] == 0xBB);
	}
	{
		uint16_t raw [2] = { 0xFFFF, 0xFFFF };
		std::tuple<std::uint16_t, std::uint8_t> x;
		uSer::deserialize<uSer::RawInfo<uint16_t, 15>> (raw, x);

		verify (std::get<0> (x) == 0xFFFF && std::get<1> (x) == 0xFF);
	}
	{
		uint16_t raw [2] = { 0x7FFF, 0x1};
		std::tuple<std::uint16_t, std::uint8_t> x;
		uSer::deserialize<uSer::RawInfo<uint16_t, 15>> (raw, x);

		verify (std::get<0> (x) == 0xFFFF && std::get<1> (x) == 0x0);
	}
	{
		uint16_t raw [2] = { 0x0, 0x1FE };
		std::tuple<std::uint16_t, std::uint8_t> x;
		uSer::deserialize<uSer::RawInfo<uint16_t, 15>> (raw, x);

		verify (std::get<0> (x) == 0 && std::get<1> (x) == 0xFF);
	}
	{
		uint16_t raw [2] = { 0x0, 0xFE00 };
		std::tuple<std::uint16_t, std::uint8_t> x;
		uSer::deserialize<uSer::RawInfo<uint16_t, 15>> (raw, x);

		verify (std::get<0> (x) == 0 && std::get<1> (x) == 0);
	}
}

bool test () {
#ifndef USER_TEST_QUICK
	testComparison ();
	testInfSize ();
	testFixedSize ();
#endif
	testDirectAttr ();
	testRawInfo ();

	std::size_t res = std::apply (RunTestcases {}, allTestcases);
	if (res == numTestcases) {
		std::cout << "== All " << res << " testcases completed successfully. ==" << std::endl;
		return true;
	} else {
		std::cout << "== " << (numTestcases-res) << "/" << numTestcases << " testcases failed ==" << std::endl;
		return false;
	}
	return true;
}

int main () {
	return test () ? 0 : -1;
}

