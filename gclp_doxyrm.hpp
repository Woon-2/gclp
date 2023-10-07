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
#include <bitset>
#include <ios>


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


#define DEFINE_ENUM_LOGICAL_OP_ALL(Enum_t)    \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, bool)    \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, char) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, signed char) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, unsigned char) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, short) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, unsigned short) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, int) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, unsigned) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, long) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, unsigned long) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, long long) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, unsigned long long)  \
    DEFINE_ENUM_BINARY_LOGICAL_OP(Enum_t, Enum_t)   \
    DEFINE_UNARY_OP(Enum_t, !, [](auto arg) { return !static_cast< std::underlying_type_t<Enum_t> >(arg); })


#define DEFINE_ENUM_COMPARE_OP_ALL(Enum_t)    \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, char) \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, signed char) \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, unsigned char)   \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, short) \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, unsigned short)  \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, int) \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, unsigned int)    \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, long) \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, unsigned long)   \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, long long)   \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, unsigned long long)  \
    DEFINE_ENUM_BINARY_COMPARE_OP(Enum_t, Enum_t)


#define DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, int_t) \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, &, [](auto lhs, auto rhs) { return lhs & rhs; })  \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, |, [](auto lhs, auto rhs) { return lhs | rhs; })  \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, &&, [](auto lhs, auto rhs) { return lhs && rhs; })    \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, ||, [](auto lhs, auto rhs) { return lhs || rhs; })
    

#define DEFINE_ENUM_BINARY_LOGICAL_OP(Enum1_t, Enum2_t) \
    DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, &, [](auto lhs, auto rhs) { return lhs & rhs; })    \
    DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, |, [](auto lhs, auto rhs) { return lhs | rhs; })    \
    DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, &&, [](auto lhs, auto rhs) { return lhs && rhs; })    \
    DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, ||, [](auto lhs, auto rhs) { return lhs || rhs; })    \


#define DEFINE_ENUM_INT_COMPARE_OP(Enum_t, int_t)   \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, <=>, [](auto lhs, auto rhs) { return lhs <=> rhs; })


#define DEFINE_ENUM_BINARY_COMPARE_OP(Enum1_t, Enum2_t) \
    DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, <=>, [](auto lhs, auto rhs) { return lhs <=> rhs; })


#define DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, opSymbol, opIntFunc)  \
    DEFINE_BINARY_OP(Enum1_t, Enum2_t, opSymbol,  \
        [](auto e1, auto e2) { return opIntFunc( static_cast< std::underlying_type_t<Enum1_t> >(e1),  \
            static_cast< std::underlying_type_t<Enum2_t> >(e2)    \
        ); } \
    )


#define DEFINE_ENUM_INT_OP(Enum_t, int_t, opSymbol, opIntFunc)   \
    DEFINE_BINARY_OP(Enum_t, int_t, opSymbol,   \
        [](auto e, auto i) { return opIntFunc( static_cast<decltype(i)>(e), i ); } \
    )   \
    DEFINE_BINARY_OP(int_t, Enum_t, opSymbol,    \
        [](auto i, auto e) { return opIntFunc( i, static_cast<decltype(i)>(e) ); } \
    )


#define DEFINE_BINARY_OP(Lhs_t, Rhs_t, opSymbol, opFunc)    \
    static decltype(auto) operator opSymbol (Lhs_t lhs, Rhs_t rhs)   \
        noexcept( noexcept(opFunc(  \
            std::forward<Lhs_t>(lhs),   \
            std::forward<Rhs_t>(rhs)    \
        )) ) {  \
        return opFunc( std::forward<Lhs_t>(lhs),    \
            std::forward<Rhs_t>(rhs) ); \
    }


#define DEFINE_UNARY_OP(Arg_t, opSymbol, opFunc) \
    static decltype(auto) operator opSymbol (Arg_t arg)    \
        noexcept( noexcept( opFunc( \
            std::forward<Arg_t>(arg)    \
        )) ) {  \
        return opFunc( std::forward<Arg_t>(arg) );  \
    }


namespace gclp {

#ifndef DOXYGEN_IGNORE_DETAIL

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
inline constexpr bool is_char_v = is_char<T>::value;


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
constexpr CharT dash() {
    return CharT('-');
}


template <class CharT>
constexpr CharT char_escape() {
    return CharT('\\');
}


template <class CharT>
constexpr CharT stream_delim() {
    return CharT(' ');
}


template <class CharT>
constexpr CharT char_assign() {
    return CharT('=');
}


template <class CharT>
constexpr CharT single_quote() {
    return CharT('\'');
}


template <class CharT>
constexpr CharT double_quote() {
    return CharT('\"');
}


template <class StringView>
bool is_single_dashed(StringView word) noexcept {
    using char_type = typename StringView::value_type;

    return std::size(word) > std::size_t(1)
        && word[0] == dash<char_type>()
        && word[1] != dash<char_type>();
}


template <class StringView>
bool is_key(StringView word) noexcept {
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
bool is_complex_key(StringView word) noexcept {
    return is_single_dashed(word) && std::size(word) > 2;
}


template <class StringView>
StringView remove_dash(StringView s) noexcept {
    return StringView(
        s.begin() + s.find_first_not_of('-'),
        s.end()
    );
}


template <class StringView>
std::vector<StringView> split_words(StringView s) {
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


template <class Param>
class default_value_adaptor {
public:
    using parent_type = Param;  ///< Alias for the parent parameter type.
    using value_type = typename parent_type::value_type;    ///< Alias for the value type of the parent parameter.

    
    default_value_adaptor(parent_type* parent)
        : parent_(parent) {}

    
    parent_type& defval(const value_type& val) {
        parent_->set_defval(val);
        return *parent_;
    }

    
    parent_type& defval(value_type&& val) {
        parent_->set_defval( std::move(val) );
        return *parent_;
    }

private:
    parent_type* parent_;
};

}   // namespace gclp::detail
#endif   // DOXYGEN_IGNORE_DETAIL


template <class T>
concept gclp_char = detail::is_char_v<T>;


template <
    class ValT, gclp_char CharT = char,
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
        std::initializer_list<char_type> short_keys,
        std::initializer_list<string_view_type> long_keys,
        string_view_type brief
    ) : short_keys_(short_keys), long_keys_(long_keys),
        brief_(brief), defval_(), val_(), fail_(false) {}

    
    bool has_value() const noexcept {
        return val_.has_value() || defval_.has_value();
    }

    
    value_type& value() noexcept {
        assert(has_value());

        if (val_.has_value()) {
            return val_.value();
        }
        return defval_.value();
    }

    
    const value_type& value() const noexcept {
        assert(has_value());

        if (val_.has_value()) {
            return val_.value();
        }
        return defval_.value();
    }

    
    void remove_value() noexcept {
        val_.reset();
    }

    
    bool contains(char_type short_key) const {
        return std::ranges::find(short_keys_, short_key) != std::end(short_keys_);
    }
    
    
    bool contains(string_view_type long_key) const {
        return std::ranges::find(long_keys_, long_key) != std::end(long_keys_);
    }
    
    
    template <std::input_iterator It>
        requires requires(It a, It b) {
            string_view_type(a, b);
        }
    bool contains(It first, It last) const {
        return std::ranges::find(
                long_keys_, string_view_type(first, last)
            ) != std::end(long_keys_);
    }
    
    
    template <std::ranges::range R>
       requires std::convertible_to<R, string_view_type>
    bool contains(R&& r) const {
        return contains(
            static_cast<string_view_type>( std::forward<R>(r) )
        );
    }

    
    const key_container<char_type>& short_keys() const noexcept {
        return short_keys_;
    }

    
    const key_container<string_view_type>& long_keys() const noexcept {
        return long_keys_;
    }

    
    bool has_brief_message() const noexcept {
        return brief_.has_value();
    }

    
    const string_view_type brief_message() const noexcept {
        assert(has_brief_message());
        return brief_.value();
    }

    
    template <class InCompatible>
        requires detail::not_convertible_to< 
            std::remove_cvref_t<InCompatible>,
            value_type
        >
    bool assign(InCompatible&& arg) {
        fail_ = true;
        return false;
    }

    
    bool assign(const value_type& val) {
        if (fail()) {
            return false;
        }
        val_ = val;
        return true;
    }

    
    bool assign(value_type&& val) {
        if (fail()) {
            return false;
        }
        val_ = std::move(val);
        return true;
    }

    
    bool fail() const noexcept {
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

    
    void set_defval(const value_type& val) {
        defval_ = val;
    }

    
    void set_defval(value_type&& val) {
        defval_ = std::move(val);
    }

    
    bool has_defval() const noexcept {
        return defval_.has_value();
    }

    
    const value_type& get_defval() const {
        assert(has_defval());
        return defval_.value();
    }

    
    void remove_defval() noexcept {
        defval_.reset();
    }

private:
    key_container<char_type> short_keys_;
    key_container<string_view_type> long_keys_;
    std::optional<string_view_type> brief_;
    std::optional<value_type> defval_;
    std::optional<value_type> val_;
    bool fail_;
};


template <class ValT, gclp_char CharT = char,
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

private:
    using defval_adaptor_type = detail::default_value_adaptor<
        basic_optional<value_type, char_type, traits_type>
    >;
    friend defval_adaptor_type;

public:
    
    basic_optional(std::initializer_list<char_type> short_keys,
        std::initializer_list<string_view_type> long_keys,
        string_view_type brief
    ) : basic_cl_param<ValT, CharT, Traits>(short_keys, long_keys, brief),
        defval_adaptor_(this) {}

    
    defval_adaptor_type* operator->() && noexcept {
        return &defval_adaptor_;
    }

private:
    defval_adaptor_type defval_adaptor_;
};


template <class ValT, gclp_char CharT = char,
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

private:
    using defval_adaptor_type = detail::default_value_adaptor<
        basic_required<value_type, char_type, traits_type>
    >;
    friend defval_adaptor_type;

public:
    
    basic_required(std::initializer_list<char_type> short_keys,
        std::initializer_list<string_view_type> long_keys,
        string_view_type brief
    ) : basic_cl_param<ValT, CharT, Traits>(short_keys, long_keys, brief),
        defval_adaptor_(this) {}

    
    defval_adaptor_type* operator->() && noexcept {
        return &defval_adaptor_;
    }

private:
    defval_adaptor_type defval_adaptor_;
};


enum class error_code {
    identifier_not_given,   ///< The identifier was not provided, i.e. the command line is empty.
    invalid_identifier, ///< The provided identifier is invalid or malformed.
    key_not_given,      ///< A required key was not provided in the command-line arguments.
    undefined_key,      ///< An undefined or unexpected key was provided in the command-line arguments.
    unparsed_argument,  ///< An argument could not be parsed successfully.
    incompatible_argument, ///< The provided argument is incompatible with the associated key.
    wrong_complex_key,  ///< An incorrect complex key format was provided.
    required_key_not_given, ///< A required key was not provided in the command-line arguments.
    duplicated_assignments  ///< More then one of the keys are assigning value to the same parameter.
};

DEFINE_ENUM_LOGICAL_OP_ALL(error_code)
DEFINE_ENUM_COMPARE_OP_ALL(error_code)


template <
    class CharT = char,
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
    using words_type = std::vector<string_view_type>;   

private:


class container {
public:
    
    container(Params&& ... params)
        : data_( std::move(params)... ),
        cached_values_() {}

    
    param_tuple_type& data() noexcept {
        return data_;
    }

    
    const param_tuple_type& data() const noexcept {
        return data_;
    }

    
    result_tuple_type& values() {
        if (!cached_values_.has_value()) {
            update_cache();
        }
        return cached_values_.value();
    }

    
    const result_tuple_type& values() const {
        if (!cached_values_.has_value()) {
            update_cache();
        }
        return cached_values_.value();
    }

    
    void clear() {
        detail::tuple_for_each(data_,
            [](auto& p) {
                p.clear();
            }
        );
        invalidate_cache();
    }

    
    void invalidate_cache() {
        cached_values_.reset();
    }

    
    void update_cache() {
        cached_values_ = result_tuple_type();
        detail::tuple_conv(data_, cached_values_.value(),
            [this](const auto& p) {
                using none_qualified_t
                    = std::remove_cvref_t<decltype(p)>;
                using value_type
                    = typename none_qualified_t::value_type;

                if (p.has_value()) {
                    return p.value();
                }

                return value_type{};
            }
        );
    }

private:
    param_tuple_type data_; 
    mutable std::optional<result_tuple_type> cached_values_; 
};  // class basic_cl_param::container


class interpreter {
public:
    using iterator = std::ranges::iterator_t<words_type>;   

    
    struct token {
        string_view_type leading; 
        words_type followings; 
    };

    interpreter() = default;

    
    template <class IntType, class StrArrType>
    interpreter( IntType argc, const StrArrType& argv )
        : words_( detail::split_words<string_view_type>(
                flatten_argc_argv(argc, argv)
            )
        ), cur_( std::begin(words_) ) {}

    
    interpreter(string_view_type command_line)
        : words_( detail::split_words(command_line) ),
        cur_( std::begin(words_) ) {}

    
    token get_token() {
        auto ret = token();
        if ( done() ) {
            return ret;
        }

        ret.leading = read();
        auto out = std::back_inserter(ret.followings);

        while ( !(done() || facing_key()) ) {
            out = read();
        }
        
        return ret;
    }

    
    void reset() {
        cur_ = std::begin(words_);
    }

    
    template <class IntType, class StrArrType>
    void reset(IntType argc, const StrArrType& argv) {
        reset( flatten_argc_argv(argc, argv) );
    }

    
    void reset(string_view_type cmd) {
        words_ = detail::split_words(cmd);
        cur_ = std::begin(words_);
    }

    
    bool done() const noexcept {
        return cur_ == std::end(words_);
    }

    
    bool facing_key() const {
        return detail::is_key(*cur_);
    }

    
    std::size_t remainder_count() const noexcept {
        return std::distance( cur_, std::end(words_) );
    }

    
    string_view_type remove_dash(string_view_type word) const {
        return detail::remove_dash(word);
    }

private:
    
    template <class IntType, class StrArrType>
    static string_type flatten_argc_argv(
        IntType argc, const StrArrType& argv
    ) {
        auto flattend = string_type();

        for (auto i = decltype(argc)(0); i < argc; ++i) {
            flattend += string_view_type(argv[i]);
            flattend += detail::stream_delim<char_type>();
        }

        return flattend;
    }

    
    string_view_type read() {
        return *cur_++;
    }

    words_type words_;
    iterator cur_;
};  // class basic_cl_parser::interpreter


class verifier {
public:
    using bitset_type = std::bitset<
        std::tuple_size_v<param_tuple_type>
    >;  

    
    verifier(string_view_type identifier)
        : identifier_(identifier), switches_(0u),
        fail_(false), bad_(false) {}

    
    string_view_type id() const noexcept {
        return identifier_;
    }

    
    bool is_valid_identifier(string_view_type word)
        const noexcept {
        return word == identifier_;
    }

    
    bool is_duplicated_assignment(
        char_type short_key, const container& params
    ) const {
        return is_duplicated_assignment_impl(short_key, params);
    }

    bool is_duplicated_assignment( string_view_type long_key,
        const container& params
    ) const {
        return is_duplicated_assignment_impl(long_key, params);
    }

    
    bool is_duplicated_complex_assignment(
        string_view_type keys, const container& params
    ) const {
        auto tmp_switches = bitset_type(0u);
        return std::ranges::any_of(keys,
            [&params, &tmp_switches, this](auto key) {
                auto is = this->is_duplicated_assignment_impl(
                    key, params, tmp_switches
                );

                this->set_assigned_impl(
                    key, params, tmp_switches
                );

                return is;
            }
        );
    }

    
    bool starts_with_short_key(
        const typename interpreter::token& token
    ) const {
        return detail::is_key( token.leading )
            && detail::is_single_dashed( token.leading )
            && !detail::is_complex_key( token.leading );
    }

    
    bool starts_with_long_key(
        const typename interpreter::token& token
    ) const {
        return detail::is_key( token.leading )
            && !detail::is_single_dashed( token.leading );
    }

    
    bool starts_with_complex_key(
        const typename interpreter::token& token
    ) const {
        return detail::is_complex_key( token.leading );
    }

    
    bool is_valid_single_key(char_type short_key,
        const container& params
    ) const {
        return is_valid_single_key_impl(short_key, params);
    }

    
    bool is_valid_single_key(string_view_type long_key,
        const container& params
    ) const {
        return is_valid_single_key_impl(long_key, params);
    }

    
    bool is_valid_complex_key(string_view_type complex_key,
        const container& params
    ) const {
        for (auto key : complex_key) {
            auto is_valid = false;

            auto is_boolean = [](const auto& p) {
                using value_type = typename
                    std::remove_cvref_t<decltype(p)>::value_type;

                return std::is_same_v<bool, value_type>
                    || std::is_base_of_v<bool, value_type>;
            };

            detail::tuple_for_each(params.data(),
                [&is_valid, key, is_boolean](const auto& p) {
                    if ( p.contains(key) && is_boolean(p) ) {
                        is_valid = true;
                    }
                }
            );

            if (!is_valid) {
                return false;
            }
        }

        return true;
    }

    
    bool satisfies_required(const container& params) const {
        auto required_switches = bitset_type(0u);
        detail::tuple_for_each(params.data(),
            [&required_switches, i = 0](const auto& p) mutable {
                using none_qualified_t
                    = std::remove_cvref_t<decltype(p)>;
                using value_type = typename
                    none_qualified_t::value_type;
                using required_t = basic_required<
                    value_type, char_type, traits_type
                >;

                if constexpr ( std::is_base_of_v<
                    required_t, none_qualified_t
                >) {
                    if (!p.has_defval()) {
                        required_switches.set(i);
                    }
                }
                ++i;
            }
        );

        return (required_switches & switches_)
            == required_switches;
    }

    
    void set_assigned(char_type short_key,
        const container& params
    ) {
        set_assigned_impl(short_key, params);
    }

    
    void set_assigned(string_view_type long_key,
        const container& params
    ) {
        set_assigned_impl(long_key, params);
    }

    
    bool assigned(char_type short_key,
        const container& params
    ) {
        return assigned_impl(short_key, params);
    }

    
    bool assigned(string_view_type long_key,
        const container& params
    ) {
        return assigned_impl(long_key, params);
    }

    
    void mark_fail( bool fail = true ) {
        fail_ = fail;
    }

    
    void mark_bad( bool bad = true ) {
        bad_ = bad;
    }

    
    bool good() const noexcept {
        return !(bad_ || fail_);
    }

    
    bool bad() const noexcept {
        return bad_;
    }

    
    bool fail() const noexcept {
        return fail_;
    }

    
    void clear() {
        switches_.reset();
        mark_fail(false);
        mark_bad(false);
    }

private:
    
    template <class KeyT>
    bool is_duplicated_assignment_impl(KeyT key,
        const container& params
    ) const {
        return is_duplicated_assignment_impl(
            key, params, this->switches_
        );
    }

    
    template <class KeyT>
    bool is_duplicated_assignment_impl(KeyT key,
        const container& params, const bitset_type& switches
    ) const {
        bool is = false;

        detail::tuple_for_each(
            params.data(),
            [&is, switches, key, i = 0](const auto& p) mutable {
                if (p.contains(key)) {
                    is = switches.test(i);
                }

                ++i;
            }
        );

        return is;
    }

    
    template <class KeyT>
    bool is_valid_single_key_impl(KeyT key,
        const container& params
    ) const {
        bool is = false;

        detail::tuple_for_each(params.data(),
            [&is, key](const auto& p) {
                if (p.contains(key)) {
                    is = true;
                }
            }
        );

        return is;
    }

    
    template <class KeyT>
    void set_assigned_impl(KeyT key, const container& params) {
        return set_assigned_impl(key, params, this->switches_);
    }

    
    template <class KeyT>
    void set_assigned_impl(KeyT key, const container& params,
        bitset_type& switches
    ) const {
        detail::tuple_for_each(
            params.data(),
            [&switches, key, i = 0](const auto& p) mutable {
                if (p.contains(key)) {
                    switches.set(i);
                }
                ++i;
            }
        );
    }

    
    template <class KeyT>
    bool assigned_impl(KeyT key,
        const container& params
    ) {
        bool ass = false;
        
        detail::tuple_for_each(
            params.data(),
            [&ass, this, key, i = 0](const auto& p) mutable {
                ass = p.contains(key) &&
                    this->switches_.test(i);

                ++i;
            }
        );

        return ass;
    }

    string_view_type identifier_;
    bitset_type switches_;
    bool fail_;
    bool bad_;
};  // class basic_cl_parser::verifier


class assigner {
public:
    
    assigner(container& params)
        : params_(params), stream_() {}

    
    void assign_complex(
        string_view_type complex_key, verifier& veri
    ) {
        /*
        fail means at least one of the keys in complex param received
        isn't defined as boolean param.
        */ 
        for (auto key : complex_key) {
            auto key_found = false;

            detail::tuple_for_each(
                params_.data(),
                [ &key_found, &veri, key ](auto& p) {
                    if (p.contains(key)) {
                        key_found = true;
                        p.assign(true);
                        veri.mark_fail( p.fail() );
                    }
                }
            );

            if (!key_found) {
                veri.mark_fail();
            }
            else {
                veri.set_assigned(key, params_);
            }
        }

        if ( veri.good() ) {
            params_.invalidate_cache();
        }
        else {
            // do not override error source.
            // (if fail, has_unassigned() is always true
            // but it's just the consequence of failure)
            veri.mark_bad( has_unassigned() );
        }
    }

    
    void assign_single(char_type short_key,
        const words_type& args, verifier& veri
    ) {
        assign_by_idx( getidx(short_key), args, veri );
        veri.set_assigned(short_key, params_);

        if ( veri.good() ) {
            params_.invalidate_cache();
        }
        else {
            // do not override error source.
            // (if fail, has_unassigned() is always true
            // but it's just the consequence of failure)
            veri.mark_bad( has_unassigned() );
        }
    }

    
    void assign_single(string_view_type long_key,
        const words_type& args, verifier& veri
    ) {
        assign_by_idx( getidx(long_key), args, veri );
        veri.set_assigned(long_key, params_);

        if ( veri.good() ) {
            params_.invalidate_cache();
        }
        else {
            // do not override error source.
            // (if fail, has_unassigned() is always true
            // but it's just the consequence of failure)
            veri.mark_bad( has_unassigned() );
        }
    }

    
    bool has_unassigned() const {
        // if any character rather than eof remain in stream,
        // stream buffer size is greater than 1. (the one stands for eof.)
        return stream_.rdbuf()->in_avail() > 1ull;
    }

    
    string_type get_unassigned() {
        auto args = string_type();

        auto view = std::ranges::basic_istream_view<
            string_type, char_type, traits_type
        >( stream_ );

        for (const auto& s : view) {
            std::format_to( std::back_inserter(args),
                __LITERAL(char_type, "\"{}\" "),
                s
            );
        }

        if (!args.empty()) {
            args.pop_back();
        }

        clear();

        return args;
    }

    
    void clear() {
        stream_.clear();
        if (stream_.rdbuf()->in_avail() > 1ull) {
            stream_.ignore(
                std::numeric_limits<std::streamsize>::max()
            );
        }
    }

private:
    
    void assign_by_idx(index_type idx,
        const words_type& args, verifier& veri
    ) {
        detail::tuple_for_each(
            params_.data(),
            [&args, &veri, idx, this, i = 0](auto& p) mutable {
                using value_type = typename std::remove_cvref_t<decltype(p)>
                    ::value_type;

                if (idx == i) {
                    // when extracting string_type,
                    // all characters in the stream regardless of white space should be extracted.
                    if constexpr (std::is_base_of_v<string_type, value_type>) {
                        auto acc = string_type();
                        for (auto&& s : std::views::istream<string_type>(stream_args(args))) {
                            acc += s;
                            acc += detail::stream_delim<char_type>();
                        }
                        acc.pop_back();
                        p.assign( std::move(acc) );
                        stream_.clear();
                    }
                    // when extracting boolean type,
                    // omitting argument means the value is true
                    else if constexpr (std::is_base_of_v<bool, value_type>
                        || std::is_same_v<bool, value_type>
                    ) {
                        if (std::empty(args)) {
                            p.assign(true);
                        }
                        else {
                            // try boolalpha and integral
                            stream_args(args);
                            stream_ >> std::boolalpha >> p;
                            if (p.fail()) {
                                p.clear();
                                stream_ >> std::noboolalpha >> p;
                            }
                            stream_ >> std::noboolalpha;
                        }
                    }
                    else {
                        stream_args(args) >> p;
                    }

                    veri.mark_fail( p.fail() );
                }
                ++i;
            }
        );
    }

    
    index_type getidx(char_type short_key) const {
        return getidx_impl(short_key);
    }

    
    index_type getidx(string_view_type long_key) const {
        return getidx_impl(long_key);
    }

    template <class KeyT>
    index_type getidx_impl(KeyT key) const {
        // assume the key's validity is already checked.
        auto ret = index_type(0);

        detail::tuple_for_each(
            params_.data(),
            [&ret, key, i = index_type(0)](const auto& p) mutable {
                if (p.contains(key)) {
                    ret = i;
                }
                ++i;
            }
        );

        return ret;
    }

    
    stream_type& stream_args(const words_type& args) {
        std::ranges::for_each(args, [this](const auto& arg) {
            stream_ << arg << detail::stream_delim<char_type>();
        });

        return stream_;
    }

    container& params_; 
    stream_type stream_; 
};  // class basic_cl_parser::assigner


class logger {
public:
    
    logger(const container& params)
        : err_stream_(), params_(params),
        err_code_() {}

    
    void log_error_identifier_not_given() {
        lock_error(error_code::identifier_not_given);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: didn't receive identifier, command-line is empty.\n"
        );
    }

    
    template <class KeyT>
    void log_error_incompatible_arguments(
        KeyT key, string_view_type args_string
    ) {
        lock_error(error_code::incompatible_argument);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: received arguments are incompatible with"
            " the specified key \""
        ) << key << __LITERAL(char_type,
            "\".\n"
            "\treceived: ["
        ) << args_string << __LITERAL(char_type,
            "]\n";
        );
    }

    
    void log_error_invalid_identifier(
        string_view_type received_identifier,
        string_view_type correct_identifier
    ) {
        lock_error(error_code::invalid_identifier);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: invalid identifier specified.\n"
            "\texpected \""
        ) << correct_identifier << __LITERAL(char_type,
            "\" but received \""
        ) << received_identifier << __LITERAL(char_type,
            "\"\n"
        );
    }

    
    void log_error_key_not_given() {
        lock_error(error_code::key_not_given);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: key is not given.\n"
        );
    }

    
    template <class KeyT>
    void log_error_undefined_key(KeyT key) {
        lock_error(error_code::undefined_key);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: undefined key \""
        ) << key << __LITERAL(char_type,
            "\" received.\n"
        );
    }

    
    void log_error_unparsed_arguments(
        string_view_type args_string
    ) {
        lock_error(error_code::unparsed_argument);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: unparsed arguments detected.\n"
            "\tremaining tokens: "
        );
        err_stream_ << args_string;
        err_stream_ << __LITERAL(char_type,
            "\n"
        );
    }

    
    template <class KeyT>
    void log_error_wrong_complex_key(KeyT keys) {
        lock_error(error_code::incompatible_argument);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: at least one of the keys in complex param received"
            " isn't defined as boolean param"
            " or at least one key in a complex param duplicated.\n"
            "\treceived: \""
        ) << keys << __LITERAL(char_type,
            "\"\n"
        );
    }

    
    void log_error_required_key_not_given() {
        lock_error(error_code::required_key_not_given);
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

        detail::tuple_for_each(params_.data(),
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
                    print_keys_to_err(p.short_keys());
                    err_stream_ << __LITERAL(char_type, "|");
                    print_keys_to_err(p.long_keys());
                    err_stream_ << __LITERAL(char_type, "]: ")
                        << p.brief_message() << __LITERAL(char_type, "\n");
                }
            }
        );
    }

    
    template <class KeyT>
    void log_error_duplicated_assignments(KeyT key) {
        lock_error(error_code::duplicated_assignments);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: duplicated assignments detected when parsing \""
        ) << key << __LITERAL(char_type,
            "\".\n"
            "\tmore than one of keys are assigning their values to same parameter.\n"
        );
    }

    
    std::optional<error_code> error() const noexcept {
        return err_code_;
    }

    
    string_view_type error_message() const noexcept {
        return err_stream_.rdbuf()->view();
    }

    
    void clear() {
        err_code_.reset();
        err_stream_.clear();
        if (err_stream_.rdbuf()->in_avail() > 1ull) {
            err_stream_.ignore(
                std::numeric_limits<std::streamsize>::max()
            );
        }
    }
private:
    
    void lock_error(error_code ec) {
        if ( !error() ) {
            err_code_ = ec;
        }
    }

    stream_type err_stream_; 
    const container& params_; 
    std::optional<error_code> err_code_; 
};  // class basic_cl_parser::logger

public:
    
    basic_cl_parser(string_view_type identifier, Params... params)
        : cont_( std::move(params)... ), veri_(identifier),
        assi_(cont_), logg_(cont_) {}

    
    std::optional<error_code> error() const noexcept {
        return logg_.error();
    }

    
    string_view_type error_message() const noexcept {
        return logg_.error_message();
    }

    
    template <class IntType, class StrArrType>
    result_tuple_type& parse(IntType argc, StrArrType argv) {
        // if error is not cleared, return early.
        if ( logg_.error() ) {
            return get();
        }

        return parse_impl( interpreter(argc, argv) );
    }

    
    result_tuple_type& parse(string_view_type command_line) {
        // if error is not cleared, return early.
        if ( logg_.error() ) {
            return get();
        }

        return parse_impl( interpreter(command_line) );
    }

    
    result_tuple_type& get() {
        return cont_.values();
    }

    
    const result_tuple_type& get() const {
        return cont_.values();
    }

    
    void clear() {
        cont_.clear();
        veri_.clear();
        logg_.clear();
    }

private:
    
    result_tuple_type& parse_impl(interpreter&& ip) {
        clear();

        if (ip.done()) {
            logg_.log_error_identifier_not_given();
        }
        
        auto [id, arg_unexpected] = ip.get_token();

        if ( !veri_.is_valid_identifier(id) ) {
            logg_.log_error_invalid_identifier(
                id, veri_.id()
            );
        }

        if ( !std::ranges::empty(arg_unexpected) ) {
            logg_.log_error_key_not_given();
        }

        while (!ip.done()) {
            if ( !ip.facing_key() ) {
                logg_.log_error_key_not_given();
            }

            auto tok = ip.get_token();

            if ( veri_.starts_with_short_key(tok) ) {
                auto key = ip.remove_dash(tok.leading);
                const auto& args = tok.followings;
                // token's leading factor is string_view_type,
                // for represent short key that is char_type,
                // fetch the front element.
                parse_single_key( *std::begin(key), args );
            }
            else if ( veri_.starts_with_long_key(tok) ) {
                auto key = ip.remove_dash(tok.leading);
                const auto& args = tok.followings;
                parse_single_key(key, args);
            }
            else if ( veri_.starts_with_complex_key(tok) ) {
                auto key = ip.remove_dash(tok.leading);
                parse_complex_key(key);
            }
            else {
                // this branch cannot be reached.
                // if reached, it's pragmma fault.
                assert(false);
            }
        }

        if ( !veri_.satisfies_required(cont_) ) {
            logg_.log_error_required_key_not_given();
        }

        if (!logg_.error()) {
            cont_.update_cache();
        }

        return get();
    }

    
    template <class KeyT, class Args>
    void parse_single_key(KeyT key, Args&& args) {
        if ( !veri_.is_valid_single_key(key, cont_) ) {
            logg_.log_error_undefined_key(key);
        }
        if ( veri_.is_duplicated_assignment(key, cont_) ) {
            logg_.log_error_duplicated_assignments(key);
        }

        assi_.assign_single(key, std::forward<Args>(args), veri_);

        if (veri_.fail()) {
            logg_.log_error_incompatible_arguments(
                key, assi_.get_unassigned()
            );
        }
        if (veri_.bad()) {
            logg_.log_error_unparsed_arguments(
                assi_.get_unassigned()
            );
        }
    }

    
    void parse_complex_key(string_view_type keys) {
        if ( !veri_.is_valid_complex_key(keys, cont_) ) {
            logg_.log_error_wrong_complex_key(keys);
        }
        if ( veri_.is_duplicated_complex_assignment(keys, cont_) ) {
            logg_.log_error_duplicated_assignments(keys);
        }

        assi_.assign_complex(keys, veri_);

        if (veri_.fail()) {
            logg_.log_error_wrong_complex_key(keys);
        }
        
        // verifier's bad bit connot be set,
        // complex key is tokenized without arguments.
        // so if bad bit is set, it's pragmma fault.
        assert( !veri_.bad() );
    }

    container cont_;
    verifier veri_;
    assigner assi_;
    logger logg_;
};  // class basic_cl_parser


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
using wparser = basic_cl_parser<wchar_t, std::char_traits<wchar_t>, Params...>;

}   // namespace gclp