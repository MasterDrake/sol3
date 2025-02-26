// sol2

// The MIT License (MIT)

// Copyright (c) 2013-2022 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SOL_TUPLE_HPP
#define SOL_TUPLE_HPP

#include <sol/forward.hpp>
#include <sol/base_traits.hpp>

#include <EASTL/tuple.h>
#include <cstddef>

namespace sol {
	namespace detail {
		using swallow = std::initializer_list<int>;
	} // namespace detail

	namespace meta {
		template <typename T>
		using is_tuple = is_specialization_of<T, eastl::tuple>;

		template <typename T>
		constexpr inline bool is_tuple_v = is_tuple<T>::value;

		namespace detail {
			template <typename... Args>
			struct tuple_types_ {
				typedef types<Args...> type;
			};

			template <typename... Args>
			struct tuple_types_<eastl::tuple<Args...>> {
				typedef types<Args...> type;
			};
		} // namespace detail

		template <typename... Args>
		using tuple_types = typename detail::tuple_types_<Args...>::type;

		template <typename Arg>
		struct pop_front_type;

		template <typename Arg>
		using pop_front_type_t = typename pop_front_type<Arg>::type;

		template <typename... Args>
		struct pop_front_type<types<Args...>> {
			typedef void front_type;
			typedef types<Args...> type;
		};

		template <typename Arg, typename... Args>
		struct pop_front_type<types<Arg, Args...>> {
			typedef Arg front_type;
			typedef types<Args...> type;
		};

		template <eastl::size_t N, typename Tuple>
		using tuple_element = eastl::tuple_element<N, eastl::remove_reference_t<Tuple>>;

		template <eastl::size_t N, typename Tuple>
		using tuple_element_t = eastl::tuple_element_t<N, eastl::remove_reference_t<Tuple>>;

		template <eastl::size_t N, typename Tuple>
		using unqualified_tuple_element = unqualified<tuple_element_t<N, Tuple>>;

		template <eastl::size_t N, typename Tuple>
		using unqualified_tuple_element_t = unqualified_t<tuple_element_t<N, Tuple>>;

	} // namespace meta
} // namespace sol

#endif // SOL_TUPLE_HPP
