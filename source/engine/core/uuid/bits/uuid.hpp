#ifndef UUIDS_UUID_HPP
#define UUIDS_UUID_HPP

#include <algorithm>
#include <iomanip>
#include <iosfwd>

#include <cctype>
#include <cstring>
#include <string>

#include <cstdint>
#include <iterator>

namespace uuids
{
namespace detail
{
template <typename T>
struct has_overloaded_addressof
{
	template <typename U>
	static constexpr bool has_overload(...)
	{
		return false;
	}

	template <typename U, ::std::size_t N = sizeof(::std::declval<U&>().operator&())>
	static constexpr bool has_overload(bool /*unused*/)
	{
		return true;
	}

	static constexpr bool value = has_overload<T>(true);
};

template <typename T, typename ::std::enable_if<!has_overloaded_addressof<T>::value, bool>::type = false>
constexpr inline T* static_addressof(T& ref)
{
	return &ref;
}

template <typename T, typename ::std::enable_if<has_overloaded_addressof<T>::value, bool>::type = false>
constexpr inline T* static_addressof(T& ref)
{
	return std::addressof(ref);
}

constexpr inline std::size_t static_strlen(const char* str)
{
	return (str == nullptr || *str == 0) ? 0 : 1 + static_strlen(++str);
}

constexpr inline std::size_t static_wstrlen(const wchar_t* str)
{
	return (str == nullptr || *str == 0) ? 0 : 1 + static_wstrlen(++str);
}

template <typename InIter1, typename InIter2>
constexpr void static_copy(InIter1 /*unused*/, InIter1 /*unused*/, InIter2 /*unused*/)
{
	// return (first1 != last1) ?
}

template <typename InIter, typename Pred>
constexpr inline bool static_all_of(InIter first, InIter last, Pred pr)
{
	return (first != last && pr(*first)) ? static_all_of(++first, last, pr) : (first == last);
}
}

struct uuid
{
	using value_type = std::uint8_t;
	using pointer = value_type*;
	using reference = value_type&;
	using const_pointer = value_type const*;
	using const_reference = value_type const&;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

	///////////////////////////////////////////////////////////////////////

	enum version_type
	{
		version_unknown = 0x00,
		version_time_based = 0x01,
		version_dce_security = 0x02,
		version_name_based_md5 = 0x03,
		version_random = 0x04,
		version_name_based_sha1 = 0x05,
	};

	enum variant_type
	{
		variant_unknown,
		variant_ncs,
		variant_rfc4122,
		variant_microsoft,
		variant_future,
	};

	///////////////////////////////////////////////////////////////////////

	constexpr uuid()
		: data_{0}
	{
	}

	constexpr uuid(uuid&&) = default;

	constexpr uuid(uuid const&) = default;

	constexpr explicit uuid(char const* cstr)
		: uuid(cstr, detail::static_strlen(cstr))
	{
	}

	constexpr uuid(char const* /*unused*/, size_type /*unused*/)
		: uuid()
	{
		// std::isxdigit(ch)..
	}

	explicit uuid(std::string const& str)
		: uuid(str.c_str(), str.size())
	{
	}

	// constexpr pl0x!
	explicit uuid(const std::initializer_list<value_type>& ilist)
		: uuid(ilist.begin(), ilist.end())
	{
	}

	template <typename InIter>
	uuid(InIter first, InIter last)
	{
		::std::copy(first, last, data_);
	}

	uuid& operator=(uuid&&) = default;

	uuid& operator=(uuid const&) = default;

	///////////////////////////////////////////////////////////////////////

	constexpr const_reference operator[](size_type idx) const noexcept
	{
		return data_[idx];
	}

	///////////////////////////////////////////////////////////////////////

	// constexpr bool is_nil() const noexcept
	//{
	//    return detail::static_all_of(begin(), end(), [](value_type x) { return x
	//    == 0; });
	//}

	static constexpr size_type size() noexcept
	{
		return static_size;
	}

	constexpr version_type version() const noexcept
	{
		return version_random; // not implemented, of course...
	}

	constexpr variant_type variant() const noexcept
	{
		return (
			(data_[8] >> 5) == 0x07
				? variant_future
				: ((data_[8] >> 5) == 0x06
					   ? variant_microsoft
					   : ((data_[8] >> 6) == 0x02
							  ? variant_rfc4122
							  : ((data_[8] >> 7) == 0x00 ? variant_ncs : variant_unknown)))); // not sure if
		// variant_unknown
		// possible... here
		// just cuz..
	}

	std::string to_string() const
	{
		static constexpr char const* const hex_digits = "0123456789abcdef";

		::std::string result(this->size() * 2 + 4, 0);
		result[8] = result[13] = result[18] = result[23] = '-';

		for(size_type i = 0, k = 0; i < result.size() - 1;)
		{
			if(result[i] == '-')
			{
				++i;
				continue;
			}

			result[i++] = hex_digits[(data_[k] & 0xf0u) >> 4];
			result[i++] = hex_digits[(data_[k] & 0x0fu)];
			++k;
		}

		return result;
	}

	///////////////////////////////////////////////////////////////////////

	constexpr const_iterator begin() const noexcept
	{
		return detail::static_addressof(data_[0]);
	}

	constexpr const_iterator end() const noexcept
	{
		return detail::static_addressof(data_[static_size - 1]);
	}

	constexpr const_iterator cbegin() const noexcept
	{
		return detail::static_addressof(data_[0]);
	}

	constexpr const_iterator cend() const noexcept
	{
		return detail::static_addressof(data_[static_size - 1]);
	}

	const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator{end()};
	}

	const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator{begin()};
	}

	const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator{cend()};
	}

	const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator{cbegin()};
	}

	///////////////////////////////////////////////////////////////////////

	void swap(uuid& other) noexcept
	{
		uuid tmp = *this;
		*this = other;
		other = tmp;
	}

	///////////////////////////////////////////////////////////////////////

	friend bool operator==(uuid const& lhs, uuid const& rhs)
	{
		return ::std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	friend bool operator<(uuid const& lhs, uuid const& rhs)
	{
		return ::std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	///////////////////////////////////////////////////////////////////////

	template <typename CharT, typename Traits>
	friend ::std::basic_ostream<CharT, Traits>& operator<<(::std::basic_ostream<CharT, Traits>& os,
														   uuid const& u)
	{
		typedef typename std::basic_ostream<CharT, Traits> ostream_type;
		typename ostream_type::sentry ok(os);

		if(ok)
		{
			for(std::size_t i = 0; i != u.size(); ++i)
			{
				os << ::std::hex << ::std::setfill('0') << ::std::setw(2) << static_cast<int>(u[i]);
				if(i == 3 || i == 5 || i == 7 || i == 9)
				{
					os << os.widen('-');
				}
			}
		}

		return os;
	}

private:
	static constexpr std::size_t static_size = 16;
	value_type data_[static_size];
};

///////////////////////////////////////////////////////////////////////

inline void swap(uuid& lhs, uuid& rhs)
{
	lhs.swap(rhs);
}

///////////////////////////////////////////////////////////////////////

inline bool operator!=(uuid const& lhs, uuid const& rhs)
{
	return !(lhs == rhs);
}

inline bool operator<=(uuid const& lhs, uuid const& rhs)
{
	return !(rhs < lhs);
}

inline bool operator>=(uuid const& lhs, uuid const& rhs)
{
	return !(lhs < rhs);
}

inline bool operator>(uuid const& lhs, uuid const& rhs)
{
	return (rhs < lhs);
}
}

namespace std
{
template <>
struct hash<uuids::uuid>
{
	size_t operator()(const uuids::uuid& obj) const
	{
		hash<std::string> hasher;
		return hasher(obj.to_string());
	}
};
}

#endif // UUIDS_UUID_HPP
