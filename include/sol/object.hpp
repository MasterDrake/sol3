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

#ifndef SOL_OBJECT_HPP
#define SOL_OBJECT_HPP

#include <sol/make_reference.hpp>
#include <sol/reference.hpp>
#include <sol/stack.hpp>
#include <sol/object_base.hpp>

namespace sol {

	template <typename base_type>
	class basic_object : public basic_object_base<base_type> {
	private:
		typedef basic_object_base<base_type> base_t;

		template <bool invert_and_pop = false>
		basic_object(eastl::integral_constant<bool, invert_and_pop>, lua_State* L_, int index_ = -1) noexcept : base_t(L_, index_) {
			if (invert_and_pop) {
				lua_pop(L_, -index_);
			}
		}

	protected:
		basic_object(detail::no_safety_tag, lua_nil_t n) : base_t(n) {
		}
		basic_object(detail::no_safety_tag, lua_State* L_, int index_) : base_t(L_, index_) {
		}
		basic_object(detail::no_safety_tag, lua_State* L_, ref_index index_) : base_t(L_, index_) {
		}
		template <typename T,
		     meta::enable<meta::neg<meta::any_same<meta::unqualified_t<T>, basic_object>>, meta::neg<eastl::is_same<base_type, stack_reference>>,
		          meta::neg<eastl::is_same<lua_nil_t, meta::unqualified_t<T>>>, is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_object(detail::no_safety_tag, T&& r) noexcept : base_t(eastl::forward<T>(r)) {
		}

		template <typename T, meta::enable<is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_object(detail::no_safety_tag, lua_State* L_, T&& r) noexcept : base_t(L_, eastl::forward<T>(r)) {
		}

	public:
		basic_object() noexcept = default;
		template <typename T,
		     meta::enable<meta::neg<eastl::is_same<meta::unqualified_t<T>, basic_object>>, meta::neg<eastl::is_same<base_type, stack_reference>>,
		          is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_object(T&& r) : base_t(eastl::forward<T>(r)) {
		}
		template <typename T, meta::enable<is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_object(lua_State* L_, T&& r) : base_t(L_, eastl::forward<T>(r)) {
		}
		basic_object(lua_State* L_, global_tag_t t) : base_t(L_, t) {
		}
		basic_object(lua_nil_t r) : base_t(r) {
		}
		basic_object(const basic_object&) = default;
		basic_object(basic_object&&) = default;
		basic_object(const stack_reference& r) noexcept : basic_object(r.lua_state(), r.stack_index()) {
		}
		basic_object(stack_reference&& r) noexcept : basic_object(r.lua_state(), r.stack_index()) {
		}
		template <typename Super>
		basic_object(const proxy_base<Super>& r) noexcept : basic_object(r.operator basic_object()) {
		}
		template <typename Super>
		basic_object(proxy_base<Super>&& r) noexcept : basic_object(r.operator basic_object()) {
		}
		basic_object(lua_State* L_, lua_nil_t r) noexcept : base_t(L_, r) {
		}
		basic_object(lua_State* L_, int index_ = -1) noexcept : base_t(L_, index_) {
		}
		basic_object(lua_State* L_, absolute_index index_) noexcept : base_t(L_, index_) {
		}
		basic_object(lua_State* L_, raw_index index_) noexcept : base_t(L_, index_) {
		}
		basic_object(lua_State* L_, ref_index index_) noexcept : base_t(L_, index_) {
		}
		template <typename T, typename... Args>
		basic_object(lua_State* L_, in_place_type_t<T>, Args&&... args) noexcept
		: basic_object(eastl::integral_constant<bool, !is_stack_based<base_t>::value>(), L_, -stack::push<T>(L_, eastl::forward<Args>(args)...)) {
		}
		template <typename T, typename... Args>
		basic_object(lua_State* L_, in_place_t, T&& arg, Args&&... args) noexcept
		: basic_object(L_, in_place_type<T>, eastl::forward<T>(arg), eastl::forward<Args>(args)...) {
		}
		basic_object& operator=(const basic_object&) = default;
		basic_object& operator=(basic_object&&) = default;
		basic_object& operator=(const base_type& b) {
			base_t::operator=(b);
			return *this;
		}
		basic_object& operator=(base_type&& b) {
			base_t::operator=(eastl::move(b));
			return *this;
		}
		template <typename Super>
		basic_object& operator=(const proxy_base<Super>& r) {
			this->operator=(r.operator basic_object());
			return *this;
		}
		template <typename Super>
		basic_object& operator=(proxy_base<Super>&& r) {
			this->operator=(r.operator basic_object());
			return *this;
		}
	};

	template <typename T>
	object make_object(lua_State* L_, T&& value) {
		return make_reference<object, true>(L_, eastl::forward<T>(value));
	}

	template <typename T, typename... Args>
	object make_object(lua_State* L_, Args&&... args) {
		return make_reference<T, object, true>(L_, eastl::forward<Args>(args)...);
	}

	template <typename T>
	object make_object_userdata(lua_State* L_, T&& value) {
		return make_reference_userdata<object, true>(L_, eastl::forward<T>(value));
	}

	template <typename T, typename... Args>
	object make_object_userdata(lua_State* L_, Args&&... args) {
		return make_reference_userdata<T, object, true>(L_, eastl::forward<Args>(args)...);
	}
} // namespace sol

#endif // SOL_OBJECT_HPP
