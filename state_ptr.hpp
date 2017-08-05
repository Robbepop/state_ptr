#ifndef UTILS_STATE_PTR_HPP
#define UTILS_STATE_PTR_HPP

#include <cassert>
#include <type_traits>
#include <memory>

/// putl := Pointer Utils
namespace putl {
	namespace detail {
		constexpr auto log2_(uint64_t number, uint64_t acc) -> uint64_t {
			return number <= 1 ? acc : log2_(number / 2, acc + 1);
		}

		constexpr auto log2(uint64_t number) -> uint64_t {
			return log2_(number, 1u) - 1;
		}

		static_assert(log2(70) == 6, "");
		static_assert(log2(64) == 6, "");
		static_assert(log2(35) == 5, "");
		static_assert(log2(32) == 5, "");
		static_assert(log2(17) == 4, "");
		static_assert(log2(10) == 3, "");
		static_assert(log2( 4) == 2, "");
		static_assert(log2( 2) == 1, "");
		static_assert(log2( 1) == 0, "");
		static_assert(log2( 0) == 0, ""); // edge case
	}

	template<typename T, class Deleter = std::default_delete<T>>
	class state_ptr {
	public:
		using element_type   = T;
		using pointer_type   = T*;
		using reference_type = typename std::add_lvalue_reference<T>::type;
		using state_type     = uintptr_t;

		static_assert(sizeof(pointer_type) == sizeof(uintptr_t), "");

		constexpr static size_t state_bits = detail::log2(sizeof(T));
		constexpr static size_t ptr_bits   = 8 * sizeof(pointer_type) - state_bits;
		constexpr static size_t threshold  = (1 << state_bits) - 1;

		state_ptr(
			pointer_type ptr,
			state_type   state = 0u
		) noexcept :
			m_ptr{reinterpret_cast<uintptr_t>(ptr) >> state_bits},
			m_state{state}
		{
			check_invariant();
		}

		void check_invariant() const noexcept {
			assert(m_state <= threshold);
		}

		void set_state(uintptr_t new_state) noexcept {
			assert(new_state <= threshold);
			m_state = new_state;
			check_invariant();
		}

		auto get_state() const noexcept -> uintptr_t {
			return m_state;
		}

		auto get_ptr() noexcept -> pointer_type {
			uintptr_t c = m_ptr;
			return reinterpret_cast<pointer_type>(c << state_bits);
		}

		auto get_ptr() const noexcept -> const pointer_type {
			uintptr_t c = m_ptr;
			return reinterpret_cast<pointer_type>(c << state_bits);
		}

		void operator++() noexcept {
			++m_ptr;
		}

		void operator++(int) noexcept {
			++m_ptr;
		}

		void operator--() noexcept {
			--m_ptr;
		}

		void operator--(int) noexcept {
			--m_ptr;
		}

		auto operator*() const noexcept -> reference_type {
			return *reinterpret_cast<pointer_type>(get_ptr());
		}

		auto operator->() const noexcept -> pointer_type {
			return reinterpret_cast<pointer_type>(get_ptr());
		}

		template<typename X> friend bool operator==(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;
		template<typename X> friend bool operator!=(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;

		template<typename X> friend bool operator<(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;


	private:
		uintptr_t m_ptr   : ptr_bits;
		uintptr_t m_state : state_bits;
	};

	template<typename T>
	auto operator==(state_ptr<T> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return lhs.m_ptr == rhs.m_ptr;
	}

	template<typename T>
	auto operator!=(state_ptr<T> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs == rhs);
	}

	template<typename T>
	auto operator<(state_ptr<T> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs.m_ptr < rhs.m_ptr);
	}

	static_assert(sizeof(state_ptr<uint32_t>) == sizeof(uint32_t*), "");
	static_assert(state_ptr<uint32_t>::state_bits == 2, "");
	static_assert(state_ptr<uint32_t>::ptr_bits == 8 * sizeof(uint32_t*) - state_ptr<uint32_t>::state_bits, "");

	static_assert(sizeof(state_ptr<uint64_t>) == sizeof(uint64_t*), "");
	static_assert(state_ptr<uint64_t>::state_bits == 3, "");
	static_assert(state_ptr<uint64_t>::ptr_bits == 8 * sizeof(uint64_t*) - state_ptr<uint64_t>::state_bits, "");
}

#endif // UTILS_STATE_PTR_HPP
