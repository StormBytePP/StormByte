/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte original source is dual-licensed:
 *
 * 1. GNU Lesser General Public License v3.0 (or later)
 *    You may redistribute and/or modify this file under the terms of the
 *    GNU Lesser General Public License as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.
 *
 * 2. Commercial license
 *    Alternatively, this file may be used under the terms of a commercial
 *    license agreement with the copyright holder
 *    (David C. Manuelda <StormByte@gmail.com>).
 *
 * Both licenses apply only to original StormByte source in this repository.
 * They do not cover other StormByte modules or any third-party material
 * shipped with this repository (including everything under thirdparty/),
 * which remains under its own license.
 *
 * Neither license grants any patent rights. Any patent licenses required
 * to use this software or third-party components must be obtained separately
 * from the patent holders.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with StormByte. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#include <StormByte/error.txx>
#include <StormByte/test_handlers.h>

#include <algorithm>
#include <cstring>
#include <ranges>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

using namespace StormByte;

// -------------------
// Fake module domains
// -------------------

enum class Inventory {
	Success = 0,
	Missing,
	Locked
};

enum class Transit {
	Success = 0,
	Timeout,
	Refused
};

template<>
struct Error::Domain<Inventory> {
	static constexpr const char* Name = "StormByte::Inventory";

	static std::string Message(Inventory e) {
		switch (e) {
			case Inventory::Success:
				return "Success";
			case Inventory::Missing:
				return "Item not in stock";
			case Inventory::Locked:
				return "Item is locked";
		}
		return "Unknown inventory error";
	}
};

template<>
struct Error::Domain<Transit> {
	static constexpr const char* Name = "StormByte::Transit";

	static std::string Message(Transit e) {
		switch (e) {
			case Transit::Success:
				return "Success";
			case Transit::Timeout:
				return "Delivery timed out";
			case Transit::Refused:
				return "Delivery refused";
		}
		return "Unknown transit error";
	}
};

std::error_code make_error_code(Inventory e) noexcept {
	static Error::Category<Inventory> instance;
	return std::error_code(static_cast<int>(e), instance);
}

std::error_code make_error_code(Transit e) noexcept {
	static Error::Category<Transit> instance;
	return std::error_code(static_cast<int>(e), instance);
}

namespace std {
	template<>
	struct is_error_code_enum<Inventory>: true_type {};

	template<>
	struct is_error_code_enum<Transit>: true_type {};
}

/**
 * @brief Stock table that reports @ref Inventory through `std::error_code`.
 */
class Warehouse {
	public:
		void Stock(std::string sku) {
			m_stock.push_back(std::move(sku));
		}

		void Lock(std::string sku) {
			m_locked.push_back(std::move(sku));
		}

		/**
		 * @brief STL-style non-throwing lookup.
		 * @param sku Item id.
		 * @param ec Set to Success, Missing or Locked.
		 * @return true if the item is in stock and not locked.
		 */
		bool Contains(const std::string& sku, std::error_code& ec) const {
			if (std::ranges::find(m_locked, sku) != m_locked.end()) {
				ec = Inventory::Locked;
				return false;
			}
			if (std::ranges::find(m_stock, sku) == m_stock.end()) {
				ec = Inventory::Missing;
				return false;
			}
			ec = Inventory::Success;
			return true;
		}

		/**
		 * @brief Throwing lookup for algorithms that propagate exceptions.
		 * @param sku Item id.
		 * @return true if available.
		 * @throw std::system_error Inventory code.
		 */
		bool ContainsOrThrow(const std::string& sku) const {
			std::error_code ec;
			if (Contains(sku, ec))
				return true;
			throw std::system_error(ec);
		}

	private:
		std::vector<std::string> m_stock;
		std::vector<std::string> m_locked;
};

/**
 * @brief Delivery table that reports @ref Transit through `std::error_code`.
 */
class Carrier {
	public:
		void Accept(std::string dest) {
			m_ok.push_back(std::move(dest));
		}

		void Refuse(std::string dest) {
			m_refused.push_back(std::move(dest));
		}

		bool Send(const std::string& dest, std::error_code& ec) const {
			if (std::ranges::find(m_refused, dest) != m_refused.end()) {
				ec = Transit::Refused;
				return false;
			}
			if (std::ranges::find(m_ok, dest) == m_ok.end()) {
				ec = Transit::Timeout;
				return false;
			}
			ec = Transit::Success;
			return true;
		}

	private:
		std::vector<std::string> m_ok;
		std::vector<std::string> m_refused;
};

// -------------------
// Suite Code
// -------------------

int test_suite_success_is_falsy() {
	int result = 0;
	const std::error_code code = Error::Code::Success;
	ASSERT_TRUE("test_suite_success_is_falsy", !code);
	ASSERT_EQUAL("test_suite_success_is_falsy", std::string("StormByte"), std::string(code.category().name()));
	RETURN_TEST("test_suite_success_is_falsy", result);
}

int test_suite_unknown() {
	int result = 0;
	const std::error_code code = Error::Code::Unknown;
	ASSERT_TRUE("test_suite_unknown", static_cast<bool>(code));
	ASSERT_TRUE("test_suite_unknown", code == Error::Code::Unknown);
	RETURN_TEST("test_suite_unknown", result);
}

int test_suite_category_is_singleton() {
	int result = 0;
	const std::error_code a = Error::Code::Success;
	const std::error_code b = make_error_code(Error::Code::Unknown);
	ASSERT_TRUE("test_suite_category_is_singleton", &a.category() == &b.category());
	RETURN_TEST("test_suite_category_is_singleton", result);
}

// -------------------
// Custom domains
// -------------------

int test_domains_do_not_share_category() {
	int result = 0;
	Warehouse warehouse;
	Carrier carrier;
	std::error_code stock;
	std::error_code ship;
	warehouse.Contains("ghost", stock);
	carrier.Send("nowhere", ship);
	ASSERT_TRUE("test_domains_do_not_share_category", stock == Inventory::Missing);
	ASSERT_TRUE("test_domains_do_not_share_category", ship == Transit::Timeout);
	ASSERT_TRUE("test_domains_do_not_share_category", &stock.category() != &ship.category());
	ASSERT_TRUE("test_domains_do_not_share_category", stock != ship);
	RETURN_TEST("test_domains_do_not_share_category", result);
}

int test_same_numeric_value_different_domain() {
	int result = 0;
	Warehouse warehouse;
	Carrier carrier;
	std::error_code stock;
	std::error_code ship;
	warehouse.Contains("ghost", stock);
	carrier.Send("nowhere", ship);
	ASSERT_EQUAL("test_same_numeric_value_different_domain", stock.value(), ship.value());
	ASSERT_TRUE("test_same_numeric_value_different_domain", stock != ship);
	RETURN_TEST("test_same_numeric_value_different_domain", result);
}

// -------------------
// Fault
// -------------------

int test_fault_default_is_success() {
	int result = 0;
	Error::Fault fault;
	ASSERT_TRUE("test_fault_default_is_success", !fault);
	ASSERT_TRUE("test_fault_default_is_success", fault.code() == Error::Code::Success);
	RETURN_TEST("test_fault_default_is_success", result);
}

int test_fault_from_warehouse() {
	int result = 0;
	Warehouse warehouse;
	std::error_code ec;
	warehouse.Lock("safe");
	warehouse.Contains("safe", ec);
	Error::Fault fault{ec};
	ASSERT_TRUE("test_fault_from_warehouse", static_cast<bool>(fault));
	ASSERT_TRUE("test_fault_from_warehouse", fault.code() == Inventory::Locked);
	ASSERT_TRUE("test_fault_from_warehouse", std::strstr(fault.what(), "Item is locked") != nullptr);
	RETURN_TEST("test_fault_from_warehouse", result);
}

int test_fault_copy_is_independent() {
	int result = 0;
	Warehouse warehouse;
	std::error_code ec;
	warehouse.Contains("ghost", ec);
	Error::Fault original{ec};
	Error::Fault copy{original};
	ASSERT_TRUE("test_fault_copy_is_independent", copy.code() == original.code());
	ASSERT_EQUAL("test_fault_copy_is_independent", 0, std::strcmp(copy.what(), original.what()));
	ASSERT_TRUE("test_fault_copy_is_independent", copy.what() != original.what());
	RETURN_TEST("test_fault_copy_is_independent", result);
}

// -------------------
// STL algorithms
// -------------------

int test_ranges_all_of_reports_locked() {
	int result = 0;
	Warehouse warehouse;
	warehouse.Stock("nail");
	warehouse.Stock("bolt");
	warehouse.Lock("bolt");
	const std::vector<std::string> want{"nail", "bolt"};
	std::error_code ec;
	const bool ok = std::ranges::all_of(want, [&](const std::string& sku) {
		return warehouse.Contains(sku, ec);
	});
	ASSERT_TRUE("test_ranges_all_of_reports_locked", !ok);
	ASSERT_TRUE("test_ranges_all_of_reports_locked", ec == Inventory::Locked);
	ASSERT_EQUAL("test_ranges_all_of_reports_locked", std::string("Item is locked"), ec.message());
	RETURN_TEST("test_ranges_all_of_reports_locked", result);
}

int test_ranges_find_if_reports_missing() {
	int result = 0;
	Warehouse warehouse;
	warehouse.Stock("nail");
	const std::vector<std::string> want{"nail", "ghost", "bolt"};
	std::error_code ec;
	const auto it = std::ranges::find_if(want, [&](const std::string& sku) {
		return !warehouse.Contains(sku, ec);
	});
	ASSERT_TRUE("test_ranges_find_if_reports_missing", it != want.end());
	ASSERT_EQUAL("test_ranges_find_if_reports_missing", std::string("ghost"), *it);
	ASSERT_TRUE("test_ranges_find_if_reports_missing", ec == Inventory::Missing);
	RETURN_TEST("test_ranges_find_if_reports_missing", result);
}

int test_for_each_collects_transit_codes() {
	int result = 0;
	Carrier carrier;
	carrier.Accept("port");
	carrier.Refuse("gate");
	const std::vector<std::string> dest{"port", "gate", "moon"};
	std::vector<std::error_code> codes;
	std::ranges::for_each(dest, [&](const std::string& stop) {
		std::error_code ec;
		carrier.Send(stop, ec);
		codes.push_back(ec);
	});
	ASSERT_EQUAL("test_for_each_collects_transit_codes", 3u, codes.size());
	ASSERT_TRUE("test_for_each_collects_transit_codes", codes[0] == Transit::Success);
	ASSERT_TRUE("test_for_each_collects_transit_codes", !codes[0]);
	ASSERT_TRUE("test_for_each_collects_transit_codes", codes[1] == Transit::Refused);
	ASSERT_TRUE("test_for_each_collects_transit_codes", codes[2] == Transit::Timeout);
	RETURN_TEST("test_for_each_collects_transit_codes", result);
}

int test_algorithm_propagates_system_error() {
	int result = 0;
	Warehouse warehouse;
	warehouse.Stock("nail");
	warehouse.Lock("bolt");
	const std::vector<std::string> want{"nail", "bolt"};
	bool threw = false;
	try {
		std::ranges::for_each(want, [&](const std::string& sku) {
			warehouse.ContainsOrThrow(sku);
		});
	} catch (const std::system_error& thrown) {
		threw = true;
		ASSERT_TRUE("test_algorithm_propagates_system_error", thrown.code() == Inventory::Locked);
		ASSERT_TRUE("test_algorithm_propagates_system_error",
			std::string(thrown.what()).find("Item is locked") != std::string::npos);
	}
	ASSERT_TRUE("test_algorithm_propagates_system_error", threw);
	RETURN_TEST("test_algorithm_propagates_system_error", result);
}

int test_any_of_stops_on_first_fault() {
	int result = 0;
	Warehouse warehouse;
	warehouse.Stock("nail");
	const std::vector<std::string> want{"ghost", "nail"};
	std::error_code ec;
	std::size_t calls = 0;
	const bool failed = std::ranges::any_of(want, [&](const std::string& sku) {
		++calls;
		return !warehouse.Contains(sku, ec);
	});
	ASSERT_TRUE("test_any_of_stops_on_first_fault", failed);
	ASSERT_EQUAL("test_any_of_stops_on_first_fault", 1u, calls);
	ASSERT_TRUE("test_any_of_stops_on_first_fault", ec == Inventory::Missing);
	RETURN_TEST("test_any_of_stops_on_first_fault", result);
}

int main() {
	int result = 0;

	// -------------------
	// Suite Code
	// -------------------
	result += test_suite_success_is_falsy();
	result += test_suite_unknown();
	result += test_suite_category_is_singleton();

	// -------------------
	// Custom domains
	// -------------------
	result += test_domains_do_not_share_category();
	result += test_same_numeric_value_different_domain();

	// -------------------
	// Fault
	// -------------------
	result += test_fault_default_is_success();
	result += test_fault_from_warehouse();
	result += test_fault_copy_is_independent();

	// -------------------
	// STL algorithms
	// -------------------
	result += test_ranges_all_of_reports_locked();
	result += test_ranges_find_if_reports_missing();
	result += test_for_each_collects_transit_codes();
	result += test_algorithm_propagates_system_error();
	result += test_any_of_stops_on_first_fault();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
