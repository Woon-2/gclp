/*
The MIT License

Copyright (c) 2023 Woon2

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __cl_parser
#define __cl_parser

#include <string>
#include <string_view>
#include <optional>
#include <tuple>
#include <ranges>
#include <algorithm>
#include <utility>
#include <iterator>
#include <functional>
#include <type_traits>
#include <concepts>
#include <sstream>
#include <vector>
#include <initializer_list>
#include <limits>
#include <cassert>
#include <format>


/* The '__LITERAL' macro generates character literals
   based on the provided 'type' and 's'.
   It handles different character types such as
   'char', 'wchar_t', 'char16_t', 'char32_t', and 'char8_t' (if supported),
   allowing seamless integration of character literals
   into template functions with generic character types.
   Note: '__cpp_char8_t' check ensures compatibility with C++20's 'char8_t'. */
#ifdef __cpp_char8_t
#define __LITERAL(type, s)  \
[]() {  \
    if constexpr ( std::is_same_v<type, char> ) {   \
        return s;   \
    }   \
    else if constexpr ( std::is_same_v<type, wchar_t> ) {   \
        return L ## s;  \
    }   \
    else if constexpr ( std::is_same_v<type, char8_t> ) {   \
        return u8 ## s; \
    }   \
    else if constexpr ( std::is_same_v<type, char16_t> ) {  \
        return u ## s;  \
    }   \
    else if constexpr ( std::is_same_v<type, char32_t> ) {  \
        return U ## s;  \
    }   \
    else {  \
        return s;   \
    }   \
}()
#else
#define __LITERAL(type, s)  \
[]() {  \
    if constexpr ( std::is_same_v<type, char> ) {   \
        return s;   \
    }   \
    else if constexpr ( std::is_same_v<type, wchar_t> ) {   \
        return L ## s;  \
    }   \
    else if constexpr ( std::is_same_v<type, char16_t> ) {  \
        return u ## s;  \
    }   \
    else if constexpr ( std::is_same_v<type, char32_t> ) {  \
        return U ## s;  \
    }   \
    else {  \
        return s;   \
    }   \
}()
#endif

namespace clp {

namespace detail {

/**
 * @brief Concept to check if a type is one of the specified types.
 * 
 * This concept checks if a given type T is the same as any of the types specified in Args.
 * @tparam T The type to be checked against Args.
 * @tparam Args Variadic template arguments representing types to check against.
 */
template <class T, class ... Args>
concept contains = (std::is_same_v<T, Args> || ...);

/**
 * @brief Struct to check if a type is a character type.
 * 
 * This struct checks if a given type T is one of the standard character types.
 * @tparam T The type to be checked.
 */
template <class>
struct is_char : std::false_type
{};

/**
 * @brief Specialization of is_char struct for character types.
 * 
 * This specialization checks if a given type T is one of the standard character types:
 * char, wchar_t, char8_t, char16_t, char32_t, signed char, or unsigned char.
 * @tparam T The type to be checked.
 */
template <class T>
requires contains<T, char, wchar_t, char8_t, char16_t, char32_t,
    signed char, unsigned char>
struct is_char<T> : std::true_type
{};

/**
 * @brief Variable template to check if a type is a character type.
 * 
 * This variable template holds true if the provided type T is a standard character type,
 * and false otherwise.
 * @tparam T The type to be checked.
 */
template <class T>
inline constexpr auto is_char_v = is_char<T>::value;

/**
 * @brief Concept to check if a type is not convertible to another type.
 * 
 * This concept checks if a given type From is not convertible to type To.
 * @tparam From The type to be checked for non-convertibility.
 * @tparam To The target type to which conversion is not allowed.
 */
template <class From, class To>
concept not_convertible_to = !std::is_convertible_v<From, To>;

/**
 * @brief Applies a function to each argument in a parameter pack.
 * 
 * This function applies the provided function `func` to each argument in the parameter pack `args`.
 * @tparam Func The function type to be applied.
 * @tparam Args Variadic template arguments representing the arguments to be passed to the function.
 * @param func The function to be applied to each argument.
 * @param args The arguments to be passed to the function.
 */
template <class Func, class ... Args>
void for_each_args(Func func, Args&& ... args) {
    ( func( std::forward<Args>(args) ), ... );
}

template <class Func, class TupleLike, std::size_t ... Idxs>
void tuple_for_each_helper(
    TupleLike&& t, Func func, std::index_sequence<Idxs...>
) {
    for_each_args( func, std::get<Idxs>(t)... );
}

/**
 * @brief Applies a function to each element in a tuple-like object.
 * 
 * This function applies the provided function `func` to each element in the tuple-like object `t`.
 * @tparam Func The function type to be applied.
 * @tparam TupleLike The tuple-like object type.
 * @param t The tuple-like object.
 * @param func The function to be applied to each element.
 */
template <class Func, class TupleLike>
void tuple_for_each(TupleLike&& t, Func func) {
    tuple_for_each_helper( std::forward<TupleLike>(t), func,
        std::make_index_sequence<
            std::tuple_size_v<
                std::remove_cvref_t<TupleLike>
            >
        >{}
    );
}

template <
    class InTupleLike, class OutTupleLike,
    class Func, std::size_t ... Idxs
>
void tuple_conv_helper(
    const InTupleLike& tuple_in, OutTupleLike& tuple_out,
    Func func, std::index_sequence<Idxs...>
) {
    (   ( std::get<Idxs>(tuple_out) = func(
                std::get<Idxs>(tuple_in)
            )
        ), ...
    );
}

/**
 * @brief Converts elements from an input tuple to an output tuple using a specified function.
 * 
 * This function applies the provided function `func` to each element in the input tuple `tuple_in`
 * and assigns the results to the corresponding elements in the output tuple `tuple_out`.
 * @tparam InTupleLike The type of the input tuple-like object.
 * @tparam OutTupleLike The type of the output tuple-like object.
 * @tparam Func The function type to be applied.
 * @param tuple_in The input tuple-like object.
 * @param tuple_out The output tuple-like object.
 * @param func The function to be applied to each element.
 */
template <class InTupleLike, class OutTupleLike, class Func>
    requires requires {
        std::tuple_size_v<
            std::remove_cvref_t<InTupleLike>
        >;
        std::tuple_size_v<
            std::remove_cvref_t<OutTupleLike>
        >;
    }
void tuple_conv(
    const InTupleLike& tuple_in, OutTupleLike& tuple_out,
    Func func
) {
    static_assert(
        std::tuple_size_v<std::remove_cvref_t<InTupleLike>>
        == std::tuple_size_v<std::remove_cvref_t<OutTupleLike>>
    );
    
    tuple_conv_helper(
        tuple_in, tuple_out, func,
        std::make_index_sequence<
            std::tuple_size_v<
                std::remove_cvref_t<InTupleLike>
            >
        >{}
    );
}

/**
 * @brief Generates a dash character.
 * 
 * This function returns the dash character ('-') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The dash character.
 */
template <class CharT>
constexpr auto dash() {
    return CharT('-');
}

/**
 * @brief Generates a character escape character.
 * 
 * This function returns the character escape character ('\\') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The character escape character.
 */
template <class CharT>
constexpr auto char_escape() {
    return CharT('\\');
}

/**
 * @brief Generates a stream delimiter character.
 * 
 * This function returns the stream delimiter character (' ') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The stream delimiter character.
 */
template <class CharT>
constexpr auto stream_delim() {
    return CharT(' ');
}

/**
 * @brief Generates a character assignment character.
 * 
 * This function returns the character assignment character ('=') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The character assignment character.
 */
template <class CharT>
constexpr auto char_assign() {
    return CharT('=');
}

/**
 * @brief Generates a single quote character.
 * 
 * This function returns the single quote character ('\'') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The single quote character.
 */
template <class CharT>
constexpr auto single_quote() {
    return CharT('\'');
}

/**
 * @brief Generates a double quote character.
 * 
 * This function returns the double quote character ('\"') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The double quote character.
 */
template <class CharT>
constexpr auto double_quote() {
    return CharT('\"');
}

/**
 * @brief Checks if a string view starts with a single dash character.
 * 
 * This function checks if the provided string view `word` starts with a single dash character and is not followed by another dash.
 * @tparam StringView The string view type.
 * @param word The string view to be checked.
 * @return true if the string starts with a single dash character, false otherwise.
 */
template <class StringView>
auto is_single_dashed(StringView word) noexcept {
    using char_type = typename StringView::value_type;

    return std::size(word) > std::size_t(1)
        && word[0] == dash<char_type>()
        && word[1] != dash<char_type>();
}

/**
 * @brief Checks if a string view represents a key (starts with a dash and not followed by another dash).
 * 
 * This function checks if the provided string view `word` represents a key, which starts with a dash character and is not followed by another dash.
 * @tparam StringView The string view type.
 * @param word The string view to be checked.
 * @return true if the string represents a key, false otherwise.
 */
template <class StringView>
auto is_key(StringView word) noexcept {
    using char_type = typename StringView::value_type;

    if ( std::size(word) == std::size_t(1) ) {
        return false;
    }
    
    if ( std::size(word) > std::size_t(2) ) {
        return word[0] == dash<char_type>()
            && word[2] != dash<char_type>();
    }
    
    return word[0] == dash<char_type>()
        && word[1] != dash<char_type>();
}

/**
 * @brief Checks if a string view represents a complex boolean parameter (starts with a single dash and has length greater than 2).
 * 
 * This function checks if the provided string view `word` starts with a single dash character and has a length greater than 2.
 * @tparam StringView The string view type.
 * @param word The string view to be checked.
 * @return true if the string represents a complex boolean parameter, false otherwise.
 */
template <class StringView>
auto is_complex_boolean_param(StringView word) noexcept {
    return is_single_dashed(word) && std::size(word) > 2;
}

/**
 * @brief Removes leading dash characters from a string view.
 * 
 * This function removes the leading dash characters from the provided string view `s`.
 * @tparam StringView The string view type.
 * @param s The input string view.
 * @return A new string view with leading dash characters removed.
 */
template <class StringView>
auto remove_dash(StringView s) noexcept {
    return StringView(
        s.begin() + s.find_first_not_of('-'),
        s.end()
    );
}

/**
 * @brief Splits a string view into words based on specified delimiters.
 * 
 * This function splits the provided string view `s` into words using delimiters, including handling quotes and escape characters.
 * @tparam StringView The string view type.
 * @param s The input string view to be split.
 * @return A vector of string views representing individual words.
 */
template <class StringView>
auto split_words(StringView s) {
    using char_type = typename StringView::value_type;

    auto ret = std::vector<StringView>{};

    auto is_double_quoted = false;
    auto is_single_quoted = false;
    auto has_escaped = false;
    auto has_valid_char = false;

    auto it_first = std::begin(s);
    auto it_last = std::end(s);
    auto it_out = std::back_inserter(ret);

    auto pred = std::equal_to<char_type>();
    auto delim = stream_delim<char_type>();
    auto is_delim = [&delim](auto ch) {
            return ch == delim;
    };
    auto new_delim = [](auto ch) {
        if ( ch == single_quote<char_type>() ) {
            return single_quote<char_type>();
        }
        else if ( ch == double_quote<char_type>() ) {
            return double_quote<char_type>();
        }
        else {
            return stream_delim<char_type>();
        }
    };


    for (; it_first != it_last; ++it_first) {
        // jump escape sequence
        if (has_escaped) {
            has_escaped = false;
            continue;
        }
        
        // check escape sequence
        if (
            has_escaped = pred(
                *it_first, char_escape<char_type>()
            )
        ) {
            continue;
        }
        
        it_first = std::ranges::find_if_not(it_first, it_last, is_delim);
        if (it_first == it_last) {
            break;
        }
        delim = new_delim(*it_first);
        
        it_first = std::ranges::find_if_not(it_first, it_last, is_delim);
        if (it_first == it_last) {
            break;
        }
        
        auto it_split_last = std::ranges::find_if(
            it_first, it_last, is_delim
        );
        it_out = StringView(it_first, it_split_last);
        if (it_split_last == it_last) {
            break;
        }
        delim = new_delim(*it_first);
        it_first = it_split_last;
    }

    return ret;
}

}   // namespace clp::detail

/**
 * @brief Concept representing character types suitable for command-line parameters.
 * 
 * This concept checks if the provided type T satisfies the requirements for a command-line parameter character.
 * @tparam T The character type to be checked.
 */
template <class T>
concept clp_char = detail::is_char_v<T>;

/**
 * @brief Represents a basic command-line parameter with specified key characters and brief description.
 * 
 * This class provides functionality to handle command-line parameters with specific key characters and brief descriptions.
 * @tparam ValT The type of the parameter value.
 * @tparam CharT The character type used for keys (default is char).
 * @tparam Traits The character traits type (default is std::char_traits<CharT>).
 */
template <
    class ValT, clp_char CharT = char,
    class Traits = std::char_traits<CharT>
>
class basic_cl_param
{
public:
    using value_type = ValT;
    using char_type = CharT;
    using traits_type = Traits;
    using string_view_type =
        std::basic_string_view<char_type, traits_type>;
    template <class T>
    using key_container = std::vector<T>;

private:
    using istream_type = std::basic_istream<char_type, traits_type>;
    using ostream_type = std::basic_ostream<char_type, traits_type>;

public:
    /**
     * @brief Constructs a basic_cl_param object with specified key characters and brief description.
     * 
     * Initializes a basic_cl_param object with the specified key characters, key strings, and brief description.
     * @param key_chars An std::intializer_list of single-character keys associated with the parameter.
     * @param key_strs An std::intializer_list of string keys associated with the parameter.
     * @param brief A brief description of the parameter.
     */
    basic_cl_param(
        std::initializer_list<char_type> key_chars,
        std::initializer_list<string_view_type> key_strs,
        string_view_type brief
    ) : key_chars_(key_chars), key_strs_(key_strs),
        brief_(brief), defval_(), val_(), fail_(false) {}

    /**
     * @brief Constructs a basic_cl_param object with specified key characters, brief description, and default value.
     * 
     * Initializes a basic_cl_param object with the specified key characters, key strings, brief description, and default value.
     * @param key_chars An std::intializer_list of single-character keys associated with the parameter.
     * @param key_strs An std::intializer_list of string keys associated with the parameter.
     * @param defval The default value of the parameter.
     * @param brief A brief description of the parameter.
     */
    basic_cl_param(
        std::initializer_list<char_type> key_chars,
        std::initializer_list<string_view_type> key_strs,
        const value_type& defval,
        string_view_type brief
    ) : key_chars_(key_chars), key_strs_(key_strs),
        brief_(brief), defval_(defval), val_(), fail_(false) {}

    /**
     * @brief Constructs a basic_cl_param object with specified key characters, brief description, and default value.
     * 
     * Initializes a basic_cl_param object with the specified key characters, key strings, brief description, and default value.
     * @param key_chars An std::intializer_list of single-character keys associated with the parameter.
     * @param key_strs An std::intializer_list of string keys associated with the parameter.
     * @param defval The default value of the parameter (rvalue reference version).
     * @param brief A brief description of the parameter.
     */
    basic_cl_param(
        std::initializer_list<char_type> key_chars,
        std::initializer_list<string_view_type> key_strs,
        value_type&& defval,
        string_view_type brief
    ) : key_chars_(key_chars), key_strs_(key_strs),
        brief_(brief), defval_( std::move(defval) ), val_(), fail_(false) {}

    /**
     * @brief Checks if the parameter has a value (either assigned or default).
     * 
     * @return true if the parameter has a value, false otherwise.
     */
    auto has_value() const noexcept {
        return val_.has_value() || defval_.has_value();
    }

    /**
     * @brief Returns a reference to the parameter value or default value.
     * 
     * This function returns a reference to the parameter value if it is assigned, or the default value otherwise.
     * @return A reference to the parameter value or default value.
     * @pre The parameter has a value (either assigned or default).
     */
    auto& value() noexcept {
        assert(has_value());

        if (val_.has_value()) {
            return val_.value();
        }
        return defval_.value();
    }

    /**
     * @brief Returns a const reference to the parameter value or default value.
     * 
     * This function returns a const reference to the parameter value if it is assigned, or the default value otherwise.
     * @return A const reference to the parameter value or default value.
     * @pre The parameter has a value (either assigned or default).
     */
    const auto& value() const noexcept {
        assert(has_value());

        if (val_.has_value()) {
            return val_.value();
        }
        return defval_.value();
    }

    /**
     * @brief Checks if the parameter contains the specified single-character key.
     * 
     * This function checks if the parameter contains the specified single-character key.
     * @param key_char The single-character key to check for.
     * @return true if the key is found, false otherwise.
     */
    auto contains(char_type key_char) const {
        return std::ranges::find(key_chars_, key_char) != std::end(key_chars_);
    }
    
    /**
     * @brief Checks if the parameter contains the specified string key.
     * 
     * This function checks if the parameter contains the specified string key.
     * @param key_string The string key to check for.
     * @return true if the key is found, false otherwise.
     */
    auto contains(string_view_type key_string) const {
        return std::ranges::find(key_strs_, key_string) != std::end(key_strs_);
    }
    
    /**
     * @brief Checks if the parameter contains the specified key range.
     * 
     * This function checks if the parameter contains the specified key range defined by iterators `first` and `last`.
     * @tparam It An input iterator type.
     * @param first The beginning of the key range.
     * @param last The end of the key range.
     * @return true if the key range is found, false otherwise.
     */
    template <std::input_iterator It>
        requires requires(It a, It b) {
            string_view_type(a, b);
        }
    auto contains(It first, It last) const {
        return std::ranges::find(
                key_strs_, string_view_type(first, last)
            ) != std::end(key_strs_);
    }
    
    /**
     * @brief Checks if the parameter contains the specified key range from a range object.
     * 
     * This function checks if the parameter contains the specified key range obtained from the input range object `r`.
     * @tparam R A range type.
     * @param r The input range object.
     * @return true if the key range is found, false otherwise.
     */
    template <std::ranges::range R>
       requires std::convertible_to<R, string_view_type>
    auto contains(R&& r) const {
        return contains(
            static_cast<string_view_type>( std::forward<R>(r) )
        );
    }

    /**
     * @brief Returns a const reference to the list of single-character keys associated with the parameter.
     * 
     * This function returns a const reference to the list of single-character keys associated with the parameter.
     * @return A const reference to the list of single-character keys.
     */
    const auto& key_chars() const noexcept {
        return key_chars_;
    }

    /**
     * @brief Returns a const reference to the list of string keys associated with the parameter.
     * 
     * This function returns a const reference to the list of string keys associated with the parameter.
     * @return A const reference to the list of string keys.
     */
    const auto& key_strs() const noexcept {
        return key_strs_;
    }

    /**
     * @brief Checks if the parameter has a brief description message.
     * 
     * This function checks if the parameter has a brief description message.
     * @return true if the parameter has a brief description, false otherwise.
     */
    auto has_brief_message() const noexcept {
        return brief_.has_value();
    }

    /**
     * @brief Returns the brief description message associated with the parameter.
     * 
     * This function returns the brief description message associated with the parameter.
     * @return The brief description message.
     * @pre The parameter has a brief description.
     */
    const auto brief_message() const noexcept {
        assert(has_brief_message());
        return brief_.value();
    }

    /**
     * @brief Assigns a value to the parameter, marking the parameter as failed if the assignment is not possible.
     * 
     * Attempts to assign the provided value to the parameter. If the assignment is not possible due to type incompatibility,
     * the parameter is marked as failed, and the function returns false.
     * @tparam InCompatible The type of the value to be assigned.
     * @param arg The value to be assigned.
     * @return true if the assignment is successful, false otherwise.
     */
    template <class InCompatible>
        requires detail::not_convertible_to< 
            std::remove_cvref_t<InCompatible>,
            value_type
        >
    auto assign(InCompatible&& arg) {
        fail_ = true;
        return false;
    }

    /**
     * @brief Assigns a const reference value to the parameter if no previous failures occurred.
     * 
     * Attempts to assign the provided const reference value to the parameter. If there were no previous failures, 
     * the assignment is successful, and the function returns true. Otherwise, the function returns false.
     * @param val The const reference value to be assigned.
     * @return true if the assignment is successful, false otherwise.
     */
    auto assign(const value_type& val) {
        if (fail()) {
            return false;
        }
        val_ = val;
        return true;
    }

    /**
     * @brief Assigns an rvalue reference value to the parameter if no previous failures occurred.
     * 
     * Attempts to assign the provided rvalue reference value to the parameter. If there were no previous failures, 
     * the assignment is successful, and the function returns true. Otherwise, the function returns false.
     * @param val The rvalue reference value to be assigned.
     * @return true if the assignment is successful, false otherwise.
     */
    auto assign(value_type&& val) {
        if (fail()) {
            return false;
        }
        val_ = std::move(val);
        return true;
    }

    /**
     * @brief Checks if the parameter has encountered a failure during assignment.
     * 
     * This function checks if the parameter has encountered a failure during assignment.
     * @return true if a failure occurred, false otherwise.
     */
    auto fail() const noexcept {
        return fail_;
    }

    /**
     * @brief Resets the failure status of the parameter.
     * 
     * This function resets the failure status of the parameter, allowing subsequent assignments to be attempted.
     */
    void clear() noexcept {
        fail_ = false;
    }

    /**
     * @brief Outputs the parameter value or default value to the output stream.
     * 
     * @param os The output stream.
     * @param p The basic_cl_param object.
     * @return The output stream.
     */
    friend ostream_type& operator<<(
        ostream_type& os, const basic_cl_param& p
    ) {
        if (p.val_.has_value()) {
            os << p.val_.value();
        }
        else if (p.defval_.has_value()) {
            os << p.defval_.value();
        }
        else {
            // no value to output
        }

        return os;
    }

    /**
     * @brief Reads a parameter value from the input stream.
     * 
     * @param is The input stream.
     * @param p The basic_cl_param object.
     * @return The input stream.
     */
    friend istream_type& operator>>(
        istream_type& is, basic_cl_param& p
    ) {
        p.val_ = value_type();
        is >> p.val_.value();

        if (is.fail()) {
            // incompatible type received.
            p.fail_ = true;
            is.clear();
        }

        return is;
    }

private:
    key_container<char_type> key_chars_;
    key_container<string_view_type> key_strs_;
    std::optional<string_view_type> brief_;
    std::optional<value_type> defval_;
    std::optional<value_type> val_;
    bool fail_;
};

/**
 * @brief Represents an optional command-line parameter.
 * 
 * This class extends basic_cl_param to handle optional command-line parameters with specific key characters, key strings, and a brief description.
 * @tparam ValT The type of the parameter value.
 * @tparam CharT The character type used for keys (default is char).
 * @tparam Traits The character traits type (default is std::char_traits<CharT>).
 */
template <class ValT, clp_char CharT = char,
    class Traits = std::char_traits<CharT>
>
class basic_optional : public basic_cl_param<ValT, CharT, Traits> {
public:
    using value_type = ValT;
    using char_type = CharT;
    using traits_type = Traits;
    using string_view_type = std::basic_string_view<
        char_type, traits_type
    >;

    /**
     * @brief Constructs a basic_optional object with specified key characters, key strings, and brief description.
     * 
     * Initializes a basic_optional object with the specified key characters, key strings, and brief description.
     * @param key_chars An std::initalizer_list of single-character keys associated with the parameter.
     * @param key_strs An std::initalizer_list of string keys associated with the parameter.
     * @param brief A brief description of the parameter.
     */
    basic_optional(std::initializer_list<char_type> key_chars,
        std::initializer_list<string_view_type> key_strs,
        string_view_type brief
    ) : basic_cl_param<ValT, CharT, Traits>(key_chars, key_strs, brief) {}
};

/**
 * @brief Represents a required command-line parameter.
 * 
 * This class extends basic_cl_param to handle required command-line parameters with specific key characters, key strings, and a brief description.
 * @tparam ValT The type of the parameter value.
 * @tparam CharT The character type used for keys (default is char).
 * @tparam Traits The character traits type (default is std::char_traits<CharT>).
 */
template <class ValT, clp_char CharT = char,
    class Traits = std::char_traits<CharT>
>
class basic_required : public basic_cl_param<ValT, CharT, Traits> {
public:
    using value_type = ValT;
    using char_type = CharT;
    using traits_type = Traits;
    using string_view_type = std::basic_string_view<
        char_type, traits_type
    >;

    /**
     * @brief Constructs a basic_required object with specified key characters, key strings, and brief description.
     * 
     * Initializes a basic_required object with the specified key characters, key strings, and brief description.
     * @param key_chars An std::initializer_list of single-character keys associated with the parameter.
     * @param key_strs An std::initializer_list of string keys associated with the parameter.
     * @param brief A brief description of the parameter.
     */
    basic_required(std::initializer_list<char_type> key_chars,
        std::initializer_list<string_view_type> key_strs,
        string_view_type brief
    ) : basic_cl_param<ValT, CharT, Traits>(key_chars, key_strs, brief) {}
};

/**
 * @brief Enumeration representing error codes during command-line argument parsing.
 * 
 * This enumeration defines various error states that can occur during the parsing of command-line arguments.
 */
enum class error_code {
    invalid_identifier, ///< The provided identifier is invalid or malformed.
    key_not_given,      ///< A required key was not provided in the command-line arguments.
    undefined_key,      ///< An undefined or unexpected key was provided in the command-line arguments.
    unparsed_argument,  ///< An argument could not be parsed successfully.
    incompatible_argument, ///< The provided argument is incompatible with the associated key.
    wrong_complex_key,  ///< An incorrect complex key format was provided.
    required_key_not_given ///< A required key was not provided in the command-line arguments.
};

template <
    clp_char CharT = char,
    class Traits = std::char_traits<CharT>,
    class ... Params
>
class basic_cl_parser
{
public:
    using char_type = CharT;
    using traits_type = Traits;
    using string_type = std::basic_string<char_type, traits_type>;
    using string_view_type = std::basic_string_view<char_type, traits_type>;
    using result_tuple_type = std::tuple<typename Params::value_type...>;
    using param_tuple_type = std::tuple<Params...>;
    using index_type = int;
    using stream_type = std::basic_stringstream<char_type, traits_type>;

    /**
     * @brief Constructs a basic_cl_parser object with the provided identifier and parameters.
     * 
     * Initializes a basic_cl_parser object with the specified identifier and parameters.
     * @param identifier The identifier used to recognize the command-line arguments.
     * @param params The parameters representing the command-line arguments to be parsed.
     */
    basic_cl_parser(string_view_type identifier, Params... params)
        : arg_stream_(), err_stream_(), params_( std::move(params)... ),
            identifier_(identifier) {}

    /**
     * @brief Retrieves the error code representing the parsing status.
     * 
     * This function returns the error code representing the parsing status.
     * @return The error code indicating the parsing status.
     */
    auto error() const noexcept {
        return err_code_;
    }

    /**
     * @brief Retrieves the error message associated with the parsing error.
     * 
     * This function returns the error message associated with the parsing error.
     * @return The error message describing the parsing error.
     */
    auto error_message() const noexcept {
        return err_stream_.str();
    }

    /**
     * @brief Clears the parsing error status and streams for re-parsing.
     * 
     * This function clears the parsing error status and streams, allowing the parser to be used again for parsing.
     */
    void clear() {
        err_code_.reset();
        err_stream_.clear();
        err_stream_.ignore(
            std::numeric_limits<std::streamsize>::max()
        );
        arg_stream_.clear();
        arg_stream_.ignore(
            std::numeric_limits<std::streamsize>::max() 
        );
        detail::tuple_for_each( params_, [](auto& p) { p.clear(); } );
    }

    /**
     * @brief Parses command-line arguments provided as an array of strings.
     * 
     * Parses the command-line arguments provided as an array of strings, represented by argc and argv,
     * and returns a tuple containing the parsed values.
     * @tparam IntType The integral type for argc.
     * @tparam StrArrType The array type for argv.
     * @param argc The number of command-line arguments.
     * @param argv The array of command-line argument strings.
     * @return A tuple containing the parsed values.
     */
    template <class IntType, class StrArrType>
    auto parse(IntType argc, StrArrType argv)
        -> result_tuple_type {
        auto flattend = string_type();

        for (auto i = decltype(argc)(0); i < argc; ++i) {
            flattend += string_view_type(argv[i]);
            flattend += detail::stream_delim<char_type>();
        }

        return parse( string_view_type(flattend) );
    }

    /**
     * @brief Parses command-line arguments provided as a single string.
     * 
     * Parses the command-line arguments provided as a single string and returns a tuple containing the parsed values.
     * @param command_line The command-line arguments as a single string.
     * @return A tuple containing the parsed values.
     */
    auto parse(string_view_type command_line)
        -> result_tuple_type {
        result_tuple_type ret;

        if (error().has_value()) {
            return ret;
        }

        auto words = detail::split_words(command_line);
        if ( words.front() != identifier_ ) {
            log_error_invalid_identifier( words.front() );
            return ret;
        }

        auto it_first = std::next(std::begin(words));
        auto it_last = std::end(words);

        // check first word is key,
        if (!detail::is_key(*it_first)) {
            log_error_key_not_given();
            return ret;
        }

        while (it_first != it_last) {
            if (detail::is_complex_boolean_param(*it_first)) {
                if (!parse_complex_keys(*it_first)) {
                    log_error_wrong_complex_key();
                    return ret;
                }
                ++it_first;
                continue;
            }

            auto param_idx = find_param_index(*it_first);
            if (!param_idx.has_value()) {
                log_error_undefined_key(*it_first);
                return ret;
            }
    
            auto it_next_key = std::ranges::find_if(
                std::next(it_first), it_last,
                detail::is_key<string_view_type>
            );

            // assign (key, next key) words to param
            std::ranges::for_each(
                std::next(it_first),
                it_next_key,
                [&, this](auto word) {
                    arg_stream_ << word << detail::stream_delim<char_type>();
                }
            );
            if ( !assign_arg_by_idx( param_idx.value() ) ) {
                log_error_incompatible_argument(*it_first);
                return ret;
            }

            if ( has_unparsed_arguments() ) {
                log_error_unparsed_arguments();
                return ret;
            }

            it_first = it_next_key;
        }   // while (it_first != last)

        detail::tuple_conv(params_, ret, [this](const auto& p) {
            using none_qualified_t = std::remove_cvref_t<decltype(p)>;
            using value_type = typename none_qualified_t::value_type;

            if (p.has_value()) {
                return p.value();
            }

            if constexpr ( std::is_base_of_v<
                    basic_required<value_type, char_type, traits_type>,
                    none_qualified_t
                >
            ) {
                log_error_required_key_not_given();
            }

            return value_type{};
        });

        // TODO: check every required arguments are passed and all constraints are met

        return ret;
    }

private:
    /**
     * @brief Parses complex keys in the command line arguments.
     * 
     * Parses complex keys in the command line arguments, assigning corresponding boolean values to parameters.
     * @param word The complex key to parse.
     * @return False if parsing fails (at least one undefined key or incompatible argument), true otherwise.
     */
    auto parse_complex_keys(string_view_type word) {
        /*
        fail means at least one of the keys in complex param received
        isn't defined as boolean param.
        */ 
        auto fail = false;

        detail::tuple_for_each(
            params_,
            [ &fail, keys = detail::remove_dash(word) ](auto& p) mutable {
                for (auto key : keys) {
                    if (p.contains(key)) {
                        p.assign(true);
                        if (p.fail()) {
                            fail = true;
                        }
                    }
                    else {
                        fail = true;
                    }
                }
            }
        );

        return !fail;
    }

    /**
     * @brief Finds the index of the parameter corresponding to the given key.
     * 
     * Finds the index of the parameter corresponding to the given key in the list of parameters.
     * @param word The key to search for.
     * @return An optional index indicating the position of the parameter, or empty if the key is undefined.
     */
    auto find_param_index(string_view_type word) {
        std::optional<index_type> idx_found{};

        detail::tuple_for_each(
            params_,
            [ &idx_found, this, key = word, i = 0 ](
                const auto& p) mutable {
                auto dash_removed_key = detail::remove_dash(key);
                if ( detail::is_single_dashed(key) ) {
                    // short param
                    if ( p.contains(*std::begin(dash_removed_key)) ) {
                        idx_found = i;
                    }
                }
                else if ( p.contains(dash_removed_key) ) {
                    // long param
                    idx_found = i;
                }
                ++i;
            }
        );

        return idx_found;
    }

    /**
     * @brief Assigns command-line arguments to the parameter at the given index.
     * 
     * Assigns command-line arguments to the parameter at the specified index using the internal argument stream.
     * @param param_idx The index of the parameter to assign arguments to.
     * @return True if assignment is successful, false otherwise.
     */
    auto assign_arg_by_idx(index_type param_idx) {
        bool success = false;

        detail::tuple_for_each(
            params_,
            [ &success, this, param_idx, i = 0 ](auto& p) mutable {
                if (param_idx == i) {
                    this->arg_stream_ >> p;
                    success = !p.fail();
                }
                ++i;
            }
        );

        return success;
    }
    
    /**
     * @brief Generates a string representing the command-line arguments.
     * 
     * Generates a string representation of the command-line arguments for error messages.
     * @return A string containing formatted command-line arguments.
     */
    auto get_args_string() {
        auto args = string_type();

        auto view = std::ranges::basic_istream_view<
            string_type, char_type, traits_type
        >( arg_stream_ );

        for (const auto& s : view) {
            std::format_to( std::back_inserter(args),
                __LITERAL(char_type, "\"{}\" "),
                s
            );
        }

        args.pop_back();

        return args;
    }

    /**
     * @brief Logs an error for incompatible arguments received.
     * 
     * Logs an error message when incompatible arguments are received for a specified key.
     * @param key The key for which incompatible arguments were received.
     */
    void log_error_incompatible_argument(string_view_type key) {
        err_code_ = error_code::incompatible_argument;
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: received arguments are incompatible with"
            " the specified key \""
        ) << key << __LITERAL(char_type,
            "\".\n"
            "\treceived: ["
        ) << get_args_string() << __LITERAL(char_type,
            "]\n";
        );
    }

    /**
     * @brief Logs an error for an invalid identifier specified.
     * 
     * Logs an error message when an invalid identifier is specified.
     * @param received_identifier The invalid identifier received in the command-line arguments.
     */
    void log_error_invalid_identifier(string_view_type received_identifier) {
        err_code_ = error_code::invalid_identifier;
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: invalid identifier specified.\n"
            "\texpected \""
        ) << identifier_ << __LITERAL(char_type,
            "\" but received \""
        ) << received_identifier << __LITERAL(char_type,
            "\"\n"
        );
    }

    /**
     * @brief Logs an error for a key not given in the command-line arguments.
     * 
     * Logs an error message when a key is not given in the command-line arguments.
     */
    void log_error_key_not_given() {
        err_code_ = error_code::key_not_given;
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: key is not given.\n"
        );
    }

    /**
     * @brief Logs an error for an undefined key received in the command-line arguments.
     * 
     * Logs an error message when an undefined key is received in the command-line arguments.
     * @param key The undefined key received in the command-line arguments.
     */
    void log_error_undefined_key(string_view_type key) {
        err_code_ = error_code::undefined_key;
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: undefined key \""
        ) << key << __LITERAL(char_type,
            "\" received.\n"
        );
    }

    /**
     * @brief Checks if there are unparsed arguments remaining in the input stream.
     * 
     * Checks if there are unparsed arguments remaining in the input stream.
     * @return True if there are unparsed arguments, false otherwise.
     */
    auto has_unparsed_arguments() const {
        // if any character rather than eof remain in stream,
        // stream buffer size is greater than 1. (the one stands for eof.)
        return arg_stream_.rdbuf()->in_avail() > 1;
    }

    /**
     * @brief Logs an error for unparsed arguments detected in the input stream.
     * 
     * Logs an error message when unparsed arguments are detected in the input stream.
     */
    void log_error_unparsed_arguments() {
        err_code_ = error_code::unparsed_argument;
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: unparsed arguments detected.\n"
            "\tremaining tokens: "
        );
        err_stream_ << get_args_string();
        err_stream_ << __LITERAL(char_type,
            "\n"
        );
    }

    /**
     * @brief Logs an error for complex keys with undefined boolean parameters.
     * 
     * Logs an error message when at least one key in a complex parameter is not defined as a boolean parameter.
     */
    void log_error_wrong_complex_key() {
        err_code_ = error_code::incompatible_argument;
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: at least one of the keys in complex param received"
            " isn't defined as boolean param.\n"
            "\treceived: ["
        ) << get_args_string() << __LITERAL(char_type,
            "]\n"
        );
    }

    /**
     * @brief Logs an error for required keys not given in the command-line arguments.
     * 
     * Logs an error message when required keys are not given in the command-line arguments.
     */
    void log_error_required_key_not_given() {
        err_code_ = error_code::required_key_not_given;
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: required keys are not given.\n"
            "required keys:\n"
        );

        auto print_keys_to_err = [this](auto&& keys) {
            std::ranges::for_each(
                std::begin(keys), std::prev(std::end(keys)),
                [this](const auto& key) {
                    err_stream_ << key << __LITERAL(char_type, "|");
                }
            );

            err_stream_ << *std::prev(std::end(keys));
        };

        detail::tuple_for_each(params_,
            [print_keys_to_err, this](const auto& p) {
                using none_qualified_t
                    = std::remove_cvref_t<decltype(p)>;
                using value_type = typename none_qualified_t::value_type;

                if constexpr ( std::is_base_of_v<
                        basic_required<value_type, char_type, traits_type>,
                        none_qualified_t
                    >
                ) {
                    err_stream_ << __LITERAL(char_type, "\t[");
                    print_keys_to_err(p.key_chars());
                    err_stream_ << __LITERAL(char_type, "|");
                    print_keys_to_err(p.key_strs());
                    err_stream_ << __LITERAL(char_type, "]: ")
                        << p.brief_message() << __LITERAL(char_type, "\n");
                }
            }
        );
    }

    stream_type arg_stream_;
    stream_type err_stream_;
    param_tuple_type params_;
    string_view_type identifier_;
    std::optional<error_code> err_code_;
};

/**
 * @brief Shortcut type alias for optional command-line parameter with char character type.
 * 
 * Represents an optional command-line parameter with char character type.
 * @tparam ValT The value type of the parameter.
 */
template <class ValT>
using optional = basic_optional<ValT, char, std::char_traits<char>>;

/**
 * @brief Shortcut type alias for optional command-line parameter with wchar_t character type.
 * 
 * Represents an optional command-line parameter with wchar_t character type.
 * @tparam ValT The value type of the parameter.
 */
template <class ValT>
using woptional = basic_optional<ValT, wchar_t, std::char_traits<wchar_t>>;

/**
 * @brief Shortcut type alias for required command-line parameter with char character type.
 * 
 * Represents a required command-line parameter with char character type.
 * @tparam ValT The value type of the parameter.
 */
template <class ValT>
using required = basic_required<ValT, char, std::char_traits<char>>;

/**
 * @brief Shortcut type alias for required command-line parameter with wchar_t character type.
 * 
 * Represents a required command-line parameter with wchar_t character type.
 * @tparam ValT The value type of the parameter.
 */
template <class ValT>
using wrequired = basic_required<ValT, wchar_t, std::char_traits<wchar_t>>;

/**
 * @brief Shortcut type alias for command-line parser with char character type.
 * 
 * Represents a command-line parser with char character type.
 * @tparam Params Parameter types to be parsed by the parser.
 */
template <class ... Params>
using parser = basic_cl_parser<char, std::char_traits<char>, Params...>;

/**
 * @brief Shortcut type alias for command-line parser with wchar_t character type.
 * 
 * Represents a command-line parser with wchar_t character type.
 * @tparam Params Parameter types to be parsed by the parser.
 */
template <class ... Params>
using wparser = basic_cl_parser<wchar_t, std::char_traits<wchar_t>, Params...>;

}   // namespace clp

#undef __LITERAL

#endif  // __cl_parser