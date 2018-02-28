/**
 * \file uSer.hh
 * \brief Main header file for the µSer Serialization library.
 * \author Niklas Gürtler
 * \copyright (C) 2018 Niklas Gürtler
 * Portions Copyright (C) 2012 William Swanson (see below)
 *
 * \copyright Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * \copyright The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * \copyright THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * \copyright Except as contained in this notice, the names of the authors or
 * their institutions shall not be used in advertising or otherwise to
 * promote the sale, use or other dealings in this Software without
 * prior written authorization from the authors.
 */

#ifndef USER_HH_
#define USER_HH_

#ifdef __cplusplus

#if (__cplusplus < 201703L)
#	error "The uSer library requires C++17 support."
#endif

#include <cstddef>
#include <array>
#include <type_traits>
#include <utility>
#include <cassert>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <memory>
#include <tuple>
#include <cstdint>
#include <limits>

#ifdef USER_EXCEPTIONS
#	include <stdexcept>
#	include <string>
#endif

/// Begin intrusive struct definition
#define USER_STRUCT(name,...)						using uSer_Self = name; using uSer_Attributes = ::uSer::DefAttr<__VA_ARGS__>;
/// Define attributes for single struct member
#define USER_MEM_ANNOT(name, ...)					static ::uSer::DefAttr<__VA_ARGS__> uSer_MemAttributes (::uSer::Helper::VTag<&uSer_Self:: name >);
/// List previously defined struct members
#define USER_ENUM_MEM(...)							using uSer_Members = ::uSer::StructMembers<USER_PPM_MAP_LIST(USER_ENUM_MEMPTR,uSer_Self,__VA_ARGS__)>;
/// Define and annotate struct member
#define USER_MEM(type,name,...)						type name; USER_MEM_ANNOT(name, __VA_ARGS__)
/// Define and annotate multiple struct members
#define USER_DEF_MEM(...)							USER_PPM_MAP(USER_DEF_MEM_I,,__VA_ARGS__) using uSer_Members = ::uSer::StructMembers<USER_PPM_MAP_LIST(USER_DEF_MEM_GET_MPTR,uSer_Self,__VA_ARGS__)>;

/// Annotate external data type
#define USER_EXT_ANNOT(name,...)					namespace uSerExtAnnot { template <> struct ExtAttr<name> { using Type = ::uSer::DefAttr<__VA_ARGS__>; }; }
/// Annotate external struct member
#define USER_EXT_MEM_ANNOT(sname,name, ...)			namespace uSerExtAnnot { template <> struct ExtMemAttr<&sname :: name> { using Type = ::uSer::DefAttr<__VA_ARGS__>; }; }
/// List external struct members
#define USER_EXT_ENUM_MEM(sname,...)				namespace uSerExtAnnot { template <> struct ExtStructMembers<sname> { using Type = ::uSer::StructMembers<USER_PPM_MAP_LIST(USER_ENUM_MEMPTR,sname,__VA_ARGS__)>; }; }
/// List and annotate external struct members
#define USER_EXT_DEF_MEM(sname,...)					USER_PPM_MAP(USER_EXT_ENUM_MEM_I,sname,__VA_ARGS__) namespace uSerExtAnnot { template <> struct ExtStructMembers <sname>{ using Type = ::uSer::StructMembers<USER_PPM_MAP_LIST(USER_EXT_ENUM_MEM_GET_MPTR,sname,__VA_ARGS__)>; }; }

#ifndef DOXYGEN

#define USER_LPAREN									(
#define USER_RPAREN									)
#define USER_ENUM_MEMPTR(name,sname)				&sname:: name
#define USER_DEF_MEM_I(data,dummy)					USER_PPM_EVAL0(USER_MEM data)
#define USER_DEF_MEM_GET_MPTR2(sname,type,name,...)	&sname:: name
#define USER_DEF_MEM_GET_MPTR(data,sname)			USER_PPM_EVAL0(USER_DEF_MEM_GET_MPTR2 USER_LPAREN sname, USER_PPM_EVAL0 data USER_RPAREN)


#define USER_EXT_ENUM_MEM_I(data,sname)				USER_PPM_EVAL0(USER_EXT_MEM_ANNOT USER_LPAREN sname, USER_PPM_EVAL0 data USER_RPAREN)
#define USER_EXT_ENUM_MEM_GET_MPTR2(sname,name,...)	&sname:: name
#define USER_EXT_ENUM_MEM_GET_MPTR(data,sname)		USER_PPM_EVAL0(USER_EXT_ENUM_MEM_GET_MPTR2 USER_LPAREN sname, USER_PPM_EVAL0 data USER_RPAREN)

#ifdef __GNUC__
#define USER_ALWAYS_INLINE  inline __attribute__((always_inline))
#define USER_RESTRICT		__restrict__
#else
#define USER_ALWAYS_INLINE
#define USER_RESTRICT
#endif

#endif

/// Namespace that collects external annotations. Contents is generated via the \ref USER_EXT_ANNOT macros.
namespace uSerExtAnnot {
	template <typename T>
	struct ExtAttr;
	template <auto T>
	struct ExtMemAttr;
	template <typename T>
	struct ExtStructMembers;
}
/// \defgroup Attr Attributes
/// \{
/// \}
/// µSer main API
namespace uSer {

	using Bit = std::size_t;
	using std::begin;
	using std::end;

	struct Attribute {};

	namespace Helper {
		template <auto X>
		struct VTag {};

		template <typename T>
		struct TTag {
			using Type = T;
		};

		template <typename T>
		struct IsArray2 {
			static constexpr bool value = false;
		};
		template <typename T, std::size_t N>
		struct IsArray2<T [N]> {
			static constexpr bool value = true;
		};
		template <typename T, std::size_t N>
		struct IsArray2<std::array<T, N>> {
			static constexpr bool value = true;
		};
		template <typename T>
		static constexpr bool IsArray = IsArray2<T>::value;

		template <typename T>
		using GetIfArray = std::enable_if_t<IsArray<T>, T>;

		template <typename T>
		struct GetArraySize2;
		template <typename T, std::size_t N>
		struct GetArraySize2<T [N]> {
			static constexpr std::size_t value = N;
		};
		template <typename T, std::size_t N>
		struct GetArraySize2<std::array<T, N>> {
			static constexpr std::size_t value = N;
		};
		template <typename T>
		static constexpr std::size_t arraySize = GetArraySize2<T>::value;

		template <typename T>
		struct ArrElType2;
		template <typename T, std::size_t N>
		struct ArrElType2<T [N]> {
			using Type = T;
		};
		template <typename T, std::size_t N>
		struct ArrElType2<std::array<T, N>> {
			using Type = T;
		};
		template <typename T>
		using ArrElType = typename ArrElType2<T>::Type;

		template <typename T>
		struct GetPointerTarget2;
		template <typename T>
		struct GetPointerTarget2<T*> {
			using Type = T;
		};
		template <typename T>
		struct GetPointerTarget2<const T*> {
			using Type = T;
		};
		template <typename T, typename Deleter>
		struct GetPointerTarget2<std::unique_ptr<T, Deleter>> {
			using Type = T;
		};
		template <typename T, typename Deleter>
		struct GetPointerTarget2<std::unique_ptr<T [], Deleter>> {
			using Type = T;
		};
		template <typename T, typename Deleter>
		struct GetPointerTarget2<std::unique_ptr<const T, Deleter>> {
			using Type = T;
		};
		template <typename T, typename Deleter>
		struct GetPointerTarget2<std::unique_ptr<const T [], Deleter>> {
			using Type = T;
		};
		template <typename T>
		struct GetPointerTarget2<std::shared_ptr<T>> {
			using Type = T;
		};
		template <typename T>
		struct GetPointerTarget2<std::shared_ptr<T []>> {
			using Type = T;
		};
		template <typename T, std::size_t N>
		struct GetPointerTarget2<std::shared_ptr<T [N]>> {
			using Type = T;
		};
		template <typename T>
		struct GetPointerTarget2<std::weak_ptr<T>> {
			using Type = T;
		};
		template <typename T>
		struct GetPointerTarget2<std::shared_ptr<const T>> {
			using Type = T;
		};
		template <typename T>
		struct GetPointerTarget2<std::shared_ptr<const T []>> {
			using Type = T;
		};
		template <typename T, std::size_t N>
		struct GetPointerTarget2<std::shared_ptr<const T [N]>> {
			using Type = T;
		};
		template <typename T>
		struct GetPointerTarget2<std::weak_ptr<const T>> {
			using Type = T;
		};
		template <typename T>
		using GetPointerTarget = typename GetPointerTarget2<T>::Type;

		template <typename A, typename... All>
		struct CheckAttr {
			static constexpr bool value = ( 0 + ... + std::is_convertible_v<All, typename A::TagBase>) == 1;
		};

		template <std::size_t Index, typename Base, typename Attr>
		struct AttrSearchBaseEl {};

		template <typename IS, typename... T>
		struct AttrSearchBase;

		template <std::size_t... I, typename... T>
		struct AttrSearchBase<std::index_sequence<I...>, T...> : AttrSearchBaseEl<I, typename T::TagBase, T>... {};

		template <typename Word>
		static constexpr Bit wordSize = std::enable_if_t<std::is_unsigned_v<Word>, std::numeric_limits<Word>>::digits;

		template <typename T, Bit Count>
		constexpr T mask () {
			if constexpr (Count == wordSize<T>)
				return std::numeric_limits<T>::max ();
			else
				return (T {1}<<Count)-1;
		}

		template <typename T>
		struct IsChar2 {
			static constexpr bool value = false;
		};
		template <>
		struct IsChar2<char> {
			static constexpr bool value = true;
		};
		template <>
		struct IsChar2<signed char> {
			static constexpr bool value = true;
		};
		template <>
		struct IsChar2<unsigned char> {
			static constexpr bool value = true;
		};
		template <typename T>
		static constexpr bool IsChar = IsChar2<T>::value;

		template <typename Iter, typename Count>
		USER_ALWAYS_INLINE 	void advanceFwIter (Iter& iter, Count count, TTag<decltype (iter += count)>*) {
			using D = typename std::iterator_traits<Iter>::difference_type;
			if constexpr (std::numeric_limits<D>::digits < std::numeric_limits<Count>::digits) {
				static constexpr D maxD = std::numeric_limits<D>::max ();
				static constexpr Count maxC = static_cast<Count> (maxD);
				while (count >= maxC) {
					iter += maxD;
					count -= maxC;
				}
				if (count) {
					iter += static_cast<D> (count);
				}
			} else
				iter += count;
		}
		template <typename Iter, typename Count>
		USER_ALWAYS_INLINE void advanceFwIter (Iter& iter, Count count, ...) {
			while (count) {
				++iter;
				--count;
			}
		}
		template <typename Iter, typename Count>
		USER_ALWAYS_INLINE void advanceOutIter2 (Iter& iter, Count count, std::forward_iterator_tag) {
			advanceFwIter (iter, count, 0);
		}
		template <typename Iter, typename Count>
		USER_ALWAYS_INLINE void advanceOutIter2 (Iter& iter, Count count, ...) {
			while (count) {
				*iter = 0;
				++iter;
				--count;
			}
		}
		template <typename Iter, typename Count>
		void advanceOutIter (Iter& iter, Count count) {
			using Cat = typename std::iterator_traits<std::remove_reference_t<Iter>>::iterator_category;
			advanceOutIter2 (iter, count, Cat {});
		}
		template <typename Iter, typename Count>
		void advanceInIter (Iter& iter, Count count) {
			advanceFwIter (iter, count, 0);
		}

		template <typename... T>
		struct PackFirst2;

		template <typename First, typename... Rest>
		struct PackFirst2<First, Rest...> {
			using Type = First;
		};

		template <typename... T>
		using PackFirst = typename PackFirst2<T...>::Type;
	}


	template <typename... Attr>
	struct Attributes : Helper::AttrSearchBase<std::make_index_sequence<sizeof...(Attr)>, Attr...> {
		static_assert ((std::is_convertible_v<Attr, Attribute> && ...), "Something that is not an attribute was passed to uSer::Attributes<...>");
		static_assert ((Helper::CheckAttr<Attr, Attr...>::value && ...), "Ambiguous attributes specified!");

		template <typename Base, typename Default, std::size_t I, typename Search>
		static constexpr Search findByBase (const Helper::AttrSearchBaseEl<I, Base, Search>&);

		template <typename, typename Default>
		static constexpr Default findByBase (...);

		template <typename Base, typename Default = void>
		using FindByBase = decltype(findByBase<Base, Default> (std::declval<Attributes> ()));
	};

	/**
	 * \ingroup Attr
	 * Dummy declaration for empty attribute list, to be used when macro variadic parameters can't be empty (e.g. with -pedantic on GCC)
	 */
	using AttrNone = Attributes<>;

	namespace Helper {
		template <typename Class, typename Mem>
		TTag<Mem> getMemVarPtrTargetTypeI (Mem Class::*ptr);

		template <typename Class, typename Mem>
		TTag<Class> getMemVarPtrClassTypeI (Mem Class::*ptr);

		template <auto MPtr>
		using MemVarPtrTargetType = typename decltype (getMemVarPtrTargetTypeI (MPtr))::Type;

		template <auto MPtr>
		using MemVarPtrClassType = typename decltype (getMemVarPtrClassTypeI (MPtr))::Type;

		template <typename Struct, typename MemberType, MemberType Struct::*MemPtr>
		auto getMemAttributesI (decltype (Struct::uSer_MemAttributes (std::declval<VTag<MemPtr>> ()))* p) -> std::remove_pointer_t<decltype (p)>;

		template <typename Struct, typename MemberType, MemberType Struct::*MemPtr>
		auto getMemAttributesI (typename uSerExtAnnot::ExtMemAttr<MemPtr>::Type* p) -> std::remove_pointer_t<decltype (p)>;

		template <typename Struct, typename MemberType, MemberType Struct::*MemPtr>
		AttrNone getMemAttributesI (...);

		template <typename Struct>
		auto getStructMembersI (typename Struct::uSer_Members* p) -> std::remove_pointer_t<decltype (p)>;

		template <typename Struct>
		auto getStructMembersI (typename uSerExtAnnot::ExtStructMembers<Struct>::Type* p) -> std::remove_pointer_t<decltype (p)>;

		template <typename Struct>
		auto getTypeAttributes (typename Struct::uSer_Attributes* p) -> std::remove_pointer_t<decltype (p)>;

		template <typename Type>
		auto getTypeAttributes (typename uSerExtAnnot::ExtAttr<Type>::Type* p) -> std::remove_pointer_t<decltype (p)>;

		template <typename Struct>
		AttrNone getTypeAttributes (...);

		template <typename... Attr>
		struct ConcatAttrI;

		template <>
		struct ConcatAttrI<> {
			using Type = AttrNone;
		};
		template <typename L0>
		struct ConcatAttrI<L0> {
			using Type = L0;
		};
		template <typename... L0, typename... L1>
		struct ConcatAttrI<Attributes<L0...>, Attributes<L1...>> {
			using Type = Attributes<L0..., L1...>;
		};
		template <typename... L0, typename... L1, typename... L2>
		struct ConcatAttrI<Attributes<L0...>, Attributes<L1...>, Attributes<L2...>> {
			using Type = Attributes<L0..., L1..., L2...>;
		};
		template <typename... L0, typename... L1, typename... L2, typename... L3>
		struct ConcatAttrI<Attributes<L0...>, Attributes<L1...>, Attributes<L2...>, Attributes<L3...>> {
			using Type = Attributes<L0..., L1..., L2..., L3...>;
		};
		template <typename... L0, typename... L1, typename... L2, typename... L3, typename... L4>
		struct ConcatAttrI<Attributes<L0...>, Attributes<L1...>, Attributes<L2...>, Attributes<L3...>, Attributes<L4...>> {
			using Type = Attributes<L0..., L1..., L2..., L3..., L4...>;
		};
		template <typename... L0, typename... L1, typename... L2, typename... L3, typename... L4, typename... L5>
		struct ConcatAttrI<Attributes<L0...>, Attributes<L1...>, Attributes<L2...>, Attributes<L3...>, Attributes<L4...>, Attributes<L5...>> {
			using Type = Attributes<L0..., L1..., L2..., L3..., L4..., L5...>;
		};
		template <typename... L0, typename... L1, typename... L2, typename... L3, typename... L4, typename... L5, typename... L6>
		struct ConcatAttrI<Attributes<L0...>, Attributes<L1...>, Attributes<L2...>, Attributes<L3...>, Attributes<L4...>, Attributes<L5...>, Attributes<L6...>> {
			using Type = Attributes<L0..., L1..., L2..., L3..., L4..., L5..., L6...>;
		};
		template <typename... L0, typename... L1, typename... L2, typename... L3, typename... L4, typename... L5, typename... L6, typename... Rest>
		struct ConcatAttrI<Attributes<L0...>, Attributes<L1...>, Attributes<L2...>, Attributes<L3...>, Attributes<L4...>, Attributes<L5...>, Attributes<L6...>, Rest...> {
			using Type = typename ConcatAttrI<Attributes<L0..., L1..., L2..., L3..., L4..., L5..., L6...>, Rest...>::Type;
		};
	}
	template <typename... Attr>
	using ConcatAttr = typename Helper::ConcatAttrI<Attr...>::Type;

	template <auto MemPtr>
	using GetMemAttributes = decltype (Helper::getMemAttributesI<Helper::MemVarPtrClassType<MemPtr>, Helper::MemVarPtrTargetType<MemPtr>, MemPtr> (0));

	template <typename Struct>
	using GetStructMembers = decltype (Helper::getStructMembersI<Struct> (0));

	template <typename Type>
	using GetTypeAttributes = decltype (Helper::getTypeAttributes<Type> (0));

	namespace Helper {
		template <bool KeepUninheritable, typename G, typename... S>
		using InheritSingle = std::conditional_t<(!G::inheritable && !KeepUninheritable) || (std::is_convertible_v<S, typename G::TagBase> || ...), AttrNone, Attributes<G>>;

		template <typename G, typename S, bool KeepUninheritable>
		struct InheritAttr;

		template <typename... G, typename... S, bool KeepUninheritable>
		struct InheritAttr<Attributes<G...>, Attributes<S...>, KeepUninheritable> {
			using Type = ConcatAttr<InheritSingle<KeepUninheritable, G, S...>..., Attributes<S...>>;
		};

		template <typename A>
		struct DefAttr2 {
			using Type = Attributes<A>;
		};
		template <typename... A>
		struct DefAttr2<Attributes<A...>> {
			using Type = Attributes<A...>;
		};

		template <typename A>
		struct DescendAttr2;

		template <typename... A>
		struct DescendAttr2<Attributes<A...>> {
			using Type = ConcatAttr<std::conditional_t<A::inheritable, Attributes<A>, AttrNone>...>;
		};

		template <typename A, typename... Tag>
		struct AttrRemoveTag2;

		template <typename A, typename... Tag>
		using AttrRemoveTag3 = std::conditional_t<(std::is_convertible_v<A, Tag> || ... ), AttrNone, Attributes<A>>;

		template <typename... A, typename... Tag>
		struct AttrRemoveTag2<Attributes<A...>, Tag...> {
			using Type = ConcatAttr<AttrRemoveTag3<A, Tag...>...>;
		};

		template <typename T, T Ref>
		struct RefContainer {
			using RefType = T;
			static constexpr RefType ref = Ref;
		};
	}

	template <typename... A>
	using DefAttr = ConcatAttr<typename Helper::DefAttr2<A>::Type...>;

	template <typename General, typename Special, bool KeepUninheritable = false>
	using Inherit = typename Helper::InheritAttr<General, Special, KeepUninheritable>::Type;

	template <typename Attr>
	using DescendAttr = typename Helper::DescendAttr2<Attr>::Type;

	template <typename Attr, typename... Tag>
	using AttrRemoveTag = typename Helper::AttrRemoveTag2<Attr, Tag...>::Type;

	/// Byte orders for integers in serialized data
	namespace ByteOrder {
		struct Tag : Attribute { using TagBase = Tag; static constexpr bool inheritable = true; };

		// Map: Index in raw data stream (Byte) => position in integer (Bit)

		/**
		 * \brief Attribute: Store an integer as Little Endian.
		 *
		 * Little endian stores the least significant bytes first (e.g. x86, AVR, most ARM implementations). For example, the 32bit-integer
		 * 0x01234567 is split into the bytes 0x67, 0x45, 0x23, 0x01, in that order.
		 * \ingroup Attr
		 */
		struct LE : public Tag {
			static constexpr Bit byteSizeBase = 8;
			// API
			using Byte = std::uint_least8_t;
			template <Bit Size> static constexpr Bit map (Bit serByte, std::integral_constant<std::size_t, Size>) { return serByte * byteSizeBase; }
			template <Bit Width> static constexpr std::size_t numBytes (std::integral_constant<Bit, Width>) { return (Width + byteSizeBase - 1) / byteSizeBase; }
			template <Bit Width, Bit I> static constexpr Bit byteSize (std::integral_constant<Bit, Width> w, std::integral_constant<std::size_t, I>) {
				return Width % byteSizeBase == 0 ? byteSizeBase :
					((I == numBytes (w) - 1) ? Width % byteSizeBase : byteSizeBase);
			}
		};
		/**
		 * \brief Attribute: Store an integer as Big Endian.
		 *
		 * Big endian stores the most significant bytes first (e.g. PowerPC, network byte order for IP,TCP,UDP). For example, the 32bit-integer
		 * 0x01234567 is split into the bytes 0x01, 0x23, 0x45, 0x67, in that order.
		 * \ingroup Attr
		 */
		struct BE : public Tag {
			static constexpr Bit byteSizeBase = 8;
			using Byte = std::uint_least8_t;
			template <Bit Size> static constexpr Bit map (Bit serByte, std::integral_constant<std::size_t, Size>) { return (Size-serByte-1) * byteSizeBase;	}
			template <Bit Width> static constexpr std::size_t numBytes (std::integral_constant<Bit, Width>) { return (Width + byteSizeBase - 1) / byteSizeBase; }
			template <Bit Width, Bit I> static constexpr Bit byteSize (std::integral_constant<Bit, Width>, std::integral_constant<std::size_t, I>) {
				return Width % byteSizeBase == 0 ? byteSizeBase :
					((I == 0) ? Width % byteSizeBase : byteSizeBase);
			}
		};
		/**
		 * \brief Attribute: Store an integer as PDP-Endian.
		 *
		 * PDP endian is a hybrid variant that stores the most significant 16bit word first and the least significant 16bit word second, both as little endian.
		 * For example, the 32bit-integer 0x01234567 is split into the bytes 0x23, 0x01, 0x67, 0x45, in that order.
		 * \ingroup Attr
		 */
		struct PDP : public Tag {
			static constexpr Bit byteSizeBase = 8;
			using Byte = std::uint_least8_t;
			template <Bit Size> static constexpr Bit map (Bit serByte, std::integral_constant<std::size_t, Size>) {
				static_assert (Size == 1 || Size == 2 || Size == 4, "PDP-Endian is only defined for sizes 8/16/32 bit");
				return (serByte ^ Bit { 2 }) * byteSizeBase;
			}
			template <Bit Width> static constexpr std::size_t numBytes (std::integral_constant<Bit, Width>) {
				static_assert (Width == 8 || Width == 16 || Width == 32, "PDP-Endian is only defined for sizes 8/16/32 bit");
				return Width / byteSizeBase;
			}
			template <Bit Width, Bit I> static constexpr Bit byteSize (std::integral_constant<Bit, Width>, std::integral_constant<std::size_t, I>) {
				static_assert (Width == 8 || Width == 16 || Width == 32, "PDP-Endian is only defined for sizes 8/16/32 bit");
				return byteSizeBase;
			}
		};

		/// This alias only exists if the compiler provides a __BYTE_ORDER__ macro. It is an alias for \ref LE, \ref BE or \ref PDP depending on the host byte order. \ingroup Attr
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		using Native = LE;
#endif
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		using Native = BE;
#endif
#if __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
		using Native = PDP;
#endif
	}
	/// Formats for signed integers
	namespace SignFormat {
		struct Tag : Attribute { static constexpr bool inheritable = true; using TagBase = Tag; };

		/**
		 * \brief Attribute: Store a signed integer in Two's Complement format.
		 *
		 * This is the most popular format, used on x86, AVR, ARM. When applied to a \f$k\f$-Bit-Integer, let \f$x_{k-1},x_{k-2},\dots,x_0 \f$ denote
		 * its raw bits with \f$x_{k-1}\f$ being the most significant and \f$x_0\f$ the least significant one. Then \f$x_{k-1}\f$ is the sign bit. The
		 * value of the integer is:
		 * \f[
		 * X = \sum_{i=0}^{k-2}{2^i \cdot x_i} - \begin{cases}
		 * 		0 & x_{k-1} = 0\\
		 * 		2^{k-1} & x_{k-1} = 1 \\
		 * \end{cases}
		 * \f]
		 * The range for a signed \f$k\f$-Bit-Integer with Two's Complement is \f$[-2^{k-1};2^{k-1}-1]\f$.
		 * \ingroup Attr
		 */
		struct TwosComplement : Tag {
			// Mask for the sign bit.
			template <Bit Width, typename U>
			static constexpr U signBit = U { 1 } << (Width-1);

			// The maximum value of the 2's complement, as signed integer
			template <Bit Width, typename U, typename S>
			static constexpr S signedMax = static_cast<S> (signBit<Width, U> - 1);

			// Only minimum has to be checked, since 2's complement's maximum does not exceed the guaranteed limit for integers (§5.2.4.2.1 C Standard)
			// Add maximum to signed minimum and check whether the result is too big, i.e. the range too small. This avoids an overflow.
			template <Bit Width, typename U, typename S>
			static constexpr bool checkRange = std::numeric_limits<U>::digits >= Width && (std::numeric_limits<S>::min () + signedMax<Width, U, S>) < 0;

			// The minimum value of the 2's complement, as signed integer. The above check makes sure it is representable.
			template <Bit Width, typename U, typename S>
			static constexpr S signedMin = static_cast<S> (-signedMax<Width, U, S> - 1);


			template <Bit Width, typename U>
			USER_ALWAYS_INLINE static std::make_signed_t<U> decode (const U& in) {
				using S = std::make_signed_t<U>;
				static_assert (checkRange <Width, U, S>, "Signed integer type can not represent all numbers of deserialized two's complement number");

				if (in & signBit<Width, U>) {
					// The inner cast is valid because of the minimum guaranteed range for signed integers.
					return static_cast<S> (static_cast<S> (in & Helper::mask<U, Width-1> ()) + signedMin<Width, U, S>);
				} else {
					// The cast is valid because of the minimum guaranteed range for signed integers.
					return static_cast<S> (in & Helper::mask<U, Width> ());
				}
			}
			template <Bit Width, typename S>
			USER_ALWAYS_INLINE static std::make_unsigned_t<S> encode (const S& in) {
				using U = std::make_unsigned_t<S>;
				static_assert (checkRange <Width, U, S>, "Signed integer type can not represent all numbers to be serialized as two's complement number");

				// Check data to be serialized for correct range
				assert ((in <= signedMax<Width, U, S>) && (in >= signedMin<Width, U, S>));

				// Casting Signed->Unsigned always yields the 2's complement (§7.8.2 C++17 Draft)
				return static_cast<U> (in) & Helper::mask<U, Width> ();
			}
		};
		/**
		 * \brief Attribute: Store a signed integer in Signed-Magnitude format.
		 *
		 * When applied to a \f$k\f$-Bit-Integer, let \f$x_{k-1},x_{k-2},\dots,x_0 \f$ denote
		 * its raw bits with \f$x_{k-1}\f$ being the most significant and \f$x_0\f$ the least significant one. Then \f$x_{k-1}\f$ is the sign bit. The
		 * value of the integer is:
		 * \f[
		 * X = (1 - 2 \cdot x_{k-1}) \cdot \sum_{i=0}^{k-2}{2^i \cdot x_i}
		 * \f]
		 * The range for a signed \f$k\f$-Bit-Integer in Signed-Magnitude format is \f$[-2^{k-1}+1;2^{k-1}-1]\f$. There are 2 representations for zero.
		 * \ingroup Attr
		 */
		struct SignedMagnitude : Tag {
			// Mask for the sign bit.
			template <Bit Width, typename U>
			static constexpr U signBit = U { 1 } << (Width-1);

			// The maximum value of signed magnitude, as signed integer
			template <Bit Width, typename U, typename S>
			static constexpr S signedMax = static_cast<S> (signBit<Width, U> - 1);

			// The minimum value of signed magnitude, as signed integer.
			template <Bit Width, typename U, typename S>
			static constexpr S signedMin = static_cast<S> (-signedMax<Width, U, S>);


			template <Bit Width, typename U>
			USER_ALWAYS_INLINE static std::make_signed_t<U> decode (const U& in) {
				using S = std::make_signed_t<U>;
				// If the number of bits is sufficient, the range is guaranteed to be large enough (§5.2.4.2.1 C Standard)
				static_assert (std::numeric_limits<U>::digits >= Width, "Signed integer type can not represent all numbers of deserialized signed-magnitude number");

				if (in & signBit<Width, U>) {
					return static_cast<S> (-static_cast<S> (in & Helper::mask<U, Width-1> ()));
				} else {
					return static_cast<S> (in);
				}
			}
			template <Bit Width, typename S>
			USER_ALWAYS_INLINE static std::make_unsigned_t<S> encode (const S& in) {
				using U = std::make_unsigned_t<S>;
				// If the number of bits is sufficient, the range is guaranteed to be large enough (§5.2.4.2.1 C Standard)
				static_assert (std::numeric_limits<U>::digits >= Width, "Signed integer type can not represent all numbers to be serialized as signed-magnitude number");

				// Check data to be serialized for correct range
				assert ((in <= signedMax<Width, U, S>) && (in >= signedMin<Width, U, S>));

				if (in < 0) {
					return static_cast<U> (-in) | signBit<Width, U>;
				} else {
					return static_cast<U> (in);
				}
			}
		};
		/**
		 * \brief Attribute: Store a signed integer in One's Complement format.
		 *
		 * When applied to a \f$k\f$-Bit-Integer, let \f$x_{k-1},x_{k-2},\dots,x_0 \f$ denote
		 * its raw bits with \f$x_{k-1}\f$ being the most significant and \f$x_0\f$ the least significant one. Then \f$x_{k-1}\f$ is the sign bit. The
		 * value of the integer is:
		 * \f[
		 * X = (1 - 2 \cdot x_{k-1}) \cdot \sum_{i=0}^{k-2}{2^i \cdot y_i}
		 * \f]
		 * with
		 * \f[
		 * y_i = \begin{cases}
		 * 	x_i & x_{k-1} = 0\\
		 * 	1-x_i & x_{k-1} = 1 \\
		 * 	\end{cases}
		 * \f]
		 * The range for a signed \f$k\f$-Bit-Integer in One's complement format is \f$[-2^{k-1}+1;2^{k-1}-1]\f$. There are 2 representations for zero.
		 * \ingroup Attr
		 */
		struct OnesComplement : Tag {
			// Mask for the sign bit.
			template <Bit Width, typename U>
			static constexpr U signBit = U { 1 } << (Width-1);

			// The maximum value of one's complement, as signed integer
			template <Bit Width, typename U, typename S>
			static constexpr S signedMax = static_cast<S> (signBit<Width, U> - 1);

			// The minimum value of one's complement, as signed integer.
			template <Bit Width, typename U, typename S>
			static constexpr S signedMin = static_cast<S> (-signedMax<Width, U, S>);

			template <Bit Width, typename U>
			USER_ALWAYS_INLINE static std::make_signed_t<U> decode (const U& in) {
				using S = std::make_signed_t<U>;
				// If the number of bits is sufficient, the range is guaranteed to be large enough (§5.2.4.2.1 C Standard)
				static_assert (std::numeric_limits<U>::digits >= Width, "Signed integer type can not represent all numbers of deserialized one's complement number");

				if (in & signBit<Width, U>) {
					return static_cast<S> (-static_cast<S> ((in & Helper::mask<U, Width-1> ()) ^ Helper::mask<U, Width-1> ()));
				} else {
					return static_cast<S> (in);
				}
			}
			template <Bit Width, typename S>
			USER_ALWAYS_INLINE static std::make_unsigned_t<S> encode (const S& in) {
				using U = std::make_unsigned_t<S>;
				// If the number of bits is sufficient, the range is guaranteed to be large enough (§5.2.4.2.1 C Standard)
				static_assert (std::numeric_limits<U>::digits >= Width, "Signed integer type can not represent all numbers to be serialized as one's complement number");

				// Check data to be serialized for correct range
				assert ((in <= signedMax<Width, U, S>) && (in >= signedMin<Width, U, S>));

				if (in < 0) {
					return static_cast<U> (-in) ^ Helper::mask<U, Width> ();
				} else {
					return static_cast<U> (in);
				}
			}
		};
		struct UnsignedDummy {
			template <Bit Width, typename U>
			USER_ALWAYS_INLINE static constexpr U decode (const U& in) {
				static_assert (std::numeric_limits<U>::digits >= Width, "Unsigned integer type too small");
				return in;
			}
			template <Bit Width, typename U>
			USER_ALWAYS_INLINE static constexpr U encode (const U& in) {
				static_assert (std::numeric_limits<U>::digits >= Width, "Unsigned integer type too small");
				return in;
			}
		};
	}
	namespace Padding {
		struct Tag : Attribute { static constexpr bool inheritable = false; using TagBase = Tag; };

		/// Attribute: Store no padding bits after the actual bits of an integer. This is the default. Can only be applied to integers. \ingroup Attr
		struct None : public Tag {
			template <typename Type, Bit Width>
			USER_ALWAYS_INLINE static constexpr Bit padding () { return 0; }
		};
		/// Attribute: Store a fixed number of padding bits after the actual bits of an integer. Can only be applied to integers. \ingroup Attr
		template <Bit Count>
		struct Fixed : public Tag {
			template <typename Type, Bit Width>
			USER_ALWAYS_INLINE static constexpr Bit padding () { return Count; }
		};
	}
	namespace Dyn {
		struct Tag : Attribute { using TagBase = Tag; static constexpr bool inheritable = false; };

		/**
		 * \brief An attribute that defines the size of a container that is a member of a struct to depend on external data, typically previously deserialized.
		 *
		 * This can be applied to C arrays (possibly pointed to by (smart) pointers), std::array, or any other container that supports iterators.
		 * The reference can be:
		 * -# A non-static member function of the surrounding struct
		 * -# A free function (=function pointer)
		 * -# A static member function of any class (=function pointer)
		 * -# A pointer to a global object that has operator () overloaded
		 * -# A member variable pointer to a member of the surrounding struct, which must be convertible to std::size_t.
		 * In the first four cases, the return type must be convertible to std::size_t. For cases 2-4, a reference to the struct
		 * is passed as an argument.
		 * \ingroup Attr
		 */
		template <auto Ref>
		struct Size : Tag {};

		/**
		 * \brief An attribute that decides whether a member of a struct is (de)serialized at all depending on external data, typically previously deserialized.
		 *
		 * This can be applied to any type.
		 * The reference can be:
		 * -# A non-static member function of the surrounding struct
		 * -# A free function (=function pointer)
		 * -# A static member function of any class (=function pointer)
		 * -# A pointer to a global object that has operator () overloaded
		 * -# A member variable pointer to a member of the surrounding struct, which must be convertible to bool.
		 * In the first four cases, the return type must be convertible to bool. For cases 2-3, a reference to the struct
		 * is passed as an argument.
		 * \ingroup Attr
		 */
		template <auto Ref>
		struct Optional : Tag {};
	}
	/// Attributes to define hook functions for uSer
	namespace Hook {
		struct SerPreTag : Attribute { static constexpr bool inheritable = false; using TagBase = SerPreTag; };
		struct SerPostTag : Attribute { static constexpr bool inheritable = false; using TagBase = SerPostTag; };
		struct DeSerPreTag : Attribute { static constexpr bool inheritable = false; using TagBase = DeSerPreTag; };
		struct DeSerPostTag : Attribute { static constexpr bool inheritable = false; using TagBase = DeSerPostTag; };

		/**
		 * \brief Attribute: Define a function to be called before the object (which can but need not be a struct member) is serialized.
		 *
		 * The function can be:
		 * -# A non-static constant member function of the surrounding struct
		 * -# A free function (=function pointer)
		 * -# A static member function of any class (=function pointer)
		 * -# A pointer to a global object that has operator () overloaded
		 * -# If the annotated object is a struct, and the annotation is *not* given as a member annotation of a surrounding struct,
		 * 		the reference may be a non-static constant member function of that struct.
		 *
		 * The return type can be:
		 * -# void, in which case the function may indicate errors via exceptions, but not fail otherwise
		 * -# \ref ErrorCode, in which case the function may indicate errors by the returned value or via exceptions (if enabled). If the function
		 * 		indicates an error via a return value and exceptions are enabled, uSer will throw an exception that forwards the error code.
		 *
		 * A constant reference to the annotated object is passed as the first argument. For cases 2-4, a constant reference to the struct is passed
		 * as a second argument, if the attribute was applied to a struct member.
		 * \ingroup Attr
		 */
		template <auto Ref>
		struct SerPre : SerPreTag {
			using Get = Helper::RefContainer<decltype(Ref), Ref>;
		};
		/**
		 * \brief Attribute: Define a function to be called before the object (which can but need not be a struct member) is deserialized.
		 *
		 * The function can be:
		 * -# A non-static member function of the surrounding struct
		 * -# A free function (=function pointer)
		 * -# A static member function of any class (=function pointer)
		 * -# A pointer to a global object that has operator () overloaded
		 * -# If the annotated object is a struct, and the annotation is *not* given as a member annotation of a surrounding struct,
		 * 		the reference may be a non-static member function of that struct.
		 *
		 * The return type can be:
		 * -# void, in which case the function may indicate errors via exceptions, but not fail otherwise
		 * -# \ref ErrorCode, in which case the function may indicate errors by the returned value or via exceptions (if enabled). If the function
		 * 		indicates an error via a return value and exceptions are enabled, uSer will throw an exception that forwards the error code.
		 *
		 * A reference to the annotated object is passed as the first argument. For cases 2-4, a reference to the struct is passed
		 * as a second argument, if the attribute was applied to a struct member.
		 * \ingroup Attr
		 */
		template <auto Ref>
		struct DeSerPre : DeSerPreTag {
			using Get = Helper::RefContainer<decltype(Ref), Ref>;
		};
		/**
		 * \brief Attribute: Define a function to be called after the object (which can but need not be a struct member) is serialized.
		 *
		 * The function can be:
		 * -# A non-static constant member function of the surrounding struct
		 * -# A free function (=function pointer)
		 * -# A static member function of any class (=function pointer)
		 * -# A pointer to a global object that has operator () overloaded
		 * -# If the annotated object is a struct, and the annotation is *not* given as a member annotation of a surrounding struct,
		 * 		the reference may be a non-static constant member function of that struct.
		 *
		 * The return type can be:
		 * -# void, in which case the function may indicate errors via exceptions, but not fail otherwise
		 * -# \ref ErrorCode, in which case the function may indicate errors by the returned value or via exceptions (if enabled). If the function
		 * 		indicates an error via a return value and exceptions are enabled, uSer will throw an exception that forwards the error code.
		 *
		 * A constant reference to the annotated object is passed as the first argument. For cases 2-4, a constant reference to the struct is passed
		 * as a second argument, if the attribute was applied to a struct member.
		 * \ingroup Attr
		 */
		template <auto Ref>
		struct SerPost : SerPostTag {
			using Get = Helper::RefContainer<decltype(Ref), Ref>;
		};
		/**
		 * \brief Attribute: Define a function to be called after the object (which can but need not be a struct member) is deserialized.
		 *
		 * The function can be:
		 * -# A non-static member function of the surrounding struct
		 * -# A free function (=function pointer)
		 * -# A static member function of any class (=function pointer)
		 * -# A pointer to a global object that has operator () overloaded
		 * -# If the annotated object is a struct, and the annotation is *not* given as a member annotation of a surrounding struct,
		 * 		the reference may be a non-static member function of that struct.
		 *
		 * The return type can be:
		 * -# void, in which case the function may indicate errors via exceptions, but not fail otherwise
		 * -# \ref ErrorCode, in which case the function may indicate errors by the returned value or via exceptions (if enabled). If the function
		 * 		indicates an error via a return value and exceptions are enabled, uSer will throw an exception that forwards the error code.
		 *
		 * A reference to the annotated object is passed as the first argument. For cases 2-4, a reference to the struct is passed
		 * as a second argument, if the attribute was applied to a struct member.
		 * \ingroup Attr
		 */
		template <auto Ref>
		struct DeSerPost : DeSerPostTag {
			using Get = Helper::RefContainer<decltype(Ref), Ref>;
		};

		struct HookNone {
		};
		static constexpr HookNone hookNone;
		struct HookNoneGet {
			using Get = Helper::RefContainer<const HookNone&, hookNone>;
		};
	}


	namespace Helper {
		struct WidthTagBase : Attribute { using TagBase = WidthTagBase; };
		struct SizeDefTag {};
		struct RawInfoTag : Attribute { using TagBase = RawInfoTag; static constexpr bool inheritable = false; };
	}

	/// Attribute: Define the serialized size of an integer type. May not be larger than the C(++) data type used. \ingroup Attr
	template <Bit W>
	struct Width : Helper::WidthTagBase {
		static constexpr bool inheritable = true;
		static constexpr Bit width = W;
	};

	/**
	 * \brief Attribute: Declares the serialization word explicitly
	 *
	 * This is useful when std::iterator_traits<RawIter>::value_type is not defined.
	 * Optionally, the size of the integer can be declared explicitly.
	 * Only valid in the template parameter list to \ref uSer::serialize and \ref uSer::deserialize .
	 * \ingroup Attr
	 */
	template <typename SWord_, Bit wSize_ = Helper::wordSize<SWord_>>
	struct RawInfo : Helper::RawInfoTag {
		/// Alias for the passed serialization word
		using SWord = SWord_;
		/// Width of the serialization word. Remaining bits will be ignored/set to zero.
		static constexpr Bit wSize = wSize_;

		static_assert (std::is_integral_v<SWord> && std::is_unsigned_v<SWord>, "The serialization word must be an unsigned integer type!");
		static_assert (wSize <= (Helper::wordSize<SWord>), "The declared width of the serialization word may not be larger than the integer's size!");
	};

	/// Define the members of a struct by providing member pointers
	template <auto... MemPtrs>
	struct StructMembers {

	};

	/// Indicates a dynamic data structure whose maximum serialized size can not be pre-calculated (e.g. when containers are used)
	struct Unlimited {
		template <std::size_t, std::size_t = 0>
		using Multiply = Unlimited;
		USER_ALWAYS_INLINE static constexpr bool check (std::size_t) { return true; }
	};
	/// Indicates a fixed maximum size for a dynamic data structure
	template <std::size_t N>
	struct MaxSize {
		static_assert (N > 0, "Use NoDyn instead of MaxSize<0>");
		static constexpr std::size_t value = N;
		template <std::size_t M, std::size_t S = 0>
		using Multiply = MaxSize<(N+S)*M>;
		USER_ALWAYS_INLINE static constexpr bool check (std::size_t n) { return n <= N; }
	};
	/// Indicates a data structure with no dynamic data
	struct NoDyn {
		static constexpr std::size_t value = 0;
		template <std::size_t M, std::size_t S = 0>
		using Multiply = std::conditional_t<S != 0, MaxSize<M*S>, NoDyn>;
	};

	/**
	 * Instances of FixedSize can be used for the "size" parameter of \ref serialize and \ref deserialize
	 * to signify a compile-time known fixed raw buffer size.
	 */
	template <std::size_t Size>
	struct FixedSize : public Helper::SizeDefTag {
		static constexpr std::size_t size = Size;
		static constexpr bool fixed = true;
		USER_ALWAYS_INLINE constexpr FixedSize () {}
	};
	/**
	 * Instances of DynSize can be used for the "size" parameter of \ref serialize and \ref deserialize
	 * to signify a runtime-time known dynamic raw buffer size.
	 */
	struct DynSize : public Helper::SizeDefTag {
		/// Specifies the number of available serialization words dynamically.
		std::size_t size;
		USER_ALWAYS_INLINE constexpr DynSize (std::size_t s) : size (s) {}
		static constexpr bool fixed = false;
	};
	/**
	 * Instances of InfSize can be used for the "size" parameter of \ref serialize and \ref deserialize
	 * to signify an infinite raw buffer size, e.g. a socket or file handle.
	 */
	struct InfSize : public Helper::SizeDefTag {
		USER_ALWAYS_INLINE constexpr InfSize () {}
	};

	/// Error code for serialization functions
	enum class
#ifndef DOXYGEN
	[[nodiscard]]
#endif
	 ErrorCode : std::uint_least8_t {
		/// No error was indicated (success)
		OK = 0,
		/// The size of a dynamic data structure exceeds the allowed maximum
		EDYNRANGE = 1,
		/// Raw buffer overflow
		EBUFSIZE = 2,
		/// This error is never raised by uSer, but can be used by hooks.
		EHOOK = 3
	};
	static constexpr const char* errorMessages [4] = { "Success", "Dynamic data size out of range", "Raw buffer overflow", "User hook indicates error" };

	/// Returns a string describing the error indicated by the given error code.
	USER_ALWAYS_INLINE const char* getErrorMessage (ErrorCode ec) {
		auto raw = static_cast<std::underlying_type_t<ErrorCode>> (ec);
		assert (raw < sizeof (errorMessages) / sizeof (errorMessages [0]));
		return errorMessages [raw];
	}

#if defined(USER_EXCEPTIONS) || defined(DOXYGEN)
	/**
	 * Exception class used by µSer to signal errors. This is only supported if \ref USER_EXCEPTIONS is defined.
	 * Use the "what" function inherited from std::runtime_error to retrieve a textual description of the error.
	 * The result is equivalent to calling \ref getErrorMessage on the \ref errorCode member variable.
	 */
	class Exception : public std::runtime_error {
		public:
			Exception (ErrorCode code) : std::runtime_error (getErrorMessage(code)), errorCode (code) {}

			/// Describes the type of error.
			const ErrorCode errorCode;
	};
#endif

	namespace Helper {
		template <typename Struct, typename Return>
		USER_ALWAYS_INLINE Return invokeRef (Return (*fun) (const Struct&), const Struct& str) {
			return fun (str);
		}
		template <typename Struct, typename Return>
		USER_ALWAYS_INLINE Return invokeRef (Return (Struct::*fun) () const, const Struct& str) {
			return (str.*fun) ();
		}
		template <typename Struct, typename Target>
		USER_ALWAYS_INLINE Target invokeRef (Target Struct::*mptr, const Struct& str) {
			return str.*mptr;
		}
		template <typename Struct, typename Functional>
		USER_ALWAYS_INLINE auto invokeRef (Functional* f, const Struct& str) -> decltype(std::declval<Functional> () (str)) {
			return (*f) (str);
		}

		template <bool saveLast, typename RawInfo>
		USER_ALWAYS_INLINE typename RawInfo::SWord readIter (typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT out) {
			typename RawInfo::SWord data (*raw);
			if (saveLast) out = data;
			++raw;
			return data;
		}

		template <typename T, typename RawInfo, Bit width, Bit bitOffset, std::size_t... I>
		USER_ALWAYS_INLINE T extractInteger (typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT last, std::index_sequence<I...>) {
			using SWord = typename RawInfo::SWord;
			static constexpr Bit wSize = RawInfo::wSize;
			static constexpr Bit shiftOff = (bitOffset == 0) ? 0 : (wSize - bitOffset);
			static constexpr std::size_t LI = sizeof...(I) - 1; // Underflow doesn't matter because LI will never be used in that case
			static constexpr Bit lastBits = (bitOffset + width) % wSize;
			static constexpr SWord lastMask = (lastBits == 0) ? RawInfo::max : Helper::mask<SWord, lastBits> ();
			static constexpr SWord midMask = RawInfo::max;

			static_cast<void> (shiftOff);
			static_cast<void> (lastMask);
			static_cast<void> (midMask);

			T temp = bitOffset == 0 ? 0 : (static_cast<T> (last >> bitOffset) & Helper::mask<T, std::min<Bit> (wSize - bitOffset, width)> ());

			((temp = static_cast<T> (temp | (static_cast<T>(readIter<I == LI, RawInfo>(raw, last) & (I == LI ? lastMask : midMask)) << (I * wSize + shiftOff)))) , ... );

			return std::move (temp);
		}

		template <typename T, typename RawInfo, Bit width, Bit bitOffset>
		USER_ALWAYS_INLINE T extractInteger (typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT last) {
			static constexpr Bit wSize = RawInfo::wSize;
			static constexpr std::size_t inpWords = (bitOffset + width + wSize - 1) / wSize - (bitOffset != 0 ? 1 : 0);
			return extractInteger<T, RawInfo, width, bitOffset> (raw, last, std::make_index_sequence<inpWords> {});
		}

		template <typename T, typename RawInfo, Bit Width, typename ByteOrder, Bit bitOffset>
		struct IntegerDecoder {
			using SWord = typename RawInfo::SWord;
			typename RawInfo::RawIter& USER_RESTRICT raw;
			SWord& USER_RESTRICT last;
			T collect;
		};
		template <typename T, typename RawInfo, Bit Width, typename ByteOrder, Bit bitOffset, std::size_t I>
		USER_ALWAYS_INLINE decltype(auto) operator + (IntegerDecoder<T, RawInfo, Width, ByteOrder, bitOffset> d, std::integral_constant<std::size_t, I> i) {
			static constexpr Bit wSize = RawInfo::wSize;
			static constexpr std::size_t numBytes = ByteOrder::numBytes (std::integral_constant<Bit, Width> {});

			static constexpr Bit bs = ByteOrder::byteSize (std::integral_constant<Bit, Width> {}, i);
			return IntegerDecoder<T, RawInfo, Width, ByteOrder, (bitOffset + bs) % wSize> {
				d.raw, d.last, static_cast<T> (d.collect |
					static_cast<T> (extractInteger<typename ByteOrder::Byte, RawInfo, bs, bitOffset> (d.raw, d.last)) << ByteOrder::map (I, std::integral_constant<std::size_t, numBytes> {}))
			};
		}

		template <typename RawInfo, Bit Width, Bit bitOffset, typename ByteOrder, typename T, std::size_t... I>
		USER_ALWAYS_INLINE T decodeInteger (typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT last, std::index_sequence<I...>) {
			return (IntegerDecoder<T, RawInfo, Width, ByteOrder, bitOffset> { raw, last, 0 } + ... + std::integral_constant<std::size_t, I> {}).collect;
		}
		template <typename RawInfo, Bit Width, Bit Padding, Bit bitOffset, typename ByteOrder, typename SignFormat, typename T>
		std::enable_if_t<!std::is_same_v<T, bool>, void> decodeInteger (T& USER_RESTRICT out, typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT last) {
			using SWord = typename RawInfo::SWord;
			static constexpr Bit wSize = RawInfo::wSize;
			static constexpr std::size_t numBytes = ByteOrder::numBytes (std::integral_constant<Bit, Width> {});

			SWord l_last = last;
			out = SignFormat::template decode<Width> (decodeInteger<RawInfo, Width, bitOffset, ByteOrder, std::make_unsigned_t<T>> (raw, l_last, std::make_index_sequence<numBytes> {}));

			static constexpr std::size_t dataEndBit = (bitOffset + Width) % wSize;
			static constexpr std::size_t paddingBytes = (dataEndBit + Padding + wSize - 1) / wSize - (dataEndBit != 0 ? 1 : 0);

			if constexpr (paddingBytes != 0) {
				Helper::advanceInIter (raw, paddingBytes-1);
				l_last = *raw;
				++raw;
			}
			last = l_last;
		}
		template <typename RawInfo, Bit Width, Bit Padding, Bit bitOffset, typename ByteOrder, typename SignFormat>
		void decodeInteger (bool& USER_RESTRICT out, typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT last) {
			using SWord = typename RawInfo::SWord;
			static constexpr Bit wSize = RawInfo::wSize;
			static_assert (Width == 1, "Width must be 1 for boolean");

			SWord l_last;
			if constexpr (bitOffset == 0) {
				l_last = *raw;
				++raw;

				out = l_last & 1;
			} else {
				l_last = last;
				out = (l_last & (SWord { 1 } << bitOffset)) != 0;
			}

			static constexpr std::size_t dataEndBit = (bitOffset + Width) % wSize;
			static constexpr std::size_t paddingBytes = (dataEndBit + Padding + wSize - 1) / wSize - (dataEndBit != 0 ? 1 : 0);

			if constexpr (paddingBytes) {
				Helper::advanceInIter (raw, paddingBytes-1);
				l_last = *raw;
				++raw;
			}
			last = l_last;
		}

		template <typename RawInfo>
		USER_ALWAYS_INLINE void writeIter (typename RawInfo::RawIter& USER_RESTRICT raw, const typename RawInfo::SWord& USER_RESTRICT data) {
			*raw = data;
			++raw;
		}

		template <typename T, typename RawInfo, Bit Width, Bit bitOffset, std::size_t... I>
		USER_ALWAYS_INLINE void storeInteger (const T& USER_RESTRICT data, typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT last, std::index_sequence<I...>) {
			using SWord = typename RawInfo::SWord;
			static constexpr Bit wSize = RawInfo::wSize;
			static constexpr Bit shiftOff = (bitOffset == 0) ? 0 : (wSize - bitOffset);
			static constexpr Bit lastBits = (bitOffset + Width) % wSize;
			static constexpr T midMask = static_cast<T> (RawInfo::max); // If this overflows, midMask will not be used anyways.

			static_cast<void> (shiftOff);
			static_cast<void> (midMask);

			if (bitOffset + Width >= wSize && bitOffset > 0) {
				*raw = static_cast<SWord> (last | (static_cast<SWord>(data & mask<T, std::min<Bit> (Width, wSize - bitOffset)> ()) << bitOffset));
				++raw;
			}

			((writeIter<RawInfo> (raw, static_cast<SWord> (data >> (I * wSize + shiftOff) & midMask))) , ... );

			if constexpr (bitOffset + Width < wSize && bitOffset != 0)
				last = static_cast<SWord> (last | (static_cast<SWord> (data & mask<T, Width> ()) << bitOffset));
			else if constexpr (lastBits != 0)
				last = static_cast<SWord> ((data >> (Width - lastBits)) & mask<T, lastBits> ());
		}

		template <typename T, typename RawInfo, Bit width, Bit bitOffset>
		USER_ALWAYS_INLINE void storeInteger (const T& USER_RESTRICT data, typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT last) {
			static constexpr Bit wSize = RawInfo::wSize;
			static constexpr std::size_t outWords = (bitOffset + width) < wSize ? 0 : ((bitOffset + width) / wSize - (bitOffset != 0 ? 1 : 0));
			return storeInteger<T, RawInfo, width, bitOffset> (data, raw, last, std::make_index_sequence<outWords> {});
		}

		template <typename T, typename RawInfo, Bit Width, typename ByteOrder, Bit bitOffset>
		struct IntegerEncoder {
			using SWord = typename RawInfo::SWord;
			typename RawInfo::RawIter& raw;
			SWord& USER_RESTRICT last;
			const T& USER_RESTRICT data;
		};
		template <typename T, typename RawInfo, Bit Width, typename ByteOrder, Bit bitOffset, std::size_t I>
		USER_ALWAYS_INLINE decltype(auto) operator + (IntegerEncoder<T, RawInfo, Width, ByteOrder, bitOffset> d, std::integral_constant<std::size_t, I> i) {
			static constexpr Bit wSize = RawInfo::wSize;
			static constexpr std::size_t numBytes = ByteOrder::numBytes (std::integral_constant<Bit, Width> {});

			static constexpr Bit bs = ByteOrder::byteSize (std::integral_constant<Bit, Width> {}, i);

			storeInteger<typename ByteOrder::Byte, RawInfo, bs, bitOffset> (static_cast<typename ByteOrder::Byte> (d.data >> ByteOrder::map (I, std::integral_constant<std::size_t, numBytes> {})), d.raw, d.last);

			return IntegerEncoder<T, RawInfo, Width, ByteOrder, (bitOffset + bs) % wSize> { d.raw, d.last, d.data };
		}

		template <typename RawInfo, Bit Width, Bit bitOffset, typename ByteOrder, typename T, std::size_t... I>
		USER_ALWAYS_INLINE void encodeInteger (const T& USER_RESTRICT data, typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT last, std::index_sequence<I...>) {
			(IntegerEncoder<T, RawInfo, Width, ByteOrder, bitOffset> { raw, last, data } + ... + std::integral_constant<std::size_t, I> {});
		}
		template <typename RawInfo, Bit Width, Bit Padding, Bit bitOffset, typename ByteOrder, typename SignFormat, typename T>
		std::enable_if_t<!std::is_same_v<T, bool>, void> encodeInteger (const T& USER_RESTRICT data, typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT last) {
			using SWord = typename RawInfo::SWord;
			static constexpr Bit wSize = RawInfo::wSize;
			static constexpr std::size_t numBytes = ByteOrder::numBytes (std::integral_constant<Bit, Width> {});

			SWord l_last = last;
			encodeInteger<RawInfo, Width, bitOffset, ByteOrder, std::make_unsigned_t<T>> (SignFormat::template encode<Width> (data), raw, l_last, std::make_index_sequence<numBytes> {});

			static constexpr std::size_t dataEndBit = (bitOffset + Width) % wSize;
			static constexpr std::size_t paddingBytes = (dataEndBit + Padding) / wSize;

			if constexpr (paddingBytes != 0) {
				*raw = l_last;
				++raw;
				Helper::advanceOutIter (raw, paddingBytes-1);
				l_last = 0;
			}
			last = l_last;
		}

		template <typename RawInfo, Bit Width, Bit Padding, Bit bitOffset, typename ByteOrder, typename SignFormat>
		void encodeInteger (const bool& USER_RESTRICT data, typename RawInfo::RawIter& USER_RESTRICT raw, typename RawInfo::SWord& USER_RESTRICT last) {
			using SWord = typename RawInfo::SWord;
			static constexpr Bit wSize = RawInfo::wSize;
			static_assert (Width == 1, "Width must be 1 for boolean");

			SWord l_last = last;
			l_last = static_cast<SWord> (l_last | (SWord { data } << bitOffset));
			if (bitOffset + 1 == wSize) {
				*raw = l_last;
				++raw;
				l_last = 0;
			}

			static constexpr std::size_t dataEndBit = (bitOffset + Width) % wSize;
			static constexpr std::size_t paddingBytes = (dataEndBit + Padding) / wSize;

			if constexpr (paddingBytes) {
				*raw = l_last;
				++raw;
				Helper::advanceOutIter (raw, paddingBytes-1);
				l_last = 0;
			}
			last = l_last;
		}
	}

	namespace SWalk {
		template <typename T, typename Attr>
		static constexpr Bit intDeclWidth = Attr::template FindByBase<Helper::WidthTagBase, Width<std::numeric_limits<T>::digits + Bit { std::is_signed_v<T> }>>::width;

		template <typename T, typename Attr>
		static constexpr Bit intPaddedWidth = intDeclWidth<T, Attr> + Attr::template FindByBase<Padding::Tag, Padding::None>::template padding <T, intDeclWidth<T, Attr>> ();


		template <typename Algo, typename Obj, typename QObj, typename Struct, typename Hook>
		struct InvokeHook {
			static_assert (!std::is_class_v<Algo>, "Invalid hook function specified (possibly missing or having a superfluous \"const\")");
		};

		// Function Pointer / ErrorCode + Struct
		template <typename Algo, typename Obj, typename QObj, typename Struct, ErrorCode (*FPtr) (QObj&, typename Algo::template Qualify<Struct>&)>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<ErrorCode (*) (QObj&, typename Algo::template Qualify<Struct>&), FPtr>> {
			static constexpr bool canFail () { return true; }
			static constexpr bool enabled () { return true; }
			using Ret = ErrorCode;

			USER_ALWAYS_INLINE static ErrorCode invoke (QObj& obj, typename Algo::template Qualify<Struct>& str) {
				return FPtr (obj, str);
			}
		};
		// Function Pointer / void + Struct
		template <typename Algo, typename Obj, typename QObj, typename Struct, void (*FPtr) (QObj&, typename Algo::template Qualify<Struct>&)>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer <void (*) (QObj&, typename Algo::template Qualify<Struct>&), FPtr>> {
			static constexpr bool canFail () { return false; }
			static constexpr bool enabled () { return true; }
			using Ret = void;

			USER_ALWAYS_INLINE static void invoke (QObj& obj, typename Algo::template Qualify<Struct>& str) {
				FPtr (obj, str);
			}
		};
		// Member Function Pointer / ErrorCode + Struct
		template <typename Algo, typename Obj, typename QObj, typename Struct, ErrorCode (Struct::*MemFun) (QObj&)>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<ErrorCode (Algo::template Qualify<Struct>::*) (QObj&), MemFun>> {
			static constexpr bool canFail () { return true; }
			static constexpr bool enabled () { return true; }
			using Ret = ErrorCode;

			USER_ALWAYS_INLINE static ErrorCode invoke (QObj& obj, typename Algo::template Qualify<Struct>& str) {
				return (str.*MemFun) (obj);
			}
		};
		// const Member Function Pointer / ErrorCode + Struct
		template <typename Algo, typename Obj, typename QObj, typename Struct, ErrorCode (Struct::*MemFun) (QObj&) const>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<ErrorCode (Algo::template Qualify<Struct>::*) (QObj&) const, MemFun>> {
			static constexpr bool canFail () { return true; }
			static constexpr bool enabled () { return true; }
			using Ret = ErrorCode;

			USER_ALWAYS_INLINE static ErrorCode invoke (QObj& obj, typename Algo::template Qualify<Struct>& str) {
				return (str.*MemFun) (obj);
			}
		};

		// Member Function Pointer / void + Struct
		template <typename Algo, typename Obj, typename QObj, typename Struct, void (Struct::*MemFun) (QObj&)>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<void (Struct::*) (QObj&), MemFun>> {
			static constexpr bool canFail () { return false; }
			static constexpr bool enabled () { return true; }
			using Ret = void;

			USER_ALWAYS_INLINE static void invoke (QObj& obj, typename Algo::template Qualify<Struct>& str) {
				(str.*MemFun) (obj);
			}
		};
		// const Member Function Pointer / void + Struct
		template <typename Algo, typename Obj, typename QObj, typename Struct, void (Struct::*MemFun) (QObj&) const>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<void (Struct::*) (QObj&) const, MemFun>> {
			static constexpr bool canFail () { return false; }
			static constexpr bool enabled () { return true; }
			using Ret = void;

			USER_ALWAYS_INLINE static void invoke (QObj& obj, typename Algo::template Qualify<Struct>& str) {
				(str.*MemFun) (obj);
			}
		};

		// Functional + Struct
		template <typename Algo, typename Obj, typename QObj, typename Struct, typename Functional, Functional* FRef>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<Functional*, FRef>> {
			using Ret = decltype ((*FRef) (std::declval<QObj&> (), std::declval<typename Algo::template Qualify<Struct>&> ()));
			static constexpr bool canFail () { return !std::is_same_v<Ret, void>; }
			static constexpr bool enabled () { return true; }

			USER_ALWAYS_INLINE static Ret invoke (QObj& obj, typename Algo::template Qualify<Struct>& str) {
				return (*FRef) (obj, str);
			}
		};

		// Function Pointer / ErrorCode
		template <typename Algo, typename Obj, typename QObj, ErrorCode (*FPtr) (QObj&)>
		struct InvokeHook<Algo, Obj, QObj, void, Helper::RefContainer<ErrorCode (*) (QObj&), FPtr>> {
			static constexpr bool canFail () { return true; }
			static constexpr bool enabled () { return true; }
			using Ret = ErrorCode;

			USER_ALWAYS_INLINE static ErrorCode invoke (QObj& obj) {
				return FPtr (obj);
			}
		};
		// Function Pointer / void
		template <typename Algo, typename Obj, typename QObj, void (*FPtr) (QObj&)>
		struct InvokeHook<Algo, Obj, QObj, void, Helper::RefContainer <void (*) (QObj&), FPtr>> {
			static constexpr bool canFail () { return false; }
			static constexpr bool enabled () { return true; }
			using Ret = void;

			USER_ALWAYS_INLINE static void invoke (QObj& obj) {
				FPtr (obj);
			}
		};
		// Functional
		template <typename Algo, typename Obj, typename QObj, typename Functional, Functional* FRef>
		struct InvokeHook<Algo, Obj, QObj, void, Helper::RefContainer<Functional*, FRef>> {
			using Ret = decltype ((*FRef) (std::declval<QObj&> ()));
			static constexpr bool canFail () { return !std::is_same_v<Ret, void>; }
			static constexpr bool enabled () { return true; }

			USER_ALWAYS_INLINE static Ret invoke (QObj& obj) {
				return (*FRef) (obj);
			}
		};
		// Member Function Pointer / ErrorCode + Struct Self
		template <typename Algo, typename Obj, typename QObj, typename Struct, ErrorCode (Obj::*MemFun) ()>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<ErrorCode (Obj::*) (), MemFun>> {
			static constexpr bool canFail () { return true; }
			static constexpr bool enabled () { return true; }
			using Ret = ErrorCode;

			USER_ALWAYS_INLINE static ErrorCode invoke (QObj& obj) {
				return (obj.*MemFun) ();
			}
		};
		// const Member Function Pointer / ErrorCode + Struct Self
		template <typename Algo, typename Obj, typename QObj, typename Struct, ErrorCode (Obj::*MemFun) () const>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<ErrorCode (Obj::*) () const, MemFun>> {
			static constexpr bool canFail () { return true; }
			static constexpr bool enabled () { return true; }
			using Ret = ErrorCode;

			USER_ALWAYS_INLINE static ErrorCode invoke (QObj& obj) {
				return (obj.*MemFun) ();
			}
		};

		// Member Function Pointer / void + Struct Self
		template <typename Algo, typename Obj, typename QObj, typename Struct, void (Obj::*MemFun) ()>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<void (Obj::*) (), MemFun>> {
			static constexpr bool canFail () { return false; }
			static constexpr bool enabled () { return true; }
			using Ret = void;

			USER_ALWAYS_INLINE static void invoke (QObj& obj) {
				(obj.*MemFun) ();
			}
		};
		// const Member Function Pointer / void + Struct Self
		template <typename Algo, typename Obj, typename QObj, typename Struct, void (Obj::*MemFun) () const>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<void (Obj::*) () const, MemFun>> {
			static constexpr bool canFail () { return false; }
			static constexpr bool enabled () { return true; }
			using Ret = void;

			USER_ALWAYS_INLINE static void invoke (QObj& obj) {
				(obj.*MemFun) ();
			}
		};
		// No hook defined
		template <typename Algo, typename Obj, typename QObj, typename Struct>
		struct InvokeHook<Algo, Obj, QObj, Struct, Helper::RefContainer<const Hook::HookNone&, Hook::hookNone>> {
			static constexpr bool canFail () { return false; }
			static constexpr bool enabled () { return false; }
		};
		// No hook defined
		template <typename Algo, typename Obj, typename QObj>
		struct InvokeHook<Algo, Obj, QObj, void, Helper::RefContainer<const Hook::HookNone&, Hook::hookNone>> {
			static constexpr bool canFail () { return false; }
			static constexpr bool enabled () { return false; }
		};
		template <bool CanFail, typename SubRet>
		struct GetFailRet {
			using Type = SubRet;
		};
		template <typename SubRet>
		struct GetFailRet<true, SubRet> {
			using Type = typename SubRet::GetFailRet;
		};

		template <typename Struct, typename Obj, typename Algo, typename SubRet, typename Attr>
		struct InvokeHooks {
			using I_PreHook  = InvokeHook<Algo, Obj, typename Algo::template Qualify<Obj>, Struct, typename Attr::template FindByBase<typename Algo::PreHookTag, Hook::HookNoneGet>::Get>;
			using I_PostHook = InvokeHook<Algo, Obj, typename Algo::template Qualify<Obj>, Struct, typename Attr::template FindByBase<typename Algo::PostHookTag, Hook::HookNoneGet>::Get>;

			static constexpr bool canFail () { return I_PreHook::canFail () || I_PostHook::canFail (); }

			using Ret = typename GetFailRet<canFail (), SubRet>::Type;
		};

		/*
		 * Prototypes
		 */
		
		template <typename Descend, typename Attr>
		struct GetHookRet;

		template <typename Descend, typename Attr>
		typename GetHookRet<Descend, Attr>::Type walk (Descend desc, Attr a);

		/*
		 * Meta-Functions for return types
		 */

		template <typename Descend, typename Attr, typename El, typename Iter>
		struct GetContSubRet2 {
			using Type = decltype (walk (
				std::declval<Descend> ().template contElem<El, Iter> (
					std::declval<decltype(std::declval<Descend> ().template beginIterateCont<El, Iter> ())&> (),
					std::declval<int&> ()
				),
				std::declval<Attr> ()
			));
		};

		template <typename Descend, int DynMode, typename Container, typename SR, typename Attr>
		struct GetContRet2;

		template <typename Algo, typename Container, typename SR, typename Attr>
		struct GetContRet2<Algo, 1, Container, SR, Attr> {
			using Type = typename Algo::template MultRetDynamic<SR>;
		};
		template <typename Algo, typename Element, std::size_t Size, typename SR, typename Attr>
		struct GetContRet2<Algo, 0, Element [Size], SR, Attr> {
			using Type = typename Algo::template MultRetStatic<SR, Size>;
		};
		template <typename Algo, typename Element, std::size_t Size, typename SR, typename Attr>
		struct GetContRet2<Algo, 0, std::array<Element, Size>, SR, Attr> {
			using Type = typename Algo::template MultRetStatic<SR, Size>;
		};
		template <typename Algo, typename Element, std::size_t Size, typename SR, typename Attr>
		struct GetContRet2<Algo, 1, Element [Size], SR, Attr> {
			using Type = typename Algo::template MultRetStatic<SR, Size>;
		};
		template <typename Algo, typename Element, std::size_t Size, typename SR, typename Attr>
		struct GetContRet2<Algo, 1, std::array<Element, Size>, SR, Attr> {
			using Type = typename Algo::template MultRetStatic<SR, Size>;
		};
		template <typename Algo, typename Element, std::size_t Size, typename SR, typename Attr>
		struct GetContRet2<Algo, 2, Element [Size], SR, Attr> {
			using Type = typename Algo::template MultRetDynamicMax<SR, Size>;
		};
		template <typename Algo, typename Element, std::size_t Size, typename SR, typename Attr>
		struct GetContRet2<Algo, 2, std::array<Element, Size>, SR, Attr> {
			using Type = typename Algo::template MultRetDynamicMax<SR, Size>;
		};
		template <typename Algo, typename Container, typename SR, typename Attr>
		struct GetContRet2<Algo, 2, Container, SR, Attr> {
			using Type = typename Algo::template MultRetDynamic<SR>;
		};

		template <typename... T>
		std::enable_if_t<!(std::is_same_v<T, NoDyn> && ...), MaxSize<(T::value + ...)>> sumRetMaxDyn2 (T...);

		template <typename... T>
		std::enable_if_t<(std::is_same_v<T, NoDyn> && ...), NoDyn> sumRetMaxDyn2 (T...);

		template <std::size_t... N>
		MaxSize<(N + ...)> sumRetMaxDyn2 (MaxSize<N>...);

		Unlimited sumRetMaxDyn2 (...);

		template <typename... T>
		using SumRetMaxDyn = decltype (sumRetMaxDyn2 (std::declval<T> ()...));

		template <typename Obj, typename IS, typename Descend, typename Attr>
		struct GetTupleRet2;

		template <typename... Elem, std::size_t... I, typename Descend, typename Attr>
		struct GetTupleRet2<std::tuple<Elem...>, std::index_sequence<I...>, Descend, Attr> {
			using Type = typename Descend::Algo::template SumRet<decltype (walk (std::declval<Descend> ().template tupleElem<I, Elem> (), std::declval<Attr> ()))...>;
		};
		template <typename... Elem, std::size_t... I, typename Descend, typename Attr>
		struct GetTupleRet2<std::pair<Elem...>, std::index_sequence<I...>, Descend, Attr> {
			using Type = typename Descend::Algo::template SumRet<decltype (walk (std::declval<Descend> ().template tupleElem<I, Elem> (), std::declval<Attr> ()))...>;
		};

		template <typename Obj, typename Attr>
		struct GetIntRetBase {
			static constexpr Bit Width = intDeclWidth<Obj,Attr>;
			using BO = typename Attr::template FindByBase <ByteOrder::Tag, ByteOrder::LE>;
			using SF = std::conditional_t<std::is_signed_v<Obj>, typename Attr::template FindByBase <SignFormat::Tag, SignFormat::TwosComplement>, SignFormat::UnsignedDummy>;
			static constexpr Bit padding = Attr::template FindByBase <Padding::Tag, Padding::None>::template padding <Obj, Width> ();
		};

		template <typename Descend, typename Attr>
		struct GetIntRetImpl : GetIntRetBase<typename Descend::Obj, Attr> {
			using B = GetIntRetBase<typename Descend::Obj, Attr>;
			using Type = decltype(std::declval<Descend> ().template walkInteger<B::Width, B::padding, typename B::BO, typename B::SF> ());
			USER_ALWAYS_INLINE static Type walk (Descend desc) {
				return desc.template walkInteger<B::Width, B::padding, typename B::BO, typename B::SF> ();
			}
		};

		template <typename Descend, typename Attr>
		struct GetEnumRetImpl : GetIntRetBase<std::underlying_type_t<typename Descend::Obj>, Attr> {
			using B = GetIntRetBase<std::underlying_type_t<typename Descend::Obj>, Attr>;
			using Type = decltype(std::declval<Descend> ().template walkEnum<B::Width, B::padding, typename B::BO, typename B::SF> ());
			static Type walk (Descend desc) {
				return desc.template walkEnum<B::Width, B::padding, typename B::BO, typename B::SF> ();
			}
		};

		template <bool Invert, typename T>
		using GetIfEven = std::enable_if_t<T::allowDynamic() != Invert, T>;

		template <typename Descend>
		using DescArrEl = Helper::ArrElType<typename Descend::Obj>;

		template <typename Container>
		using GetContIter = decltype (begin (std::declval <Container&> ()));

		template <typename Container>
		using GetContEl = typename std::iterator_traits<GetContIter<Container>>::value_type;

		template <typename Descend>
		using GetDescIter = GetContIter<typename Descend::QObj>;

		template <typename Descend>
		using GetDescEl = GetContEl<typename Descend::QObj>;


		template <typename Descend, typename Attr>
		using GetContSubRet = typename GetContSubRet2<Descend, Attr, GetDescEl<Descend>, GetDescIter<Descend>>::Type;

		template <typename Descend, typename Attr, int DynMode>
		using GetContRet = typename GetContRet2<std::enable_if_t<GetContSubRet<Descend, Attr>::allowDynamic () == (DynMode>0), typename Descend::Algo>, DynMode,
				typename Descend::Obj, GetContSubRet<Descend, Attr>, Attr>::Type;

		template <typename Descend, auto MemPtr, auto SizeRef>
		using GetDynContSubDesc = decltype (std::declval<Descend> ().template iterateDynSize<MemPtr, SizeRef, GetContEl<Helper::MemVarPtrTargetType<MemPtr>>> ());

		template <typename Descend, typename Attr, auto MemPtr, auto SizeRef>
		using GetDynContRet = GetContRet<GetDynContSubDesc<Descend, MemPtr, SizeRef>, AttrRemoveTag<Attr, Dyn::Tag>, 2>;

		template <typename T>
		struct GetContMaxSize2 {
			using Type = Unlimited;
		};
		template <typename T, std::size_t N>
		struct GetContMaxSize2<T[N]> {
			using Type = MaxSize<N>;
		};
		template <typename T, std::size_t N>
		struct GetContMaxSize2<std::array<T, N>> {
			using Type = MaxSize<N>;
		};
		template <typename T>
		using GetContMaxSize = typename GetContMaxSize2<T>::Type;

		template <typename Descend, typename Attr>
		using GetTupleRet = typename GetTupleRet2<typename Descend::Obj, std::make_index_sequence<std::tuple_size<typename Descend::Obj>::value>, Descend, Attr>::Type;

		template <typename Descend, typename Attr>
		using GetIntRet = GetIntRetImpl<std::enable_if_t<std::is_integral_v<typename Descend::Obj>, Descend>, Attr>;

		template <typename Descend, typename Attr>
		using GetEnumRet = GetEnumRetImpl<std::enable_if_t<std::is_enum_v<typename Descend::Obj>, Descend>, Attr>;

		template <typename Descend, auto MemPtr, typename Attr>
		struct GetStructHookRet;

		template <typename Descend, typename Attr, auto... MemPtrs>
		using GetStructReturn = typename Descend::Algo::template SumRet<typename GetStructHookRet<Descend, MemPtrs, Attr>::Type...>;

		/*
		 * Struct serialization
		 */

		template <auto MemPtr, typename Descend, typename Attr>
		decltype(auto) walkStructMem (int, Descend desc, Attr) {
			return walk (desc.template structMember<Helper::MemVarPtrTargetType<MemPtr>, MemPtr> (), Attr {});
		}

		template <auto MemPtr, auto SizeRef>
		decltype(auto) walkStructMem (Dyn::Size<SizeRef>, ...) {
			static_assert (!std::is_class_v<Dyn::Size<SizeRef>>, "Dyn::Size<> used with unsuitable data type (not container/array type or element size not a multiple of serialization word size)");
		}

		template <auto MemPtr, auto MetaRef>
		decltype(auto) walkStructMem (Dyn::Optional<MetaRef>, ...) {
			static_assert (!std::is_class_v<Dyn::Size<MetaRef>>, "Dyn::Optional<> used with unsuitable data type (object size not a multiple of serialization word size)");
		}

		template <auto MemPtr, typename Descend, typename Attr, auto SizeRef>
		USER_ALWAYS_INLINE GetDynContRet<Descend, Attr, MemPtr, SizeRef> walkStructMem (Dyn::Size<SizeRef>, Descend desc, Attr) {
			using AttrI = AttrRemoveTag<Attr, Dyn::Tag>;
			using SubDesc = GetDynContSubDesc<Descend, MemPtr, SizeRef>;
			using Obj = typename SubDesc::Obj;
			using El = GetContEl<Obj>;
			using SubRet = GetContSubRet<SubDesc, AttrI>;
			using Ret = GetDynContRet<Descend, Attr, MemPtr, SizeRef>;
			using Iter = GetContIter<typename SubDesc::QObj>;

			const AttrI sa {};

			if constexpr (std::is_same_v<decltype (SizeRef), decltype (MemPtr)>) {
				static_assert (SizeRef != MemPtr, "Target of Dyn::Size attribute can't be the same variable!");
			}

			using Max = GetContMaxSize<Obj>;

			std::size_t count;
			bool ok;
			auto ret = desc.template checkIterateDynSize<MemPtr, SizeRef, El, SubRet> (count, ok, Max {});
			if (!ok) {
				return desc.template iterateDynAbort <SubRet, Ret> (std::move (ret));
			}

			auto subDesc = desc.template iterateDynSize<MemPtr, SizeRef, El> ();
			auto iter = subDesc.template beginIterateCont<El, Iter> ();

			subDesc.beginContainer ();
			for (std::size_t i = 0; i < count; ++i) {
				if (i) desc.stepContainer ();
				auto ret = walk (subDesc.template contElem <El, Iter> (iter), sa);
				if constexpr (ret.canFail ()) {
					if (ret.cancel ())
						return { std::move (ret) };
				}
				subDesc = subDesc.advance (std::move (ret));
			}
			subDesc.endContainer ();
			return std::move (subDesc);
		}

		template <auto MemPtr, typename Descend, typename Attr, auto MetaRef>
		USER_ALWAYS_INLINE auto walkStructMem (Dyn::Optional<MetaRef>, Descend desc, Attr) ->
			typename Descend::Algo::template MultRetDynamicMax<
				GetIfEven<false, decltype (walk (desc.template walkOptional<MemPtr, MetaRef, Helper::MemVarPtrTargetType<MemPtr>> (), AttrRemoveTag<Attr, Dyn::Tag> {}))>, 1>
		{
			using Obj = Helper::MemVarPtrTargetType<MemPtr>;
			using AttrI = AttrRemoveTag<Attr, Dyn::Tag>;

			const AttrI sa {};
			using Ret = typename Descend::Algo::template MultRetDynamicMax<decltype (walk (desc.template walkOptional<MemPtr, MetaRef, Obj> (), sa)), 1>;

			using SubRet = decltype (walk (desc.template walkOptional<MemPtr, MetaRef, Obj> (), sa));
			if constexpr (std::is_same_v<decltype (MetaRef), decltype (MemPtr)>) {
				static_assert (MetaRef != MemPtr, "Target of Dyn::Optional<> attribute can't be the same variable!");
			}

			bool present, ok;
			auto ret = desc.template checkOptional<MemPtr, MetaRef, Obj, SubRet> (present, ok);
			if (!ok) {
				return desc.template walkOptionalAbort <MemPtr, SubRet, Ret> (std::move (ret));
			}
			if (present) {
				return desc.advance (walk (desc.template walkOptional<MemPtr, MetaRef, Obj> (), sa));
			} else
				return std::move (desc);
		}

		template <auto MemPtr, typename Descend, typename Attr, auto SizeRef>
		USER_ALWAYS_INLINE auto walkStructMem (Dyn::Size<SizeRef>, Descend desc, Attr) ->
			typename Descend::Algo::template MultRetDynamic<
				GetIfEven<false, decltype (walk (desc	.template structMember<Helper::MemVarPtrTargetType<MemPtr>, MemPtr> ()
														.template ptrArrayElem<Helper::GetPointerTarget<Helper::MemVarPtrTargetType<MemPtr>>> (0), AttrRemoveTag<Attr, Dyn::Tag> {}))>
			>
		{
			using Obj = Helper::MemVarPtrTargetType<MemPtr>;
			using Target = Helper::GetPointerTarget<Obj>;
			using AttrI = AttrRemoveTag<Attr, Dyn::Tag>;

			const AttrI sa {};
			using SubRet = decltype (walk (desc.template structMember<Obj, MemPtr> ().template ptrArrayElem<Target> (0), sa));
			using Ret = typename Descend::Algo::template MultRetDynamic<SubRet>;

			if constexpr (std::is_same_v<decltype (SizeRef), decltype (MemPtr)>) {
				static_assert (SizeRef != MemPtr, "Target of Dyn::Size<> attribute can't be the same variable!");
			}

			std::size_t count;
			bool ok;
			auto ret = desc.template checkIterateDynSize<MemPtr, SizeRef, Target, SubRet> (count, ok, Unlimited {});
			if (!ok) {
				return desc.template iterateDynAbort <SubRet, Ret> (std::move (ret));
			}

			auto subDesc = desc.template structMember<Obj, MemPtr> ();
			subDesc.beginContainer ();

			for (std::size_t i = 0; i < count; ++i) {
				if (i) desc.stepContainer ();
				auto ret = walk (subDesc.template ptrArrayElem <Target> (i), sa);
				if constexpr (ret.canFail ()) {
					if (ret.cancel ())
						return { std::move (ret) };
				}
				subDesc = subDesc.advance (std::move (ret));
			}
			subDesc.endContainer ();
			return std::move (subDesc);
		}

		template <typename Descend, auto MemPtr, typename Attr>
		struct GetStructHookRet {
			using IAttr = Inherit<Attr, GetMemAttributes<MemPtr>>;
			using SubAttr = AttrRemoveTag<IAttr, Hook::SerPreTag, Hook::SerPostTag, Hook::DeSerPreTag, Hook::DeSerPostTag>;

			using DTag = typename GetMemAttributes<MemPtr>::template FindByBase<Dyn::Tag, int>;
			using Invoke = InvokeHooks<	Helper::MemVarPtrClassType<MemPtr>,
										Helper::MemVarPtrTargetType<MemPtr>,
										typename Descend::Algo,
										decltype (walkStructMem<MemPtr> (
												DTag {},
												std::declval<Descend> (),
												std::declval<SubAttr> ())), IAttr>;
			using Type = typename Invoke::Ret;
		};

		template <typename Attr, typename Ret, typename Descend, auto First, auto... Rest>
		USER_ALWAYS_INLINE Ret walkStruct2 (Descend desc) {
			using X = GetStructHookRet<Descend, First, Attr>;
			using SubAttr = typename X::SubAttr;
			using Invoke = typename X::Invoke;
			using Pre = typename Invoke::I_PreHook;
			using Post = typename Invoke::I_PostHook;
			using DTag = typename X::DTag;

			if constexpr (Pre::enabled ()) {
				if constexpr (Pre::canFail ()) {
					ErrorCode ec = desc.template invokeStructHook<typename Pre::Ret, Pre, First> ();
					if (ec != ErrorCode::OK)
						return desc.template abortHook<Ret> (ec);
				} else
					desc.template invokeStructHook<typename Pre::Ret, Pre, First> ();
			}

			auto ret = walkStructMem<First> (DTag {}, std::move (desc), SubAttr {});
			if constexpr (ret.canFail ())
				if (ret.cancel ())
					return { std::move (ret) };

			if constexpr (Post::enabled ()) {
				if constexpr (Post::canFail ()) {
					ErrorCode ec = desc.template invokeStructHook<typename Post::Ret, Post, First> ();
					if (ec != ErrorCode::OK)
						return desc.template abortHook<Ret> (ec);
				} else
					desc.template invokeStructHook<typename Post::Ret, Post, First> ();
			}

			if constexpr (sizeof...(Rest) == 0) {
				desc.endContainer ();
				return desc.template advance (std::move (ret));
			} else {
				desc.stepContainer ();
				return walkStruct2 <Attr, Ret, typename Descend::template Advance<decltype(ret)>, Rest...> (desc.template advance (std::move (ret)));
			}
		}

		template <typename Descend, typename Attr, auto... MemPtrs>
		USER_ALWAYS_INLINE GetStructReturn<Descend, Attr, MemPtrs...> walkStruct (Descend desc, Attr, StructMembers<MemPtrs...>) {
			desc.beginContainer ();
			return walkStruct2<Attr, GetStructReturn<Descend, Attr, MemPtrs...>, Descend, MemPtrs...> (std::move (desc));
		}

		template <typename Descend, typename Attr, class StrMembers = GetStructMembers<typename Descend::Obj>>
		decltype(auto) walkImpl (Descend desc, Attr) {
			return walkStruct(std::move (desc), DescendAttr<Attr> {}, StrMembers {});
		}

		/*
		 * Array Serialization
		 */

		template <typename Descend, typename Attr>
		GetContRet<Descend, Attr, 1> walkImpl (Descend desc, Attr a) {
			using El = GetDescEl<Descend>;

			using Ret = GetContRet<Descend, Attr, 1>;
			using SubRet = GetContSubRet<Descend, Attr>;
			using Iter = GetDescIter<Descend>;

			int state;
			if constexpr (Helper::IsArray<typename Descend::Obj>) {
				state = 1;
			} else {
				auto ret1 = desc.template checkIterateDyn <El, SubRet> (state);
				if (state < 0) {
					return desc.template iterateDynAbort <SubRet, Ret> (std::move (ret1));
				} else if (state == 0)
					return std::move (desc);
			}

			auto iter = desc.template beginIterateCont<El, Iter> ();

			desc.beginContainer ();
			while (state > 0) {
				auto ret = desc.template contElem <El, Iter> (iter, state);
				auto wret = walk (std::move (ret), a);
				if constexpr (wret.canFail ()) {
					if (wret.cancel ()) {
						return std::move (wret);
					}
				}
				if (state)
					desc.stepContainer ();
				desc = desc.advance (std::move (wret));
			}
			desc.endContainer ();
			return std::move (desc);
		}

		template <typename Attr, typename Ret, std::size_t I, typename Descend, typename Iter>
		USER_ALWAYS_INLINE Ret walkStArray (Descend desc, Iter& iter) {
			using Obj = typename Descend::Obj;
			using El = GetDescEl<Descend>;
			static constexpr std::size_t arrSize = Helper::arraySize<Obj>;
			if constexpr (I >= arrSize) {
				desc.endContainer ();
				return std::move (desc);
			} else {
				int state;
				auto ret = walk (desc.template contElem<El, GetDescIter<Descend>> (iter, state), Attr {});
				assert (I == (arrSize-1) || state == 1);
				if constexpr (ret.canFail ())
					if (ret.cancel ())
						return std::move (ret);

				if constexpr (I != arrSize-1) {
					desc.stepContainer ();
				}

				return walkStArray<Attr, Ret, I+1> (desc.template advance (std::move (ret)), iter);
			}
		}

		template <typename Descend, typename Attr>
		GetContRet<Descend, Attr, 0> walkImpl (Descend desc, Attr) {
			using El = GetDescEl<Descend>;
			using Iter = GetDescIter<Descend>;

			auto iter = desc.template beginIterateCont<El, Iter> ();
			desc.beginContainer ();
			return walkStArray<Attr, GetContRet<Descend, Attr, false>, 0> (std::move (desc), iter);
		}

		/*
		 * Tuple Serialization
		 */

		template <typename Ret, typename Descend, typename Attr, typename FirstT, typename... RestT, std::size_t FirstI, std::size_t... RestI>
		USER_ALWAYS_INLINE Ret walkTupleElem (Descend desc, Attr a, Helper::TTag<std::tuple<FirstT, RestT...>>, std::index_sequence<FirstI, RestI...>) {
			auto ret = walk (desc.template tupleElem<FirstI, FirstT> (), a);
			if constexpr (ret.canFail ()) {
				if (ret.cancel ())
					return std::move (ret);
			}
			auto nextDesc = desc.advance (std::move (ret));
			if constexpr (sizeof...(RestT) == 0) {
				desc.endContainer ();
				return std::move (nextDesc);
			} else {
				desc.stepContainer ();
				return walkTupleElem<Ret> (std::move (nextDesc), a, Helper::TTag<std::tuple<RestT...>> {}, std::index_sequence<RestI...> {});
			}
		}

		template <typename Ret, typename Descend, typename Attr, typename... Elem, std::size_t... IS>
		USER_ALWAYS_INLINE Ret walkTuple (Descend desc, Attr a, Helper::TTag<std::tuple<Elem...>> x, std::index_sequence<IS...> y) {
			desc.beginContainer ();
			return walkTupleElem<Ret> (std::move (desc), a, x, y);
		}
		// Re-Use tuple implementation for pairs
		template <typename Ret, typename Descend, typename Attr, typename... Elem, std::size_t... IS>
		USER_ALWAYS_INLINE Ret walkTuple (Descend desc, Attr a, Helper::TTag<std::pair<Elem...>>, std::index_sequence<IS...> y) {
			desc.beginContainer ();
			return walkTupleElem<Ret> (std::move (desc), a, Helper::TTag<std::tuple<Elem...>> {}, y);
		}

		template <typename Descend, typename Attr>
		GetTupleRet<Descend, Attr> walkImpl (Descend desc, Attr a) {
			return walkTuple<GetTupleRet<Descend, Attr>> (std::move (desc), a, Helper::TTag<typename Descend::Obj> {}, std::make_index_sequence<std::tuple_size_v<typename Descend::Obj>> {});
		}

		/*
		 * Integer Serialization
		 */

		template <typename Descend, typename Attr>
		USER_ALWAYS_INLINE typename GetIntRet<Descend, Attr>::Type walkImpl (Descend desc, Attr) {
			return GetIntRet<Descend, Attr>::walk (std::move (desc));
		}

		template <typename Descend, typename Attr>
		USER_ALWAYS_INLINE typename GetEnumRet<Descend, Attr>::Type walkImpl (Descend desc, Attr) {
			return GetEnumRet<Descend, Attr>::walk (std::move (desc));
		}

		/*
		 * Hook Handling (outer serialization function)
		 */

		template <typename Descend, typename Attr>
		struct GetHookRet {
			using IAttr = Inherit<Attr, GetTypeAttributes<typename Descend::Obj>, true>;

			using SubAttr = AttrRemoveTag<IAttr, Hook::SerPreTag, Hook::SerPostTag, Hook::DeSerPreTag, Hook::DeSerPostTag>;
			using Invoke = InvokeHooks<void, typename Descend::Obj, typename Descend::Algo, decltype (walkImpl (std::declval <Descend> (), std::declval<SubAttr> ())), IAttr>;
			using Type = typename Invoke::Ret;
		};

		template <typename Descend, typename Attr>
		USER_ALWAYS_INLINE typename GetHookRet<Descend, Attr>::Type walk (Descend desc, Attr) {
			using X = GetHookRet<Descend, Attr>;
			using Ret = typename X::Type;
			using SubAttr = typename X::SubAttr;
			using Invoke = typename X::Invoke;
			using Pre = typename Invoke::I_PreHook;
			using Post = typename Invoke::I_PostHook;

			if constexpr (Pre::enabled ()) {
				if constexpr (Pre::canFail ()) {
					ErrorCode ec = desc.template invokeHook<typename Pre::Ret, Pre> ();
					if (ec != ErrorCode::OK)
						return desc.template abortHook<Ret> (ec);
				} else
					desc.template invokeHook<typename Pre::Ret, Pre> ();
			}

			auto wret = walkImpl (std::move (desc), SubAttr {});

			if constexpr (wret.canFail ()) {
				if (wret.cancel ())
					return std::move (wret);
			}

			if constexpr (Post::enabled ()) {
				if constexpr (Post::canFail ()) {
					ErrorCode ec = desc.template invokeHook<typename Post::Ret, Post> ();
					if (ec != ErrorCode::OK)
						return desc.template abortHook<Ret> (ec);
				} else
					desc.template invokeHook<typename Post::Ret, Post> ();
			}

			return desc.advance (std::move (wret));
		}
	}

	namespace Helper {
		template <bool Enable>
		class ErrorContainer;

		template <>
		class ErrorContainer<true> {
			public:
				USER_ALWAYS_INLINE constexpr ErrorContainer (const ErrorContainer&) = default;
				USER_ALWAYS_INLINE constexpr ErrorContainer (ErrorContainer&&) = default;
				USER_ALWAYS_INLINE ErrorContainer& operator = (const ErrorContainer&) = default;
				USER_ALWAYS_INLINE ErrorContainer& operator = (ErrorContainer&&) = default;

#ifdef USER_EXCEPTIONS
				USER_ALWAYS_INLINE constexpr ErrorContainer () {}
				static ErrorContainer signal (ErrorCode e) { throw Exception (e); }
				USER_ALWAYS_INLINE bool cancel () const { return false; }
				USER_ALWAYS_INLINE void errorOrVoid () const {}
#else
				USER_ALWAYS_INLINE constexpr ErrorContainer (ErrorCode e = {}) : m_error (e) {}
				USER_ALWAYS_INLINE static ErrorContainer signal (ErrorCode e) { return {e}; }
				USER_ALWAYS_INLINE constexpr ErrorCode errorOrVoid () const { return m_error; }
				USER_ALWAYS_INLINE bool cancel () const { return m_error != ErrorCode::OK; }
			private:
				ErrorCode m_error;
#endif
		};
		template <>
		class ErrorContainer<false> {
			public:
				USER_ALWAYS_INLINE constexpr ErrorContainer () {}
				USER_ALWAYS_INLINE constexpr ErrorContainer (const ErrorContainer&) = default;
				USER_ALWAYS_INLINE constexpr ErrorContainer (ErrorContainer&&) = default;
				USER_ALWAYS_INLINE ErrorContainer& operator = (const ErrorContainer&) = default;
				USER_ALWAYS_INLINE ErrorContainer& operator = (ErrorContainer&&) = default;

				USER_ALWAYS_INLINE static constexpr bool cancel () { return false; }
				USER_ALWAYS_INLINE void errorOrVoid () const {}
		};

		class SizeTrackerNone {
			public:
				USER_ALWAYS_INLINE bool advance (std::size_t) { return true; }
		};
		class SizeTrackerRemaining {
			public:
				USER_ALWAYS_INLINE constexpr SizeTrackerRemaining (std::size_t rem) : m_remain (rem) {}
				USER_ALWAYS_INLINE SizeTrackerRemaining (SizeTrackerRemaining&&) = default;
				SizeTrackerRemaining (const SizeTrackerRemaining&) = delete;
				SizeTrackerRemaining& operator = (const SizeTrackerRemaining&) = delete;
				USER_ALWAYS_INLINE SizeTrackerRemaining& operator = (SizeTrackerRemaining&&) = default;

				bool advance (std::size_t use) {
					if (m_remain >= use) {
						m_remain -= use;
						return true;
					} else {
						return false;
					}
				}
				USER_ALWAYS_INLINE std::size_t remaining () const { return m_remain; }
			private:
				std::size_t m_remain;
		};
		class SizeTrackerUsage {
			public:
				USER_ALWAYS_INLINE constexpr SizeTrackerUsage (std::size_t u) : m_usage (u) {}
				USER_ALWAYS_INLINE SizeTrackerUsage (SizeTrackerUsage&&) = default;
				SizeTrackerUsage (const SizeTrackerUsage&) = delete;
				SizeTrackerUsage& operator = (const SizeTrackerUsage&) = delete;
				USER_ALWAYS_INLINE SizeTrackerUsage& operator = (SizeTrackerUsage&&) = default;
				USER_ALWAYS_INLINE bool advance (std::size_t use) {
					m_usage += use;
					return true;
				}
				USER_ALWAYS_INLINE std::size_t usage () const { return m_usage; }
			private:
				std::size_t m_usage;
		};

		template <typename SizeType, Bit staticBits, typename MaxDyn, Bit wSize>
		struct GetSizeTracker;

		template <Bit staticBits, typename MaxDyn, Bit wSize>
		struct GetSizeTracker<InfSize, staticBits, MaxDyn, wSize> {
			using Type = SizeTrackerUsage;
			static constexpr std::size_t staticWords = (staticBits + wSize - 1) / wSize;
			static constexpr bool canFail = false;

			static constexpr Type get (InfSize)		{	return { staticWords }; }
			static constexpr std::size_t used (InfSize, Type& track) { return track.usage (); }
		};
		template <Bit staticBits, Bit wSize>
		struct GetSizeTracker<InfSize, staticBits, NoDyn, wSize> {
			using Type = SizeTrackerNone;
			static constexpr bool canFail = false;

			static constexpr Type get (InfSize)		{	return {}; }
			static constexpr std::size_t used (InfSize, Type&) { return (staticBits + wSize - 1) / wSize; }
		};

		template <Bit staticBits, typename MaxDyn, Bit wSize>
		struct GetSizeTracker<DynSize, staticBits, MaxDyn, wSize> {
			using Type = SizeTrackerRemaining;
			static constexpr std::size_t staticWords = (staticBits + wSize - 1) / wSize;
			static constexpr bool canFail = true;

			static constexpr bool check (DynSize ds)	{ return ds.size >= staticWords; }
			static constexpr Type get (DynSize ds)		{ return { ds.size - staticWords }; }
			static constexpr std::size_t used (DynSize ds, Type& track) { return ds.size - track.remaining (); }
		};
		template <Bit staticBits, Bit wSize>
		struct GetSizeTracker<DynSize, staticBits, NoDyn, wSize> {
			using Type = SizeTrackerNone;
			static constexpr std::size_t staticWords = (staticBits + wSize - 1) / wSize;
			static constexpr bool canFail = true;

			static constexpr bool check (DynSize ds)	{ return ds.size >= staticWords; }
			static constexpr Type get (DynSize)			{ return { }; }
			static constexpr std::size_t used (DynSize, Type&) { return staticWords; }
		};
		template <std::size_t fSize, Bit staticBits, typename MaxDyn, Bit wSize>
		struct GetSizeTracker<FixedSize<fSize>, staticBits, MaxDyn, wSize> {
			static_assert (fSize * wSize >= staticBits, "Buffer too small for (de)serialized data structure");

			using Type = SizeTrackerRemaining;
			static constexpr std::size_t staticWords = (staticBits + wSize - 1) / wSize;
			static constexpr bool canFail = true;

			static constexpr bool check (FixedSize<fSize> ds)	{ return ds.size >= staticWords; }
			static constexpr Type get (FixedSize<fSize> ds)		{ return { ds.size - staticWords }; }
			static constexpr std::size_t used (FixedSize<fSize> ds, Type& track) { return ds.size - track.remaining (); }
		};
		template <std::size_t fSize, Bit staticBits, Bit wSize>
		struct GetSizeTracker<FixedSize<fSize>, staticBits, NoDyn, wSize> {
			static_assert (fSize * wSize >= staticBits, "Buffer too small for (de)serialized data structure");
			static constexpr std::size_t staticWords = (staticBits + wSize - 1) / wSize;
			using Type = SizeTrackerNone;
			static constexpr bool canFail = false;

			static constexpr Type get (FixedSize<fSize>)			{ return { }; }
			static constexpr std::size_t used (FixedSize<fSize>, Type&) { return staticWords; }
		};

		template <typename RawIter_, typename BaseDef>
		struct RawInfo2 {
			using RawIter = RawIter_;
			using SWord = typename BaseDef::SWord;
			static constexpr Bit wSize = BaseDef::wSize;

			static constexpr SWord max = (wSize >= std::numeric_limits<SWord>::digits) ? std::numeric_limits<SWord>::max () : static_cast<SWord> ((SWord {1} << wSize) - 1);
		};
		template <typename RawIter_>
		struct RawInfo2<RawIter_, void> {
			using RawIter = RawIter_;
			using SWord = typename std::iterator_traits<RawIter>::value_type;
			static constexpr Bit wSize = wordSize<SWord>;

			static_assert (!std::is_void_v<SWord>, "Serialization word could not be inferred from iterator via std::iterator_traits; user uSer::RawInfo to declare serialization word type explicitly.");

			static constexpr SWord max = (wSize >= std::numeric_limits<SWord>::digits) ? std::numeric_limits<SWord>::max () : static_cast<SWord> ((SWord {1} << wSize) - 1);
		};

		template <typename RawIter, typename Attr>
		using GetRawInfo2 = RawInfo2<RawIter, typename Attr::template FindByBase<Helper::RawInfoTag, void>>;
	}

	template <typename RawInfo, bool Read>
	struct W_Serialize {
		using RawIter = typename RawInfo::RawIter;
		using SWord = typename RawInfo::SWord;
		static constexpr Bit wSize = RawInfo::wSize;

		template <typename Obj_, Bit BitOffset_, typename SizeTracker>
		class Descend;
		template <Bit Bits_, typename MaxDyn, bool CanFail_, typename SizeTracker>
		class Return;

		template <typename... Ret>
		using SumRet = Return<(Ret::bits + ...), SWalk::SumRetMaxDyn<typename Ret::MaxDyn...>, (Ret::canFail () || ...), typename Helper::PackFirst<Ret...>::SizeTracker>;

		template <typename Ret, std::size_t Count>
		using MultRetStatic = Return<Count * Ret::bits, typename Ret::MaxDyn::template Multiply<Count>, Ret::canFail (), typename Ret::SizeTracker>;

		template <typename Ret>
		using MultRetDynamic = Return<0, Unlimited, true, typename Ret::SizeTracker>;

		template <typename Ret, std::size_t MaxSize>
		using MultRetDynamicMax = Return<0, typename Ret::MaxDyn::template Multiply<MaxSize, Ret::bits / wSize>, true, typename Ret::SizeTracker>;

		template <typename T>
		using Qualify = std::conditional_t<Read, T, const T>;

		using PreHookTag = std::conditional_t<Read, Hook::DeSerPreTag, Hook::SerPreTag>;
		using PostHookTag = std::conditional_t<Read, Hook::DeSerPostTag, Hook::SerPostTag>;

		template <Bit Bits_, typename MaxDyn_, bool CanFail_, typename SizeTracker_>
		class Return : public Helper::ErrorContainer<CanFail_>, public SizeTracker_ {
			template <typename, Bit, typename>
			friend class Descend;
			template <Bit, typename, bool, typename>
			friend class Return;

			public:
				using Err = Helper::ErrorContainer<CanFail_>;
				using SizeTracker = SizeTracker_;
				using MaxDyn = MaxDyn_;
				static constexpr Bit bits = Bits_;
				static constexpr bool dynamic = !std::is_same_v<MaxDyn, NoDyn>;
				static constexpr bool allowDynamic () { return (bits % wSize) == 0; }
				static constexpr bool canFail () { return CanFail_; }
				using GetFailRet = Return<bits, MaxDyn, true, SizeTracker>;

				static_assert (!dynamic || canFail ());

				// Comment this line if a type is empty on purpose.
				static_assert (bits > 0 || dynamic, "(De)Serialization of empty type requested!");

				template <Bit OrigBO, typename D, bool CF>
				USER_ALWAYS_INLINE constexpr Return (Return<OrigBO, D, CF, SizeTracker>&& r) : Err (std::move (r)), SizeTracker (std::move (r)), raw (std::move (r.raw)), last (std::move (r.last)) {}

				template <typename Obj, Bit OrigBO>
				USER_ALWAYS_INLINE constexpr Return (Descend<Obj, OrigBO, SizeTracker>&& desc, Err&& e = {}) : Err (std::move (e)), SizeTracker (std::move (desc)), raw (std::move (desc.raw)), last (std::move (desc.last)) {}

				Return (const Return&) = delete;
				USER_ALWAYS_INLINE Return& operator = (Return&&) = default;
				Return& operator = (Return&) = delete;

				template <bool R = Read>
				std::enable_if_t<!R, Err> finish () {
					if (!Err::cancel () && bits % wSize != 0) {
						*raw = last & Helper::mask<SWord, bits % wSize> ();
						++raw;
					}
					return *this;
				}
				template <bool R = Read>
				std::enable_if_t<R, Err> finish () { return *this; }
			private:
				RawIter raw;
				SWord last;
		};

		template <typename Obj_, Bit BitOffset_, typename SizeTracker_>
		class Descend : private SizeTracker_ {
			template <Bit, typename, bool, typename>
			friend class Return;
			friend struct W_Serialize;
			public:
				static constexpr Bit bitOffset = BitOffset_;
				using Obj = Obj_;
				using QObj = Qualify<Obj>;
				using ObjRef = QObj&;
				using Algo = W_Serialize;
				using SizeTracker = SizeTracker_;

				template <typename Ret>
				using Advance = Descend<Obj, (bitOffset + Ret::bits) % wSize, SizeTracker>;

				USER_ALWAYS_INLINE Descend (Descend&&) = default;
				Descend (const Descend&) = delete;
				USER_ALWAYS_INLINE Descend& operator = (Descend&& src) { raw = std::move (src.raw); last = std::move (src.last); st () = std::move (src.st ()); return *this; }
				Descend& operator = (Descend&) = delete;

				template <Bit Width, Bit Padding, typename ByteOrder, typename SignFormat>
				USER_ALWAYS_INLINE std::enable_if_t<Read, Return<Width+Padding, NoDyn, false, SizeTracker>> walkInteger () {
					Helper::decodeInteger<RawInfo, Width, Padding, bitOffset, ByteOrder, SignFormat> (obj, raw, last);
					return { std::move (*this) };
				}

				template <Bit Width, Bit Padding, typename ByteOrder, typename SignFormat>
				USER_ALWAYS_INLINE std::enable_if_t<!Read, Return<Width+Padding, NoDyn, false, SizeTracker>> walkInteger () {
					Helper::encodeInteger<RawInfo, Width, Padding, bitOffset, ByteOrder, SignFormat> (obj, raw, last);
					return { std::move (*this) };
				}

				template <Bit Width, Bit Padding, typename ByteOrder, typename SignFormat>
				USER_ALWAYS_INLINE std::enable_if_t<Read, Return<Width+Padding, NoDyn, false, SizeTracker>> walkEnum () {
					std::underlying_type_t<Obj> e;
					Helper::decodeInteger<RawInfo, Width, Padding, bitOffset, ByteOrder, SignFormat> (e, raw, last);
					obj = static_cast<Obj> (e);
					return { std::move (*this) };
				}

				template <Bit Width, Bit Padding, typename ByteOrder, typename SignFormat>
				USER_ALWAYS_INLINE std::enable_if_t<!Read, Return<Width+Padding, NoDyn, false, SizeTracker>> walkEnum () {
					Helper::encodeInteger<RawInfo, Width, Padding, bitOffset, ByteOrder, SignFormat> (static_cast<std::underlying_type_t<Obj>> (obj), raw, last);
					return { std::move (*this) };
				}

				template <typename Ret>
				USER_ALWAYS_INLINE constexpr Advance<Ret> advance (Ret ret) {
					return { std::move (ret.raw), std::move (ret.last), obj, std::move (ret) };
				}

				template <typename Target, auto MemPtr>
				USER_ALWAYS_INLINE constexpr Descend<Target, bitOffset, SizeTracker> structMember () {
					return { std::move (raw), std::move (last), obj.*MemPtr, std::move (st ()) };
				}

				template <std::size_t Index, typename Elem>
				USER_ALWAYS_INLINE constexpr Descend<Elem, bitOffset, SizeTracker> tupleElem () {
					return { std::move (raw), std::move (last), std::get<Index> (obj), std::move (st ()) };
				}

				template <typename El, typename Iter>
				USER_ALWAYS_INLINE constexpr Descend<El, bitOffset, SizeTracker> contElem (Iter& iter, int& state) {
					Qualify<El>& elem = *iter;
					++iter;
					if (iter == end (obj))
						state = 0;
					else
						state = 1;
					return { std::move (raw), std::move (last), elem, std::move (st ()) };
				}
				template <typename El, typename Iter>
				USER_ALWAYS_INLINE constexpr Descend<El, bitOffset, SizeTracker> contElem (Iter& iter) {
					Qualify<El>& elem = *iter;
					++iter;
					return { std::move (raw), std::move (last), elem, std::move (st ()) };
				}
				template <typename El>
				USER_ALWAYS_INLINE constexpr Descend<El, bitOffset, SizeTracker> ptrArrayElem (std::size_t index) {
					return { std::move (raw), std::move (last), obj [index], std::move (st ()) };
				}
				template <typename El>
				USER_ALWAYS_INLINE constexpr Descend<El, bitOffset, SizeTracker> ptrTarget () {
					return { std::move (raw), std::move (last), *obj, std::move (st ()) };
				}

				template <auto ArrRef, auto SizeRef, typename El, typename SubRet, typename MaxT>
				USER_ALWAYS_INLINE constexpr ErrorCode checkIterateDynSize (std::size_t& count, bool& ok, MaxT max) {
					count = Helper::invokeRef (SizeRef, obj);
					if (!max.check (count)) {
						ok = false;
						return ErrorCode::EDYNRANGE;
					}
					if (!st ().advance ((SubRet::bits / wSize) * count)) {
						ok = false;
						return ErrorCode::EBUFSIZE;
					}
					ok = true;
					return ErrorCode::OK;
				}
				template <typename El, typename SubRet>
				USER_ALWAYS_INLINE constexpr ErrorCode checkIterateDyn (int& res) {
					auto s = std::size (obj);
					if (!st ().advance ((SubRet::bits / wSize) * s)) {
						res = -1;
						return ErrorCode::EBUFSIZE;
					}
					if (s == 0)
						res = 0;
					else
						res = 1;
					return ErrorCode::OK;
				}

				template <typename SubRet, typename Ret>
				USER_ALWAYS_INLINE constexpr Ret iterateDynAbort (ErrorCode code) {
					return { std::move (*this), Ret::signal (code)};
				}

				template <auto ArrRef, auto SizeRef, typename El>
				USER_ALWAYS_INLINE constexpr Descend<Helper::MemVarPtrTargetType<ArrRef>, bitOffset, SizeTracker> iterateDynSize () {
					return { std::move (raw), std::move (last), obj.*ArrRef, std::move (st ()) };
				}

				template <typename El, typename Iter>
				USER_ALWAYS_INLINE Iter beginIterateCont () {
					return begin (obj);
				}

				template <auto MemRef, auto MetaRef, typename El, typename SubRet>
				USER_ALWAYS_INLINE constexpr ErrorCode checkOptional (bool& present, bool& ok) {
					present = Helper::invokeRef (MetaRef, obj);
					if (present && !st ().advance ((SubRet::bits/wSize))) {
						ok = false;
						return ErrorCode::EBUFSIZE;
					} else {
						ok = true;
						return ErrorCode::OK;
					}
				}

				template <auto MemRef, typename SubRet, typename Ret>
				USER_ALWAYS_INLINE constexpr Ret walkOptionalAbort (ErrorCode code) {
					return { std::move (*this), Ret::signal (code)};
				}

				template <auto MemRef, auto PresRef, typename El>
				USER_ALWAYS_INLINE constexpr Descend<El, bitOffset, SizeTracker> walkOptional () {
					return { std::move (raw), std::move (last), obj.*MemRef, std::move (st ()) };
				}

				USER_ALWAYS_INLINE void beginContainer () {}
				USER_ALWAYS_INLINE void endContainer () {}
				USER_ALWAYS_INLINE void stepContainer () {}

				template <typename Ret, typename Invoker, auto MemRef>
				USER_ALWAYS_INLINE Ret invokeStructHook () {
					return Invoker::invoke (obj.*MemRef, obj);
				}

				template <typename Ret, typename Invoker>
				USER_ALWAYS_INLINE Ret invokeHook () {
					return Invoker::invoke (obj);
				}
				template <typename Ret>
				USER_ALWAYS_INLINE Ret abortHook (ErrorCode code) {
					return { std::move (*this), Ret::signal (code) };
				}
			private:
				USER_ALWAYS_INLINE constexpr Descend (RawIter r, SWord l, ObjRef o, SizeTracker t) : SizeTracker (std::move (t)), raw (std::move (r)), last (std::move (l)), obj (o) {}
				USER_ALWAYS_INLINE SizeTracker& st () { return *this; }

				RawIter raw;
				SWord last;
				ObjRef obj;
		};

		template <typename Obj, typename SizeT, typename Attr>
		using GetTracker = typename Helper::GetSizeTracker<SizeT,	decltype (SWalk::walk (std::declval<Descend<Obj, 0, Helper::SizeTrackerNone>> (), std::declval<Attr> ()))::bits,
																	typename decltype (SWalk::walk (std::declval<Descend<Obj, 0, Helper::SizeTrackerNone>> (), std::declval<Attr> ()))::MaxDyn,
																	wSize>;

		template <typename Obj, typename SizeT, typename Attr>
		USER_ALWAYS_INLINE static Descend<Obj, 0, typename GetTracker<Obj, SizeT, Attr>::Type> root (RawIter raw, std::conditional_t<Read, Obj&, const Obj&> obj, SizeT size) {
			return { std::move (raw), SWord {}, obj, GetTracker<Obj, SizeT, Attr>::get (std::move (size)) };
		}
	};

	template <bool Read, typename RawIter, typename... Attr>
	using GetSerAlgo = W_Serialize<Helper::GetRawInfo2<RawIter, DefAttr<Attr...>>, Read>;

	struct W_Compare {
		template <typename Obj_>
		class Descend;
		class Return;

		template <typename...>
		using SumRet = Return;

		template <typename, std::size_t>
		using MultRetStatic = Return;

		template <typename>
		using MultRetDynamic = Return;

		template <typename, std::size_t>
		using MultRetDynamicMax = Return;

		template <typename T>
		using Qualify = const T;

		using PreHookTag = void;
		using PostHookTag = void;

		class Return {
			template <typename>
			friend class Descend;
			public:
				using GetFailRet = Return;
				USER_ALWAYS_INLINE static constexpr bool allowDynamic () { return true; }
				USER_ALWAYS_INLINE static constexpr bool canFail () { return true; }

				USER_ALWAYS_INLINE constexpr Return (Return&& r) = default;

				template <typename Obj>
				USER_ALWAYS_INLINE constexpr Return (Descend<Obj>&& ) : m_result (true) {}
				USER_ALWAYS_INLINE constexpr Return (bool res) : m_result (res) {}

				Return (const Return&) = delete;
				USER_ALWAYS_INLINE Return& operator = (Return&&) = default;
				Return& operator = (Return&) = delete;

				USER_ALWAYS_INLINE bool result () const { return m_result; }
				USER_ALWAYS_INLINE bool cancel () const { return !m_result; }
			private:
				bool m_result;
		};
		template <typename Obj_>
		class Descend {
			friend class Return;
			friend struct W_Compare;
			public:
				using Obj = Obj_;
				using QObj = Qualify<Obj>;
				using ObjRef = QObj&;
				using Algo = W_Compare;

				template <typename>
				using Advance = Descend;

				USER_ALWAYS_INLINE Descend (Descend&&) = default;
				Descend (const Descend&) = delete;
				USER_ALWAYS_INLINE Descend& operator = (Descend&&) { return *this; }
				Descend& operator = (Descend&) = delete;

				template <Bit, Bit, typename, typename>
				USER_ALWAYS_INLINE Return walkInteger () {
					return { m_a == m_b };
				}

				template <Bit, Bit, typename, typename>
				USER_ALWAYS_INLINE Return walkEnum () {
					return { m_a == m_b };
				}

				template <typename Ret>
				USER_ALWAYS_INLINE constexpr Advance<Ret> advance (Ret) {
					return { m_a, m_b };
				}

				template <typename Target, auto MemPtr>
				USER_ALWAYS_INLINE constexpr Descend<Target> structMember () {
					return { m_a.*MemPtr, m_b.*MemPtr };
				}

				template <std::size_t Index, typename Elem>
				USER_ALWAYS_INLINE constexpr Descend<Elem> tupleElem () {
					return { std::get<Index> (m_a), std::get<Index> (m_b) };
				}

				template <typename El, typename Iter>
				USER_ALWAYS_INLINE constexpr Descend<El> contElem (std::pair<Iter, Iter>& iter, int& state) {
					Qualify<El>& a = *iter.first;
					Qualify<El>& b = *iter.second;
					++iter.first;
					++iter.second;
					assert ((iter.first == end (m_a)) == (iter.second == end (m_b)));
					if (iter.first == end (m_a))
						state = 0;
					else
						state = 1;
					return { a, b };
				}
				template <typename El, typename Iter>
				USER_ALWAYS_INLINE constexpr Descend<El> contElem (std::pair<Iter, Iter>& iter) {
					Qualify<El>& a = *iter.first;
					Qualify<El>& b = *iter.second;
					++iter.first;
					++iter.second;
					assert ((iter.first == end (m_a)) == (iter.second == end (m_b)));
					return { a, b };
				}

				template <typename El>
				USER_ALWAYS_INLINE constexpr Descend<El> ptrArrayElem (std::size_t index) {
					return { m_a [index], m_b [index] };
				}

				template <auto ArrRef, auto SizeRef, typename El, typename SubRet, typename MaxT>
				USER_ALWAYS_INLINE constexpr int checkIterateDynSize (std::size_t& count, bool& ok, MaxT max) {
					std::size_t countA = Helper::invokeRef (SizeRef, m_a);
					std::size_t countB = Helper::invokeRef (SizeRef, m_b);
					if (countA != countB) { ok = false; return 0; }
					count = countA;
					ok = max.check (countA);
					return 0;
				}
				template <typename El, typename SubRet>
				USER_ALWAYS_INLINE constexpr int checkIterateDyn (int& res) {
					auto countA = std::size (m_a);
					auto countB = std::size (m_b);
					if (countA != countB) { res = -1; return 0; }
					if (countA == 0)
						res = 0;
					else
						res = 1;
					return 0;
				}

				template <typename SubRet, typename Ret>
				USER_ALWAYS_INLINE constexpr Return iterateDynAbort (int) {
					return { false };
				}

				template <auto ArrRef, auto SizeRef, typename El>
				USER_ALWAYS_INLINE constexpr Descend<Helper::MemVarPtrTargetType<ArrRef>> iterateDynSize () {
					return { m_a.*ArrRef, m_b.*ArrRef };
				}

				template <typename El, typename Iter>
				USER_ALWAYS_INLINE std::pair<Iter,Iter> beginIterateCont () {
					return { begin (m_a), begin (m_b) };
				}

				template <auto MemRef, auto MetaRef, typename El, typename SubRet>
				USER_ALWAYS_INLINE constexpr int checkOptional (bool& present, bool& ok) {
					bool pA = Helper::invokeRef (MetaRef, m_a);
					bool pB = Helper::invokeRef (MetaRef, m_b);
					if (pA != pB) {
						ok = false;
						return 0;
					}
					present = pA;
					ok = true;
					return 0;
				}

				template <auto MemRef, typename SubRet, typename Ret>
				USER_ALWAYS_INLINE constexpr Ret walkOptionalAbort (int) {
					return { false };
				}

				template <auto MemRef, auto PresRef, typename El>
				USER_ALWAYS_INLINE constexpr Descend<El> walkOptional () {
					return { m_a.*MemRef, m_b.*MemRef };
				}
				USER_ALWAYS_INLINE void beginContainer () {}
				USER_ALWAYS_INLINE void endContainer () {}
				USER_ALWAYS_INLINE void stepContainer () {}
			private:
				USER_ALWAYS_INLINE constexpr Descend (ObjRef a, ObjRef b) : m_a (a), m_b (b) {}
				ObjRef m_a, m_b;
		};

		template <typename Obj>
		static Descend<Obj> root (const Obj& a, const Obj& b) {
			return { a, b };
		}
	};

	struct W_Print {
		template <typename Obj_>
		class Descend;
		class Return;

		template <typename...>
		using SumRet = Return;

		template <typename, std::size_t>
		using MultRetStatic = Return;

		template <typename>
		using MultRetDynamic = Return;

		template <typename, std::size_t>
		using MultRetDynamicMax = Return;

		template <typename T>
		using Qualify = const T;

		using PreHookTag = void;
		using PostHookTag = void;

		class Return {
			template <typename>
			friend class Descend;
			public:
				using GetFailRet = Return;

				USER_ALWAYS_INLINE static constexpr bool allowDynamic () { return true; }
				USER_ALWAYS_INLINE static constexpr bool canFail () { return false; }

				USER_ALWAYS_INLINE constexpr Return () = default;
				template <typename Obj>
				USER_ALWAYS_INLINE constexpr Return (Descend<Obj>) { }
				USER_ALWAYS_INLINE Return (Return&&) = default;
				Return (const Return&) = delete;
				USER_ALWAYS_INLINE Return& operator = (Return&&) = default;
				Return& operator = (Return&) = delete;
		};

		template <typename Obj_>
		class Descend {
			friend class Return;
			friend struct W_Print;
			public:
				using Obj = Obj_;
				using QObj = Qualify<Obj>;
				using ObjRef = QObj&;
				using Algo = W_Print;

				template <typename>
				using Advance = Descend;

				USER_ALWAYS_INLINE Descend (Descend&&) = default;
				Descend (const Descend&) = delete;
				USER_ALWAYS_INLINE Descend& operator = (Descend&&) { return *this; }
				Descend& operator = (Descend&) = delete;

				template <Bit Width, Bit, typename, typename, class X = Obj>
				USER_ALWAYS_INLINE std::enable_if_t<!Helper::IsChar<X>,Return> walkInteger () {
					stream << "0x" << std::hex << std::setw ((Width + 3) / 4) << std::setfill('0') << obj;
					return {};
				}
				template <Bit Width, Bit, typename, typename, class X = Obj>
				USER_ALWAYS_INLINE std::enable_if_t<Helper::IsChar<X>,Return> walkInteger () {
					stream << "0x" << std::hex << std::setw ((Width + 3) / 4) << std::setfill('0') << static_cast<int> (obj);
					return {};
				}
				template <Bit Width, Bit, typename, typename, class X = Obj>
				USER_ALWAYS_INLINE std::enable_if_t<!Helper::IsChar<std::underlying_type_t<X>>,Return> walkEnum () {
					stream << "0x" << std::hex << std::setw ((Width + 3) / 4) << std::setfill('0') << static_cast<std::underlying_type_t<Obj>> (obj);
					return {};
				}
				template <Bit Width, Bit, typename, typename, class X = Obj>
				USER_ALWAYS_INLINE std::enable_if_t<Helper::IsChar<std::underlying_type_t<X>>,Return> walkEnum () {
					stream << "0x" << std::hex << std::setw ((Width + 3) / 4) << std::setfill('0') << static_cast<int> (static_cast<std::underlying_type_t<Obj>> (obj));
					return {};
				}
				USER_ALWAYS_INLINE constexpr Descend<Obj> advance (Return) {
					return { obj, stream };
				}

				template <typename Target, auto MemPtr>
				USER_ALWAYS_INLINE constexpr Descend<Target> structMember () {
					return { obj.*MemPtr , stream };
				}

				template <std::size_t Index, typename Elem>
				USER_ALWAYS_INLINE constexpr Descend<Elem> tupleElem () {
					return { std::get<Index> (obj), stream };
				}

				template <typename El, typename Iter>
				USER_ALWAYS_INLINE constexpr Descend<El> contElem (Iter& iter, int& state) {
					Qualify<El>& elem = *iter;
					++iter;
					if (iter == end (obj))
						state = 0;
					else
						state = 1;
					return { elem, stream };
				}
				template <typename El, typename Iter>
				USER_ALWAYS_INLINE constexpr Descend<El> contElem (Iter& iter) {
					Qualify<El>& elem = *iter;
					++iter;
					return { elem, stream };
				}

				template <typename El>
				USER_ALWAYS_INLINE constexpr Descend<El> ptrArrayElem (std::size_t index) {
					return { obj [index], stream };
				}

				template <auto ArrRef, auto SizeRef, typename El, typename SubRet, typename MaxT>
				USER_ALWAYS_INLINE constexpr int checkIterateDynSize (std::size_t& count, bool& ok, MaxT max) {
					count = Helper::invokeRef (SizeRef, obj);
					ok = max.check (count);
					return 0;
				}
				template <typename El, typename SubRet>
				USER_ALWAYS_INLINE constexpr int checkIterateDyn (int& res) {
					auto s = std::size (obj);
					if (s == 0)
						res = 0;
					else
						res = 1;
					return 0;
				}
				template <typename SubRet, typename Ret>
				USER_ALWAYS_INLINE constexpr Ret iterateDynAbort (int) {
					return { };
				}
				template <auto ArrRef, auto SizeRef, typename El>
				USER_ALWAYS_INLINE constexpr Descend<Helper::MemVarPtrTargetType<ArrRef>> iterateDynSize () {
					return { obj.*ArrRef, stream };
				}
				template <typename El, typename Iter>
				USER_ALWAYS_INLINE Iter beginIterateCont () {
					return begin (obj);
				}

				template <auto MemRef, auto MetaRef, typename El, typename SubRet>
				USER_ALWAYS_INLINE constexpr int checkOptional (bool& present, bool& ok) {
					present = Helper::invokeRef (MetaRef, obj);
					ok = true;
					return 0;
				}

				template <auto MemRef, typename SubRet, typename Ret>
				USER_ALWAYS_INLINE constexpr Ret walkOptionalAbort (int) {
					return { };
				}

				template <auto MemRef, auto PresRef, typename El>
				USER_ALWAYS_INLINE constexpr Descend<El> walkOptional () {
					return { obj.*MemRef, stream };
				}
				USER_ALWAYS_INLINE void beginContainer () {
					stream << "{";
				}
				USER_ALWAYS_INLINE void endContainer () {
					stream << "}";
				}
				USER_ALWAYS_INLINE void stepContainer () {
					stream << ", ";
				}
			private:
				USER_ALWAYS_INLINE Descend (ObjRef o, std::ostream& str) : obj (o), stream (str) {}

				ObjRef obj;
				std::ostream& stream;
		};


		template <typename Obj>
		USER_ALWAYS_INLINE static Descend<Obj> root (std::ostream& str, const Obj& obj) {
			return { obj, str };
		}
	};

	namespace Helper {
		template <typename... Attr>
		using GetRootAttr = AttrRemoveTag<uSer::DefAttr<Attr...>, Helper::RawInfoTag>;

		template <typename RawIter, typename Obj, bool Read, typename SizeT, typename... Attr>
		static constexpr bool SerWalkCanFail = decltype (SWalk::walk (GetSerAlgo<Read, RawIter, Attr...>
				::template root<Obj, SizeT, GetRootAttr<Attr...>>
				 	 (std::declval<RawIter> (), std::declval<Obj&> (), std::declval<SizeT> ()), GetRootAttr<Attr...> {}))::canFail ();

		template <typename RawIter, typename Obj, bool Read, typename SizeT, typename... Attr>
		static constexpr bool SerPrepFail = GetSerAlgo<Read, RawIter, Attr...>::template GetTracker<Obj, SizeT, uSer::DefAttr<Attr...>>::canFail;
	}

	/// Checks whether a serialization procedure can fail and the serialize/deserialize functions may indicate an error
	template <typename RawIter, typename Obj, bool Read, typename SizeT, typename... Attr>
	static constexpr bool SerCanFail = Helper::SerWalkCanFail<RawIter, Obj, Read, SizeT, Attr...> || Helper::SerPrepFail<RawIter, Obj, Read, SizeT, Attr...>;

	namespace Helper {
		template <typename RawIter, typename T, typename... Attr>
		std::true_type IsSerializable (decltype(SWalk::walk (std::declval<GetSerAlgo<false, RawIter, Attr...>::template Descend <T, 0, Helper::SizeTrackerNone>> (), GetRootAttr<Attr...> {}))*);
		template <typename RawIter, typename T, typename... Attr>
		std::false_type IsSerializable (...);

#ifndef USER_EXCEPTIONS
		template <typename RawIter, typename Obj, bool Read, typename SizeT, typename... Attr>
		using RootErrRet = std::conditional_t<SerCanFail<RawIter, Obj, Read, SizeT, Attr...>, ErrorCode, void>;
#else
		template <typename RawIter, typename Obj, bool Read, typename... Attr>
		using RootErrRet = void;
#endif


		template <typename Obj, typename ObjRef, bool Read, typename SizeT, typename... Attr, typename RawIter>
		Helper::RootErrRet<RawIter, Obj, Read, SizeT, Attr...>
		USER_ALWAYS_INLINE serializeDo (RawIter raw, ObjRef obj, SizeT size, std::size_t* sizeUsed) {
			using A = GetRootAttr<Attr...>;

			using STracker = typename GetSerAlgo<Read, RawIter, Attr...>::template GetTracker<Obj, SizeT, A>;

			if constexpr (STracker::canFail) {
				if (!STracker::check (size)) {
#ifndef USER_EXCEPTIONS
					return ErrorCode::EBUFSIZE;
#else
					throw Exception (ErrorCode::EBUFSIZE);
#endif
				}
			}

			auto desc = GetSerAlgo<Read, RawIter, Attr...>::template root<Obj, SizeT, A> (std::move (raw), obj, std::move (size));
			auto lastRet = SWalk::walk (std::move (desc), A {});

			if (sizeUsed) {
				*sizeUsed = STracker::used (size, lastRet);
			}

#ifndef USER_EXCEPTIONS
			if constexpr (SerWalkCanFail<RawIter, Obj, Read, SizeT, Attr...>)
				return lastRet.finish ().errorOrVoid ();
			else {
#endif
				lastRet.finish ();
#ifndef USER_EXCEPTIONS
				if constexpr (STracker::canFail)
						return ErrorCode::OK;
			}
#endif
		}

		template <typename T>
		USER_ALWAYS_INLINE auto getRawIter (T&& obj) -> decltype(begin(std::forward<T> (obj))) {
			return begin (std::forward<T> (obj));
		}

		template <typename T, class = typename std::iterator_traits<std::remove_reference_t<T>>::value_type>
		USER_ALWAYS_INLINE T&& getRawIter (T&& iter) {
			return std::forward<T> (iter);
		}
	}

	// API

	/// Convenience template variable to create an instance of \ref FixedSize
	template <std::size_t S>
	static constexpr FixedSize<S> fixedSize {};

	/// Convenience variable to create an instance of \ref InfSize
	static constexpr InfSize infSize {};

	/// Returns the number of fixed bits in raw data
	template <typename RawIter, typename Obj, typename... Attr>
	static constexpr std::size_t rawStaticBits = decltype (SWalk::walk (GetSerAlgo<true, RawIter, Attr...>::template root<Obj, InfSize, Helper::GetRootAttr<Attr...>> (std::declval<RawIter> (), std::declval<Obj&> (), infSize), Helper::GetRootAttr<Attr...> {}))::bits;

	/// Indicates the maximum number of dynamic serialization words in raw data; may be an alias to \ref Unlimited, \ref NoDyn, or \ref MaxSize
	template <typename RawIter, typename Obj, typename... Attr>
	using RawMaxDynamic = typename decltype (SWalk::walk (GetSerAlgo<true, RawIter, Attr...>::template root<Obj, InfSize, Helper::GetRootAttr<Attr...>> (std::declval<RawIter> (), std::declval<Obj&> (), infSize), Helper::GetRootAttr<Attr...> {}))::MaxDyn;

	/// The minimum required buffer size required to serialize a certain type
	template <typename RawIter, typename Obj, typename... Attr>
	static constexpr std::size_t minBufSize = (rawStaticBits<RawIter, Obj, Attr...>
		+ Helper::GetRawInfo2<RawIter, DefAttr<Attr...>>::wSize - 1)
			/ Helper::GetRawInfo2<RawIter, DefAttr<Attr...>>::wSize;

	/// The maximum required buffer size required to serialize a certain type; this requires a maximum to exist, i.e. no containers may be used except arrays.
	template <typename RawIter, typename Obj, typename... Attr>
	static constexpr std::size_t maxBufSize = RawMaxDynamic<RawIter, Obj, Attr...>::value + minBufSize<RawIter, Obj, Attr...>;

	/*
	 * == Overloads for the serialize() API function ==
	 */

	/**
	 * Serialize to an arbitrary raw output stream or container given via an iterator. The output buffer size must me given explicitly.
	 * \param[out]	raw			Any output iterator for the raw data.
	 * \param[in]	obj			The object to serialize
	 * \param[in]	size		The size of the raw data stream; either a \ref fixedSize or \ref infSize
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If an error is possible for the requested data types AND exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename RawIter, typename Obj, typename SizeT, std::enable_if_t<std::is_convertible_v<SizeT, Helper::SizeDefTag>, int> = 42>
	decltype(auto) serialize (RawIter&& raw, const Obj& obj, SizeT size, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, const Obj&, false, SizeT, Attr...> (Helper::getRawIter (std::forward<RawIter> (raw)), obj, std::move (size), sizeUsed);
	}

	/**
	 * Serialize to a C-array given as a reference. The array's type must include the size, i.e. it can't be a pointer to an array.
	 * \param[out]	raw			A reference to a C array.
	 * \param[in]	obj			The object to serialize
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If an error is possible for the requested data types AND exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename SWord, typename Obj, std::size_t N>
	decltype(auto) serialize (SWord (&raw) [N], const Obj& obj, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, const Obj&, false, FixedSize<N>, Attr...> (begin (raw), obj, {}, sizeUsed);
	}

	/**
	 * Serialize to std::array<T,N> given as a reference.
	 * \param[out]	raw			A reference to std::array<T,N>.
	 * \param[in]	obj			The object to serialize
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If an error is possible for the requested data types AND exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename SWord, typename Obj, std::size_t N>
	decltype(auto) serialize (std::array<SWord,N>& raw, const Obj& obj, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, const Obj&, false, FixedSize<N>, Attr...> (begin (raw), obj, {}, sizeUsed);
	}

	/**
	 * Serialize to an arbitrary raw output stream or container given via an iterator. The output buffer size must me given explicitly as an integer.
	 * \param[out]	raw			Any output iterator for the raw data.
	 * \param[in]	obj			The object to serialize
	 * \param[in]	size		The size of the raw data stream given as a std::size_t
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename RawIter, typename Obj>
	decltype(auto) serialize (RawIter&& raw, const Obj& obj, std::size_t size, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, const Obj&, false, DynSize, Attr...> (Helper::getRawIter (std::forward<RawIter> (raw)), obj, DynSize {size}, sizeUsed);
	}

	/**
	 * Serialize to an arbitrary container given as a reference. The output buffer's size is queried via its .size() member function.
	 * \param[out]	raw			Any container supporting iterators and a .size() member function
	 * \param[in]	obj			The object to serialize
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename RawContainer, typename Obj, decltype (std::size (std::declval<RawContainer> ()))* = nullptr>
	decltype(auto) serialize (RawContainer&& raw, const Obj& obj, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, const Obj&, false, DynSize, Attr...> (Helper::getRawIter (std::forward<RawContainer> (raw)), obj, DynSize { std::size (raw) }, sizeUsed);
	}

	/*
	 * == Overloads for the deserialize() API function ==
	 */

	/**
	 * Deserialize an arbitrary raw input stream or container given via an iterator. The input buffer size must me given explicitly.
	 * \param[in]	raw			Any input iterator for the raw data.
	 * \param[out]	obj			The object to receive the deserialized data
	 * \param[in]	size		The size of the raw data stream; either a \ref fixedSize or \ref infSize
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If an error is possible for the requested data types AND exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename RawIter, typename Obj, typename SizeT, std::enable_if_t<std::is_convertible_v<SizeT, Helper::SizeDefTag>, int> = 42>
	decltype(auto) deserialize (RawIter&& raw, Obj& obj, SizeT size, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, Obj&, true, SizeT, Attr...> (Helper::getRawIter (std::forward<RawIter> (raw)), obj, std::move (size), sizeUsed);
	}

	/**
	 * Deserialize from a const C-array given as a reference. The array's type must include the size, i.e. it can't be a pointer to an array.
	 * \param[in]	raw			A reference to a constant C array.
	 * \param[out]	obj			The object to receive the deserialized data
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If an error is possible for the requested data types AND exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename SWord, typename Obj, std::size_t N>
	decltype(auto) deserialize (const SWord (&raw) [N], Obj& obj, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, Obj&, true, FixedSize<N>, Attr...> (begin (raw), obj, {}, sizeUsed);
	}

	/**
	 * Deserialize from a const std::array<T,N> given as a reference.
	 * \param[in]	raw			A reference to constant std::array<T,N>.
	 * \param[out]	obj			The object to receive the deserialized data
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If an error is possible for the requested data types AND exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename SWord, typename Obj, std::size_t N>
	decltype(auto) deserialize (const std::array<SWord,N>& raw, Obj& obj, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, Obj&, true, FixedSize<N>, Attr...> (begin (raw), obj, {}, sizeUsed);
	}

	/**
	 * Deserialize from a non-const C-array given as a reference. The array's type must include the size, i.e. it can't be a pointer to an array.
	 * This overload prevents the one for generic containers from being called if the array is non-const.
	 *
	 * \param[in]	raw			A reference to a C array.
	 * \param[out]	obj			The object to receive the deserialized data
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If an error is possible for the requested data types AND exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename SWord, typename Obj, std::size_t N>
	decltype(auto) deserialize (SWord (&raw) [N], Obj& obj, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, Obj&, true, FixedSize<N>, Attr...> (begin (raw), obj, {}, sizeUsed);
	}

	/**
	 * Deserialize from a non-const std::array<T,N> given as a reference.
	 * This overload prevents the one for generic containers from being called if the array is non-const.
	 *
	 * \param[in]	raw			A reference to std::array<T,N>.
	 * \param[out]	obj			The object to receive the deserialized data
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If an error is possible for the requested data types AND exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	// Non-Const std::array without explicit size. This overload prevents the last one from being called if the array is non-const.
	template <typename... Attr, typename SWord, typename Obj, std::size_t N>
	decltype(auto) deserialize (std::array<SWord,N>& raw, Obj& obj, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, Obj&, true, FixedSize<N>, Attr...> (begin (raw), obj, {}, sizeUsed);
	}

	/**
	 * Deserialize from an arbitrary raw input stream or container given via an iterator. The input buffer size must me given explicitly as an integer.
	 * \param[in]	raw			Any input iterator for the raw data.
	 * \param[out]	obj			The object to receive the deserialized data
	 * \param[in]	size		The size of the raw data stream given as a std::size_t
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename RawIter, typename Obj>
	decltype(auto) deserialize (RawIter&& raw, Obj& obj, std::size_t size, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, Obj&, true, DynSize, Attr...> (Helper::getRawIter (std::forward<RawIter> (raw)), obj, DynSize {size}, sizeUsed);
	}

	/**
	 * Deserialize from an arbitrary container given as a reference. The input buffer's size is queried via its .size() member function.
	 * \param[in]	raw			Any container supporting iterators and a .size() member function
	 * \param[out]	obj			The object to receive the deserialized data
	 * \param[out]	sizeUsed	Optional output parameter indicating the number of serialization words actually used in the raw data stream.
	 * \return					If exceptions are disabled, an \ref ErrorCode is returned; else the return type is void.
	 */
	template <typename... Attr, typename RawContainer, typename Obj, decltype (std::size (std::declval<RawContainer> ()))* = nullptr>
	decltype(auto) deserialize (RawContainer&& raw, Obj& obj, std::size_t* sizeUsed = nullptr) {
		return Helper::serializeDo<Obj, Obj&, true, DynSize, Attr...> (Helper::getRawIter (std::forward<RawContainer> (raw)), obj, DynSize { std::size (raw) }, sizeUsed);
	}

	/**
	 * Compares two serializable objects of equal type.
	 * \return					true if a and b are equal, else false.
	 */
	template <typename... Attr, typename Obj>
	constexpr bool compare (const Obj& a, const Obj& b) {
		using A = Helper::GetRootAttr<Attr...>;
		return SWalk::walk (W_Compare::template root<Obj> (a, b), A {}).result ();
	}

	/**
	 * Outputs a textual version of a serializable object.
	 * \param[out] os			A stream to write the textual data to, e.g. std::cout.
	 * \param[in] obj			The object to print
	 * \return					os
	 */
	template <typename... Attr, typename Obj>
	std::ostream& print (std::ostream& os, const Obj& obj) {
		using A = Helper::GetRootAttr<Attr...>;
		std::ios  state (nullptr);
		state.copyfmt (os);
		SWalk::walk (W_Print::template root<Obj> (os, obj), A {});
		os.copyfmt(state);
		return os;
	}

	/// A static constant defined to true if the given type can be serialized
	template <typename RawIter, typename T, typename... Attr>
	static constexpr bool IsSerializable = decltype(Helper::IsSerializable<RawIter, T, Attr...> (nullptr))::value;
}

#else

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

/// Begin intrusive struct definition
#define USER_STRUCT(name,...)
/// Define attributes for single struct member
#define USER_MEM_ANNOT(name, ...)
/// List previously defined struct members
#define USER_ENUM_MEM(...)
/// Define and annotate struct member
#define USER_MEM(type,name,...)					type name;
/// Define and annotate multiple struct members
#define USER_DEF_MEM(...)						USER_PPM_MAP(USER_DEF_MEM_I,,__VA_ARGS__)

/// Annotate external struct
#define USER_EXT_ANNOT(name,...)
/// Annotate external struct member
#define USER_EXT_MEM_ANNOT(sname,name, ...)
/// List external struct members
#define USER_EXT_ENUM_MEM(sname,...)
/// List and annotate external struct members
#define USER_EXT_DEF_MEM(sname,...)

#define USER_DEF_MEM_I(data,dummy)				USER_PPM_EVAL0(USER_MEM data)

#endif


/*
 * Modified version of William Swanson's "MAP" macro
 *
 * https://github.com/swansontec/map-macro
 * Copyright (C) 2012 William Swanson
 *
 */

#define USER_PPM_EVAL0(...) __VA_ARGS__
#define USER_PPM_EVAL1(...) USER_PPM_EVAL0(USER_PPM_EVAL0(USER_PPM_EVAL0(__VA_ARGS__)))
#define USER_PPM_EVAL2(...) USER_PPM_EVAL1(USER_PPM_EVAL1(USER_PPM_EVAL1(__VA_ARGS__)))
#define USER_PPM_EVAL3(...) USER_PPM_EVAL2(USER_PPM_EVAL2(USER_PPM_EVAL2(__VA_ARGS__)))
#define USER_PPM_EVAL4(...) USER_PPM_EVAL3(USER_PPM_EVAL3(USER_PPM_EVAL3(__VA_ARGS__)))
#define USER_PPM_EVAL(...)  USER_PPM_EVAL4(USER_PPM_EVAL4(USER_PPM_EVAL4(__VA_ARGS__)))

#define USER_PPM_MAP_END(...)
#define USER_PPM_MAP_OUT
#define USER_PPM_MAP_COMMA ,

#define USER_PPM_MAP_GET_END2() 0, USER_PPM_MAP_END
#define USER_PPM_MAP_GET_END1(...) USER_PPM_MAP_GET_END2
#define USER_PPM_MAP_GET_END(...) USER_PPM_MAP_GET_END1
#define USER_PPM_MAP_NEXT0(test, next, ...) next USER_PPM_MAP_OUT
#define USER_PPM_MAP_NEXT1(test, next) USER_PPM_MAP_NEXT0(test, next, 0)
#define USER_PPM_MAP_NEXT(test, next)  USER_PPM_MAP_NEXT1(USER_PPM_MAP_GET_END test, next)

#define USER_PPM_MAP0(f,data, x, peek, ...) f(x,data) USER_PPM_MAP_NEXT(peek, USER_PPM_MAP1)(f, data, peek, __VA_ARGS__)
#define USER_PPM_MAP1(f,data, x, peek, ...) f(x,data) USER_PPM_MAP_NEXT(peek, USER_PPM_MAP0)(f, data, peek, __VA_ARGS__)

#define USER_PPM_MAP_LIST_NEXT1(test, next) USER_PPM_MAP_NEXT0(test, USER_PPM_MAP_COMMA next, 0)
#define USER_PPM_MAP_LIST_NEXT(test, next)  USER_PPM_MAP_LIST_NEXT1(USER_PPM_MAP_GET_END test, next)

#define USER_PPM_MAP_LIST0(f, data, x, peek, ...) f(x,data) USER_PPM_MAP_LIST_NEXT(peek, USER_PPM_MAP_LIST1)(f, data, peek, __VA_ARGS__)
#define USER_PPM_MAP_LIST1(f, data, x, peek, ...) f(x,data) USER_PPM_MAP_LIST_NEXT(peek, USER_PPM_MAP_LIST0)(f, data, peek, __VA_ARGS__)

/*
 * Applies the function macro `f` to each of the remaining parameters.
 */
#define USER_PPM_MAP(f,data, ...) USER_PPM_EVAL(USER_PPM_MAP1(f,data, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/*
 * Applies the function macro `f` to each of the remaining parameters and
 * inserts commas between the results.
 */
#define USER_PPM_MAP_LIST(f,data, ...) USER_PPM_EVAL(USER_PPM_MAP_LIST1(f,data, __VA_ARGS__, ()()(), ()()(), ()()(), 0))


#endif /* USER_HH_ */
