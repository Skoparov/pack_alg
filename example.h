#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "type_pack.h"

using namespace type_pack;

struct speed {};
struct health {};
struct damage {};
struct flying {};
struct huge {};
struct lightweight {};
struct fat {};

template< typename... components >
struct entity : components...{};

template< typename entity_type, typename... components >
using add_components = remove_duplicates_t< add_types_t< entity_type, components... > >;

template< typename... entity_types >
using combine_all = remove_duplicates_t< concat_t< entity_types... > >;

template< typename entity_type >
constexpr bool is_flying_v = has_types_v< entity_type, flying, lightweight >;

template< typename entity_type >
constexpr bool is_huge_v = has_types_v< entity_type, huge >;

template<typename entity_type>
using cripple = remove_types_t< entity_type, speed >;

template<typename entity_type>
using make_fat = add_types_t< remove_types_if_t< entity_type, of< flying, lightweight >::fits_any >, fat >;

template<typename entity_type>
using make_huge = add_types_t< entity_type, huge >;

template< typename entity_type >
using make_fat_if_flying = std::conditional_t
<
	is_flying_v< entity_type >,
	make_fat< entity_type >,
	entity_type
>;

template< typename entity_type, typename component_type >
struct traits
{
	template< typename type >
	struct if_flying
	{
		static constexpr bool value{ std::is_same<type, component_type>::value && is_flying_v< entity_type > };
	};

	template< typename type >
	struct if_huge
	{
		static constexpr bool value{ std::is_same<type, component_type>::value && is_huge_v< entity_type > };
	};
};

template< typename entity_type >
using cripple_if_flying_or_huge_t = remove_types_if_t
<
	entity_type,
	or_
	<
		traits< entity_type, speed >::template if_flying,
		traits< entity_type, speed >::template if_huge
	>::template type
>;

//

using creature = entity< health >;

using mobile_creature = add_components< creature, speed >;
static_assert(std::is_same< mobile_creature, entity< health, speed > >::value, "");

using predator = add_components< creature, damage >;
static_assert(std::is_same< predator, entity< health, damage > >::value, "");

using mobile_predator = combine_all< mobile_creature, predator >;
static_assert(std::is_same< mobile_predator, entity< speed, health, damage > >::value, "");

using flying_predator = add_components< mobile_predator, flying, lightweight >;
static_assert(std::is_same< flying_predator, entity< speed, health, damage, flying, lightweight > >::value, "");

using crippled_predator = cripple< mobile_predator >;
static_assert(std::is_same< crippled_predator, entity< health, damage > >::value, "");

using tiger = mobile_predator;
using eagle = flying_predator;
using griphin = flying_predator;
using hawk = flying_predator;
using huge_tiger = make_huge< tiger >;

using fat_eagle = make_fat< eagle >;
static_assert(std::is_same< fat_eagle, entity< speed, health, damage, fat > >::value, "");

using fat_hawk = make_fat_if_flying< hawk >;
static_assert(std::is_same< fat_eagle, fat_hawk >::value, "");

using fit_tiger = make_fat_if_flying< tiger >;
static_assert(std::is_same< tiger, fit_tiger >::value, "");

using crippled_huge_tiger = cripple_if_flying_or_huge_t< huge_tiger >;
static_assert(std::is_same< crippled_huge_tiger, entity< health, damage, huge > >::value, "");

using not_crippled_tiger = cripple_if_flying_or_huge_t< tiger >;
static_assert(std::is_same< not_crippled_tiger, tiger >::value, "");

#endif
