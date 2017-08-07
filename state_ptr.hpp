#ifndef UTILS_STATE_PTR_HPP
#define UTILS_STATE_PTR_HPP

#include <cassert>
#include <type_traits>
#include <memory>

// Users can change the namespace of `state_ptr`.
// The default namespace is `putl` which stands for "Pointer Utils".
#ifndef UTILS_STATE_PTR_HPP_NAMESPACE
#define UTILS_STATE_PTR_HPP_NAMESPACE putl
#endif

namespace UTILS_STATE_PTR_HPP_NAMESPACE {
	namespace detail {
		/// \brief Returns the logarithm to the power of `2` for the given `number`.
		/// 
		/// Note: Returns `0` for the special case when `number` is `0`.
		constexpr auto log2(uint64_t number) -> uint64_t {
			uint64_t acc{0};
			while (number > 1) {
				number /= 2;
				acc += 1;
			}
			return acc;
		}

		/// ===================================================================
		///  Tests for the constexpr log2 implementation.
		/// ===================================================================

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
		):
			m_ptr{reinterpret_cast<uintptr_t>(ptr) >> state_bits},
			m_state{state}
		{
			check_invariant();
		}

		void check_invariant() {
			assert(m_state <= threshold);
		}

		void set_state(uintptr_t new_state) {
			assert(new_state <= threshold);
			m_state = new_state;
			check_invariant();
		}

		auto get_state() -> uintptr_t {
			return m_state;
		}

		auto get_ptr() -> pointer_type {
			uintptr_t c = m_ptr;
			return reinterpret_cast<pointer_type>(c << state_bits);
		}

		auto get_ptr() const -> const pointer_type {
			uintptr_t c = m_ptr;
			return reinterpret_cast<pointer_type>(c << state_bits);
		}

		void operator++() {
			++m_ptr;
		}

		void operator++(int) {
			++m_ptr;
		}

		void operator--() {
			--m_ptr;
		}

		void operator--(int) {
			--m_ptr;
		}

		auto operator*() const -> reference_type {
			return *reinterpret_cast<pointer_type>(get_ptr());
		}

		auto operator->() const -> pointer_type {
			return reinterpret_cast<pointer_type>(get_ptr());
		}

		template<typename X> friend bool operator==(state_ptr<X> const& lhs, state_ptr<X> const& rhs);
		template<typename X> friend bool operator!=(state_ptr<X> const& lhs, state_ptr<X> const& rhs);

		template<typename X> friend bool operator<(state_ptr<X> const& lhs, state_ptr<X> const& rhs);


	private:
		uintptr_t m_ptr   : ptr_bits;
		uintptr_t m_state : state_bits;
	};

	template<typename T>
	auto operator==(state_ptr<T> const& rhs, state_ptr<T> const& lhs) -> bool {
		return lhs.m_ptr == rhs.m_ptr;
	}

	template<typename T>
	auto operator!=(state_ptr<T> const& rhs, state_ptr<T> const& lhs) -> bool {
		return !(lhs == rhs);
	}

	template<typename T>
	auto operator<(state_ptr<T> const& rhs, state_ptr<T> const& lhs) -> bool {
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
