#pragma once

#include <helios/initializer_list.hpp>
#include <helios/utility.hpp>

#include <new>

namespace helios
{
	struct nullopt_t
	{
		inline explicit constexpr nullopt_t(int) {};
	};

	template <typename Type>
	class optional
	{
		template <typename U>
		friend class optional;
	public:
		using value_type = Type;

		constexpr optional() noexcept;
		constexpr optional(nullopt_t) noexcept;
		constexpr optional(const optional& other);
		constexpr optional(optional&& other) noexcept;

		template <typename U>
		optional(const optional<U>& other);

		template <typename U>
		optional(optional<U>&& other);

		template <typename ... Args>
		constexpr explicit optional(in_place_t, Args&& ... args);

		template <typename U, typename ... Args>
		constexpr explicit optional(in_place_t, std::initializer_list<U> ilist, Args&& ... args);

		template <typename U = value_type>
		constexpr optional(U&& value);

		~optional();

		optional& operator=(nullopt_t) noexcept;
		constexpr optional& operator=(const optional& other);
		constexpr optional& operator=(optional&&) noexcept;

		template <typename U = Type>
		optional& operator=(U&& value);

		template <typename U>
		optional& operator=(const optional<U>& other);

		template <typename U>
		optional& operator=(optional<U>&& other);

		constexpr const Type* operator->() const;
		constexpr Type* operator->();
		constexpr const Type& operator*() const&;
		constexpr Type& operator*()&;
		constexpr const Type&& operator*() const&&;
		constexpr Type&& operator*()&&;

		constexpr explicit operator bool() const noexcept;
		[[nodiscard]] constexpr bool has_value() const noexcept;

		constexpr Type& value()&;
		constexpr const Type& value() const&;

		constexpr Type&& value()&&;
		constexpr const Type&& value() const&&;

		template< typename U>
		constexpr Type value_or(U&& default_value) const&;

		template< typename U>
		constexpr Type value_or(U&& default_value)&&;

		void swap(optional& other) noexcept;
		void reset() noexcept;

		template< class... Args >
		Type& emplace(Args&&... args);

		template< class U, class... Args >
		Type& emplace(std::initializer_list<U> ilist, Args&&... args);
	private:
		char _value[sizeof(Type)];
		bool _present = false;

		void _release();
	};

	template <typename Type>
	inline constexpr optional<Type>::optional() noexcept
	{
		for (size_t i = 0; i < sizeof(Type); i++)
		{
			_value[i] = 0;
		}
	}

	template <typename Type>
	inline constexpr optional<Type>::optional(nullopt_t) noexcept
	{
		for (size_t i = 0; i < sizeof(Type); i++)
		{
			_value[i] = 0;
		}
	}

	template <typename Type>
	inline constexpr optional<Type>::optional(const optional& other)
	{
		if (other)
		{
			_present = true;
			::new (reinterpret_cast<void*>(_value)) Type(other.value());
		}
	}

	template <typename Type>
	inline constexpr optional<Type>::optional(optional&& other) noexcept
	{
		if (other)
		{
			_present = true;
			::new (reinterpret_cast<void*>(_value)) Type(helios::move(other.value()));
		}
	}

	template <typename Type>
	template <typename U>
	inline optional<Type>::optional(const optional<U>& other)
	{
		if (other)
		{
			_present = true;
			::new (reinterpret_cast<void*>(_value)) Type(other.value());
		}
	}

	template <typename Type>
	template <typename U>
	inline optional<Type>::optional(optional<U>&& other)
	{
		if (other)
		{
			_present = true;
			::new (reinterpret_cast<void*>(_value)) Type(helios::move(other.value()));
		}
	}

	template <typename Type>
	template <typename ... Args>
	inline constexpr optional<Type>::optional(in_place_t, Args&&... args)
	{
		_present = true;
		::new (reinterpret_cast<void*>(_value)) Type(forward<Args>(args)...);
	}

	template <typename Type>
	template <typename U, typename ... Args>
	inline constexpr optional<Type>::optional(in_place_t, std::initializer_list<U> ilist, Args&&... args)
	{
		_present = true;
		::new (reinterpret_cast<void*>(_value)) Type(ilist, forward<Args>(args)...);
	}

	template <typename Type>
	template <typename U>
	inline constexpr optional<Type>::optional(U&& value)
	{
		_present = true;
		::new (reinterpret_cast<void*>(_value)) Type(helios::move(value));
	}

	template <typename Type>
	inline optional<Type>::~optional()
	{
		_release();
	}

	template <typename Type>
	inline optional<Type>& optional<Type>::operator=(nullopt_t) noexcept
	{
		_release();
		return *this;
	}

	template <typename Type>
	inline constexpr optional<Type>& optional<Type>::operator=(const optional& other)
	{
		_release();

		if (other)
		{
			::new (reinterpret_cast<void*>(_value)) Type(other.value());
			_present = true;
		}

		return *this;
	}

	template <typename Type>
	inline constexpr optional<Type>& optional<Type>::operator=(optional&& other) noexcept
	{
		_release();

		if (other)
		{
			::new (reinterpret_cast<void*>(_value)) Type(helios::move(other.value()));
			other._present = false;
			_present = true;
		}

		return *this;
	}

	template <typename Type>
	template <typename U>
	inline optional<Type>& optional<Type>::operator=(U&& value)
	{
		_release();

		::new (reinterpret_cast<void*>(_value)) Type(helios::move(value));
		_present = true;

		return *this;
	}

	template <typename Type>
	template <typename U>
	inline optional<Type>& optional<Type>::operator=(const optional<U>& other)
	{
		_release();

		if (other)
		{
			::new (reinterpret_cast<void*>(_value)) Type(other.value());
			_present = true;
		}

		return *this;
	}

	template <typename Type>
	template <typename U>
	inline optional<Type>& optional<Type>::operator=(optional<U>&& other)
	{
		_release();

		if (other)
		{
			::new (reinterpret_cast<void*>(_value)) Type(helios::move(other.value()));
			_present = true;
		}

		return *this;
	}

	template <typename Type>
	inline constexpr const Type* optional<Type>::operator->() const
	{
		return reinterpret_cast<const Type*>(_value);
	}

	template <typename Type>
	inline constexpr Type* optional<Type>::operator->()
	{
		return reinterpret_cast<Type*>(_value);
	}

	template <typename Type>
	inline constexpr const Type& optional<Type>::operator*() const&
	{
		return *reinterpret_cast<const Type*>(_value);
	}

	template <typename Type>
	inline constexpr Type& optional<Type>::operator*()&
	{
		return *reinterpret_cast<Type*>(_value);
	}

	template <typename Type>
	inline constexpr const Type&& optional<Type>::operator*() const&&
	{
		return helios::move(*reinterpret_cast<const Type*>(_value));
	}

	template <typename Type>
	inline constexpr Type&& optional<Type>::operator*()&&
	{
		return helios::move(*reinterpret_cast<Type*>(_value));
	}

	template <typename Type>
	inline constexpr optional<Type>::operator bool() const noexcept
	{
		return has_value();
	}

	template <typename Type>
	inline constexpr bool optional<Type>::has_value() const noexcept
	{
		return _present;
	}

	template <typename Type>
	inline constexpr Type& optional<Type>::value()&
	{
		return *reinterpret_cast<Type*>(_value);
	}

	template <typename Type>
	inline constexpr const Type& optional<Type>::value() const&
	{
		return *reinterpret_cast<const Type*>(_value);
	}

	template <typename Type>
	inline constexpr Type&& optional<Type>::value()&&
	{
		return helios::move(*reinterpret_cast<Type*>(_value));
	}

	template <typename Type>
	inline constexpr const Type&& optional<Type>::value() const&&
	{
		return helios::move(*reinterpret_cast<const Type*>(_value));
	}

	template <typename Type>
	template <typename U>
	inline constexpr Type optional<Type>::value_or(U&& default_value) const&
	{
		if (_present)
		{
			return *reinterpret_cast<Type*>(_value);
		}
		return helios::move(default_value);
	}

	template <typename Type>
	template <typename U>
	inline constexpr Type optional<Type>::value_or(U&& default_value)&&
	{
		if (_present)
		{
			return helios::move(*reinterpret_cast<Type*>(_value));
		}
		return helios::move(default_value);
	}


	template <typename Type>
	inline void optional<Type>::swap(optional& other) noexcept
	{
		auto present = _present;
		_present = other._present;
		other._present = present;

		for (size_t i = 0; i < sizeof(Type); i++)
		{
			char byte = _value[i];
			_value[i] = other._value[i];
			other._value[i] = byte;
		}
	}

	template <typename Type>
	inline void optional<Type>::reset() noexcept
	{
		_release();
	}

	template <typename Type>
	template <class ... Args>
	inline Type& optional<Type>::emplace(Args&&... args)
	{
		_release();
		Type* value = ::new (reinterpret_cast<void*>(_value)) Type(forward<Args>(args)...);
		_present = true;
		return *value;
	}

	template <typename Type>
	template <class U, class ... Args>
	inline Type& optional<Type>::emplace(std::initializer_list<U> ilist, Args&&... args)
	{
		_release();
		Type* value = ::new (reinterpret_cast<void*>(_value)) Type(ilist, forward<Args>(args)...);
		_present = true;
		return *value;
	}

	template <typename Type>
	inline void optional<Type>::_release()
	{
		if (_present)
		{
			reinterpret_cast<Type*>(_value)->~Type();
			_present = false;
		}
	}

	template<typename T, typename U>
	inline constexpr bool operator==(const optional<T>& lhs, const optional<U>& rhs)
	{
		if (bool(lhs) != bool(rhs))
		{
			return false;
		}
		if (bool(lhs) == false && bool(rhs) == false)
		{
			return true;
		}
		return *lhs == *rhs;
	}

	template<typename T, typename U>
	inline constexpr bool operator!=(const optional<T>& lhs, const optional<U>& rhs)
	{
		if (bool(lhs) != bool(rhs))
		{
			return true;
		}
		if (bool(lhs) == false && bool(rhs) == false)
		{
			return false;
		}
		return *lhs != *rhs;
	}

	template<typename T, typename U>
	inline constexpr bool operator<(const optional<T>& lhs, const optional<U>& rhs)
	{
		if (bool(rhs) == false)
		{
			return false;
		}
		if (bool(lhs) == false)
		{
			return true;
		}
		return *lhs < *rhs;
	}

	template<typename T, typename U>
	inline constexpr bool operator<=(const optional<T>& lhs, const optional<U>& rhs)
	{
		if (bool(lhs) == false)
		{
			return true;
		}
		if (bool(rhs) == false)
		{
			return false;
		}
		return *lhs <= *rhs;
	}

	template<typename T, typename U>
	inline constexpr bool operator>(const optional<T>& lhs, const optional<U>& rhs)
	{
		if (bool(lhs) == false)
		{
			return false;
		}
		if (bool(rhs) == false)
		{
			return true;
		}
		return *lhs > * rhs;
	}

	template<typename T, typename U>
	inline constexpr bool operator>=(const optional<T>& lhs, const optional<U>& rhs)
	{
		if (bool(rhs) == false)
		{
			return true;
		}
		if (bool(lhs) == false)
		{
			return false;
		}
		return *lhs >= *rhs;
	}

	template<typename T, typename U>
	inline constexpr bool operator==(const optional<T>& opt, const U& value)
	{
		return bool(opt) ? *opt == value : false;
	}

	template<typename T, typename U>
	inline constexpr bool operator==(const T& value, const optional<U>& opt)
	{
		return bool(opt) ? value == *opt : false;
	}

	template<typename T, typename U>
	inline constexpr bool operator!=(const optional<T>& opt, const U& value)
	{
		return bool(opt) ? *opt != value : true;
	}

	template<typename T, typename U>
	inline constexpr bool operator!=(const T& value, const optional<U>& opt)
	{
		return bool(opt) ? value != *opt : true;
	}

	template<typename T, typename U>
	inline constexpr bool operator<(const optional<T>& opt, const U& value)
	{
		return bool(opt) ? *opt < value : true;
	}

	template<typename T, typename U>
	inline constexpr bool operator<(const T& value, const optional<U>& opt)
	{
		return bool(opt) ? value < *opt : false;
	}

	template<typename T, typename U>
	inline constexpr bool operator<=(const optional<T>& opt, const U& value)
	{
		return bool(opt) ? *opt <= value : true;
	}

	template<typename T, typename U>
	inline constexpr bool operator<=(const T& value, const optional<U>& opt)
	{
		return bool(opt) ? value <= *opt : false;
	}

	template<typename T, typename U>
	inline constexpr bool operator>(const optional<T>& opt, const U& value)
	{
		return bool(opt) ? *opt > value : false;
	}

	template<typename T, typename U>
	inline constexpr bool operator>(const T& value, const optional<U>& opt)
	{
		return bool(opt) ? value > * opt : true;
	}

	template<typename T, typename U>
	inline constexpr bool operator>=(const optional<T>& opt, const U& value)
	{
		return bool(opt) ? *opt >= value : false;
	}

	template<typename T, typename U>
	inline constexpr bool operator>=(const T& value, const optional<U>& opt)
	{
		return bool(opt) ? value >= *opt : true;
	}

	template<typename T, typename ... Args>
	inline constexpr optional<T> make_optional(Args&&... args)
	{
		return optional<T>(forward<Args>(args)...);
	}

	template<typename T, typename U, typename ... Args>
	inline constexpr optional<T> make_optional(std::initializer_list<U> il, Args&&... args)
	{
		return optional<T>(il, forward<Args>(args)...);
	}
}