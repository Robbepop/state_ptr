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
	}

	template<typename T, class Deleter = std::default_delete<T>>
	class state_ptr {
	public:
		using element_type   = T;
		using pointer_type   = T*;
		using reference_type = typename std::add_lvalue_reference<T>::type;
		using state_type     = uintptr_t;

		static_assert(sizeof(pointer_type) == sizeof(uintptr_t), "");

		constexpr static size_t state_bits = detail::log2(alignof(T));
		constexpr static size_t ptr_bits   = 8 * sizeof(pointer_type) - state_bits;
		constexpr static size_t threshold  = (1 << state_bits) - 1;

		state_ptr(
			pointer_type ptr,
			state_type   state
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

		auto operator*() const -> reference_type {
			return *reinterpret_cast<pointer_type>(get_ptr());
		}

		auto operator->() const noexcept -> pointer_type {
			return reinterpret_cast<pointer_type>(get_ptr());
		}

		explicit operator bool() const noexcept {
			return get_ptr() != nullptr;
		}

		template<typename X> friend bool operator==(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;
		template<typename X> friend bool operator!=(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;

		template<typename X> friend bool operator<(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;
		template<typename X> friend bool operator<=(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;
		template<typename X> friend bool operator> (state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;
		template<typename X> friend bool operator<=(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;

	private:
		uintptr_t m_ptr   : ptr_bits;
		uintptr_t m_state : state_bits;
	};


	/// =======================================================================
	///  Implementation of comparison operators.
	/// =======================================================================

	template<typename T>
	auto operator==(state_ptr<T> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return lhs.m_ptr == rhs.m_ptr;
	}

	template<typename T>
	auto operator!=(state_ptr<T> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs == rhs);
	}

	/// =======================================================================
	///  Implementation of relational operators.
	/// =======================================================================

	template<typename T>
	auto operator<(state_ptr<T> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs.m_ptr < rhs.m_ptr);
	}

	template<typename T>
	auto operator<=(state_ptr<T> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs > rhs);
	}

	template<typename T>
	auto operator>=(state_ptr<T> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs < rhs);
	}

	template<typename T>
	auto operator>(state_ptr<T> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs <= rhs);
	}
}

/// ===========================================================================
///  Implementation of default std::hash template specialization.
/// ===========================================================================

namespace std {
	template<typename T>
	struct hash<putl::state_ptr<T>> {
	public:
	    size_t operator()(const putl::state_ptr<T> &p) const 
	    {
	    	return std::hash<uintptr_t>()(p.m_ptr); // TODO
	    }
	};
}

#endif // UTILS_STATE_PTR_HPP
