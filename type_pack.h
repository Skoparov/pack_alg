#ifndef TYPE_PACK_H
#define TYPE_PACK_H

#include <cstddef>
#include <type_traits>

namespace type_pack
{

template< typename... types >
struct pack; // default pack declaration provided for convenience

template< typename type >
struct is_type_pack;

template< typename pack >
struct pack_size;

template< typename pack >
struct end; // value = size< pack >::value or 1 if pack is empty

template< typename pack, size_t pos >
struct type_at_pos;

template< typename pack, typename type, size_t start_pos = 0 >
struct first_pos_of; // value = position or end<pack>::value if type not found

template< typename pack, typename... types >
struct has_types;

template< typename pack, template< typename > class predicate, typename... types >
struct add_types_if;

template< typename pack, typename... types >
struct add_types;

template< typename pack, template< typename > class predicate >
struct remove_types_if; // predicate should have bool 'value' set to true or false

template< typename pack, typename... types >
struct remove_types;

template< typename pack >
struct remove_duplicates;

template< typename... packs >
struct concat;

// predicates

template< typename > using always = std::true_type;
template< typename > using never = std::false_type;

template< template< typename > class... predicates >
struct and_;

template< template< typename > class... predicates >
struct or_;

template< template< typename > class predicate >
struct not_
{
	template< typename spec_type >
	struct type
	{
		static constexpr bool value{ !predicate< spec_type >::value };
	};
};

template< typename... types >
struct of
{
	template< typename spec_type >
	using fits_any = has_types< pack< types... >, spec_type >;

	template< typename spec_type >
	using fits_none = typename not_< fits_any >::template type< spec_type >;
};

// convenience typedefs

template< typename type >
constexpr bool is_type_pack_v = is_type_pack< type >::value;

template< typename pack >
constexpr size_t pack_size_v = pack_size< pack >::value;

template< typename pack, size_t pos >
using type_at_pos_t = typename type_at_pos< pack, pos >::type;
	
template< typename pack >
constexpr size_t end_v = end< pack >::value;

template< typename pack, typename type, size_t start_pos = 0 >
constexpr size_t first_pos_of_v = first_pos_of< pack, type, start_pos >::value;

template< typename pack, typename... types >
constexpr bool has_types_v = has_types< pack, types... >::value;

template< typename pack, template< typename > class predicate, typename... types >
using add_types_if_t = typename add_types_if< pack, predicate, types... >::type;

template< typename pack, typename... types >
using add_types_t = add_types_if_t< pack, always, types... >;

template< typename pack, template< typename > class predicate >
using remove_types_if_t = typename remove_types_if< pack, predicate >::type;

template< typename pack, typename... types >
using remove_types_t = typename remove_types_if< pack, of< types... >::template fits_any >::type;

template< typename pack >
using remove_duplicates_t = typename remove_duplicates< pack >::type;

template< typename... packs >
using concat_t = typename concat< packs... >::type;


///////
// impl
///////

namespace detail
{

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

struct end_guard;

template<
	typename type_to_find,
	typename curr_type = end_guard,
	typename... other_types >
constexpr size_t first_pos_of(size_t pos, size_t start_pos) noexcept
{
	return
		(std::is_same< curr_type, type_to_find >::value && pos >= start_pos) ||
		std::is_same< curr_type, end_guard >::value ?
			pos : first_pos_of<type_to_find, other_types...>(pos + 1, start_pos);
}

template<typename pack, template< typename > class predicate, typename result_pack>
struct remove_types_if_impl;

template<
	template< typename... > class pack,
	template< typename > class predicate,
	typename curr_type,
	typename... other_types,
	typename... result_types >
struct remove_types_if_impl< pack< curr_type, other_types... >, predicate, pack< result_types... > >
{
	using type = std::conditional_t
	<
		predicate< curr_type >::value,
		typename remove_types_if_impl< pack< other_types... >, predicate, pack< result_types... > >::type,
		typename remove_types_if_impl< pack< other_types... >, predicate, pack< result_types..., curr_type > >::type
	>;
};

template<
	template< typename... > class pack,
	template< typename > class predicate,
	typename... result_types >
struct remove_types_if_impl< pack<>, predicate, pack< result_types... > >
{
	using type = pack< result_types... >;
};

template< typename pack, typename result_pack >
struct remove_duplicates_impl;

template<
	template< typename... > class pack,
	typename curr_type,
	typename... other_types,
	typename... result_types >
	struct remove_duplicates_impl< pack< curr_type, other_types... >, pack< result_types... > > :
	std::conditional_t
	<
		has_types_v< pack< other_types... >, curr_type >,
		remove_duplicates_impl< pack< other_types... >, pack< result_types... > >,
		remove_duplicates_impl< pack< other_types... >, pack< result_types..., curr_type > >
	> {};

template< template< typename... > class pack, typename... result_types >
struct remove_duplicates_impl< pack<>, pack< result_types... > >
{
	using type = pack< result_types... >;
};

}// detail

// is_type_pack

template< template< typename... > class pack, typename... types >
struct is_type_pack< pack< types... > >
{
	static constexpr bool value{ true };
};

template< typename pack >
struct is_type_pack
{
	static constexpr bool value{ false };
};

// pack_size

template< template< typename... > class pack, typename... types >
struct pack_size< pack< types... > >
{
	static constexpr size_t value{ sizeof...(types) };
};

template< typename pack >
struct pack_size
{
	static_assert(is_type_pack_v< pack >, "Pack type is not valid");
};

// end

template< typename pack >
struct end
{
	static_assert(is_type_pack_v< pack >, "pack type is not valid");
	static constexpr size_t value{ pack_size_v< pack > ? pack_size_v< pack > : 1 };
};

// type_at_pos

template<
	template<typename...> class pack,
	typename curr_type,
	typename... other_types,
	size_t pos >
struct type_at_pos< pack< curr_type, other_types... >, pos >
{
	static_assert(pos < sizeof...(other_types) + 1, "Type position is out of range");
	using type = type_at_pos_t< pack< other_types... >, pos - 1 >;
};

template<
	template<typename...> class pack,
	typename curr_type,
	typename... other_types >
struct type_at_pos< pack< curr_type, other_types... >, 0 >
{
	using type = curr_type;
};

template< typename pack, size_t pos >
struct type_at_pos
{
	static_assert(is_type_pack_v< pack >, "Pack type is not valid or empty");
};

// first_pos_of

template<
	template< typename... > class pack,
	typename... types,
	typename type_to_match,
	size_t start_pos >
struct first_pos_of< pack< types... >, type_to_match, start_pos >
{
	static constexpr size_t value{ sizeof...(types) > 0?
		detail::first_pos_of< type_to_match, types... >(0, start_pos) : 1 };
};

template< typename pack, typename type, size_t pos_to_start >
struct first_pos_of
{
	static_assert(is_type_pack_v< pack >, "Pack type is not valid");
};

// types present

template<
	typename pack,
	typename curr_type,
	typename... other_types >
struct has_types< pack, curr_type, other_types... > :
	std::conditional_t
	<
		first_pos_of_v< pack, curr_type > != end_v< pack >,
		has_types< pack, other_types... >,
		std::false_type
	>
{
	static_assert(is_type_pack_v< pack >, "pack type is not valid");
};

template< typename pack, typename curr_type_to_find >
struct has_types< pack, curr_type_to_find > :
	std::conditional_t
	<
		first_pos_of< pack, curr_type_to_find >::value != end_v< pack >,
		std::true_type,
		std::false_type
	>
{
	static_assert(is_type_pack_v< pack >, "Pack type is not valid");
};

// add_types_if

template<
	typename pack,
	template< typename > class predicate,
	typename... types >
struct add_types_if;

template<
	template< typename... > class pack,
	typename... types,
	template< typename > class predicate,
	typename curr_type,
	typename... other_types >
struct add_types_if< pack< types... >, predicate, curr_type, other_types... >
{
	using type = std::conditional_t
	<
		predicate<curr_type >::value,
		add_types_if_t< pack< types..., curr_type >, predicate, other_types...>,
		add_types_if_t< pack< types... >, predicate, other_types... >
	>;
};

template<
	template< typename... > class pack,
	typename... types,
	template< typename > class predicate >
struct add_types_if< pack< types... >, predicate >
{
	using type = pack< types... >;
};

// remove_types_if

template<
	template< typename... > class pack,
	typename... types,
	template< typename > class predicate >
struct remove_types_if< pack< types... >, predicate > :
	detail::remove_types_if_impl< pack< types... >, predicate, pack<> > {};

template< typename invalid_pack, template< typename > class predicate>
struct remove_types_if
{
	static_assert(is_type_pack_v< invalid_pack >, "Pack type is not valid");
};

// remove_duplicates

template< template< typename... > class pack, typename... types >
struct remove_duplicates< pack< types... > > :
	detail::remove_duplicates_impl< pack< types... >, pack<> > {};

template< typename invalid_pack >
struct remove_duplicates
{
	static_assert(is_type_pack_v< invalid_pack >, "Pack type is not valid");
};

// concat

template<
	template< typename... > class pack,
	typename... pack1_types,
	typename... pack2_types,
	typename... other_packs >
struct concat< pack< pack1_types... >, pack< pack2_types... >, other_packs... >
{
	using type = concat_t< pack< pack1_types..., pack2_types... >, other_packs... >;
};

template<
	template< typename... > class pack,
	typename... pack1_types,
	typename... pack2_types >
struct concat< pack< pack1_types... >, pack< pack2_types... > >
{
	using type = pack< pack1_types..., pack2_types... >;
};

// and or

template< template< typename> class... predicates >
struct and_
{
	// static_assert(sizeof...(predicates) > 1, "At least two predicates should be provided");

	template< typename type_to_specify >
	using type = detail::eval< detail::logical_and, type_to_specify, predicates...>;
};

template< template< typename> class... predicates >
struct or_
{
	// static_assert(sizeof...(predicates) > 1, "At least two predicates should be provided");

	template< typename type_to_specify >
	using type = detail::eval< detail::logical_or, type_to_specify, predicates...>;
};

}// type_pack

#endif
