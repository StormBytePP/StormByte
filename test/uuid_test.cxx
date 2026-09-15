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

#include <StormByte/uuid.hxx>
#include <iostream>
#include <set>
#include <string>
bool IsHex(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

int main() {
    std::set<std::string> seen;
    for (int i = 0; i < 1000; ++i) {
        auto u = StormByte::GenerateUUIDv4();
        if (u.size() != 36 || u[8] != '-' || u[13] != '-' || u[18] != '-' ||
                u[23] != '-' || u[14] != '4' ||
                (u[19] != '8' && u[19] != '9' && u[19] != 'a' && u[19] != 'b')) {
            std::cerr << "Invalid UUIDv4 format: " << u << "\n";
            return 3;
        }

        for (std::size_t i = 0; i < u.size(); ++i) {
            if (i != 8 && i != 13 && i != 18 && i != 23 && !IsHex(u[i])) {
                std::cerr << "Invalid UUID character: " << u << "\n";
                return 4;
            }
        }

        if (seen.find(u) != seen.end()) {
            std::cerr << "Duplicate UUID generated: " << u << "\n";
            return 2;
        }

        seen.insert(u);
    }

    std::cout << "Sample UUID: " << *seen.begin() << "\n";
    return 0;
}
