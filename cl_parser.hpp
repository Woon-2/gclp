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

template <class T, class ... Args>
concept contains = (std::is_same_v<T, Args> || ...);

template <class>
struct is_char : std::false_type
{};

template <class T>
requires contains<T, char, wchar_t, char8_t, char16_t, char32_t,
    signed char, unsigned char>
struct is_char<T> : std::true_type
{};
    
template <class T>
inline constexpr auto is_char_v = is_char<T>::value;

template <class From, class To>
concept not_convertible_to = !std::is_convertible_v<From, To>;

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

template <class CharT>
constexpr auto dash() {
    return CharT('-');
}

template <class CharT>
constexpr auto char_escape() {
    return CharT('\\');
}

template <class CharT>
constexpr auto stream_delim() {
    return CharT(' ');
}

template <class CharT>
constexpr auto char_assign() {
    return CharT('=');
}

template <class CharT>
constexpr auto single_quote() {
    return CharT('\'');
}

template <class CharT>
constexpr auto double_quote() {
    return CharT('\"');
}

template <class StringView>
auto is_single_dashed(StringView word) noexcept {
    using char_type = typename StringView::value_type;

    return std::size(word) > std::size_t(1)
        && word[0] == dash<char_type>()
        && word[1] != dash<char_type>();
}

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

template <class StringView>
auto is_complex_boolean_param(StringView word) noexcept {
    return is_single_dashed(word) && std::size(word) > 2;
}

template <class StringView>
auto remove_dash(StringView s) noexcept {
    return StringView(
        s.begin() + s.find_first_not_of('-'),
        s.end()
    );
}

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

template <class T>
concept clp_char = detail::is_char_v<T>;

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
    basic_cl_param(
        std::initializer_list<char_type> key_chars,
        std::initializer_list<string_view_type> key_strs,
        string_view_type brief
    ) : key_chars_(key_chars), key_strs_(key_strs),
        brief_(brief), defval_(), val_(), fail_(false) {}

    basic_cl_param(
        std::initializer_list<char_type> key_chars,
        std::initializer_list<string_view_type> key_strs,
        const value_type& defval,
        string_view_type brief
    ) : key_chars_(key_chars), key_strs_(key_strs),
        brief_(brief), defval_(defval), val_(), fail_(false) {}

    basic_cl_param(
        std::initializer_list<char_type> key_chars,
        std::initializer_list<string_view_type> key_strs,
        value_type&& defval,
        string_view_type brief
    ) : key_chars_(key_chars), key_strs_(key_strs),
        brief_(brief), defval_( std::move(defval) ), val_(), fail_(false) {}

    auto has_value() const noexcept {
        return val_.has_value() || defval_.has_value();
    }

    auto& value() noexcept {
        // assert(val_.has_value(), defval_.has_value());

        if (val_.has_value()) {
            return val_.value();
        }
        if (defval_.has_value()) {
            return defval_.value();
        }
        std::terminate();
    }

    const auto& value() const noexcept {
        // assert(val_.has_value(), defval_.has_value());

        if (val_.has_value()) {
            return val_.value();
        }
        if (defval_.has_value()) {
            return defval_.value();
        }
        std::terminate();
    }

    auto contains(char_type key_char) const {
        return std::ranges::find(key_chars_, key_char) != std::end(key_chars_);
    }
    
    auto contains(string_view_type key_string) const {
        return std::ranges::find(key_strs_, key_string) != std::end(key_strs_);
    }
    
    template <std::input_iterator It>
        requires requires(It a, It b) {
            string_view_type(a, b);
        }
    auto contains(It first, It last) const {
        return std::ranges::find(
                key_strs_, string_view_type(first, last)
            ) != std::end(key_strs_);
    }
    
    template <std::ranges::range R>
       requires std::convertible_to<R, string_view_type>
    auto contains(R&& r) const {
        return contains(
            static_cast<string_view_type>( std::forward<R>(r) )
        );
    }

    const auto& key_chars() const noexcept {
        return key_chars_;
    }

    const auto& key_strs() const noexcept {
        return key_strs_;
    }

    auto has_brief_message() const noexcept {
        return brief_.has_value();
    }

    const auto brief_message() const noexcept {
        if (brief_.has_value()) {
            return brief_.value();
        }

        std::terminate();
    }

    template <class InCompatible>
        requires detail::not_convertible_to< 
            std::remove_cvref_t<InCompatible>,
            value_type
        >
    auto assign(InCompatible&& arg) {
        fail_ = true;
        return false;
    }

    auto assign(const value_type& val) {
        if (fail()) {
            return false;
        }
        val_ = val;
        return true;
    }

    auto assign(value_type&& val) {
        if (fail()) {
            return false;
        }
        val_ = std::move(val);
        return true;
    }

    auto fail() const noexcept {
        return fail_;
    }

    void clear() noexcept {
        fail_ = false;
    }

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
    basic_optional(std::initializer_list<char_type> key_chars,
        std::initializer_list<string_view_type> key_strs,
        string_view_type brief
    ) : basic_cl_param<ValT, CharT, Traits>(key_chars, key_strs, brief) {}
};

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
    basic_required(std::initializer_list<char_type> key_chars,
        std::initializer_list<string_view_type> key_strs,
        string_view_type brief
    ) : basic_cl_param<ValT, CharT, Traits>(key_chars, key_strs, brief) {}
};

enum class error_code {
    invalid_identifier,
    key_not_given,
    undefined_key,
    unparsed_argument,
    incompatible_argument,
    wrong_complex_key,
    required_key_not_given
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

    basic_cl_parser(string_view_type identifier, Params... params)
        : arg_stream_(), err_stream_(), params_( std::move(params)... ),
            identifier_(identifier) {}

    auto error() const noexcept {
        return err_code_;
    }

    auto error_message() const noexcept {
        return err_stream_.str();
    }

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

        // check every required arguments are passed and all constraints are met

        return ret;
    }

private:
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

        return fail;
    }

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

    void log_error_key_not_given() {
        err_code_ = error_code::key_not_given;
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: key is not given.\n"
        );
    }

    void log_error_undefined_key(string_view_type key) {
        err_code_ = error_code::undefined_key;
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: undefined key \""
        ) << key << __LITERAL(char_type,
            "\" received.\n"
        );
    }

    auto has_unparsed_arguments() const {
        // if any character rather than eof remain in stream,
        // stream buffer size is greater than 1. (the one stands for eof.)
        return arg_stream_.rdbuf()->in_avail() > 1;
    }

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

template <class ValT>
using optional = basic_optional<ValT, char, std::char_traits<char>>;
template <class ValT>
using woptional = basic_optional<ValT, wchar_t, std::char_traits<wchar_t>>;
template <class ValT>
using required = basic_required<ValT, char, std::char_traits<char>>;
template <class ValT>
using wrequired = basic_required<ValT, wchar_t, std::char_traits<wchar_t>>;
template <class ... Params>
using parser = basic_cl_parser<char, std::char_traits<char>, Params...>;
template <class ... Params>
using wparser = basic_cl_parser<char, std::char_traits<char>, Params...>;

}   // namespace clp

#undef __LITERAL

#endif  // __cl_parser