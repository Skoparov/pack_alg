#ifndef PACK_ALG_H
#define PACK_ALG_H

#include <cstddef>
#include <utility>
#include <type_traits>

namespace pack_alg {

template<typename... types>
struct pack {}; // default pack definition provided for convenience

template<typename pack>
struct pack_size;

template<typename pack>
struct end; // value = size<pack>::value or 1 if pack is empty

template<typename pack, size_t pos>
struct type_at;

template<typename pack, typename predicate, size_t start_pos = 0>
struct find_if; // value = position or end<pack>::value if type not found

template<typename pack, typename predicate>
struct enumerate_if; // value = index_sequence<idxs of types satisfying pred...>

template<typename pack, typename... types>
struct has_types;

template<typename pack, typename... types>
struct has_types_no_dup;

template<typename pack, typename predicate, typename... types>
struct add_types_if;

template<typename pack, typename predicate>
struct remove_types_if; // predicate should have bool 'value' set to true or false

template<typename pack>
struct unique;

template<typename... packs>
struct concat;

template<typename pack>
struct invert;

// predicates

struct placeholder;
using _1 = placeholder;
using _2 = placeholder;
using _3 = placeholder;
using _4 = placeholder;
using _5 = placeholder;

using always = std::true_type;
using never = std::false_type;

template<typename... predicates>
struct and_;

template<typename... predicates>
struct or_;

template<typename predicate>
struct not_;

template<typename... types>
using any_of = has_types<pack<types...>, _1>;

template<typename... types>
using any_of_no_dup = has_types_no_dup<pack<types...>, _1>;

template<typename... types>
using none_of = not_<any_of<types...>>;

template<typename... types>
using none_of_no_dup = not_<any_of_no_dup<types...>>;

// convenience typedefs

template<typename pack>
constexpr size_t pack_size_v = pack_size<pack>::value;

template<typename pack, size_t pos>
using type_at_t = typename type_at<pack, pos>::type;
    
template<typename pack>
constexpr size_t end_v = end<pack>::value;

template<typename pack, typename predicate, size_t start_pos = 0>
constexpr size_t find_if_v = find_if<pack, predicate, start_pos>::value;

template<typename pack, typename type, size_t start_pos = 0>
constexpr size_t find_v = find_if_v<pack, any_of_no_dup<type>, start_pos>;

template<typename pack, typename predicate>
using enumerate_if_t = typename enumerate_if<pack, predicate>::type;

template<typename pack, typename type>
using enumerate_t = typename enumerate_if<pack, any_of_no_dup<type>>::type;

template<typename pack, typename... types>
constexpr bool has_types_v = has_types<pack, types...>::value;

template<typename pack, typename... types>
constexpr bool has_types_no_dup_v = has_types_no_dup<pack, types...>::value;

template<typename pack, typename predicate, typename... types>
using add_types_if_t = typename add_types_if<pack, predicate, types...>::type;

template<typename pack, typename... types>
using add_types_t = add_types_if_t<pack, always, types...>;

template<typename pack, typename predicate>
using remove_types_if_t = typename remove_types_if<pack, predicate>::type;

template<typename pack, typename... types>
using remove_types_t = remove_types_if_t<pack, any_of<types...>>;

template<typename pack>
using unique_t = typename unique<pack>::type;

template<typename pack, typename predicate>
using filter_t = remove_types_if_t<pack, not_<predicate>>;

template<typename... packs>
using concat_t = typename concat<packs...>::type;

template<typename pack>
using invert_t = typename invert<pack>::type;

///////
// impl
///////

namespace detail {

template<
    template<typename...> class pred,
    typename types,
    typename result,
    typename... args>
struct with_args;

template<
    template<typename...> class pred,
    typename type, typename... tail,
    typename... result,
    typename arg, typename...args>
struct with_args<pred, pack<type, tail...>, pack<result...>, arg, args...>
{
    static constexpr bool value{ std::conditional_t<
        std::is_same_v<type, placeholder>,
        with_args<pred, pack<tail...>, pack<result..., arg>, args...>,
        with_args<pred, pack<tail...>, pack<result..., type>, arg, args...>>::value };
};

template<template<typename...> class pred, typename... tail, typename... result>
struct with_args<pred, pack<tail...>, pack<result...>>
{
    static constexpr bool value{ pred<result..., tail...>::value };
};

template<typename pred, typename... args>
struct eval
{
    static constexpr bool value{ pred::value };
};

template<template<typename...> class pred, typename... types, typename... args>
struct eval<pred<types...>, args...>
{
    static constexpr bool value{ 
        with_args<pred, pack<types...>, pack<>, args...>::value };
};

template<typename... preds, typename... args>
struct eval<and_<preds...>, args...>
{
    static constexpr bool value{ std::conjunction_v<eval<preds, args...>...> };
};

template<typename... preds, typename... args>
struct eval<or_<preds...>, args...>
{
    static constexpr bool value{ std::disjunction_v<eval<preds, args...>...> };
};

template<typename pred, typename... args>
struct eval<not_<pred>, args...>
{
    static constexpr bool value{ !eval<pred, args...>::value };
};

template<typename pred, typename... args>
constexpr bool eval_v = eval<pred, args...>::value;

struct end_guard;

template<typename pred, typename type = end_guard, typename... tail>
constexpr size_t find_if(size_t pos, size_t start_pos) noexcept
{
    return (pos>= start_pos && eval_v<pred, type>) || 
            std::is_same_v<type, end_guard> ?
                pos :
                find_if<pred, tail...>(pos + 1, start_pos);
}

template<typename pack, typename result_pack>
struct unique;

template<
    template<typename...> class pack,
    typename curr_type,
    typename... other_types,
    typename... result_types>
struct unique<pack<curr_type, other_types...>, pack<result_types...>>
{
    using type = typename std::conditional_t<
            has_types_no_dup_v<pack<result_types...>, curr_type>,
            unique<pack<other_types...>, pack<result_types...>>,
            unique<pack<other_types...>, pack<result_types..., curr_type>>>::type; 
};

template<template<typename...> class pack, typename... result_types>
struct unique<pack<>, pack<result_types...>>
{
    using type = pack<result_types...>;
};

template<typename pred, size_t pos, typename indexes, typename... types>
struct enumerate_if;

template<typename pred, size_t pos, size_t... indexes, typename curr, typename... types>
struct enumerate_if<pred, pos, std::index_sequence<indexes...>, curr, types...>
{
    using seq = std::conditional_t<eval_v<pred, curr>,
                        std::index_sequence<indexes..., pos>,
                        std::index_sequence<indexes...>>;

    using type = typename enumerate_if<pred, pos + 1, seq, types...>::type;
};

template<typename pred, size_t pos, size_t... indexes>
struct enumerate_if<pred, pos, std::index_sequence<indexes...>>
{
    using type = std::index_sequence<indexes...>;
};

template<typename id>
struct identity{ using type = id; };

template<typename... types>
struct inherit : identity<types>... {};

template<bool val>
struct bool_v
{
    static constexpr bool value{ val };
};

}// detail

// pack_size

template<template<typename...> class pack, typename... types>
struct pack_size<pack<types...>>
{
    static constexpr size_t value{ sizeof...(types) };
};

// end

template<typename pack>
struct end
{
    static constexpr size_t value{ pack_size_v<pack> ? pack_size_v<pack> : 1 };
};

// type_at

template<
    template<typename...> class pack,
    typename curr_type,
    typename... other_types,
    size_t pos>
struct type_at<pack<curr_type, other_types...>, pos>
{
    static_assert(pos <= sizeof...(other_types), "Type position is out of range");
    using type = type_at_t<pack<other_types...>, pos - 1>;
};

template<
    template<typename...> class pack,
    typename curr_type,
    typename... other_types>
struct type_at<pack<curr_type, other_types...>, 0>
{
    using type = curr_type;
};

// find_if

template<
    template<typename...> class pack,
    typename... types,
    typename pred,
    size_t start_pos>
struct find_if<pack<types...>, pred, start_pos>
{
    static constexpr size_t value{ 
        sizeof...(types) > 0?
        detail::find_if<pred, types...>(0, start_pos) : 1 };
};

// enumerate_if

template<
    template<typename...> class pack,
    typename... types,
    typename pred>
struct enumerate_if<pack<types...>, pred>
{
    using type = typename detail::enumerate_if<
        pred, 0, std::index_sequence<>, types...>::type;
};

// has_types

template<typename pack, typename... types>
struct has_types
{
    static constexpr bool value{ std::conjunction_v<
        detail::bool_v<find_v<pack, types> != end_v<pack>>...>};
};

// has_types_no_dup

template<
    template<typename...> class pack,
    typename... uniq_types,
    typename... types>
struct has_types_no_dup<pack<uniq_types...>, types...>
{
    static constexpr bool value{ std::conjunction_v<
        std::is_base_of<
            detail::identity<types>,
            detail::inherit<uniq_types...>>...> };
};

// add_types_if

template<
    template<typename...> class pack,
    typename... types,
    typename pred,
    typename... types_to_add>
struct add_types_if<pack<types...>, pred, types_to_add...>
{
    using type = concat_t<pack<types...>, std::conditional_t<
                    detail::eval_v<pred, types_to_add>,
                    pack<types_to_add>,
                    pack<>>...>;
};

// remove_types_if

template<
    template<typename...> class pack,
    typename... types,
    typename pred>
struct remove_types_if<pack<types...>, pred>
{
    using type = concat_t<pack<>, std::conditional_t<
                    detail::eval_v<pred, types>,
                    pack<>,
                    pack<types>>...>;
};

// unique

template<template<typename...> class pack, typename... types>
struct unique<pack<types...>>
{
    using type = typename detail::unique<pack<types...>, pack<>>::type;
};

// concat

template<
    template<typename...> class pack,
    typename... pack1_types,
    typename... pack2_types,
    typename... other_packs>
struct concat<pack<pack1_types...>, pack<pack2_types...>, other_packs...>
{
    using type = concat_t<pack<pack1_types..., pack2_types...>, other_packs...>;
};

template<template<typename...> class pack, typename... pack_types>
struct concat<pack<pack_types...>>
{
    using type = pack<pack_types...>;
};

// invert

template<template<typename...> class pack>
struct invert<pack<>>
{
    using type = pack<>;
};

template<template<typename...> class pack, typename curr_type, typename... types>
struct invert<pack<curr_type, types...>>
{
    using type = concat_t<typename invert<pack<types...>>::type, pack<curr_type>>;
};

}// pack_alg

#endif