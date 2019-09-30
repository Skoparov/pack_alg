#ifndef TYPE_PACK_H
#define TYPE_PACK_H

#include <cstddef>
#include <type_traits>
#include <tuple>

namespace type_pack {

template<typename... types>
struct pack {}; // default pack definition provided for convenience

template<typename type>
struct is_type_pack;

template<typename pack>
struct pack_size;

template<typename pack>
struct end; // value = size<pack>::value or 1 if pack is empty

template<typename pack, size_t pos>
struct type_at_pos;

template<typename pack, typename type, size_t start_pos = 0>
struct first_pos_of; // value = position or end<pack>::value if type not found

template<typename pack, typename... types>
struct has_types;

template<typename pack, template<typename> class predicate, typename... types>
struct add_types_if;

template<typename pack, typename... types>
struct add_types;

template<typename pack, template<typename> class predicate>
struct remove_types_if; // predicate should have bool 'value' set to true or false

template<typename pack, typename... types>
struct remove_types;

template<typename pack>
struct remove_duplicates;

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
struct predicate_traits
{
    template<typename spec_type>
    struct not_
    {
        static constexpr bool value{ !predicate<spec_type>::value };
    };
};

template<typename pack>
struct pack_predicates
{
    template<typename type>
    struct has_type
    {
        static constexpr bool value{ 
            first_pos_of<pack, type>::value != end<pack>::value };
    };
};

template<typename... types>
struct of
{
    template<typename spec_type>
    using fits_any = has_types<pack<types...>, spec_type>;

    template<typename spec_type>
    using fits_none = typename predicate_traits<fits_any>::template not_<spec_type>;
};

// convenience typedefs

template<typename type>
constexpr bool is_type_pack_v = is_type_pack<type>::value;

template<typename pack>
constexpr size_t pack_size_v = pack_size<pack>::value;

template<typename pack, size_t pos>
using type_at_pos_t = typename type_at_pos<pack, pos>::type;
    
template<typename pack>
constexpr size_t end_v = end<pack>::value;

template<typename pack, typename type, size_t start_pos = 0>
constexpr size_t first_pos_of_v = first_pos_of<pack, type, start_pos>::value;

template<typename pack, typename... types>
constexpr bool has_types_v = has_types<pack, types...>::value;

template<typename pack, template<typename> class predicate, typename... types>
using add_types_if_t = typename add_types_if<pack, predicate, types...>::type;

template<typename pack, typename... types>
using add_types_t = add_types_if_t<pack, always, types...>;

template<typename pack, template<typename> class predicate>
using remove_types_if_t = typename remove_types_if<pack, predicate>::type;

template<typename pack, typename... types>
using remove_types_t = typename remove_types_if<pack, of<types...>::template fits_any>::type;

template<typename pack>
using remove_duplicates_t = typename remove_duplicates<pack>::type;

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

template<bool...> struct bool_seq;

template<template<bool...> class logic_oper, typename vals>
struct eval
{
    template<typename> struct dep_false : std::false_type {};
    static_assert(dep_false<vals>::value, "Invalid vals type");
};

template<template<bool...> class logic_oper, bool... vals>
struct eval<logic_oper, bool_seq<vals...>>
{
    static constexpr bool value{ logic_oper<vals...>::value };
};

struct end_guard;

template<
    typename type_to_find,
    typename curr_type = end_guard,
    typename... other_types>
constexpr size_t first_pos_of(size_t pos, size_t start_pos) noexcept
{
    return
        (std::is_same<curr_type, type_to_find>::value && pos>= start_pos) ||
        std::is_same<curr_type, end_guard>::value ?
            pos : first_pos_of<type_to_find, other_types...>(pos + 1, start_pos);
}

template<typename pack, typename result_pack>
struct remove_duplicates_impl;

template<
    template<typename...> class pack,
    typename curr_type,
    typename... other_types,
    typename... result_types>
struct remove_duplicates_impl<pack<curr_type, other_types...>, pack<result_types...>> :
    std::conditional_t<
        has_types_v<pack<other_types...>, curr_type>,
        remove_duplicates_impl<pack<other_types...>, pack<result_types...>>,
        remove_duplicates_impl<pack<other_types...>, pack<result_types..., curr_type>>> {};

template<template<typename...> class pack, typename... result_types>
struct remove_duplicates_impl<pack<>, pack<result_types...>>
{
    using type = pack<result_types...>;
};

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

// type_at_pos

template<
    template<typename...> class pack,
    typename curr_type,
    typename... other_types,
    size_t pos>
struct type_at_pos<pack<curr_type, other_types...>, pos>
{
    static_assert(pos < sizeof...(other_types) + 1, "Type position is out of range");
    using type = type_at_pos_t<pack<other_types...>, pos - 1>;
};

template<
    template<typename...> class pack,
    typename curr_type,
    typename... other_types>
struct type_at_pos<pack<curr_type, other_types...>, 0>
{
    using type = curr_type;
};

// first_pos_of

template<
    template<typename...> class pack,
    typename... types,
    typename type_to_match,
    size_t start_pos>
struct first_pos_of<pack<types...>, type_to_match, start_pos>
{
    static constexpr size_t value{ sizeof...(types) > 0?
        detail::first_pos_of<type_to_match, types...>(0, start_pos) : 
        1 };
};

// has_types

template<typename pack, typename... types>
struct has_types
{
    static constexpr bool value{ detail::eval<
      detail::logic_and,
      detail::bool_seq<pack_predicates<pack>::template has_type<types>::value...>>::value 
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

// remove_duplicates

template<template<typename...> class pack, typename... types>
struct remove_duplicates<pack<types...>> :
    detail::remove_duplicates_impl<pack<types...>, pack<>> {};

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

    template<typename type_to_specify>
    using type = detail::eval<
        detail::logic_and,
        detail::bool_seq<predicates<type_to_specify>::value...>>;
};

template<template<typename> class... predicates>
struct or_
{
    static_assert(sizeof...(predicates) > 1, "Required at least two predicates");

    template<typename type_to_specify>
    using type = detail::eval<
        detail::logic_or,
        detail::bool_seq<predicates<type_to_specify>::value...>>;
};

}// type_pack

#endif
