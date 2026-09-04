/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * or later, as published by the Free Software Foundation.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 */

#include <StormByte/type_traits.hxx>
#include <StormByte/test_handlers.h>
#include <array>
#include <cstdint>
#include <deque>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>
#include <iostream>
using namespace StormByte;
template<typename T>
constexpr bool is_string_v = Type::String<T>;
template<typename T>
constexpr bool is_container_v = Type::Container<T>;
template<typename T>
constexpr bool has_push_back_v = Type::HasPushBack<T>;
template<typename T>
constexpr bool has_push_front_v = Type::HasPushFront<T>;
template<typename T>
constexpr bool has_insert_v = Type::HasInsert<T>;
template<typename T, typename U>
constexpr bool has_subscript_v = Type::HasSubscript<T, U>;
template<typename T>
constexpr bool has_key_type_v = Type::HasKeyType<T>;
template<typename T>
constexpr bool has_mapped_type_v = Type::HasMappedType<T>;
template<typename T>
constexpr bool is_optional_v = Type::Optional<T>;
template<typename T>
constexpr bool is_pair_v = Type::Pair<T>;
template<typename T>
constexpr bool is_variant_v = Type::Variant<T>;
template<typename T, typename U>
constexpr bool variant_has_type_v = Type::VariantHasType<T, U>;
int test_string_concept() {
	int result = 0;
	ASSERT_TRUE("test_string_concept", is_string_v<std::string>);
	ASSERT_TRUE("test_string_concept", is_string_v<std::wstring>);
	ASSERT_FALSE("test_string_concept", is_string_v<const char*>);
	ASSERT_FALSE("test_string_concept", (is_string_v<std::vector<char>>));
	ASSERT_FALSE("test_string_concept", is_string_v<int>);
	RETURN_TEST("test_string_concept", result);
}
int test_container_excludes_string() {
	int result = 0;
	ASSERT_TRUE("test_container_excludes_string", (is_container_v<std::vector<int>>));
	ASSERT_TRUE("test_container_excludes_string", (is_container_v<std::deque<int>>));
	ASSERT_TRUE("test_container_excludes_string", (is_container_v<std::list<int>>));
	ASSERT_TRUE("test_container_excludes_string", (is_container_v<std::map<int, int>>));
	ASSERT_TRUE("test_container_excludes_string", (is_container_v<std::set<int>>));
	ASSERT_FALSE("test_container_excludes_string", is_container_v<std::string>);
	ASSERT_FALSE("test_container_excludes_string", is_container_v<std::wstring>);
	ASSERT_FALSE("test_container_excludes_string", is_container_v<int>);
	ASSERT_FALSE("test_container_excludes_string", is_container_v<void*>);
	RETURN_TEST("test_container_excludes_string", result);
}
int test_has_push_back_sequences() {
	int result = 0;
	ASSERT_TRUE("test_has_push_back_sequences", (has_push_back_v<std::vector<int>>));
	ASSERT_TRUE("test_has_push_back_sequences", (has_push_back_v<std::deque<int>>));
	ASSERT_TRUE("test_has_push_back_sequences", (has_push_back_v<std::list<int>>));
	ASSERT_FALSE("test_has_push_back_sequences", (has_push_back_v<std::map<int, int>>));
	ASSERT_FALSE("test_has_push_back_sequences", (has_push_back_v<std::set<int>>));
	ASSERT_FALSE("test_has_push_back_sequences", has_push_back_v<std::string>);
	RETURN_TEST("test_has_push_back_sequences", result);
}
int test_has_push_front_sequences() {
	int result = 0;
	ASSERT_FALSE("test_has_push_front_sequences", (has_push_front_v<std::vector<int>>));
	ASSERT_TRUE("test_has_push_front_sequences", (has_push_front_v<std::deque<int>>));
	ASSERT_TRUE("test_has_push_front_sequences", (has_push_front_v<std::list<int>>));
	ASSERT_FALSE("test_has_push_front_sequences", (has_push_front_v<std::map<int, int>>));
	ASSERT_FALSE("test_has_push_front_sequences", (has_push_front_v<std::set<int>>));
	RETURN_TEST("test_has_push_front_sequences", result);
}
int test_has_insert_associative_only() {
	int result = 0;
	ASSERT_TRUE("test_has_insert_associative_only", (has_insert_v<std::map<int, int>>));
	ASSERT_TRUE("test_has_insert_associative_only", (has_insert_v<std::unordered_map<int, int>>));
	ASSERT_TRUE("test_has_insert_associative_only", (has_insert_v<std::set<int>>));
	ASSERT_TRUE("test_has_insert_associative_only", (has_insert_v<std::unordered_set<int>>));
	ASSERT_FALSE("test_has_insert_associative_only", (has_insert_v<std::vector<int>>));
	ASSERT_FALSE("test_has_insert_associative_only", (has_insert_v<std::deque<int>>));
	ASSERT_FALSE("test_has_insert_associative_only", (has_insert_v<std::list<int>>));
	ASSERT_FALSE("test_has_insert_associative_only", has_insert_v<std::string>);
	RETURN_TEST("test_has_insert_associative_only", result);
}
int test_cvref_decay_on_container_concepts() {
	int result = 0;
	ASSERT_TRUE("test_cvref_decay_on_container_concepts", (has_push_back_v<std::vector<int>&>));
	ASSERT_TRUE("test_cvref_decay_on_container_concepts", (has_push_back_v<const std::vector<int>>));
	ASSERT_TRUE("test_cvref_decay_on_container_concepts", (has_push_back_v<const std::vector<int>&>));
	ASSERT_FALSE("test_cvref_decay_on_container_concepts", (has_push_front_v<std::vector<int>&>));
	ASSERT_FALSE("test_cvref_decay_on_container_concepts", (has_push_front_v<const std::vector<int>&>));
	ASSERT_TRUE("test_cvref_decay_on_container_concepts", (has_insert_v<std::map<int, int>&>));
	ASSERT_TRUE("test_cvref_decay_on_container_concepts", (has_insert_v<const std::map<int, int>&>));
	ASSERT_FALSE("test_cvref_decay_on_container_concepts", (has_insert_v<std::vector<int>&>));
	RETURN_TEST("test_cvref_decay_on_container_concepts", result);
}
int test_vector_add_path_is_push_back_only() {
	int result = 0;
	constexpr bool pb = Type::HasPushBack<std::vector<int>>;
	constexpr bool pf = Type::HasPushFront<std::vector<int>>;
	constexpr bool ins = Type::HasInsert<std::vector<int>>;
	ASSERT_TRUE("test_vector_add_path_is_push_back_only", pb);
	ASSERT_FALSE("test_vector_add_path_is_push_back_only", pf);
	ASSERT_FALSE("test_vector_add_path_is_push_back_only", ins);
	ASSERT_EQUAL("test_vector_add_path_is_push_back_only", 1, (pb ? 1 : 0) + (pf ? 1 : 0) + (ins ? 1 : 0));
	RETURN_TEST("test_vector_add_path_is_push_back_only", result);
}
int test_map_add_path_is_insert_only() {
	int result = 0;
	constexpr bool pb = Type::HasPushBack<std::map<std::string, int>>;
	constexpr bool pf = Type::HasPushFront<std::map<std::string, int>>;
	constexpr bool ins = Type::HasInsert<std::map<std::string, int>>;
	ASSERT_FALSE("test_map_add_path_is_insert_only", pb);
	ASSERT_FALSE("test_map_add_path_is_insert_only", pf);
	ASSERT_TRUE("test_map_add_path_is_insert_only", ins);
	ASSERT_EQUAL("test_map_add_path_is_insert_only", 1, (pb ? 1 : 0) + (pf ? 1 : 0) + (ins ? 1 : 0));
	RETURN_TEST("test_map_add_path_is_insert_only", result);
}
int test_deque_may_have_both_push_apis() {
	int result = 0;
	ASSERT_TRUE("test_deque_may_have_both_push_apis", (has_push_back_v<std::deque<int>>));
	ASSERT_TRUE("test_deque_may_have_both_push_apis", (has_push_front_v<std::deque<int>>));
	ASSERT_FALSE("test_deque_may_have_both_push_apis", (has_insert_v<std::deque<int>>));
	RETURN_TEST("test_deque_may_have_both_push_apis", result);
}
int test_has_key_and_mapped_type() {
	int result = 0;
	ASSERT_TRUE("test_has_key_and_mapped_type", (has_key_type_v<std::map<int, int>>));
	ASSERT_TRUE("test_has_key_and_mapped_type", (has_mapped_type_v<std::map<int, int>>));
	ASSERT_TRUE("test_has_key_and_mapped_type", (has_key_type_v<std::set<int>>));
	ASSERT_FALSE("test_has_key_and_mapped_type", (has_mapped_type_v<std::set<int>>));
	ASSERT_FALSE("test_has_key_and_mapped_type", (has_key_type_v<std::vector<int>>));
	ASSERT_FALSE("test_has_key_and_mapped_type", (has_mapped_type_v<std::vector<int>>));
	RETURN_TEST("test_has_key_and_mapped_type", result);
}
int test_has_subscript() {
	int result = 0;
	ASSERT_TRUE("test_has_subscript", (has_subscript_v<std::vector<int>, std::size_t>));
	ASSERT_TRUE("test_has_subscript", (has_subscript_v<std::deque<int>, std::size_t>));
	ASSERT_TRUE("test_has_subscript", (has_subscript_v<std::map<int, int>, int>));
	ASSERT_FALSE("test_has_subscript", (has_subscript_v<std::list<int>, std::size_t>));
	ASSERT_FALSE("test_has_subscript", (has_subscript_v<std::set<int>, int>));
	RETURN_TEST("test_has_subscript", result);
}
int test_optional_concept() {
	int result = 0;
	ASSERT_TRUE("test_optional_concept", (is_optional_v<std::optional<int>>));
	ASSERT_TRUE("test_optional_concept", (is_optional_v<std::optional<std::string>>));
	ASSERT_FALSE("test_optional_concept", is_optional_v<int>);
	ASSERT_FALSE("test_optional_concept", (is_optional_v<std::vector<int>>));
	RETURN_TEST("test_optional_concept", result);
}
int test_pair_concept() {
	int result = 0;
	ASSERT_TRUE("test_pair_concept", (is_pair_v<std::pair<int, int>>));
	ASSERT_TRUE("test_pair_concept", (is_pair_v<std::pair<std::string, double>>));
	ASSERT_FALSE("test_pair_concept", is_pair_v<int>);
	ASSERT_FALSE("test_pair_concept", (is_pair_v<std::vector<int>>));
	RETURN_TEST("test_pair_concept", result);
}
int test_variant_concepts() {
	int result = 0;
	using V = std::variant<int, std::string, double>;
	ASSERT_TRUE("test_variant_concepts", is_variant_v<V>);
	ASSERT_FALSE("test_variant_concepts", is_variant_v<int>);
	ASSERT_FALSE("test_variant_concepts", (is_variant_v<std::optional<int>>));
	ASSERT_TRUE("test_variant_concepts", (variant_has_type_v<V, int>));
	ASSERT_TRUE("test_variant_concepts", (variant_has_type_v<V, std::string>));
	ASSERT_TRUE("test_variant_concepts", (variant_has_type_v<V, double>));
	ASSERT_FALSE("test_variant_concepts", (variant_has_type_v<V, float>));
	ASSERT_FALSE("test_variant_concepts", (variant_has_type_v<V, char*>));
	ASSERT_TRUE("test_variant_concepts", (variant_has_type_v<V, const int>));
	ASSERT_TRUE("test_variant_concepts", (variant_has_type_v<V, int&>));
	RETURN_TEST("test_variant_concepts", result);
}
enum UnscopedEnum { UE_A = 1 };
enum class ScopedEnum : std::uint16_t { A = 2 };
enum class SignedScoped : int { B = -1 };
int test_enum_concepts() {
	int result = 0;
	ASSERT_TRUE("test_enum_concepts", Type::Enum<UnscopedEnum>);
	ASSERT_TRUE("test_enum_concepts", Type::Enum<ScopedEnum>);
	ASSERT_TRUE("test_enum_concepts", Type::ScopedEnum<ScopedEnum>);
	ASSERT_FALSE("test_enum_concepts", Type::ScopedEnum<UnscopedEnum>);
	ASSERT_TRUE("test_enum_concepts", Type::UnsignedEnum<ScopedEnum>);
	ASSERT_FALSE("test_enum_concepts", Type::UnsignedEnum<SignedScoped>);
	ASSERT_EQUAL("test_enum_concepts", 2, static_cast<int>(Type::ToUnderlying(ScopedEnum::A)));
	ASSERT_EQUAL("test_enum_concepts", -1, Type::ToUnderlying(SignedScoped::B));
	ASSERT_TRUE("test_enum_concepts", (Type::SameAs<Type::UnderlyingType<ScopedEnum>, std::uint16_t>));
	RETURN_TEST("test_enum_concepts", result);
}
int test_arithmetic_and_cv_concepts() {
	int result = 0;
	ASSERT_TRUE("test_arithmetic_and_cv_concepts", Type::Integral<int>);
	ASSERT_TRUE("test_arithmetic_and_cv_concepts", Type::FloatingPoint<double>);
	ASSERT_TRUE("test_arithmetic_and_cv_concepts", Type::Arithmetic<float>);
	ASSERT_TRUE("test_arithmetic_and_cv_concepts", Type::Signed<int>);
	ASSERT_TRUE("test_arithmetic_and_cv_concepts", Type::Unsigned<unsigned>);
	ASSERT_FALSE("test_arithmetic_and_cv_concepts", Type::Signed<unsigned>);
	ASSERT_TRUE("test_arithmetic_and_cv_concepts", Type::Const<const int>);
	ASSERT_FALSE("test_arithmetic_and_cv_concepts", Type::Const<int>);
	ASSERT_TRUE("test_arithmetic_and_cv_concepts", Type::Pointer<int*>);
	ASSERT_FALSE("test_arithmetic_and_cv_concepts", Type::Pointer<int>);
	ASSERT_TRUE("test_arithmetic_and_cv_concepts", Type::Reference<int&>);
	ASSERT_TRUE("test_arithmetic_and_cv_concepts", Type::Reference<int&&>);
	ASSERT_FALSE("test_arithmetic_and_cv_concepts", Type::Reference<int>);
	ASSERT_TRUE("test_arithmetic_and_cv_concepts", Type::Class<std::string>);
	ASSERT_FALSE("test_arithmetic_and_cv_concepts", Type::Class<int>);
	RETURN_TEST("test_arithmetic_and_cv_concepts", result);
}
int test_same_as_and_convertible() {
	int result = 0;
	ASSERT_TRUE("test_same_as_and_convertible", (Type::SameAs<int, int>));
	ASSERT_TRUE("test_same_as_and_convertible", (Type::SameAs<int, const int&>));
	ASSERT_FALSE("test_same_as_and_convertible", (Type::SameAs<int, long>));
	ASSERT_TRUE("test_same_as_and_convertible", (Type::ConvertibleTo<int, long>));
	ASSERT_FALSE("test_same_as_and_convertible", (Type::ConvertibleTo<std::string, int>));
	RETURN_TEST("test_same_as_and_convertible", result);
}
int test_constructible_and_callable() {
	int result = 0;
	ASSERT_TRUE("test_constructible_and_callable", Type::DefaultConstructible<int>);
	ASSERT_TRUE("test_constructible_and_callable", Type::CopyConstructible<std::string>);
	ASSERT_TRUE("test_constructible_and_callable", (Type::MoveConstructible<std::vector<int>>));
	ASSERT_TRUE("test_constructible_and_callable", (Type::Callable<int(*)(int), int>));
	auto lambda = [](int x) { return x + 1; };
	ASSERT_TRUE("test_constructible_and_callable", (Type::Callable<decltype(lambda), int>));
	ASSERT_FALSE("test_constructible_and_callable", (Type::Callable<int, int>));
	RETURN_TEST("test_constructible_and_callable", result);
}
int test_trivially_copyable() {
	int result = 0;
	ASSERT_TRUE("test_trivially_copyable", Type::TriviallyCopyable<int>);
	ASSERT_TRUE("test_trivially_copyable", Type::TriviallyDestructible<int>);
	ASSERT_FALSE("test_trivially_copyable", Type::TriviallyCopyable<std::string>);
	RETURN_TEST("test_trivially_copyable", result);
}
int test_array_container_behaviour() {
	int result = 0;
	ASSERT_TRUE("test_array_container_behaviour", (is_container_v<std::array<int, 3>>));
	ASSERT_FALSE("test_array_container_behaviour", (has_push_back_v<std::array<int, 3>>));
	ASSERT_FALSE("test_array_container_behaviour", (has_push_front_v<std::array<int, 3>>));
	ASSERT_FALSE("test_array_container_behaviour", (has_insert_v<std::array<int, 3>>));
	ASSERT_TRUE("test_array_container_behaviour", (has_subscript_v<std::array<int, 3>, std::size_t>));
	RETURN_TEST("test_array_container_behaviour", result);
}
int main() {
	int result = 0;
	result += test_string_concept();
	result += test_container_excludes_string();
	result += test_has_push_back_sequences();
	result += test_has_push_front_sequences();
	result += test_has_insert_associative_only();
	result += test_cvref_decay_on_container_concepts();
	result += test_vector_add_path_is_push_back_only();
	result += test_map_add_path_is_insert_only();
	result += test_deque_may_have_both_push_apis();
	result += test_has_key_and_mapped_type();
	result += test_has_subscript();
	result += test_optional_concept();
	result += test_pair_concept();
	result += test_variant_concepts();
	result += test_enum_concepts();
	result += test_arithmetic_and_cv_concepts();
	result += test_same_as_and_convertible();
	result += test_constructible_and_callable();
	result += test_trivially_copyable();
	result += test_array_container_behaviour();
	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}
