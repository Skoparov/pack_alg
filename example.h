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
using add_components = remove_duplicates_t<add_types_t<entity_type, components...>>;

template<typename entity_type, template<typename> class predicate>
using get_components_if = filter_t<entity_type, predicate>;

template<typename... entity_types>
using combine_all = remove_duplicates_t<concat_t<entity_types...>>;

template<typename entity_type>
constexpr bool is_flying_v = has_types_no_dup_v<entity_type, flying, lightweight>;

template<typename entity_type>
constexpr bool is_huge_v = has_types_no_dup_v<entity_type, huge>;

template<typename entity_type>
using cripple = remove_types_t<entity_type, speed>;

template<typename entity_type>
using make_fat = add_types_t<remove_types_if_t<entity_type, of<flying, lightweight>::fits_any>, fat>;

template<typename entity_type>
using make_huge = add_types_t<entity_type, huge>;

template<typename entity_type>
using make_fat_if_flying = std::conditional_t
<
	is_flying_v<entity_type>,
	make_fat<entity_type>,
	entity_type
>;

template<typename component>
using is_body_type_related = std::is_base_of<body_type_related, component>;

template<typename component>
using is_size_related = std::is_base_of<size_related, component>;

template<typename entity>
using get_body_type_and_size_related = get_components_if<
    entity,
    or_<is_body_type_related, is_size_related>:: template pred>;

//

using creature = entity<health>;

using mobile_creature = add_components<creature, speed>;
static_assert(std::is_same<mobile_creature, entity<health, speed>>::value);

using predator = add_components<creature, damage>;
static_assert(std::is_same<predator, entity<health, damage>>::value);

using mobile_predator = combine_all<mobile_creature, predator>;
static_assert(std::is_same<mobile_predator, entity<health, speed, damage>>::value);

using flying_predator = add_components<mobile_predator, flying, lightweight>;
static_assert(std::is_same<flying_predator, entity<health, speed, damage, flying, lightweight>>::value);

using crippled_predator = cripple<mobile_predator>;
static_assert(std::is_same<crippled_predator, entity<health, damage>>::value);

using tiger = mobile_predator;
using eagle = flying_predator;
using griphin = flying_predator;
using hawk = flying_predator;
using huge_tiger = make_huge<tiger>;
using huge_fat_tiger = make_fat<huge_tiger>;

using fat_eagle = make_fat<eagle>;
static_assert(std::is_same<fat_eagle, entity<health, speed, damage, fat>>::value);

using fat_hawk = make_fat_if_flying<hawk>;
static_assert(std::is_same<fat_eagle, fat_hawk>::value);

using fit_tiger = make_fat_if_flying<tiger>;
static_assert(std::is_same<tiger, fit_tiger>::value);

using body_type_and_size_related = get_body_type_and_size_related<huge_fat_tiger>;
static_assert(std::is_same<body_type_and_size_related, entity<huge, fat>>::value);
