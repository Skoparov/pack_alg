#ifndef PACK_ALG_H
#define PACK_ALG_H

#include <cstddef>
#include <utility>
#include <type_traits>

namespace pack_alg {

template<typename... types>
struct pack {}; // default pack definition provided for convenience

template<typename type>
struct is_type_pack;

template<typename pack>
struct pack_size;

template<typename pack>
struct end; // value = size<pack>::value or 1 if pack is empty

template<typename pack, size_t pos>
struct type_at;

template<typename pack, template<typename> class pred, size_t start_pos = 0>
struct find_if; // value = position or end<pack>::value if type not found

template<typename pack, template<typename> class pred>
struct enumerate_if; // value = index_sequence<idxs of types satisfying pred...>

template<typename pack, typename... types>
struct has_types;

template<typename pack, typename... types>
struct has_types_no_dup;

template<typename pack, template<typename> class predicate, typename... types>
struct add_types_if;

template<typename pack, template<typename> class predicate>
struct remove_types_if; // predicate should have bool 'value' set to true or false

template<typename pack>
struct unique;

template<typename pack, template<typename> class predicate>
struct filter;

template<typename... packs>
struct concat;

template<typename pack>
struct invert;

// predicates

template<typename> using always = std::true_type;
template<typename> using never = std::false_type;

template<template<typename> class... predicates>
struct and_;

template<template<typename> class... predicates>
struct or_;

template<template<typename> class predicate>
struct not_
{
    template<typename type>
    struct pred
    {
        static constexpr bool value{ !predicate<type>::value };
    };
};

template<typename... types>
struct of
{
    template<typename type>
    using fits_any = has_types<pack<types...>, type>;

    template<typename type>
    using fits_any_no_dup = has_types_no_dup<pack<types...>, type>;

    template<typename type>
    using fits_none = typename not_<fits_any>::template pred<type>;

    template<typename type>
    using fits_none_no_dup = typename not_<fits_any_no_dup>::template pred<type>;
};

template<typename pack> struct from;

template<template<typename...> class pack, typename... types>
struct from<pack<types...>> : of<types...> {};

// convenience typedefs

template<typename type>
constexpr bool is_type_pack_v = is_type_pack<type>::value;

template<typename pack>
constexpr size_t pack_size_v = pack_size<pack>::value;

template<typename pack, size_t pos>
using type_at_t = typename type_at<pack, pos>::type;
    
template<typename pack>
constexpr size_t end_v = end<pack>::value;

template<typename pack, template<typename> class pred, size_t start_pos = 0>
constexpr size_t find_if_v = find_if<pack, pred, start_pos>::value;

template<typename pack, typename type, size_t start_pos = 0>
constexpr size_t find_v = find_if_v<pack, of<type>::template fits_any_no_dup, start_pos>;

template<typename pack, template<typename> class pred>
using enumerate_if_t = typename enumerate_if<pack, pred>::type;

template<typename pack, typename type>
using enumerate_t = typename enumerate_if<pack, of<type>:: template fits_any_no_dup>::type;

template<typename pack, typename... types>
constexpr bool has_types_v = has_types<pack, types...>::value;

template<typename pack, typename... types>
constexpr bool has_types_no_dup_v = has_types_no_dup<pack, types...>::value;

template<typename pack, template<typename> class predicate, typename... types>
using add_types_if_t = typename add_types_if<pack, predicate, types...>::type;

template<typename pack, typename... types>
using add_types_t = add_types_if_t<pack, always, types...>;

template<typename pack, template<typename> class predicate>
using remove_types_if_t = typename remove_types_if<pack, predicate>::type;

template<typename pack, typename... types>
using remove_types_t = remove_types_if_t<pack, of<types...>::template fits_any>;

template<typename pack>
using unique_t = typename unique<pack>::type;

template<typename pack, template<typename> class predicate>
using filter_t = typename filter<pack, predicate>::type;

template<typename... packs>
using concat_t = typename concat<packs...>::type;

template<typename pack>
using invert_t = typename invert<pack>::type;


///////
// impl
///////

namespace detail {

template<bool... vals> struct logic_and
{
    static constexpr bool value{ ( vals && ... ) };
};

template<bool... vals> struct logic_or
{
    static constexpr bool value{ ( vals || ... ) };
};

template<bool... vals>
constexpr bool logic_and_v = logic_and<vals...>::value;

template<bool... vals>
constexpr bool logic_or_v = logic_or<vals...>::value;

struct end_guard;

template<
    template<typename> class pred,
    typename curr_type = end_guard,
    typename... other_types>
constexpr size_t find(size_t pos, size_t start_pos) noexcept
{
    return (pos>= start_pos && pred<curr_type>::value) || std::is_same_v<curr_type, end_guard> ?
                pos :
                find<pred, other_types...>(pos + 1, start_pos);
}

template<typename pack, typename result_pack>
struct unique;

template<
    template<typename...> class pack,
    typename curr_type,
    typename... other_types,
    typename... result_types>
struct unique<pack<curr_type, other_types...>, pack<result_types...>> :
    std::conditional_t<
            has_types_no_dup_v<pack<result_types...>, curr_type>,
            unique<pack<other_types...>, pack<result_types...>>,
            unique<pack<other_types...>, pack<result_types..., curr_type>>> 
{};

template<template<typename...> class pack, typename... result_types>
struct unique<pack<>, pack<result_types...>>
{
    using type = pack<result_types...>;
};

template<template<typename> class pred, size_t pos, typename indexes, typename... types>
struct enumerate_if;

template<
    template<typename> class pred,
    size_t pos,
    size_t... indexes,
    typename curr,
    typename... types>
struct enumerate_if<pred, pos, std::index_sequence<indexes...>, curr, types...>
{
    using seq = std::conditional_t<pred<curr>::value,
                        std::index_sequence<indexes..., pos>,
                        std::index_sequence<indexes...>>;

    using type = typename enumerate_if<pred, pos + 1, seq, types...>::type;
};

template<template<typename> class pred, size_t pos, size_t... indexes>
struct enumerate_if<pred, pos, std::index_sequence<indexes...>>
{
    using type = std::index_sequence<indexes...>;
};

template<typename id>
struct identity{ using type = id; };

template<typename... types>
struct inherit : identity<types>... {};

}// detail

// is_type_pack

template<template<typename...> class pack, typename... types>
struct is_type_pack<pack<types...>>
{
    static constexpr bool value{ true };
};

template<typename pack>
struct is_type_pack
{
    static constexpr bool value{ false };
};

// pack_size

template<template<typename...> class pack, typename... types>
struct pack_size<pack<types...>>
{
    static constexpr size_t value{ sizeof...(types) };
};

template<typename pack>
struct pack_size
{
    static_assert(is_type_pack_v<pack>, "Pack type is not valid");
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
    template<typename> class pred,
    size_t start_pos>
struct find_if<pack<types...>, pred, start_pos>
{
    static constexpr size_t value{ 
        sizeof...(types) > 0?
        detail::find<pred, types...>(0, start_pos) : 1 };
};

// enumerate_if

template<
    template<typename...> class pack,
    typename... types,
    template<typename> class pred>
struct enumerate_if<pack<types...>, pred>
{
    using type = typename detail::enumerate_if<
        pred, 0, std::index_sequence<>, types...>::type;
};

// has_types

template<typename pack, typename... types>
struct has_types
{
    static constexpr bool value{ 
        detail::logic_and_v<(find_v<pack, types> != end_v<pack>)...>};
};

// has_types_no_dup

template<
    template<typename...> class pack,
    typename... uniq_types,
    typename... types>
struct has_types_no_dup<pack<uniq_types...>, types...>
{
    static constexpr bool value{ detail::logic_and_v<
        std::is_base_of_v<
            detail::identity<types>,
            detail::inherit<uniq_types...>>...>
    };
};

// add_types_if

template<
    template<typename...> class pack,
    typename... types,
    template<typename> class predicate,
    typename... types_to_add>
struct add_types_if<pack<types...>, predicate, types_to_add...>
{
    using type = concat_t<pack<types...>, std::conditional_t<
                    predicate<types_to_add>::value,
                    pack<types_to_add>,
                    pack<>>...>;
};

// remove_types_if

template<
    template<typename...> class pack,
    typename... types,
    template<typename> class predicate>
struct remove_types_if<pack<types...>, predicate>
{
    using type = concat_t<pack<>, std::conditional_t<
                    predicate<types>::value,
                    pack<>,
                    pack<types>>...>;
};

// unique

template<template<typename...> class pack, typename... types>
struct unique<pack<types...>> :
    detail::unique<pack<types...>, pack<>> {};

// filter

template<template<typename...> class pack, typename...types, template<typename> class predicate>
struct filter<pack<types...>, predicate>
{
    using type = concat_t<pack<>, std::conditional_t<
        predicate<types>::value,
        pack<types>,
        pack<>>...>;
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

// and or

template<template<typename> class... predicates>
struct and_
{
    static_assert(sizeof...(predicates) > 1, "Required at least two predicates");

    template<typename spec_type>
    using pred = detail::logic_and<predicates<spec_type>::value...>;
};

template<template<typename> class... predicates>
struct or_
{
    static_assert(sizeof...(predicates) > 1, "Required at least two predicates");

    template<typename spec_type>
    using pred = detail::logic_or<predicates<spec_type>::value...>;
};

}// pack_alg

#endif