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

	/// \brief A non-owning smart pointer that allows for storing an additional space-optimized
	///        state along the pointer value that is dependend on the wrapped type's alignment.
	/// 
	/// Users can use custom state types (e.g. user-defined enums) as state type.
	/// It is possible to further limit the representable state value, e.g. to adjust the state_ptr
	/// to the state type dimensions. E.g. When there are just two possible states (enum Bit { set=0, unset=1, dontknow=2 };)
	/// and the state_ptr for T allows for 3 state bits the user is still able to limit it to 2 bits.
	/// 
	/// Increasing the limit is not possible and will fail at compile-time!
	/// 
	/// The state_ptr interface is based on the interface that is provided by std::unique_ptr.
	/// 
	/// Note: It is planned to provide an implementation of an owning_state_ptr in the future.
	/// 
	template<typename T,
	         typename S = uintptr_t,
	         size_t req_state_bits = detail::log2(alignof(T))>
	class state_ptr {
	public:
		/// \brief The element type.
		/// 
		/// The element type is the type that this state pointer points to.
		using element_type       = T;

		/// \brief Type representing a pointer to an element type.
		using pointer_type       = T*;

		/// \brief Type representing a pointer to a const element type.
		using const_pointer_type = T const*;

		/// \brief Type representing a reference to the element type.
		using reference_type     = typename std::add_lvalue_reference<T>::type;

		/// \brief The user defined type used to represent the state.
		using state_type         = S;

		/// \brief The type used internally to store the pointer and the state.
		using internal_type      = uintptr_t;

	private:
		/// \brief The maximum number of bits that can be reserved to represent
		///        the state depending on the alignment of the element type.
		constexpr static size_t state_bits_max = detail::log2(alignof(T));

		/// \brief The number of bits reserved for the value of the state.
		constexpr static size_t state_bits     = std::min(req_state_bits, state_bits_max);
		static_assert(req_state_bits <= state_bits_max, "The alignment of T is not sufficient to store the requested amount of state bits.");
		// static_assert(state_bits > 0, "The alignment of T is not sufficient to store an additional state.");

		/// \brief The number of bits reserved for the value of the pointer.
		constexpr static size_t ptr_bits   = 8 * sizeof(pointer_type) - state_bits;

		/// \brief The maximum value that is possible to be stored as state
		///        with the given amount of bits reserved for the value of the state.
		constexpr static internal_type state_max  = (1u << state_bits) - 1u;

		/// \brief The bit-mask to extract the state value out of the shared memory.
		constexpr static internal_type state_mask = state_bits == 0 ? 0 : (~internal_type{0}) >> ptr_bits;

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
		explicit state_ptr(state_ptr const&) = default;
		explicit state_ptr(state_ptr&&) = default;

		state_ptr& operator=(state_ptr const&) noexcept = default;
		state_ptr& operator=(state_ptr&&) noexcept = default;
		// state_ptr& operator=(std::nullptr_t);

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

		template<typename T1, typename S1, size_t ReqStateBits>
		friend bool operator==(state_ptr<T1, S1, ReqStateBits> const&, state_ptr<T1, S1, ReqStateBits> const&) noexcept;
		
		template<typename T1, typename S1, size_t ReqStateBits>
		friend bool operator!=(state_ptr<T1, S1, ReqStateBits> const&, state_ptr<T1, S1, ReqStateBits> const&) noexcept;

		template<typename T1, typename S1, size_t ReqStateBits>
		friend bool operator==(state_ptr<T1, S1, ReqStateBits> const&, std::nullptr_t) noexcept;
		
		template<typename T1, typename S1, size_t ReqStateBits>
		friend bool operator!=(state_ptr<T1, S1, ReqStateBits> const&, std::nullptr_t) noexcept;

		template<typename T1, typename S1, size_t ReqStateBits>
		friend bool operator==(std::nullptr_t, state_ptr<T1, S1, ReqStateBits> const&) noexcept;
		
		template<typename T1, typename S1, size_t ReqStateBits>
		friend bool operator!=(std::nullptr_t, state_ptr<T1, S1, ReqStateBits> const&) noexcept;

		template<typename T1, typename S1, size_t ReqStateBits>
		friend bool operator<(state_ptr<T1, S1, ReqStateBits> const& lhs, state_ptr<T1, S1, ReqStateBits> const& rhs) noexcept;
		
		template<typename T1, typename S1, size_t ReqStateBits>
		friend bool operator<=(state_ptr<T1, S1, ReqStateBits> const& lhs, state_ptr<T1, S1, ReqStateBits> const& rhs) noexcept;
		
		template<typename T1, typename S1, size_t ReqStateBits>
		friend bool operator> (state_ptr<T1, S1, ReqStateBits> const& lhs, state_ptr<T1, S1, ReqStateBits> const& rhs) noexcept;
		
		template<typename T1, typename S1, size_t ReqStateBits>
		friend bool operator<=(state_ptr<T1, S1, ReqStateBits> const& lhs, state_ptr<T1, S1, ReqStateBits> const& rhs) noexcept;


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

	template<typename T, typename S, size_t StateBits>
	constexpr bool state_ptr<T, S, StateBits>::is_valid_state(state_type state) noexcept {
		return static_cast<internal_type>(state) <= state_max;
	}

	template<typename T, typename S, size_t StateBits>
	void state_ptr<T, S, StateBits>::assert_valid_state(state_type state) noexcept {
		return assert(is_valid_state(state) && "state value is out of bounds for this state_ptr");
	}

	template<typename T, typename S, size_t StateBits>
	constexpr state_ptr<T, S, StateBits>::state_ptr(
		std::nullptr_t,
		state_type state
	) noexcept :
		m_ptr_and_state{0}
	{
		assert_valid_state(state);
		set_state(state);
	}

	template<typename T, typename S, size_t StateBits>
	state_ptr<T, S, StateBits>::state_ptr(
		pointer_type ptr,
		state_type   state
	) noexcept :
		m_ptr_and_state{reinterpret_cast<uintptr_t>(ptr)}
	{
		assert_valid_state(state);
		set_state(state);
	}

	template<typename T, typename S, size_t StateBits>
	constexpr auto state_ptr<T, S, StateBits>::get_ptr_bits() const noexcept -> internal_type {
		return m_ptr_and_state & ptr_mask;
	}

	template<typename T, typename S, size_t StateBits>
	constexpr auto state_ptr<T, S, StateBits>::get_state_bits() const noexcept -> internal_type {
		return m_ptr_and_state & state_mask;
	}

	template<typename T, typename S, size_t StateBits>
	void state_ptr<T, S, StateBits>::assert_invariant() const noexcept {
		assert_valid_state(get_state());
	}

	template<typename T, typename S, size_t StateBits>
	void state_ptr<T, S, StateBits>::set_state(state_type new_state) noexcept {
		assert_valid_state(new_state);
		m_ptr_and_state = get_ptr_bits() | static_cast<internal_type>(new_state);
	}

	template<typename T, typename S, size_t StateBits>
	auto state_ptr<T, S, StateBits>::get_state() const noexcept -> state_type {
		return static_cast<state_type>(get_state_bits());
	}

	template<typename T, typename S, size_t StateBits>
	auto state_ptr<T, S, StateBits>::get_ptr() noexcept -> pointer_type {
		return reinterpret_cast<pointer_type>(get_ptr_bits());
	}

	template<typename T, typename S, size_t StateBits>
	auto state_ptr<T, S, StateBits>::get_ptr() const noexcept -> const_pointer_type {
		return reinterpret_cast<pointer_type>(get_ptr_bits());
	}

	template<typename T, typename S, size_t StateBits>
	auto state_ptr<T, S, StateBits>::operator*() const -> reference_type {
		return reinterpret_cast<reference_type>(get_ptr());
	}

	template<typename T, typename S, size_t StateBits>
	auto state_ptr<T, S, StateBits>::operator->() const noexcept -> pointer_type {
		return get_ptr();
	}

	template<typename T, typename S, size_t StateBits>
	state_ptr<T, S, StateBits>::operator bool() const noexcept {
		return get_ptr() != nullptr;
	}

	/// =======================================================================
	///  Implementation of comparison operators.
	/// =======================================================================

	template<typename T, typename S, size_t StateBits>
	auto operator==(state_ptr<T, S, StateBits> const& rhs, state_ptr<T, S, StateBits> const& lhs) noexcept -> bool {
		return lhs.m_ptr_and_state == rhs.m_ptr_and_state;
	}

	template<typename T, typename S, size_t StateBits>
	auto operator!=(state_ptr<T, S, StateBits> const& rhs, state_ptr<T, S, StateBits> const& lhs) noexcept -> bool {
		return !(lhs == rhs);
	}

	template<typename T, typename S, size_t StateBits>
	bool operator==(state_ptr<T, S, StateBits> const& rhs, std::nullptr_t) noexcept {
		return rhs.get_ptr() == nullptr;
	}

	template<typename T, typename S, size_t StateBits>
	bool operator!=(state_ptr<T, S, StateBits> const& rhs, std::nullptr_t) noexcept {
		return rhs.get_ptr() != nullptr;
	}

	template<typename T, typename S, size_t StateBits>
	bool operator==(std::nullptr_t, state_ptr<T, S, StateBits> const& lhs) noexcept {
		return nullptr == lhs;
	}

	template<typename T, typename S, size_t StateBits>
	bool operator!=(std::nullptr_t, state_ptr<T, S, StateBits> const& lhs) noexcept {
		return nullptr == lhs;
	}

	/// =======================================================================
	///  Implementation of relational operators.
	/// =======================================================================

	template<typename T, typename S, size_t StateBits>
	auto operator<(state_ptr<T, S, StateBits> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs.m_ptr < rhs.m_ptr);
	}

	template<typename T, typename S, size_t StateBits>
	auto operator<=(state_ptr<T, S, StateBits> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs > rhs);
	}

	template<typename T, typename S, size_t StateBits>
	auto operator>=(state_ptr<T, S, StateBits> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs < rhs);
	}

	template<typename T, typename S, size_t StateBits>
	auto operator>(state_ptr<T, S, StateBits> const& rhs, state_ptr<T> const& lhs) noexcept -> bool {
		return !(lhs <= rhs);
	}
}

/// ===========================================================================
///  Implementation of default std::hash template specialization.
/// ===========================================================================

namespace std {
	template<typename T, typename S, size_t StateBits>
	struct hash<putl::state_ptr<T, S, StateBits>> {
	public:
		size_t operator()(const putl::state_ptr<T, S> &p) const 
		{
			return std::hash<typename putl::state_ptr<T, S>::internal_type>()(p.m_ptr_and_state);
		}
	};
}

#endif // POINTER_UTILS_STATE_PTR_HPP
