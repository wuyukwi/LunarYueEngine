#ifndef MML_UTF_HPP
#define MML_UTF_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <algorithm>
#include <locale>
#include <string>
#include <cstdlib>
#include <cstdint>

namespace mml
{
template <unsigned int N>
class utf;

////////////////////////////////////////////////////////////
/// \brief Specialization of the utf template for UTF-8
///
////////////////////////////////////////////////////////////
template <>
class utf<8>
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Decode a single UTF-8 character
    ///
    /// Decoding a character means finding its unique 32-bits
    /// code (called the codepoint) in the Unicode standard.
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Codepoint of the decoded UTF-8 character
    /// \param replacement Replacement character to use in case the UTF-8 sequence is invalid
    ///
    /// \return Iterator pointing to one past the last read element of the input sequence
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static In decode(In begin, In end, std::uint32_t& output, std::uint32_t replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Encode a single UTF-8 character
    ///
    /// Encoding a character means converting a unique 32-bits
    /// code (called the codepoint) in the target encoding, UTF-8.
    ///
    /// \param input       Codepoint to encode as UTF-8
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to UTF-8 (use 0 to skip them)
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename Out>
    static Out encode(std::uint32_t input, Out output, std::uint8_t replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Advance to the next UTF-8 character
    ///
    /// This function is necessary for multi-elements encodings, as
    /// a single character may use more than 1 storage element.
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end   Iterator pointing to the end of the input sequence
    ///
    /// \return Iterator pointing to one past the last read element of the input sequence
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static In next(In begin, In end);

    ////////////////////////////////////////////////////////////
    /// \brief Count the number of characters of a UTF-8 sequence
    ///
    /// This function is necessary for multi-elements encodings, as
    /// a single character may use more than 1 storage element, thus the
    /// total size can be different from (begin - end).
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end   Iterator pointing to the end of the input sequence
    ///
    /// \return Iterator pointing to one past the last read element of the input sequence
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static std::size_t count(In begin, In end);

    ////////////////////////////////////////////////////////////
    /// \brief Convert an ANSI characters range to UTF-8
    ///
    /// The current global locale will be used by default, unless you
    /// pass a custom one in the \a locale parameter.
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    /// \param locale Locale to use for conversion
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out from_ansi(In begin, In end, Out output, const std::locale& locale = std::locale());

    ////////////////////////////////////////////////////////////
    /// \brief Convert a wide characters range to UTF-8
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out from_wide(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a latin-1 (ISO-5589-1) characters range to UTF-8
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out from_latin1(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert an UTF-8 characters range to ANSI characters
    ///
    /// The current global locale will be used by default, unless you
    /// pass a custom one in the \a locale parameter.
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to ANSI (use 0 to skip them)
    /// \param locale      Locale to use for conversion
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_ansi(In begin, In end, Out output, char replacement = 0, const std::locale& locale = std::locale());

    ////////////////////////////////////////////////////////////
    /// \brief Convert an UTF-8 characters range to wide characters
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to wide (use 0 to skip them)
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_wide(In begin, In end, Out output, wchar_t replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Convert an UTF-8 characters range to latin-1 (ISO-5589-1) characters
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to wide (use 0 to skip them)
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_latin1(In begin, In end, Out output, char replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a UTF-8 characters range to UTF-8
    ///
    /// This functions does nothing more than a direct copy;
    /// it is defined only to provide the same interface as other
    /// specializations of the mml::utf<> template, and allow
    /// generic code to be written on top of it.
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_utf8(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a UTF-8 characters range to UTF-16
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_utf16(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a UTF-8 characters range to UTF-32
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_utf32(In begin, In end, Out output);
};

////////////////////////////////////////////////////////////
/// \brief Specialization of the utf template for UTF-16
///
////////////////////////////////////////////////////////////
template <>
class utf<16>
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Decode a single UTF-16 character
    ///
    /// Decoding a character means finding its unique 32-bits
    /// code (called the codepoint) in the Unicode standard.
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Codepoint of the decoded UTF-16 character
    /// \param replacement Replacement character to use in case the UTF-8 sequence is invalid
    ///
    /// \return Iterator pointing to one past the last read element of the input sequence
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static In decode(In begin, In end, std::uint32_t& output, std::uint32_t replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Encode a single UTF-16 character
    ///
    /// Encoding a character means converting a unique 32-bits
    /// code (called the codepoint) in the target encoding, UTF-16.
    ///
    /// \param input       Codepoint to encode as UTF-16
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to UTF-16 (use 0 to skip them)
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename Out>
    static Out encode(std::uint32_t input, Out output, std::uint16_t replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Advance to the next UTF-16 character
    ///
    /// This function is necessary for multi-elements encodings, as
    /// a single character may use more than 1 storage element.
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end   Iterator pointing to the end of the input sequence
    ///
    /// \return Iterator pointing to one past the last read element of the input sequence
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static In next(In begin, In end);

    ////////////////////////////////////////////////////////////
    /// \brief Count the number of characters of a UTF-16 sequence
    ///
    /// This function is necessary for multi-elements encodings, as
    /// a single character may use more than 1 storage element, thus the
    /// total size can be different from (begin - end).
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end   Iterator pointing to the end of the input sequence
    ///
    /// \return Iterator pointing to one past the last read element of the input sequence
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static std::size_t count(In begin, In end);

    ////////////////////////////////////////////////////////////
    /// \brief Convert an ANSI characters range to UTF-16
    ///
    /// The current global locale will be used by default, unless you
    /// pass a custom one in the \a locale parameter.
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    /// \param locale Locale to use for conversion
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out from_ansi(In begin, In end, Out output, const std::locale& locale = std::locale());

    ////////////////////////////////////////////////////////////
    /// \brief Convert a wide characters range to UTF-16
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out from_wide(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a latin-1 (ISO-5589-1) characters range to UTF-16
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out from_latin1(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert an UTF-16 characters range to ANSI characters
    ///
    /// The current global locale will be used by default, unless you
    /// pass a custom one in the \a locale parameter.
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to ANSI (use 0 to skip them)
    /// \param locale      Locale to use for conversion
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_ansi(In begin, In end, Out output, char replacement = 0, const std::locale& locale = std::locale());

    ////////////////////////////////////////////////////////////
    /// \brief Convert an UTF-16 characters range to wide characters
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to wide (use 0 to skip them)
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_wide(In begin, In end, Out output, wchar_t replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Convert an UTF-16 characters range to latin-1 (ISO-5589-1) characters
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to wide (use 0 to skip them)
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_latin1(In begin, In end, Out output, char replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a UTF-16 characters range to UTF-8
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_utf8(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a UTF-16 characters range to UTF-16
    ///
    /// This functions does nothing more than a direct copy;
    /// it is defined only to provide the same interface as other
    /// specializations of the mml::utf<> template, and allow
    /// generic code to be written on top of it.
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_utf16(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a UTF-16 characters range to UTF-32
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_utf32(In begin, In end, Out output);
};

////////////////////////////////////////////////////////////
/// \brief Specialization of the utf template for UTF-32
///
////////////////////////////////////////////////////////////
template <>
class utf<32>
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Decode a single UTF-32 character
    ///
    /// Decoding a character means finding its unique 32-bits
    /// code (called the codepoint) in the Unicode standard.
    /// For UTF-32, the character value is the same as the codepoint.
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Codepoint of the decoded UTF-32 character
    /// \param replacement Replacement character to use in case the UTF-8 sequence is invalid
    ///
    /// \return Iterator pointing to one past the last read element of the input sequence
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static In decode(In begin, In end, std::uint32_t& output, std::uint32_t replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Encode a single UTF-32 character
    ///
    /// Encoding a character means converting a unique 32-bits
    /// code (called the codepoint) in the target encoding, UTF-32.
    /// For UTF-32, the codepoint is the same as the character value.
    ///
    /// \param input       Codepoint to encode as UTF-32
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to UTF-32 (use 0 to skip them)
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename Out>
    static Out encode(std::uint32_t input, Out output, std::uint32_t replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Advance to the next UTF-32 character
    ///
    /// This function is trivial for UTF-32, which can store
    /// every character in a single storage element.
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end   Iterator pointing to the end of the input sequence
    ///
    /// \return Iterator pointing to one past the last read element of the input sequence
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static In next(In begin, In end);

    ////////////////////////////////////////////////////////////
    /// \brief Count the number of characters of a UTF-32 sequence
    ///
    /// This function is trivial for UTF-32, which can store
    /// every character in a single storage element.
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end   Iterator pointing to the end of the input sequence
    ///
    /// \return Iterator pointing to one past the last read element of the input sequence
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static std::size_t count(In begin, In end);

    ////////////////////////////////////////////////////////////
    /// \brief Convert an ANSI characters range to UTF-32
    ///
    /// The current global locale will be used by default, unless you
    /// pass a custom one in the \a locale parameter.
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    /// \param locale Locale to use for conversion
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out from_ansi(In begin, In end, Out output, const std::locale& locale = std::locale());

    ////////////////////////////////////////////////////////////
    /// \brief Convert a wide characters range to UTF-32
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out from_wide(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a latin-1 (ISO-5589-1) characters range to UTF-32
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out from_latin1(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert an UTF-32 characters range to ANSI characters
    ///
    /// The current global locale will be used by default, unless you
    /// pass a custom one in the \a locale parameter.
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to ANSI (use 0 to skip them)
    /// \param locale      Locale to use for conversion
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_ansi(In begin, In end, Out output, char replacement = 0, const std::locale& locale = std::locale());

    ////////////////////////////////////////////////////////////
    /// \brief Convert an UTF-32 characters range to wide characters
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to wide (use 0 to skip them)
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_wide(In begin, In end, Out output, wchar_t replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Convert an UTF-16 characters range to latin-1 (ISO-5589-1) characters
    ///
    /// \param begin       Iterator pointing to the beginning of the input sequence
    /// \param end         Iterator pointing to the end of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to wide (use 0 to skip them)
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_latin1(In begin, In end, Out output, char replacement = 0);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a UTF-32 characters range to UTF-8
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_utf8(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a UTF-32 characters range to UTF-16
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_utf16(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Convert a UTF-32 characters range to UTF-32
    ///
    /// This functions does nothing more than a direct copy;
    /// it is defined only to provide the same interface as other
    /// specializations of the mml::utf<> template, and allow
    /// generic code to be written on top of it.
    ///
    /// \param begin  Iterator pointing to the beginning of the input sequence
    /// \param end    Iterator pointing to the end of the input sequence
    /// \param output Iterator pointing to the beginning of the output sequence
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename In, typename Out>
    static Out to_utf32(In begin, In end, Out output);

    ////////////////////////////////////////////////////////////
    /// \brief Decode a single ANSI character to UTF-32
    ///
    /// This function does not exist in other specializations
    /// of mml::utf<>, it is defined for convenience (it is used by
    /// several other conversion functions).
    ///
    /// \param input  Input ANSI character
    /// \param locale Locale to use for conversion
    ///
    /// \return Converted character
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static std::uint32_t decode_ansi(In input, const std::locale& locale = std::locale());

    ////////////////////////////////////////////////////////////
    /// \brief Decode a single wide character to UTF-32
    ///
    /// This function does not exist in other specializations
    /// of mml::utf<>, it is defined for convenience (it is used by
    /// several other conversion functions).
    ///
    /// \param input Input wide character
    ///
    /// \return Converted character
    ///
    ////////////////////////////////////////////////////////////
    template <typename In>
    static std::uint32_t decode_wide(In input);

    ////////////////////////////////////////////////////////////
    /// \brief Encode a single UTF-32 character to ANSI
    ///
    /// This function does not exist in other specializations
    /// of mml::utf<>, it is defined for convenience (it is used by
    /// several other conversion functions).
    ///
    /// \param codepoint   Iterator pointing to the beginning of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement if the input character is not convertible to ANSI (use 0 to skip it)
    /// \param locale      Locale to use for conversion
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename Out>
    static Out encode_ansi(std::uint32_t codepoint, Out output, char replacement = 0, const std::locale& locale = std::locale());

    ////////////////////////////////////////////////////////////
    /// \brief Encode a single UTF-32 character to wide
    ///
    /// This function does not exist in other specializations
    /// of mml::utf<>, it is defined for convenience (it is used by
    /// several other conversion functions).
    ///
    /// \param codepoint   Iterator pointing to the beginning of the input sequence
    /// \param output      Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement if the input character is not convertible to wide (use 0 to skip it)
    ///
    /// \return Iterator to the end of the output sequence which has been written
    ///
    ////////////////////////////////////////////////////////////
    template <typename Out>
    static Out encode_wide(std::uint32_t codepoint, Out output, wchar_t replacement = 0);
};

#include <mml/system/utf.inl>

// Make typedefs to get rid of the template syntax
using utf8 = utf<8>;
using utf16 = utf<16>;
using utf32 = utf<32>;

} // namespace mml


#endif // MML_UTF_HPP
