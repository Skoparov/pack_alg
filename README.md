# pack_alg
A set of C++ type pack algorithms.

####  pack
```
template<typename... types>
struct pack {};
```
A basic type pack struct provided for convenience.

## Algorithms

####  pack_size
```
template<typename pack_type>
struct pack_size;

template<typename pack_type>
constexpr size_t pack_size_v = pack_size<pack_type>::value;
``` 
Holds the size of the specified type pack.

####  end
```
template<typename pack_type>
struct end; // pack size + 1

template<typename pack_type>
constexpr size_t end_v = end<pack_type>::value;
``` 
Imitates the end() iterator for type packs. Evaluates to size of the pack + 1.

####  type_at
```
template<typename pack_type, size_t pos>
struct type_at;

template<typename pack_type, size_t pos>
using type_at_t = typename type_at<pack_type, pos>::type;
``` 
Gets the type at the specified position. If the position is out of range, a static_assert is triggered.

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
template<typename pack_type, typename... types_to_find>
struct has_types;

template<typename pack_type, typename... types_to_find>
constexpr bool has_types_v = has_types<pack_type, types_to_find...>::value;
``` 
Determines whether the specified types are present in the pack.

####  has_types_no_dup
```
template<typename pack_type, typename... types_to_find>
struct has_types_no_dup;

template<typename pack_type, typename... types_to_find>
constexpr bool has_types_no_dup_v = has_types_no_dup<pack_type, types_to_find...>::value;
``` 
Determines whether the specified types are present in the pack in O(1) time. Only applicable if there are no duplicates in the pack.

####  add_types_if
```
template<typename pack_type, typename predicate, typename... types_to_add>
struct add_types_if;

template<typename pack_type, typename predicate, typename... types_to_add>
using add_types_if_t = typename add_types_if<pack_type, predicate, types_to_add...>::type;
``` 
Appends the types that satisfy the predicate's condition to the pack.

####  add_types
```
template<typename pack_type, typename... types_to_add>
using add_types_t = add_types_if_t<pack_type, always, types_to_add...>;
``` 
Unconditionally appends the specified types to the pack.

####  remove_types_if
```
template<typename pack_type, typename predicate>
struct remove_types_if;

template<typename pack_type, typename predicate>
using remove_types_if_t = typename remove_types_if<pack_type, predicate>::type;
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
template<typename pack_type>
struct unique;

template<typename pack_type>
using unique_t = typename unique<pack_type>::type;
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