# type_pack
A set of C++ type pack tools

####  pack
```
template<typename... types>
struct pack {};
```
A basic type pack struct provided for convenience.

## Type traits and 'functions'

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

####  type_at_pos
```
template<typename pack_type, size_t pos>
struct type_at_pos;

template<typename pack_type, size_t pos>
using type_at_pos_t = typename type_at_pos<pack_type, pos>::type;
``` 
Gets the type at the specified position. If the position is out of range, a static_assert is triggered.

####  first_pos_of
```
template<typename pack_type, typename type, size_t start_pos = 0>
struct first_pos_of; // value = position/end_v<pack> if type not found

template<typename pack_type, typename type, size_t start_pos>
constexpr size_t first_pos_of_v = first_pos_of<pack_type, type, start_pos>::value;
``` 
Finds the position of the first occurance of the type in the type pack. Evaluates to either the found position or end_v

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
template<typename pack_type, template<typename> class predicate, typename... types_to_add>
struct add_types_if;

template<typename pack_type, template<typename> class predicate, typename... types_to_add>
using add_types_if_t = typename add_types_if<pack_type, predicate, types_to_add...>::type;
``` 
Appends the types that satisfy the predicate's condition to the pack.

####  add_types
```
template<typename pack_type, typename... types_to_add>
struct add_types;

template<typename pack_type, typename... types_to_add>
using add_types_t = add_types_if_t<pack_type, always, types_to_add...>;
``` 
Unconditionally appends the specified types to the pack.

####  remove_types_if
```
template<typename pack_type, template<typename> class predicate>
struct remove_types_if;

template<typename pack_type, template<typename> class predicate>
using remove_types_if_t = typename remove_types_if<pack_type, predicate>::type;
``` 
Removes the types that satisfy the predicate's condition from the pack.

####  remove_types
```
template<typename pack_type, typename... types_to_remove>
struct remove_types;

template<typename pack_type, typename... types_to_remove>
using remove_types_t = typename remove_types_if<pack_type, of<types_to_remove...>::template fits_any>::type;
``` 
Unconditionally removes the specified types from the pack.

####  remove_duplicates
```
template<typename pack_type>
struct remove_duplicates;

template<typename pack_type>
using remove_duplicates_t = typename remove_duplicates<pack_type>::type;
``` 
Creates a new pack with no duplicate types.

####  filter
```
template<typename pack, template<typename> class predicate>
struct filter;

template<typename pack, template<typename> class predicate>
using filter_t = typename filter<pack, predicate>::type;
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
A predicate is a template structure specialized by a type and containing a boolean result value, for example:
```
template<typename type>
struct is_int
{
  static constexpr bool value{ std::is_same_v<type, int> };
}
``` 

The following convenience predicates are predefined:
####  always
```
template<typename> 
using always = std::true_type;
``` 
Evaluates to true regardless of the type

####  never
```
template<typename> 
using never = std::false_type;
``` 
Evaluates to false regardless of the type

####  and_
```
template< template< typename > class... predicates >
struct and_;
``` 
Contains a predicate that applies ```logical and``` to the results of the specified predicates.

####  or_
```
template< template< typename > class... predicates >
struct or_;
``` 
Contains a predicate that applies ```logical or``` to the results of the specified predicates.

####  not_
```
template<template<typename> class predicate>
struct not_
``` 
Contains a predicate that applies ```logical not``` to the result of the predicate.

####  of::fits_any/of::fits_none
```
template<typename... types>
struct of
{
	template<typename type>
	using fits_any = has_types<pack<types...>, type>;

	template<typename type>
	using fits_none = not_<fits_any<type>>;
};
``` 
A predicate 'factory' that contains two convenience predicate.

```of<types...>::fits_any<type>``` holds true if ```type``` is found amoung the ```types```

```of<types...>::fits_none<type>``` holds true if ```type``` is not found amoung the ```types```

####  from::fits_any/from::fits_none
```
template<typename pack>
struct from
```

Same as above, but is specialized by type pack classes.

####  pack_predicates
```
template<typename pack>
struct pack_predicates
{
    template<typename type>
    struct has_type
    {
        static constexpr bool value{ first_pos_of<pack, type>::value != end_v<pack> };
    };

    template<typename type>
    struct has_type_no_dup
    {
        static constexpr bool value{ has_types_no_dup_v<pack, type> };
    };
};
```

A predicate 'factory' that constains ```has_type``` and ```has_type_no_dup``` predicates. 
