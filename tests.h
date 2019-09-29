#pragma once

#include "type_pack.h"

using namespace type_pack;

void test_size()
{
	static_assert(pack_size_v<pack<>> == 0);
	static_assert(pack_size_v<pack<void>> == 1);
	static_assert(pack_size_v<pack<void, double>> == 2);
}

void test_first_pos_of()
{
	static_assert(first_pos_of_v<pack<bool, void, bool>, bool> == 0);
	static_assert(first_pos_of_v<pack<bool, void, bool>, bool, 1> == 2);
	static_assert(first_pos_of_v<pack<bool, void, bool>, void> == 1);
	static_assert(first_pos_of_v<pack<bool, void, bool>, void, 1> == 1);
	static_assert(first_pos_of_v<pack<bool, void, bool>, float> == 3);
	static_assert(first_pos_of_v<pack<>, float> == 1);
}

void test_has_types()
{
	static_assert(has_types_v<pack<bool, void, double>, bool>);
	static_assert(has_types_v<pack<bool, void, double>, bool, void, double>);
	static_assert(!has_types_v<pack<int, int>, bool>);
	static_assert(!has_types_v<pack<int, int>, bool, int>);
	static_assert(!has_types_v<pack<>, bool, int>);
}

void test_type_at_pos()
{
	using void_type = type_at_pos_t<pack<void>, 0>;
	static_assert(std::is_same_v<void_type, void>);

    using double_type = type_at_pos_t<pack<void, double>, 1>;
	static_assert(std::is_same_v<double_type, double>);
}

void test_add_types_if()
{
	using int_double_pack = add_types_if_t<pack<>, of<int, double>::fits_any, int, double>;
	static_assert(std::is_same_v<int_double_pack, pack<int, double>>);

	using empty_pack = add_types_if_t<pack<>, of<int, double>::fits_none, int, double>;
	static_assert(std::is_same_v<empty_pack, pack<>>);

	using int_pack = add_types_if_t<pack<>, of<int>::fits_any, int, double>;
	static_assert(std::is_same_v<int_pack, pack<int>>);

	using int_double_int_pack = add_types_if_t<pack<int, double>, of<int>::fits_any, int>;
	static_assert(std::is_same_v<int_double_int_pack, pack<int, double, int>>);

	using empty_pack2 = add_types_if_t<pack<>, of<int>::fits_any>;
	static_assert(std::is_same_v<empty_pack2, pack<>>);

	using int_double_pack2 = add_types_t<pack<>, int, double>;
	static_assert(std::is_same_v<int_double_pack2, pack<int, double>>);
}

void test_concat()
{
    static_assert(std::is_same_v<concat_t<pack<void>>, pack<void>>);

	using void_double_pack = concat_t<pack<>, pack<void, double>>;
	static_assert(std::is_same_v<void_double_pack, pack<void, double>>);

	using void_double_pack2 = concat_t<pack<void, double>, pack<>>;
	static_assert(std::is_same_v<void_double_pack2, pack<void, double>>);

	using void_int_double_pack = concat_t<pack<void, int>, pack<double>>;
	static_assert(std::is_same_v<void_int_double_pack, pack<void, int, double>>);

	using void_int_double_pack2 = concat_t<pack<void>, pack<int>, pack<double>>;
	static_assert(std::is_same_v<void_int_double_pack2, pack<void, int, double>>);
}

void test_invert()
{
	static_assert(std::is_same_v<pack<>, invert_t<pack<>>>);
	static_assert(std::is_same_v<pack<void>, invert_t<pack<void>>>);

    using pack_double_void = invert_t<pack<void, double>>;
	static_assert(std::is_same_v<pack<double, void>, pack_double_void>);
}

void test_remove_types_if()
{
    using empty_pack = remove_types_if_t<pack<>, of<int>::fits_any>;
    static_assert(std::is_same<pack<>, empty_pack>::value);

	using double_pack = remove_types_if_t<pack<int, double>, of<int>::fits_any>;
	static_assert(std::is_same_v<double_pack, pack<double>>);

	using empty_pack = remove_types_if_t<pack<int, double>, of<int, double>::fits_any>;
	static_assert(std::is_same_v<empty_pack, pack<>>);

	using int_double_pack = remove_types_if_t<pack<int, double>, of<int, double>::fits_none>;
	static_assert(std::is_same_v<int_double_pack, pack<int, double>>);

	using empty_pack2 = remove_types_t<pack<int, double>, int, double>;
	static_assert(std::is_same_v<empty_pack2, pack<>>);
}

void test_remove_duplicates()
{
	using int_pack = remove_duplicates_t<pack<int, int>>;
	static_assert(std::is_same<int_pack, pack<int>>::value);

	using int_double_pack = remove_duplicates_t<pack<int, double, int, double>>;
	static_assert(std::is_same_v<int_double_pack, pack<int, double>>);

	using empty_pack = remove_duplicates_t<pack<>>;
	static_assert(std::is_same_v<empty_pack, pack<>>);
}

void test_predicates()
{
	static_assert(!predicate_traits<always>::not_<void>::value);

	static_assert(or_<never, always>::type<void>::value);
	static_assert(or_<never, never, always>::type<void>::value);
	static_assert(!or_<never, never>::type<void>::value);
	static_assert(!or_<never, never, never>::type<void>::value);

	static_assert(and_<always, always>::type<void>::value);
	static_assert(and_<always, always, always>::type<void>::value);
	static_assert(!and_<never, never>::type<void>::value);
	static_assert(!and_<never, always>::type<void>::value);
	static_assert(!and_<never, never, never>::type<void>::value);
	static_assert(!and_<never, always, never>::type<void>::value);

	static_assert(and_<always, predicate_traits<never>::template not_>::type<void>::value);
	static_assert(and_<always, or_<always, never>::type, always>::type<void>::value);
}
