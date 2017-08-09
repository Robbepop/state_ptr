#ifndef POINTER_UTILS_STATE_PTR_HPP
#define POINTER_UTILS_STATE_PTR_HPP

#include <cassert>
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

	template<typename T>
	class state_ptr {
	public:
		using element_type       = T;
		using pointer_type       = T*;
		using const_pointer_type = T const*;
		using reference_type     = typename std::add_lvalue_reference<T>::type;
		using state_type         = uintptr_t;

	private:
		/// \brief The number of bits reserved for the value of the state.
		constexpr static size_t state_bits = detail::log2(alignof(T));

		/// \brief The number of bits reserved for the value of the pointer.
		constexpr static size_t ptr_bits   = 8 * sizeof(pointer_type) - state_bits;

		/// \brief The maximum value that is possible to be stored as state
		///        with the given amount of bits reserved for the value of the state.
		constexpr static size_t threshold  = (1u << state_bits) - 1u;

		/// \brief The bit-mask to extract the state value out of the shared memory.
		constexpr static uintptr_t state_mask = (~0) >> ptr_bits;

		/// \brief The bit-mask to extract the pointer value out of the shared memory.
		constexpr static uintptr_t ptr_mask   = ~state_mask;

	public:
		/// \brief Creates a state_ptr instance initialized by a null-pointer and a given state.
		constexpr state_ptr(std::nullptr_t, state_type) noexcept;

		/// \brief Creates a state_ptr instance pointing to the given pointee and
		///        initialized with the given state.
		state_ptr(pointer_type ptr, state_type) noexcept;

		/// \brief Copies the given state_ptr.
		explicit state_ptr(state_ptr const& p) = default;

		/// \brief Sets the state value of this state_ptr to the given value.
		/// 
		/// Note: This panicks if assertions are enabled whenever the given state
		///       is out of bounds of the valid state space.
		///       When assertions are disabled this has undefined behaviour in the 
		///       error case.
		void set_state(uintptr_t new_state) noexcept;

		/// \brief Returns the current state of this state_ptr.
		auto get_state() const noexcept -> uintptr_t;

		/// \brief Returns the wrapped pointer of this state_ptr.
		auto get_ptr() noexcept -> pointer_type;

		/// \brief Returns the wrapped pointer of this state_ptr.
		auto get_ptr() const noexcept -> const_pointer_type;

		/// \brief Forwards to the wrapped pointer as reference.
		auto operator*() const -> reference_type;

		/// \brief Forwards to the wrapped pointer.
		auto operator->() const noexcept -> pointer_type;

		/// \brief Returns false if this state_ptr wraps nullptr, and returns true otherwise.
		explicit operator bool() const noexcept;

		template<typename X> friend bool operator==(state_ptr<X> const&, state_ptr<X> const&) noexcept;
		template<typename X> friend bool operator!=(state_ptr<X> const&, state_ptr<X> const&) noexcept;

		template<typename X> friend bool operator==(state_ptr<X> const&, nullptr_t) noexcept;
		template<typename X> friend bool operator!=(state_ptr<X> const&, nullptr_t) noexcept;

		template<typename X> friend bool operator==(nullptr_t, state_ptr<X> const&) noexcept;
		template<typename X> friend bool operator!=(nullptr_t, state_ptr<X> const&) noexcept;

		template<typename X> friend bool operator<(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;
		template<typename X> friend bool operator<=(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;
		template<typename X> friend bool operator> (state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;
		template<typename X> friend bool operator<=(state_ptr<X> const& lhs, state_ptr<X> const& rhs) noexcept;

	private:
		/// \brief Returns `true` if the given state is within valid bounds for the state.
		///        This is associated to the reserved bits for the state value.
		constexpr static bool is_valid_state(state_type) noexcept;

		/// \brief Asserts that the state of this state_ptr is within bounds.
		void assert_invariant() const noexcept;

	private:
		uintptr_t m_ptr   : ptr_bits;
		uintptr_t m_state : state_bits;
	};

	/// =======================================================================
	///  Implementation of constructors and member functions.
	/// =======================================================================

	template<typename T>
	constexpr bool state_ptr<T>::is_valid_state(state_type state) noexcept {
		return state <= threshold;
	}

	template<typename T>
	constexpr state_ptr<T>::state_ptr(
		std::nullptr_t,
		state_type state
	) noexcept :
		m_ptr{0u},
		m_state{state}
	{
		assert_invariant();
	}

	template<typename T>
	state_ptr<T>::state_ptr(
		pointer_type ptr,
		state_type   state
	) noexcept :
		m_ptr{reinterpret_cast<uintptr_t>(ptr) >> state_bits},
		m_state{state}
	{
		assert_invariant();
	}

	template<typename T>
	void state_ptr<T>::assert_invariant() const noexcept {
		assert(is_valid_state(m_state));
	}

	template<typename T>
	void state_ptr<T>::set_state(uintptr_t new_state) noexcept {
		assert(is_valid_state(new_state));
		m_state = new_state;
		assert_invariant();
	}

	template<typename T>
	auto state_ptr<T>::get_state() const noexcept -> uintptr_t {
		return m_state;
	}

	template<typename T>
	auto state_ptr<T>::get_ptr() noexcept -> pointer_type {
		uintptr_t c = m_ptr;
		return reinterpret_cast<pointer_type>(c << state_bits);
	}

	template<typename T>
	auto state_ptr<T>::get_ptr() const noexcept -> const_pointer_type {
		uintptr_t c = m_ptr;
		return reinterpret_cast<pointer_type>(c << state_bits);
	}

	template<typename T>
	auto state_ptr<T>::operator*() const -> reference_type {
		return *reinterpret_cast<pointer_type>(get_ptr());
	}

	template<typename T>
	auto state_ptr<T>::operator->() const noexcept -> pointer_type {
		return reinterpret_cast<pointer_type>(get_ptr());
	}

	template<typename T>
	state_ptr<T>::operator bool() const noexcept {
		return get_ptr() != nullptr;
	}

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


	template<typename T>
	bool operator==(state_ptr<T> const& rhs, nullptr_t) noexcept {
		return rhs.get_ptr() == nullptr;
	}

	template<typename T>
	bool operator!=(state_ptr<T> const& rhs, nullptr_t) noexcept {
		return rhs.get_ptr() != nullptr;
	}

	template<typename T>
	bool operator==(nullptr_t, state_ptr<T> const& lhs) noexcept {
		return nullptr == lhs;
	}

	template<typename T>
	bool operator!=(nullptr_t, state_ptr<T> const& lhs) noexcept {
		return nullptr == lhs;
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

#endif // POINTER_UTILS_STATE_PTR_HPP
