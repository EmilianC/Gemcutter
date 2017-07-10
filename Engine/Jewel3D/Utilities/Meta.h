// Copyright (c) 2017 Emilian Cioca
#pragma once
#include <type_traits>

// Contains various helpers and utilities for compile-time metaprogramming.
namespace Jwl { namespace Meta
{
	// Executes the expression on each instance (if any) of a referenced parameter pack.
	#define EXECUTE_PACK(expr) \
		do {														\
			s32 unused[] = { 0, ((expr), 0) ... }; (void)unused;	\
			__pragma(warning(suppress:4127))						\
		} while (false)

	// Allows for the use of complex template types as macro parameters.
	// Normally the ','s in a template parameter list would interfere with the macro expansion.
	#define TEMPLATE_TYPE(...) decltype(__VA_ARGS__())

	namespace /* all_of */
	{
		// Used to help validate parameter packs.
		// Evaluates to true only if all of the expressions are true.
		template<bool... b> struct all_of;

		template<bool... tail> struct all_of<true, tail...> : all_of<tail...> {};
		template<bool... tail> struct all_of<false, tail...> : std::false_type {};
		template<> struct all_of<> : std::true_type {};
		
		template<bool... b>
		constexpr bool all_of_v = all_of<b...>::value;
	} /* all_of */

	namespace /* any_of */
	{
		// Used to help validate parameter packs.
		// Evaluates to true if at least one of the expressions are true.
		template<bool... b> struct any_of;

		template<bool... tail> struct any_of<true, tail...> : std::true_type {};
		template<bool... tail> struct any_of<false, tail...> : any_of<tail...> {};
		template<> struct any_of<> : std::true_type {};

		template<bool... b>
		constexpr bool any_of_v = any_of<b...>::value;
	} /* any_of */

	namespace /* constexpr Hash */
	{
		// Used for compile-time hashing of strings using HashCRC().

		constexpr u32 StrLen(const char* str)
		{
			return *str ? 1 + StrLen(str + 1) : 0;
		}

		namespace detail
		{
			// Generate CRC lookup table.
			template <u32 c, s32 k = 8> struct f : f<((c & 1) ? 0xEDB88320 : 0) ^ (c >> 1), k - 1> {};
			template <u32 c> struct f<c, 0> { static constexpr u32 value = c; };

			#define HASH_GEN_A(x) HASH_GEN_B(x) HASH_GEN_B(x + 128)
			#define HASH_GEN_B(x) HASH_GEN_C(x) HASH_GEN_C(x + 64)
			#define HASH_GEN_C(x) HASH_GEN_D(x) HASH_GEN_D(x + 32)
			#define HASH_GEN_D(x) HASH_GEN_E(x) HASH_GEN_E(x + 16)
			#define HASH_GEN_E(x) HASH_GEN_F(x) HASH_GEN_F(x + 8)
			#define HASH_GEN_F(x) HASH_GEN_G(x) HASH_GEN_G(x + 4)
			#define HASH_GEN_G(x) HASH_GEN_H(x) HASH_GEN_H(x + 2)
			#define HASH_GEN_H(x) HASH_GEN_I(x) HASH_GEN_I(x + 1)
			#define HASH_GEN_I(x) f<x>::value ,

			constexpr u32 crcTable[] = { HASH_GEN_A(0) };

			#undef HASH_GEN_A
			#undef HASH_GEN_B
			#undef HASH_GEN_C
			#undef HASH_GEN_D
			#undef HASH_GEN_E
			#undef HASH_GEN_F
			#undef HASH_GEN_G
			#undef HASH_GEN_H
			#undef HASH_GEN_I

			constexpr u32 ComputeCRC(const char* p, u32 len, u32 crc)
			{
				return len ?
					ComputeCRC(p + 1, len - 1, (crc >> 8) ^ crcTable[(crc & 0xFF) ^ *p])
					: crc;
			}

			constexpr u32 HashCRC(const char* str, u32 length)
			{
				return ~detail::ComputeCRC(str, length, ~0u);
			}
		}

		constexpr u32 HashCRC(const char* str)
		{
			return detail::HashCRC(str, StrLen(str));
		}
	} /* constexpr Hash */

	namespace /* constexpr string */
	{
		// Used for compile-time in-place strings for use as template parameters.
		// STRING("...") will generate a strongly typed and constexpr string up to 64 max length.
		// Sized macros are available to select more specific lengths, up to 128.

		template<char... str>
		struct string
		{
		private:
			static_assert(sizeof...(str) <= 128, "Maximum length of a Meta::string is 128.");

			static constexpr u32 len = sizeof...(str);
			static constexpr char buff[len + 1] = { str..., '\0' };

		public:
			//- The size of the string, null character not included.
			static constexpr u32 size() { return StrLen(buff); }

			//- The length of the string, null character not included.
			static constexpr u32 length() { return StrLen(buff); }

			//- Returns a pointer to the first character in the string.
			static constexpr const char* data() { return buff; }

			//- Returns a pointer to the first character in the string.
			static constexpr const char* c_str() { return buff; }

			//- Returns a pointer to the first character in the string.
			static constexpr const char* cbegin() { return buff; }

			//- Returns a pointer to the last character in the string.
			static constexpr const char* cend() { return &buff[len]; }
		};

		namespace detail
		{
			/* Safely extract a single char from a cstring. */
			template<u32 index, u32 size>
			constexpr char getChar(const char (&c)[size])
			{
				return c[index < size ? index : size - 1];
			}

			#define EXPAND_STRING_16(n,str) \
				Jwl::Meta::detail::getChar<0x##n##0>(str), Jwl::Meta::detail::getChar<0x##n##1>(str), \
				Jwl::Meta::detail::getChar<0x##n##2>(str), Jwl::Meta::detail::getChar<0x##n##3>(str), \
				Jwl::Meta::detail::getChar<0x##n##4>(str), Jwl::Meta::detail::getChar<0x##n##5>(str), \
				Jwl::Meta::detail::getChar<0x##n##6>(str), Jwl::Meta::detail::getChar<0x##n##7>(str), \
				Jwl::Meta::detail::getChar<0x##n##8>(str), Jwl::Meta::detail::getChar<0x##n##9>(str), \
				Jwl::Meta::detail::getChar<0x##n##A>(str), Jwl::Meta::detail::getChar<0x##n##B>(str), \
				Jwl::Meta::detail::getChar<0x##n##C>(str), Jwl::Meta::detail::getChar<0x##n##D>(str), \
				Jwl::Meta::detail::getChar<0x##n##E>(str), Jwl::Meta::detail::getChar<0x##n##F>(str)

			#define EXPAND_STRING_32(str) EXPAND_STRING_16(0,str), EXPAND_STRING_16(1,str)
			#define EXPAND_STRING_48(str) EXPAND_STRING_32(str), EXPAND_STRING_16(2,str)
			#define EXPAND_STRING_64(str) EXPAND_STRING_48(str), EXPAND_STRING_16(3,str)
			#define EXPAND_STRING_80(str) EXPAND_STRING_64(str), EXPAND_STRING_16(4,str)
			#define EXPAND_STRING_96(str) EXPAND_STRING_80(str), EXPAND_STRING_16(5,str)
			#define EXPAND_STRING_112(str) EXPAND_STRING_96(str), EXPAND_STRING_16(6,str)
			#define EXPAND_STRING_128(str) EXPAND_STRING_112(str), EXPAND_STRING_16(7,str)
		}

		#define STRING_32(str) Jwl::Meta::string<EXPAND_STRING_32(str)>
		#define STRING_48(str) Jwl::Meta::string<EXPAND_STRING_48(str)>
		#define STRING_64(str) Jwl::Meta::string<EXPAND_STRING_64(str)>
		#define STRING_80(str) Jwl::Meta::string<EXPAND_STRING_80(str)>
		#define STRING_96(str) Jwl::Meta::string<EXPAND_STRING_96(str)>
		#define STRING_112(str) Jwl::Meta::string<EXPAND_STRING_112(str)>
		#define STRING_128(str) Jwl::Meta::string<EXPAND_STRING_128(str)>
		#define STRING(str) STRING_64(str)
	} /* constexpr string */
}}
