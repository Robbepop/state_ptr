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
		template<typename T>
		constexpr auto log2(T number) -> T {
			T acc{0};
			while (number > 1) {
				number /= 2;
				acc += 1;
			}
			return acc;
		}
	}

	template<typename T, typename S = uintptr_t>
	class state_ptr {
	public:
		using element_type       = T;
		using pointer_type       = T*;
		using const_pointer_type = T const*;
		using reference_type     = typename std::add_lvalue_reference<T>::type;
		using state_type         = S;
		using internal_type      = uintptr_t;

	private:
		/// \brief The number of bits reserved for the value of the state.
		constexpr static size_t state_bits = detail::log2(alignof(T*));

		/// \brief The number of bits reserved for the value of the pointer.
		constexpr static size_t ptr_bits   = 8 * sizeof(pointer_type) - state_bits;

		/// \brief The maximum value that is possible to be stored as state
		///        with the given amount of bits reserved for the value of the state.
		constexpr static internal_type state_max  = (1u << state_bits) - 1u;

		/// \brief The bit-mask to extract the state value out of the shared memory.
		constexpr static internal_type state_mask = (~internal_type{0}) >> ptr_bits;

		/// \brief The bit-mask to extract the pointer value out of the shared memory.
		constexpr static internal_type ptr_mask   = ~state_mask;

	public:
		/// \brief Creates a state_ptr instance initialized by a null-pointer and a given state.
		/// 
		/// Panics if the given state is out of bounds of valid state.
		constexpr state_ptr(std::nullptr_t, state_type) noexcept;

		/// \brief Creates a state_ptr instance pointing to the given pointee and
		///        initialized with the given state.
		/// 
		/// Panics if the given state is out of bounds of valid state.
		state_ptr(pointer_type ptr, state_type) noexcept;

		/// \brief Copies the given state_ptr.
		explicit state_ptr(state_ptr const& p) = default;

		/// \brief Sets the state value of this state_ptr to the given value.
		/// 
		/// Note: This panicks if assertions are enabled whenever the given state
		///       is out of bounds of the valid state space.
		///       When assertions are disabled this has undefined behaviour in the 
		///       error case.
		void set_state(state_type new_state) noexcept;

		/// \brief Returns the current state of this state_ptr.
		auto get_state() const noexcept -> state_type;

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

		template<typename T1, typename S1> friend bool operator==(state_ptr<T1, S1> const&, state_ptr<T1, S1> const&) noexcept;
		template<typename T1, typename S1> friend bool operator!=(state_ptr<T1, S1> const&, state_ptr<T1, S1> const&) noexcept;

		template<typename T1, typename S1> friend bool operator==(state_ptr<T1, S1> const&, nullptr_t) noexcept;
		template<typename T1, typename S1> friend bool operator!=(state_ptr<T1, S1> const&, nullptr_t) noexcept;

		template<typename T1, typename S1> friend bool operator==(nullptr_t, state_ptr<T1, S1> const&) noexcept;
		template<typename T1, typename S1> friend bool operator!=(nullptr_t, state_ptr<T1, S1> const&) noexcept;

		template<typename T1, typename S1> friend bool operator<(state_ptr<T1, S1> const& lhs, state_ptr<T1, S1> const& rhs) noexcept;
		template<typename T1, typename S1> friend bool operator<=(state_ptr<T1, S1> const& lhs, state_ptr<T1, S1> const& rhs) noexcept;
		template<typename T1, typename S1> friend bool operator> (state_ptr<T1, S1> const& lhs, state_ptr<T1, S1> const& rhs) noexcept;
		template<typename T1, typename S1> friend bool operator<=(state_ptr<T1, S1> const& lhs, state_ptr<T1, S1> const& rhs) noexcept;

	private:
		/// \brief Returns the bits representing the pointer value as internal type.
		constexpr auto get_ptr_bits() const noexcept -> internal_type;

		/// \brief Returns the bits representing the state value as internal type.
		constexpr auto get_state_bits() const noexcept -> internal_type;

		/// \brief Returns `true` if the given state is within valid bounds for the state value.
		///        This is associated to the reserved bits for the state value.
		constexpr static bool is_valid_state(state_type) noexcept;

		/// \brief Asserts that the given state is within bounds for the state value.
		static void assert_valid_state(state_type) noexcept;

		/// \brief Asserts that the state of this state_ptr is within bounds.
		void assert_invariant() const noexcept;

	private:
		internal_type m_ptr_and_state;
	};

	/// =======================================================================
	///  Implementation of constructors and member functions.
	/// =======================================================================

	template<typename T, typename S>
	constexpr bool state_ptr<T, S>::is_valid_state(state_type state) noexcept {
		return static_cast<internal_type>(state) <= state_max;
	}

	template<typename T, typename S>
	void state_ptr<T, S>::assert_valid_state(state_type state) noexcept {
		return assert(is_valid_state(state));
	}

	template<typename T, typename S>
	constexpr state_ptr<T, S>::state_ptr(
		std::nullptr_t,
		state_type state
	) noexcept :
		m_ptr_and_state{0}
	{
		assert_valid_state(state);
		set_state(state);
	}

	template<typename T, typename S>
	state_ptr<T, S>::state_ptr(
		pointer_type ptr,
		state_type   state
	) noexcept :
		m_ptr_and_state{reinterpret_cast<uintptr_t>(ptr)}
	{
		assert_valid_state(state);
		set_state(state);
	}

	template<typename T, typename S>
	constexpr auto state_ptr<T, S>::get_ptr_bits() const noexcept -> internal_type {
		return m_ptr_and_state & ptr_mask;
	}

	template<typename T, typename S>
	constexpr auto state_ptr<T, S>::get_state_bits() const noexcept -> internal_type {
		return m_ptr_and_state & state_mask;
	}

	template<typename T, typename S>
	void state_ptr<T, S>::assert_invariant() const noexcept {
		assert_valid_state(get_state());
	}

	template<typename T, typename S>
	void state_ptr<T, S>::set_state(state_type new_state) noexcept {
		assert_valid_state(new_state);
		m_ptr_and_state = get_ptr_bits() | new_state;
		assert_invariant(); // TODO: is already covered by first assertion. Could be removed.
	}

	template<typename T, typename S>
	auto state_ptr<T, S>::get_state() const noexcept -> state_type {
		return state_type{get_state_bits()};
	}

	template<typename T, typename S>
	auto state_ptr<T, S>::get_ptr() noexcept -> pointer_type {
		return reinterpret_cast<pointer_type>(get_ptr_bits());
	}

	template<typename T, typename S>
	auto state_ptr<T, S>::get_ptr() const noexcept -> const_pointer_type {
		return reinterpret_cast<pointer_type>(get_ptr_bits());
	}

	template<typename T, typename S>
	auto state_ptr<T, S>::operator*() const -> reference_type {
		return reinterpret_cast<reference_type>(get_ptr());
	}

	template<typename T, typename S>
	auto state_ptr<T, S>::operator->() const noexcept -> pointer_type {
		return get_ptr();
	}

	template<typename T, typename S>
	state_ptr<T, S>::operator bool() const noexcept {
		return get_ptr() != nullptr;
	}

	/// =======================================================================
	///  Implementation of comparison operators.
	/// =======================================================================

	template<typename T, typename S>
	auto operator==(state_ptr<T, S> const& rhs, state_ptr<T, S> const& lhs) noexcept -> bool {
		return lhs.m_ptr_and_state == rhs.m_ptr_and_state;
	}

	template<typename T, typename S>
	auto operator!=(state_ptr<T, S> const& rhs, state_ptr<T, S> const& lhs) noexcept -> bool {
		return !(lhs == rhs);
	}

	template<typename T, typename S>
	bool operator==(state_ptr<T, S> const& rhs, nullptr_t) noexcept {
		return rhs.get_ptr() == nullptr;
	}

	template<typename T, typename S>
	bool operator!=(state_ptr<T, S> const& rhs, nullptr_t) noexcept {
		return rhs.get_ptr() != nullptr;
	}

	template<typename T, typename S>
	bool operator==(nullptr_t, state_ptr<T, S> const& lhs) noexcept {
		return nullptr == lhs;
	}

	template<typename T, typename S>
	bool operator!=(nullptr_t, state_ptr<T, S> const& lhs) noexcept {
		return nullptr == lhs;
	}

	/// =======================================================================
	///  Implementation of relational operators.
	/// =======================================================================

	template<typename T, typename S>
	auto operator<(state_ptr<T, S> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs.m_ptr < rhs.m_ptr);
	}

	template<typename T, typename S>
	auto operator<=(state_ptr<T, S> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs > rhs);
	}

	template<typename T, typename S>
	auto operator>=(state_ptr<T, S> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs < rhs);
	}

	template<typename T, typename S>
	auto operator>(state_ptr<T, S> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs <= rhs);
	}
}

/// ===========================================================================
///  Implementation of default std::hash template specialization.
/// ===========================================================================

namespace std {
	template<typename T, typename S>
	struct hash<putl::state_ptr<T, S>> {
	public:
		size_t operator()(const putl::state_ptr<T, S> &p) const 
		{
			return std::hash<typename putl::state_ptr<T, S>::internal_type>()(p.m_ptr_and_state);
		}
	};
}

#endif // POINTER_UTILS_STATE_PTR_HPP
