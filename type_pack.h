#ifndef TYPE_PACK_H
#define TYPE_PACK_H

#include <cstddef>
#include <type_traits>

namespace type_pack
{

template< typename... types >
struct pack; // default pack declaration provided for convenience

template< typename pack_type >
struct pack_size;

template< typename pack_type >
struct end; // value = size< pack_type >::value or 1 if pack is empty

template< typename pack_type, size_t pos >
struct type_at_pos;

template< typename pack_type, typename type, size_t start_pos = 0 >
struct first_pos_of; // value = position or end<pack>::value if type not found

template< typename pack_type, typename... types_to_find >
struct has_types;

template< typename pack_type, template< typename > class predicate, typename... types_to_add >
struct add_types_if;

template< typename pack_type, typename... types_to_add >
struct add_types;

template< typename pack_type, template< typename > class predicate >
struct remove_types_if; // predicate should have bool 'value' set to true or false

template< typename pack_type, typename... types_to_remove >
struct remove_types;

template< typename pack_type >
struct remove_duplicates;

template< typename... packs>
struct concat;

// predicates

template< typename > using always = std::true_type;
template< typename > using never = std::false_type;

template< template< typename > class... predicates >
struct and_;

template< template< typename > class... predicates >
struct or_;

template< typename predicate >
struct not_
{
	static constexpr bool value{ !predicate::value };
};

template< typename... types >
struct of
{
	template< typename type >
	using fits_any = has_types< pack< types... >, type >;

	template< typename type >
	using fits_none = not_< fits_any< type > >;
};

// convenience typedefs

template< typename pack_type >
constexpr size_t pack_size_v = pack_size< pack_type >::value;

template< typename pack_type, size_t pos >
using type_at_pos_t = typename type_at_pos< pack_type, pos >::type;
	
template< typename pack_type >
constexpr size_t end_v = end< pack_type >::value;

template< typename pack_type, typename type, size_t start_pos >
constexpr size_t first_pos_of_v = first_pos_of< pack_type, type, start_pos >::value;

template< typename pack_type, typename... types_to_find >
constexpr bool has_types_v = has_types< pack_type, types_to_find... >::value;

template< typename pack_type, template< typename > class predicate, typename... types_to_add >
using add_types_if_t = typename add_types_if< pack_type, predicate, types_to_add... >::type;

template< typename pack_type, typename... types_to_add >
using add_types_t = add_types_if_t< pack_type, always, types_to_add... >;

template< typename pack_type, template< typename > class predicate >
using remove_types_if_t = typename remove_types_if< pack_type, predicate >::type;

template< typename pack_type, typename... types_to_remove >
using remove_types_t = typename remove_types_if< pack_type, of< types_to_remove... >::template fits_any >::type;

template< typename pack_type >
using remove_duplicates_t = typename remove_duplicates< pack_type >::type;

template< typename... packs >
using concat_t = typename concat< packs... >::type;


///////
// impl
///////

namespace detail
{

template< typename pack_type, typename = void >
struct is_type_pack : std::false_type {};

template< template< typename... > class pack_type, typename... types >
struct is_type_pack< pack_type< types... >, void > : std::true_type {};

template< bool value1, bool value2 >
struct logical_and
{
	static constexpr bool value{ value1 && value2 };
	static constexpr bool default_value{ true };
};

template< bool value1, bool value2 >
struct logical_or
{
	static constexpr bool value{ value1 || value2 };
	static constexpr bool default_value{ false };
};

template<
	template< bool, bool > class logic_oper,
	typename type,
	template< typename > class... predicates >
struct eval;

template<
	template< bool, bool > class logical_oper,
	typename type,
	template< typename > class predicate1,
	template< typename > class predicate2,
	template< typename > class... other_predicates >
struct eval< logical_oper, type, predicate1, predicate2, other_predicates... >
{
	static constexpr bool value{
		logical_oper
		<
			logical_oper< predicate1< type >::value, predicate2< type >::value >::value,
			eval< logical_oper, type, other_predicates... >::value
		>::value };
};

template< template< bool, bool > class logical_oper, typename type, template< typename > class predicate >
struct eval< logical_oper, type, predicate >
{
	static constexpr bool value{ predicate< type >::value };
};

template< template< bool, bool > class logical_oper, typename type >
struct eval< logical_oper, type >
{
	static constexpr bool value{ logical_oper< true, true >::default_value };
};

struct guard;

template<
	typename type_to_match,
	typename curr_type = detail::guard,
	typename... other_types >
constexpr size_t first_pos_of(size_t pos, size_t start_pos) noexcept
{
	return
		((std::is_same<curr_type, type_to_match>::value && pos >= start_pos) ||
		std::is_same<curr_type, detail::guard>::value) ?
			pos : first_pos_of<type_to_match, other_types...>(pos + 1, start_pos);
}


template<typename pack_type, template< typename > class predicate, typename result_pack>
struct remove_types_if_helper;

template<
	template< typename... > class pack_type,
	typename curr_type,
	typename... other_types,
	template< typename > class predicate,
	typename... result_types>
struct remove_types_if_helper< pack_type< curr_type, other_types... >, predicate, pack_type< result_types... > >
{
	using type = std::conditional_t
	<
		predicate<curr_type>::value,
		typename remove_types_if_helper< pack_type< other_types... >, predicate, pack_type< result_types... > >::type,
		typename remove_types_if_helper< pack_type< other_types... >, predicate, pack_type< result_types..., curr_type > >::type
	>;
};

template<
	template< typename... > class pack_type,
	template< typename > class predicate,
	typename... result_types >
struct remove_types_if_helper< pack_type<>, predicate, pack_type< result_types... > >
{
	using type = pack_type< result_types... >;
};

template<typename pack_type, typename result_pack>
struct remove_duplicates;

template<
	template< typename... > class pack_type,
	typename curr_type,
	typename... other_types,
	typename... result_types >
	struct remove_duplicates< pack_type< curr_type, other_types... >, pack_type< result_types... > > :
	std::conditional_t
	<
		has_types< pack_type< other_types... >, curr_type >::value,
		remove_duplicates< pack_type< other_types... >, pack_type< result_types... > >,
		remove_duplicates< pack_type< other_types... >, pack_type< result_types..., curr_type > >
	> {};

template< template< typename... > class pack_type, typename... result_types >
struct remove_duplicates< pack_type<>, pack_type< result_types... > >
{
	using type = pack_type< result_types... >;
};

}// detail

template< template< typename... > class pack_type, typename... types >
struct pack_size< pack_type< types... > >
{
	static constexpr size_t value{ sizeof...(types) };
};

template< typename pack_type >
struct pack_size
{
	static_assert(detail::is_type_pack< pack_type >::value, "Pack type is not valid");
};

// end

template< typename pack_type >
struct end
{
	static_assert(detail::is_type_pack< pack_type >::value, "pack type is not valid");
	static constexpr size_t value{ pack_size< pack_type >::value ? pack_size< pack_type >::value : 1 };
};

// type_at_pos

template<
	template<typename...> class pack_type,
	typename curr_type,
	typename... other_types,
	size_t pos >
struct type_at_pos< pack_type< curr_type, other_types... >, pos >
{
	static_assert(pos < sizeof...(other_types)+1, "Type position is out of range");
	using type = typename type_at_pos< pack_type< other_types... >, pos - 1 >::type;
};

template<
	template<typename...> class pack_type,
	typename curr_type,
	typename... other_types >
struct type_at_pos< pack_type< curr_type, other_types... >, 0 >
{
	using type = curr_type;
};

template< typename pack_type, size_t pos >
struct type_at_pos
{
	static_assert(detail::is_type_pack< pack_type >::value, "Pack type is not valid or empty");
};

// first_pos_of

template<
	template< typename... > class pack_type,
	typename... types,
	typename type_to_match,
	size_t start_pos >
struct first_pos_of< pack_type< types... >, type_to_match, start_pos >
{
	static constexpr size_t value{ sizeof...(types) > 0?
		detail::first_pos_of< type_to_match, types... >(0, start_pos) : 1 };
};

template< typename pack_type, typename type, size_t pos_to_start >
struct first_pos_of
{
	static_assert(detail::is_type_pack< pack_type >::value, "pack type is not valid");
};

// types present

template<
	typename pack_type,
	typename curr_type_to_find,
	typename... other_types_to_find >
struct has_types< pack_type, curr_type_to_find, other_types_to_find... > :
	std::conditional_t
	<
		first_pos_of< pack_type, curr_type_to_find >::value != end< pack_type >::value,
		has_types< pack_type, other_types_to_find... >,
		std::false_type
	>
{
	static_assert(detail::is_type_pack<pack_type>::value, "pack_type type is not valid");
};

template< typename pack_type, typename curr_type_to_find >
struct has_types< pack_type, curr_type_to_find > :
	std::conditional_t
	<
		first_pos_of< pack_type, curr_type_to_find >::value != end< pack_type >::value,
		std::true_type,
		std::false_type
	>
{
	static_assert(detail::is_type_pack< pack_type >::value, "Pack type is not valid");
};

// add_types_if

template<
	typename pack_type,
	template< typename > class predicate,
	typename... types_to_add >
struct add_types_if;

template<
	template< typename... > class pack_type,
	typename... types,
	template< typename > class predicate,
	typename curr_type_to_add,
	typename... other_types_to_add >
struct add_types_if< pack_type< types... >, predicate, curr_type_to_add, other_types_to_add... >
{
	using type = std::conditional_t
	<
		predicate<curr_type_to_add >::value,
		typename add_types_if< pack_type< types..., curr_type_to_add >, predicate, other_types_to_add...>::type,
		typename add_types_if< pack_type< types... >, predicate, other_types_to_add... >::type
	>;
};

template<
	template< typename... > class pack_type,
	typename... types,
	template< typename > class predicate >
struct add_types_if< pack_type< types... >, predicate >
{
	using type = pack_type< types... >;
};

// remove_types_if

template<
	template< typename... > class pack_type,
	typename... types,
	template< typename > class predicate >
struct remove_types_if<pack_type< types... >, predicate > :
	detail::remove_types_if_helper< pack_type< types... >, predicate, pack_type<> > {};

template< typename invalid_pack, template< typename > class predicate>
struct remove_types_if
{
	static_assert(detail::is_type_pack< invalid_pack >::value, "Pack type is not valid");
};

// remove_duplicates

template< template< typename... > class pack_type, typename... types >
struct remove_duplicates< pack_type< types... > > :
	detail::remove_duplicates< pack_type< types... >, pack_type<> > {};

template< typename invalid_pack >
struct remove_duplicates
{
	static_assert(detail::is_type_pack< invalid_pack >::value, "Pack type is not valid");
};

// concat

template<
	template< typename... > class pack_type,
	typename... pack1_types,
	typename... pack2_types,
	typename... other_packs >
struct concat< pack_type< pack1_types... >, pack_type< pack2_types... >, other_packs... >
{
	using type = typename concat< pack_type< pack1_types..., pack2_types... >, other_packs... >::type;
};

template<
	template< typename... > class pack_type,
	typename... pack1_types,
	typename... pack2_types >
struct concat< pack_type< pack1_types... >, pack_type< pack2_types... > >
{
	using type = pack_type< pack1_types..., pack2_types... >;
};

// and or

template< template< typename> class... predicates >
struct and_
{
	static_assert(sizeof...(predicates) > 1, "At least two predicates should be provided");

	template< typename type_to_specify >
	using type = detail::eval< detail::logical_and, type_to_specify, predicates...>;
};

template< template< typename> class... predicates >
struct or_
{
	static_assert(sizeof...(predicates) > 1, "At least two predicates should be provided");

	template< typename type_to_specify >
	using type = detail::eval< detail::logical_or, type_to_specify, predicates...>;
};

}// type_pack

#endif
