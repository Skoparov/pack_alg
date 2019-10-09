# Palg
A collection of C++ compile time algorithms.

####  pack
```
template<typename... types>
struct pack {};
```
A basic type pack struct provided for convenience.

## Algorithms

####  pack_size
```
template<typename pack>
struct pack_size;

template<typename pack>
constexpr size_t pack_size_v = pack_size<pack>::value;
``` 
Calculates the size of the type pack.

####  end
```
template<typename pack>
struct end; // pack size + 1

template<typename pack>
constexpr size_t end_v = end<pack>::value;
``` 
Imitates the end() iterator for type packs. Evaluates to the size of the pack + 1.

####  copy_to
```
template<typename pack, template<typename...> class dest>
struct copy_to;

template<typename pack, template<typename...> class dest>
using copy_to_t = typename copy_to<pack, dest>::type;
``` 
Creates a new pack of type 'dest' with the same types as the source pack.

####  type_at
```
template<typename pack, size_t pos>
struct type_at;

template<typename pack, size_t pos>
using type_at_t = typename type_at<pack, pos>::type;
``` 
Gets the type at the specified position. If the position is out of range, a static_assert is triggered.

####  front/back
```
template<typename pack>
using front_t = type_at_t<pack, 0>;

template<typename pack>
using back_t = type_at_t<pack, end_v<pack> - 1>;
``` 
Gets the type at the front/back position.

####  find_if
```
template<typename pack, typename predicate, size_t start_pos = 0>
struct find_if; 

template<typename pack, typename predicate, size_t start_pos = 0>
constexpr size_t find_if_v = find_if<pack, predicate, start_pos>::value;
``` 
Finds the position of a first type satisfying the predicate. Evaluates to either the found position or end_v

####  find
```
template<typename pack, typename type, size_t start_pos = 0>
constexpr size_t find_v = find_if_v<pack, any_of_no_dup<type>, start_pos>;
``` 
Finds the position of the first occurance of the type in the pack. Evaluates to either the found position or end_v

####  enumerate_if
```
template<typename pack, typename predicate>
struct enumerate_if; // value = index_sequence<idxs of types satisfying predicate...>

template<typename pack, typename predicate>
using enumerate_if_t = typename enumerate_if<pack, predicate>::type;
``` 
Constructs an index sequence for types satisfying the predicate.

####  enumerate_if
```
template<typename pack, typename type>
using enumerate_t = typename enumerate_if<pack, any_of_no_dup<type>>::type;
``` 
Constructs an index sequence for types same as the provided type.

####  has_types
```
template<typename pack, typename... types_to_find>
struct has_types;

template<typename pack, typename... types_to_find>
constexpr bool has_types_v = has_types<pack, types_to_find...>::value;
``` 
Determines whether the specified types are present in the pack.

####  has_types_no_dup
```
template<typename pack, typename... types_to_find>
struct has_types_no_dup;

template<typename pack, typename... types_to_find>
constexpr bool has_types_no_dup_v = has_types_no_dup<pack, types_to_find...>::value;
``` 
Determines whether the specified types are present in the pack in O(1) time. Only applicable if there are no duplicates in the pack.

####  append_if
```
template<typename pack, typename predicate, typename... types_to_add>
struct append_if;

template<typename pack, typename predicate, typename... types_to_add>
using append_if_t = typename append_if<pack, predicate, types_to_add...>::type;
``` 
Appends the types that satisfy the predicate's condition to the pack.

####  append
```
template<typename pack, typename... types_to_add>
struct append;

template<typename pack, typename... types_to_add>
using append_t = typename append<pack, types_to_add...>::type;
``` 
Appends the types to the pack.

####  prepend_if
```
template<typename pack, typename predicate, typename... types_to_add>
struct prepend_if;

template<typename pack, typename predicate, typename... types_to_add>
using prepend_if_t = typename prepend_if<pack, predicate, types_to_add...>::type;
``` 
Prepends the types that satisfy the predicate's condition to the pack.

####  prepend
```
template<typename pack, typename... types_to_add>
struct prepend;

template<typename pack, typename... types_to_add>
using prepend_t = typename prepend<pack, types_to_add...>::type;
``` 
Prepends the types to the pack.

####  remove_types_if
```
template<typename pack, typename predicate>
struct remove_types_if;

template<typename pack, typename predicate>
using remove_types_if_t = typename remove_types_if<pack, predicate>::type;
``` 
Removes the types that satisfy the predicate's condition from the pack.

####  remove_types
```
template<typename pack, typename... types>
using remove_types_t = remove_types_if_t<pack, any_of<types...>>;
``` 
Unconditionally removes the specified types from the pack.

####  unique
```
template<typename pack>
struct unique;

template<typename pack>
using unique_t = typename unique<pack>::type;
``` 
Creates a new pack with no duplicate types.

####  filter
```
template<typename pack, typename predicate>
using filter_t = remove_types_if_t<pack, not_<predicate>>;
``` 
Creates a pack comprised of the original types that satisfy the predicate's condition.

####  concat
```
template<typename... packs>
struct concat;

template<typename... packs>
using concat_t = typename concat<packs...>::type;
``` 
Concatenates several type packs.

####  invert
```
template<typename pack>
struct invert;

template<typename pack>
using invert_t = typename invert<pack>::type;
``` 
Inverts the order of types in the type pack (i.e pack<int, double> -> pack<double, int>).

####  transform
```
template<typename pack, typename type_predicate>
struct transform;

template<typename pack, typename type_predicate>
using transform_t = typename transform<pack, type_predicate>::type;
``` 
Transforms the types in the type pack into types yielded by the type predicate.
Placeholders can be used to specialize template type predicates.
The predicate will be specialized by each type in the pack, so it should not have more that one placeholder.
The algorightm supports predicate nesting:
```
using const_int_pack = transform_t<pack<int>, std::add_pointer<std::add_const<_1>>>; // pack<const int*>
```
If the predicate contains a 'type' typedef(like the one above), it will be used as the evaluation result, otherwise the predicate type itself
will be used, replacing all types in the pack:
```
using double_pack = transform_t<pack<int>, double>; // pack<double>
```

##  Predicates

A predicate is a template structure fully specialized by types/placeholders and containing a boolean value:
```
using is_int = std::is_same<int, pack_alg::_1>;

``` 
Where pack_alg::_1 is one of the of predefined placeholders:
```
using _1 = placeholder;
using _2 = placeholder;
using _3 = placeholder;
using _4 = placeholder;
using _5 = placeholder;
```
Predicates can be combined and modified using the basic logic predicates:
```
using is_pod_class = and_<std::pod<_1>, std::is_class<_1>>;
using is_not_pod_class = not_<is_pod_class>;
using is_int_or_double = or_<std::is_same<int, _1>, std::is_same<double, _1>>;
```
A predicate can be used with any of the algorithms supporting them, for example:
```
using int_double_pack = filter_t<pack<int, double, void>, is_int_or_double>;
```

The following convenience predicates are predefined:
####  always
```
using always = std::true_type;
``` 
Always evaluates to true

####  never
```
using never = std::false_type;
``` 
Always evaluates to false

####  and_
```
template<typename... predicates>
struct and_;
``` 
Applies ```logical and``` to the results of the specified predicates.

####  or_
```
template<typename... predicates>
struct and_;
``` 
Applies ```logical or``` to the results of the specified predicates.

####  not_
```
template<typename predicate>
struct not_
``` 
Applies ```logical not``` to the result of the provided predicate.

####  any_of
```
template<typename... types>
using any_of = has_types<pack<types...>, _1>;
``` 
Evaluates to true if the predicate is specialized with a type present in the pack.

####  any_of_no_dup
```
template<typename... types>
using any_of = has_types<pack<types...>, _1>;
``` 
Same as above, but works in O(1) time. Only applicable if there are no duplicates in the pack.

####  none_of
```
template<typename... types>
using none_of = not_<any_of<types...>>;
``` 
Evaluates to false if the predicate is specialized with a type present in the pack.

####  none_of_no_dup
```
template<typename... types>
using none_of_no_dup = not_<any_of_no_dup<types...>>;
``` 
Same as above, but works in O(1) time. Only applicable if there are no duplicates in the pack.