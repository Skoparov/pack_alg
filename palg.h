#ifndef PALG_H
#define PALG_H

#include <cstddef>
#include <utility>
#include <type_traits>

namespace palg {

template<typename... types>
struct pack {};

template<template<typename...> class pred>
struct fun;

// Algorithms

template<typename pack>
struct size;

template<typename pack>
struct end;

template<typename pack, template<typename...> class dest>
struct repack;

template<typename pack, size_t pos>
struct type_at;

template<typename pack>
struct front;

template<typename pack>
struct back;

template<typename pack, typename predicate, size_t start_pos = 0>
struct find_if;

template<typename pack, typename type, size_t start_pos = 0>
struct find;

template<typename pack, typename predicate>
struct enumerate_if;

template<typename pack, typename type>
struct enumerate;

template<typename pack, typename... types>
struct has_types;

template<typename pack, typename... types>
struct has_types_nodup;

template<typename pack, typename predicate, typename... types>
struct append_if;

template<typename pack, typename... types>
struct append;

template<typename pack, typename predicate, typename... types>
struct prepend_if;

template<typename pack, typename... types>
struct prepend;

template<typename pack, typename predicate, size_t num>
struct pop_front_n_if;

template<typename pack, size_t num>
struct pop_front_n;

template<typename pack, typename predicate>
struct pop_front_if;

template<typename pack>
struct pop_front;

template<typename pack, typename predicate, size_t num>
struct pop_back_n_if;

template<typename pack, size_t num>
struct pop_back_n;

template<typename pack, typename predicate>
struct pop_back_if;

template<typename pack>
struct pop_back;

template<typename pack, typename predicate>
struct remove_if;

template<typename pack, typename... types>
struct remove;

template<typename pack>
struct unique;

template<typename... packs>
struct concat;

template<typename pack>
struct invert;

template<typename pack, typename type_predicate>
struct transform;

namespace detail {

template<typename pred, typename... types>
using apply_t = typename pred::template apply<types...>;

template<typename pred, typename... types>
constexpr bool eval_v{ apply_t<pred, types...>::value };

template<typename pred, typename type>
using eval_t = typename apply_t<pred, type>::type;

template<typename... preds>
struct and_p
{
    template<typename... types>
    using pred = std::conjunction<apply_t<preds, types...>...>;
};

template<typename... preds>
struct or_p
{
    template<typename... types>
    using pred = std::disjunction<apply_t<preds, types...>...>;
};

template<typename pred_to_negate>
struct not_p
{
    template<typename... types>
    using pred = std::negation<apply_t<pred_to_negate, types...>>;
};

template<typename inner>
struct type_identity
{
    using type = inner;
};

template<auto val>
struct value_identity
{
    static constexpr auto value{ val };
};

template<auto val>
struct eval_to
{
    template<typename...>
    struct pred{ static constexpr auto value{ val }; };
};

template<typename... types>
struct inherit : type_identity<types>... {};

template<typename... types>
struct any_of_p
{
    template<typename type>
    using pred = has_types<pack<types...>, type>;
};

template<typename... types>
struct any_of_nodup_p
{
    template<typename type>
    using pred = has_types_nodup<pack<types...>, type>;
};

}// detail

// Predicates

using always = fun<detail::eval_to<true>::template pred>;
using never = fun<detail::eval_to<false>::template pred>;

template<typename... predicates>
using and_ = fun<detail::and_p<predicates...>::template pred>;

template<typename... predicates>
using or_ = fun<detail::or_p<predicates...>::template pred>;

template<typename predicate>
using not_ = fun<detail::not_p<predicate>::template pred>;

template<typename... types>
using any_of = fun<detail::any_of_p<types...>::template pred>;

template<typename... types>
using any_of_nodup = fun<detail::any_of_nodup_p<types...>::template pred>;

template<typename... types>
using none_of = not_<any_of<types...>>;

// Convenience typedefs

template<typename pack>
constexpr size_t size_v{ size<pack>::value };
    
template<typename pack>
constexpr size_t end_v{ end<pack>::value };

template<typename pack, template<typename...> class dest>
using repack_t = typename repack<pack, dest>::type;

template<typename pack, size_t pos>
using type_at_t = typename type_at<pack, pos>::type;

template<typename pack>
using front_t = typename front<pack>::type;

template<typename pack>
using back_t = typename back<pack>::type;

template<typename pack, typename predicate, size_t start_pos = 0>
constexpr size_t find_if_v{ find_if<pack, predicate, start_pos>::value };

template<typename pack, typename type, size_t start_pos = 0>
constexpr size_t find_v{ find<pack, type, start_pos>::value };

template<typename pack, typename predicate>
using enumerate_if_t = typename enumerate_if<pack, predicate>::type;

template<typename pack, typename type>
using enumerate_t = typename enumerate<pack, type>::type;

template<typename pack, typename... types>
constexpr bool has_types_v{ has_types<pack, types...>::value };

template<typename pack, typename... types>
constexpr bool has_types_nodup_v{ has_types_nodup<pack, types...>::value };

template<typename pack, typename predicate, typename... types>
using append_if_t = typename append_if<pack, predicate, types...>::type;

template<typename pack, typename... types>
using append_t = typename append<pack, types...>::type;

template<typename pack, typename predicate, typename... types>
using prepend_if_t = typename prepend_if<pack, predicate, types...>::type;

template<typename pack, typename... types>
using prepend_t = typename prepend<pack, types...>::type;

template<typename pack, typename predicate, size_t num>
using pop_front_n_if_t = typename pop_front_n_if<pack, predicate, num>::type;

template<typename pack, size_t num>
using pop_front_n_t = typename pop_front_n<pack, num>::type;

template<typename pack, typename predicate>
using pop_front_if_t = typename pop_front_if<pack, predicate>::type;

template<typename pack>
using pop_front_t = typename pop_front<pack>::type;

template<typename pack, typename predicate, size_t num>
using pop_back_n_if_t = typename pop_back_n_if<pack, predicate, num>::type;

template<typename pack, size_t num>
using pop_back_n_t = typename pop_back_n<pack, num>::type;

template<typename pack, typename predicate>
using pop_back_if_t = typename pop_back_if<pack, predicate>::type;

template<typename pack>
using pop_back_t = typename pop_back<pack>::type;

template<typename pack, typename predicate>
using remove_if_t = typename remove_if<pack, predicate>::type;

template<typename pack, typename... types>
using remove_t = typename remove<pack, types...>::type;

template<typename pack>
using unique_t = typename unique<pack>::type;

template<typename pack, typename predicate>
using filter_t = remove_if_t<pack, not_<predicate>>;

template<typename... packs>
using concat_t = typename concat<packs...>::type;

template<typename pack>
using invert_t = typename invert<pack>::type;

template<typename pack, typename pred>
using transform_t = typename transform<pack, pred>::type;

///////
// impl
///////

namespace detail {

struct guard;

template<typename pred, typename type = guard, typename... tail>
constexpr size_t find_if(size_t pos, size_t start_pos) noexcept
{
    return ((pos >= start_pos && eval_v<pred, type>) || std::is_same_v<type, guard>) ?
        pos : find_if<pred, tail...>(pos + 1, start_pos);
}

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

template<typename pack, typename res, size_t num>
struct unwrapped_pop_back_n;

template<
    template<typename...> class pack,
    typename head, typename... tail,
    typename... result,
    size_t num>
struct unwrapped_pop_back_n<pack<head, tail...>, pack<result...>, num>
{
    using type = std::conditional_t<
            sizeof...(tail) == num,
            pack<result..., head>,
            unwrapped_pop_back_n<pack<tail...>, pack<result..., head>, num>>;
};

template<typename pack, size_t num>
struct pop_back_n;

template<template<typename...> class pack, typename... types, size_t num>
struct pop_back_n<pack<types...>, num>
{
    using type = typename unwrapped_pop_back_n<pack<types...>, pack<>, num>::type;
};

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
            has_types_nodup_v<pack<result_types...>, curr_type>,
            unique<pack<other_types...>, pack<result_types...>>,
            unique<pack<other_types...>, pack<result_types..., curr_type>>>::type; 
};

template<template<typename...> class pack, typename... result_types>
struct unique<pack<>, pack<result_types...>>
{
    using type = pack<result_types...>;
};

}// detail

// fun

template<template<typename...> class pred>
struct fun
{
    template <typename... types>
    struct apply : pred<types...> {};
};

// size

template<template<typename...> class pack, typename... types>
struct size<pack<types...>>
{
    static constexpr size_t value{ sizeof...(types) };
};

// end

template<typename pack>
struct end
{
    static constexpr size_t value{ size_v<pack> ? size_v<pack> : 1 };
};

// repack

template<
    template<typename...> class pack,
    typename... types,
    template<typename...> class dest>
struct repack<pack<types...>, dest>
{
    using type = dest<types...>;
};

// type_at

template<
    template<typename...> class pack,
    typename curr_type,
    typename... other_types,
    size_t pos>
struct type_at<pack<curr_type, other_types...>, pos>
{
    static_assert(pos <= sizeof...(other_types), "Position is out of range");
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

// front

template<template<typename...> class pack, typename head, typename... tail>
struct front<pack<head, tail...>>
{
    using type = head;
};

// back

template<typename pack>
struct back
{
    using type = type_at_t<pack, end_v<pack> - 1>;
};

// find_if

template<
    template<typename...> class pack,
    typename... types,
    typename pred,
    size_t start_pos>
struct find_if<pack<types...>, pred, start_pos>
{
    static_assert(start_pos <= sizeof...(types), "Position is out of range");
    static constexpr size_t value{ 
        sizeof...(types) > 0?
        detail::find_if<pred, types...>(0, start_pos) : 1 };
};

// find

template<
    template<typename...> class pack,
    typename... types,
    typename type,
    size_t start_pos>
struct find<pack<types...>, type, start_pos>
{
    static_assert(start_pos <= sizeof...(types), "Position is out of range");
    static constexpr size_t value{
        sizeof...(types) > 0?
        detail::find_if<any_of_nodup<type>, types...>(0, start_pos) : 1 };
};

// enumerate_if

template<template<typename...> class pack, typename... types, typename pred>
struct enumerate_if<pack<types...>, pred>
{
    using type = typename detail::enumerate_if<
        pred, 0, std::index_sequence<>, types...>::type;
};

// enumerate

template<typename pack, typename target_type>
struct enumerate
{
    using type = enumerate_if_t<pack, any_of_nodup<target_type>>;
};

// has_types

template<typename pack, typename... types>
struct has_types
{
    static constexpr bool value{ std::conjunction_v<
        detail::value_identity<find_v<pack, types> != end_v<pack>>...>};
};

// has_types_nodup

template<
    template<typename...> class pack,
    typename... uniq_types,
    typename... types>
struct has_types_nodup<pack<uniq_types...>, types...>
{
    static constexpr bool value{ std::conjunction_v<
        std::is_base_of<
            detail::type_identity<types>,
            detail::inherit<uniq_types...>>...> };
};

// append_if

template<
    template<typename...> class pack,
    typename... types,
    typename pred,
    typename... to_add>
struct append_if<pack<types...>, pred, to_add...>
{
    using type = concat_t<pack<types...>, std::conditional_t<
                    detail::eval_v<pred, to_add>,
                    pack<to_add>,
                    pack<>>...>;
};

// append

template<
    template<typename...> class pack,
    typename... types,
    typename... to_add>
struct append<pack<types...>, to_add...>
{
    using type = pack<types..., to_add...>;
};

// prepend_if

template<
    template<typename...> class pack,
    typename... types,
    typename pred,
    typename... types_to_add>
struct prepend_if<pack<types...>, pred, types_to_add...>
{
    using type = concat_t<std::conditional_t<
                    detail::eval_v<pred, types_to_add>,
                    pack<types_to_add>,
                    pack<>>...,
                    pack<types...>>;
};

// prepend

template<template<typename...> class pack, typename... types, typename... to_add>
struct prepend<pack<types...>, to_add...>
{
    using type = pack<to_add..., types...>;
};

// pop_front_n_if

template<typename pack, typename pred, size_t num>
struct pop_front_n_if
{
    using type = std::conditional_t<detail::eval_v<pred>, pop_front_n_t<pack, num>, pack>;
};

// pop_front_n

template<typename pack, size_t num>
struct pop_front_n
{
    using type = pop_front_n_t<pop_front_t<pack>, num - 1>;
};

template<typename pack>
struct pop_front_n<pack, 0>
{
    using type = pack;
};

// pop_front_if

template<typename pack, typename pred>
struct pop_front_if
{
    using type = std::conditional_t<detail::eval_v<pred>, pop_front_t<pack>, pack>;
};

// pop_front

template<template<typename...> class pack, typename head, typename... tail>
struct pop_front<pack<head, tail...>>
{
    using type = pack<tail...>;
};

// pop_back_n_if

template<typename pack, typename pred, size_t num>
struct pop_back_n_if
{
    using type = std::conditional_t<detail::eval_v<pred>, pop_back_n_t<pack, num>, pack>;
};

// pop_back_n

template<typename pack, size_t num>
struct pop_back_n
{
    using type = typename detail::pop_back_n<pack, num>::type;
};

template<typename pack>
struct pop_back_n<pack, 0>
{
    using type = pack;
};

// pop_back_if

template<typename pack, typename pred>
struct pop_back_if
{
    using type = pop_back_n_if_t<pack, pred, 1>;
};

// pop_back

template<typename pack>
struct pop_back
{
    using type = pop_back_n_t<pack, 1>;
};

// remove_if

template<
    template<typename...> class pack,
    typename... types,
    typename pred>
struct remove_if<pack<types...>, pred>
{
    using type = concat_t<pack<>, std::conditional_t<
                    detail::eval_v<pred, types>,
                    pack<>,
                    pack<types>>...>;
};

// remove

template<typename pack, typename... types>
struct remove
{
    using type = remove_if_t<pack, any_of<types...>>;
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

template<template<typename...> class pack, typename curr_type, typename... types>
struct invert<pack<curr_type, types...>>
{
    using type = concat_t<typename invert<pack<types...>>::type, pack<curr_type>>;
};

template<template<typename...> class pack>
struct invert<pack<>>
{
    using type = pack<>;
};

// transform

template<template<typename...> class pack, typename... types, typename pred>
struct transform<pack<types...>, pred>
{
    using type = pack<detail::eval_t<pred, types>...>;
};

}// palg

#endif