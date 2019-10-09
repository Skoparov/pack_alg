#pragma once
#include "palg.h"

using namespace palg;

struct body_type_related {};
struct size_related {};
struct movement_related {};
struct combat_related {};

struct speed : movement_related {};
struct health : combat_related {};
struct damage : combat_related {};
struct flying : movement_related {};
struct huge : size_related {};
struct lightweight : body_type_related {};
struct fat : body_type_related {};

template<typename... components> struct entity;

template<typename entity_type, typename... components>
using add_components = unique_t<add_types_t<entity_type, components...>>;

template<typename entity_type, typename predicate>
using get_components_if = filter_t<entity_type, predicate>;

template<typename... entity_types>
using combine_all = unique_t<concat_t<entity_types...>>;

template<typename entity_type>
constexpr bool is_flying_v = has_types_no_dup_v<entity_type, flying, lightweight>;

template<typename entity_type>
constexpr bool is_huge_v = has_types_no_dup_v<entity_type, huge>;

template<typename entity_type>
using cripple = remove_types_t<entity_type, speed>;

template<typename entity_type>
using make_fat = add_types_t<remove_types_if_t<entity_type, any_of<flying, lightweight>>, fat>;

template<typename entity_type>
using make_huge = add_types_t<entity_type, huge>;

template<typename entity_type>
using make_fat_if_flying = std::conditional_t<
    is_flying_v<entity_type>,
    make_fat<entity_type>,
    entity_type>;

using is_body_type_related = std::is_base_of<body_type_related, _1>;
using is_size_related = std::is_base_of<size_related, _1>;

template<typename entity>
using get_body_type_and_size_related = get_components_if<
    entity,
    or_<is_body_type_related, is_size_related>>;

//

using creature = entity<health>;

using mobile_creature = add_components<creature, speed>;
static_assert(std::is_same_v<mobile_creature, entity<health, speed>>);

using predator = add_components<creature, damage>;
static_assert(std::is_same_v<predator, entity<health, damage>>);

using mobile_predator = combine_all<mobile_creature, predator>;
static_assert(std::is_same_v<mobile_predator, entity<health, speed, damage>>);

using flying_predator = add_components<mobile_predator, flying, lightweight>;
static_assert(std::is_same_v<flying_predator, entity<health, speed, damage, flying, lightweight>>);

using crippled_predator = cripple<mobile_predator>;
static_assert(std::is_same_v<crippled_predator, entity<health, damage>>);

using tiger = mobile_predator;
using eagle = flying_predator;
using griphin = flying_predator;
using hawk = flying_predator;
using huge_tiger = make_huge<tiger>;
using huge_fat_tiger = make_fat<huge_tiger>;

using fat_eagle = make_fat<eagle>;
static_assert(std::is_same_v<fat_eagle, entity<health, speed, damage, fat>>);

using fat_hawk = make_fat_if_flying<hawk>;
static_assert(std::is_same_v<fat_eagle, fat_hawk>);

using fit_tiger = make_fat_if_flying<tiger>;
static_assert(std::is_same_v<tiger, fit_tiger>);

using huge_and_fat = get_body_type_and_size_related<huge_fat_tiger>;
static_assert(std::is_same_v<huge_and_fat, entity<huge, fat>>);