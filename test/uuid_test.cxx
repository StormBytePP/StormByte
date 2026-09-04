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
int main() {
    std::set<std::string> seen;
    for (int i = 0; i < 1000; ++i) {
        auto u = StormByte::GenerateUUIDv4();
        if (seen.find(u) != seen.end()) {
            std::cerr << "Duplicate UUID generated: " << u << "\n";
            return 2;
        }
        seen.insert(u);
    }
    std::cout << "Sample UUID: " << *seen.begin() << "\n";
    return 0;
}
