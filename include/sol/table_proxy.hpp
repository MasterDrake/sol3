// sol3

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

#ifndef SOL_TABLE_PROXY_HPP
#define SOL_TABLE_PROXY_HPP

#include <sol/traits.hpp>
#include <sol/function.hpp>
#include <sol/protected_function.hpp>
#include <sol/proxy_base.hpp>

namespace sol {

	template <typename Table, typename Key>
	struct table_proxy : public proxy_base<table_proxy<Table, Key>> {
	private:
		using key_type = detail::proxy_key_t<Key>;

		template <typename T, eastl::size_t... I>
		decltype(auto) tuple_get(eastl::index_sequence<I...>) const& {
			return tbl.template traverse_get<T>(eastl::get<I>(key)...);
		}

		template <typename T, eastl::size_t... I>
		decltype(auto) tuple_get(eastl::index_sequence<I...>) && {
			return tbl.template traverse_get<T>(eastl::get<I>(eastl::move(key))...);
		}

		template <eastl::size_t... I, typename T>
		void tuple_set(eastl::index_sequence<I...>, T&& value) & {
			tbl.traverse_set(eastl::get<I>(key)..., eastl::forward<T>(value));
		}

		template <eastl::size_t... I, typename T>
		void tuple_set(eastl::index_sequence<I...>, T&& value) && {
			tbl.traverse_set(eastl::get<I>(eastl::move(key))..., eastl::forward<T>(value));
		}

		auto setup_table(eastl::true_type) {
			auto p = stack::probe_get_field<eastl::is_same_v<meta::unqualified_t<Table>, global_table>>(lua_state(), key, tbl.stack_index());
			lua_pop(lua_state(), p.levels);
			return p;
		}

		bool is_valid(eastl::false_type) {
			auto pp = stack::push_pop(tbl);
			auto p = stack::probe_get_field<eastl::is_same_v<meta::unqualified_t<Table>, global_table>>(lua_state(), key, lua_gettop(lua_state()));
			lua_pop(lua_state(), p.levels);
			return p;
		}

	public:
		Table tbl;
		key_type key;

		template <typename T>
		table_proxy(Table table, T&& k) : tbl(table), key(eastl::forward<T>(k)) {
		}

		table_proxy(const table_proxy&) = default;
		table_proxy(table_proxy&&) = default;
		table_proxy& operator=(const table_proxy& right) {
			return set(right);
		}
		table_proxy& operator=(table_proxy&& right) {
			return set(eastl::move(right));
		}

		template <typename T>
		table_proxy& set(T&& item) & {
			tuple_set(eastl::make_index_sequence<eastl::tuple_size_v<meta::unqualified_t<key_type>>>(), eastl::forward<T>(item));
			return *this;
		}

		template <typename T>
		table_proxy&& set(T&& item) && {
			eastl::move(*this).tuple_set(eastl::make_index_sequence<eastl::tuple_size_v<meta::unqualified_t<key_type>>>(), eastl::forward<T>(item));
			return eastl::move(*this);
		}

		template <typename... Args>
		table_proxy& set_function(Args&&... args) & {
			tbl.set_function(key, eastl::forward<Args>(args)...);
			return *this;
		}

		template <typename... Args>
		table_proxy&& set_function(Args&&... args) && {
			tbl.set_function(eastl::move(key), eastl::forward<Args>(args)...);
			return eastl::move(*this);
		}

		template <typename T, eastl::enable_if_t<!eastl::is_same_v<meta::unqualified_t<T>, table_proxy>>* = nullptr>
		table_proxy& operator=(T&& other) & {
			using Tu = meta::unwrap_unqualified_t<T>;
			if constexpr (!is_lua_reference_or_proxy_v<Tu> && meta::is_invocable_v<Tu>) {
				return set_function(eastl::forward<T>(other));
			}
			else {
				return set(eastl::forward<T>(other));
			}
		}

		template <typename T, eastl::enable_if_t<!eastl::is_same_v<meta::unqualified_t<T>, table_proxy>>* = nullptr>
		table_proxy&& operator=(T&& other) && {
			using Tu = meta::unwrap_unqualified_t<T>;
			if constexpr (!is_lua_reference_or_proxy_v<Tu> && meta::is_invocable_v<Tu> && !detail::is_msvc_callable_rigged_v<T>) {
				return eastl::move(*this).set_function(eastl::forward<T>(other));
			}
			else {
				return eastl::move(*this).set(eastl::forward<T>(other));
			}
		}

		template <typename T>
		table_proxy& operator=(std::initializer_list<T> other) & {
			return set(eastl::move(other));
		}

		template <typename T>
		table_proxy&& operator=(std::initializer_list<T> other) && {
			return eastl::move(*this).set(eastl::move(other));
		}

		template <typename T>
		bool is() const {
			typedef decltype(get<T>()) U;
			optional<U> option = this->get<optional<U>>();
			return option.has_value();
		}

		template <typename T>
		decltype(auto) get() const& {
			using idx_seq = eastl::make_index_sequence<eastl::tuple_size_v<meta::unqualified_t<key_type>>>;
			return tuple_get<T>(idx_seq());
		}

		template <typename T>
		decltype(auto) get() && {
			using idx_seq = eastl::make_index_sequence<eastl::tuple_size_v<meta::unqualified_t<key_type>>>;
			return eastl::move(*this).template tuple_get<T>(idx_seq());
		}

		template <typename T>
		decltype(auto) get_or(T&& otherwise) const {
			typedef decltype(get<T>()) U;
			optional<U> option = get<optional<U>>();
			if (option) {
				return static_cast<U>(option.value());
			}
			return static_cast<U>(eastl::forward<T>(otherwise));
		}

		template <typename T, typename D>
		decltype(auto) get_or(D&& otherwise) const {
			optional<T> option = get<optional<T>>();
			if (option) {
				return static_cast<T>(option.value());
			}
			return static_cast<T>(eastl::forward<D>(otherwise));
		}


		template <typename T>
		decltype(auto) get_or_create() {
			return get_or_create<T>(new_table());
		}

		template <typename T, typename Otherwise>
		decltype(auto) get_or_create(Otherwise&& other) {
			if (!this->valid()) {
				this->set(eastl::forward<Otherwise>(other));
			}
			return get<T>();
		}

		template <typename K>
		decltype(auto) operator[](K&& k) const& {
			auto keys = meta::tuplefy(key, eastl::forward<K>(k));
			return table_proxy<Table, decltype(keys)>(tbl, eastl::move(keys));
		}

		template <typename K>
		decltype(auto) operator[](K&& k) & {
			auto keys = meta::tuplefy(key, eastl::forward<K>(k));
			return table_proxy<Table, decltype(keys)>(tbl, eastl::move(keys));
		}

		template <typename K>
		decltype(auto) operator[](K&& k) && {
			auto keys = meta::tuplefy(eastl::move(key), eastl::forward<K>(k));
			return table_proxy<Table, decltype(keys)>(tbl, eastl::move(keys));
		}

		template <typename... Ret, typename... Args>
		decltype(auto) call(Args&&... args) {
			lua_State* L = this->lua_state();
			push(L);
			int idx = lua_gettop(L);
			stack_aligned_function func(L, idx);
			return func.call<Ret...>(eastl::forward<Args>(args)...);
		}

		template <typename... Args>
		decltype(auto) operator()(Args&&... args) {
			return call<>(eastl::forward<Args>(args)...);
		}

		bool valid() const {
			auto pp = stack::push_pop(tbl);
			auto p = stack::probe_get_field<eastl::is_same<meta::unqualified_t<Table>, global_table>::value>(lua_state(), key, lua_gettop(lua_state()));
			lua_pop(lua_state(), p.levels);
			return p;
		}

		int push() const noexcept {
			return push(this->lua_state());
		}

		int push(lua_State* L) const noexcept {
			if constexpr (eastl::is_same_v<meta::unqualified_t<Table>, global_table> || is_stack_table_v<meta::unqualified_t<Table>>) {
				auto pp = stack::push_pop<true>(tbl);
				int tableindex = pp.index_of(tbl);
				int top_index = lua_gettop(L);
				stack::get_field<true>(lua_state(), key, tableindex);
				lua_replace(L, top_index + 1);
				lua_settop(L, top_index + 1);
			}
			else {
				auto pp = stack::push_pop<false>(tbl);
				int tableindex = pp.index_of(tbl);
				int aftertableindex = lua_gettop(L);
				stack::get_field<false>(lua_state(), key, tableindex);
				lua_replace(L, tableindex);
				lua_settop(L, aftertableindex + 1);
			}
			return 1;
		}

		type get_type() const {
			type t = type::none;
			auto pp = stack::push_pop(tbl);
			auto p = stack::probe_get_field<eastl::is_same<meta::unqualified_t<Table>, global_table>::value>(lua_state(), key, lua_gettop(lua_state()));
			if (p) {
				t = type_of(lua_state(), -1);
			}
			lua_pop(lua_state(), p.levels);
			return t;
		}

		lua_State* lua_state() const {
			return tbl.lua_state();
		}

		table_proxy& force() {
			if (!this->valid()) {
				this->set(new_table());
			}
			return *this;
		}
	};

	template <typename Table, typename Key, typename T>
	inline bool operator==(T&& left, const table_proxy<Table, Key>& right) {
		using G = decltype(stack::get<T>(nullptr, 0));
		return right.template get<optional<G>>() == left;
	}

	template <typename Table, typename Key, typename T>
	inline bool operator==(const table_proxy<Table, Key>& right, T&& left) {
		using G = decltype(stack::get<T>(nullptr, 0));
		return right.template get<optional<G>>() == left;
	}

	template <typename Table, typename Key, typename T>
	inline bool operator!=(T&& left, const table_proxy<Table, Key>& right) {
		using G = decltype(stack::get<T>(nullptr, 0));
		return right.template get<optional<G>>() != left;
	}

	template <typename Table, typename Key, typename T>
	inline bool operator!=(const table_proxy<Table, Key>& right, T&& left) {
		using G = decltype(stack::get<T>(nullptr, 0));
		return right.template get<optional<G>>() != left;
	}

	template <typename Table, typename Key>
	inline bool operator==(lua_nil_t, const table_proxy<Table, Key>& right) {
		return !right.valid();
	}

	template <typename Table, typename Key>
	inline bool operator==(const table_proxy<Table, Key>& right, lua_nil_t) {
		return !right.valid();
	}

	template <typename Table, typename Key>
	inline bool operator!=(lua_nil_t, const table_proxy<Table, Key>& right) {
		return right.valid();
	}

	template <typename Table, typename Key>
	inline bool operator!=(const table_proxy<Table, Key>& right, lua_nil_t) {
		return right.valid();
	}

	template <bool b>
	template <typename Super>
	basic_reference<b>& basic_reference<b>::operator=(proxy_base<Super>&& r) {
		basic_reference<b> v = r;
		this->operator=(eastl::move(v));
		return *this;
	}

	template <bool b>
	template <typename Super>
	basic_reference<b>& basic_reference<b>::operator=(const proxy_base<Super>& r) {
		basic_reference<b> v = r;
		this->operator=(eastl::move(v));
		return *this;
	}

	namespace stack {
		template <typename Table, typename Key>
		struct unqualified_pusher<table_proxy<Table, Key>> {
			static int push(lua_State* L, const table_proxy<Table, Key>& p) {
				return p.push(L);
			}
		};
	} // namespace stack
} // namespace sol

#endif // SOL_TABLE_PROXY_HPP
