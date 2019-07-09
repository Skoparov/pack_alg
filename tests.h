void test_size()
{
	using namespace type_pack;

	static_assert(pack_size_v< pack<> > == 0, "");
	static_assert(pack_size_v< pack< void > > == 1, "");
	static_assert(pack_size_v< pack< void, double > > == 2, "");
}

void test_first_pos_of()
{
	using namespace type_pack;

	static_assert(first_pos_of< pack< bool, void, bool >, bool >::value == 0, "");
	static_assert(first_pos_of< pack< bool, void, bool >, bool, 1 >::value == 2, "");
	static_assert(first_pos_of< pack< bool, void, bool >, void >::value == 1, "");
	static_assert(first_pos_of< pack< bool, void, bool >, void, 1 >::value == 1, "");
	static_assert(first_pos_of< pack< bool, void, bool >, float >::value == 3, "");
	static_assert(first_pos_of< pack<>, float >::value == 1, "");
}

void test_has_types()
{
	using namespace type_pack;

	static_assert(has_types_v< pack< bool, void, double >, bool >, "");
	static_assert(has_types_v< pack< bool, void, double >, bool, void, double >, "");
	static_assert(!has_types_v< pack< int, int >, bool >, "");
	static_assert(!has_types_v< pack< int, int >, bool, int >, "");
	static_assert(!has_types_v< pack<>, bool, int >, "");
}

void test_type_at_pos()
{
	using namespace type_pack;

	using VoidType = type_at_pos_t< pack< void >, 0>;
	static_assert(std::is_same< VoidType, void >::value, "");
}

void test_add_types_if()
{
	using namespace type_pack;

	using int_double_pack = add_types_if_t< pack<>, of<int, double >::fits_any, int, double>;
	static_assert(std::is_same< int_double_pack, pack< int, double > >::value, "");

	using empty_pack = add_types_if_t< pack<>, of< int, double >::fits_none, int, double>;
	static_assert(std::is_same< empty_pack, pack<> >::value, "");

	using int_pack = add_types_if_t< pack<>, of< int >::fits_any, int, double >;
	static_assert(std::is_same< int_pack, pack< int > >::value, "");

	using int_double_int_pack = add_types_if_t< pack< int, double >, of<int>::fits_any, int >;
	static_assert(std::is_same< int_double_int_pack, pack< int, double, int > >::value, "");

	using EmptyPack2 = add_types_if_t< pack<>, of< int >::fits_any >;
	static_assert(std::is_same< EmptyPack2, pack<> >::value, "");

	using int_double_pack2 = add_types_t< pack<>, int, double >;
	static_assert(std::is_same< int_double_pack2, pack< int, double > >::value, "");
}

void test_concat()
{
	using namespace type_pack;

	using void_double_pack = concat_t< pack<>, pack< void, double > >;
	static_assert(std::is_same< void_double_pack, pack< void, double > >::value, "");

	using void_double_pack2 = concat_t< pack< void, double >, pack<> >;
	static_assert(std::is_same< void_double_pack2, pack< void, double > >::value, "");

	using void_int_double_pack = concat_t< pack< void, int >, pack< double > >;
	static_assert(std::is_same< void_int_double_pack, pack< void, int, double > >::value, "");

	using void_int_double_pack2 = concat_t< pack< void >, pack< int >, pack< double > >;
	static_assert(std::is_same< void_int_double_pack2, pack< void, int, double > >::value, "");
}

void test_remove_types_if()
{
	using namespace type_pack;

	using double_pack = remove_types_if_t< pack< int, double >, of< int >::fits_any >;
	static_assert(std::is_same< double_pack, pack< double > >::value, "");

	using empty_pack = remove_types_if_t< pack< int, double >, of< int, double >::fits_any >;
	static_assert(std::is_same< empty_pack, pack<> >::value, "");

	using int_double_pack = remove_types_if_t< pack< int, double >, of< int, double >::fits_none >;
	static_assert(std::is_same< int_double_pack, pack< int, double >>::value, "");

	using empty_pack2 = remove_types_t< pack< int, double >, int, double >;
	static_assert(std::is_same< empty_pack2, pack<> >::value, "");
}

void test_remove_duplicates()
{
	using namespace type_pack;

	using int_pack = remove_duplicates_t< pack< int, int > >;
	static_assert(std::is_same< int_pack, pack< int > >::value, "");

	using int_double_pack = remove_duplicates_t< pack< int, double, int, double > >;
	static_assert(std::is_same< int_double_pack, pack< int, double > >::value, "");

	using empty_pack = remove_duplicates_t< pack<> >;
	static_assert(std::is_same< empty_pack, pack<> >::value, "");
}

void test_predicates()
{
	using namespace type_pack;

	static_assert(!not_<always>::type<void>::value, "");

	static_assert(or_< never, always >::type<void>::value, "");
	static_assert(or_< never, never, always >::type<void>::value, "");
	static_assert(!or_< never, never >::type<void>::value, "");
	static_assert(!or_< never, never, never >::type<void>::value, "");

	static_assert(and_< always, always >::type<void>::value, "");
	static_assert(and_< always, always, always >::type<void>::value, "");
	static_assert(!and_< never, never >::type<void>::value, "");
	static_assert(!and_< never, always >::type<void>::value, "");
	static_assert(!and_< never, never, never >::type<void>::value, "");
	static_assert(!and_< never, always, never >::type<void>::value, "");

	static_assert(and_< always, not_<never>::template type >::type<void>::value, "");
	static_assert(and_< always, or_<always, never>::type, always >::type<void>::value, "");
}
