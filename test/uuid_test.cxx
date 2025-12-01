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
