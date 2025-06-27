#include <tuesday/assert.hpp>
#include <tuesday/ecs/assoc_vector.hpp>

#include <print>
#include <string>

int main() {
    tue::ecs::assoc_vector<int, std::string> vec;
    tue_assert(vec.size() == 0);
    tue_assert(vec.keys().size() == vec.size());
    tue_assert(vec.values().size() == 0);

    vec.insert(1, std::string{"A"});
    tue_assert(vec.size() == 1);
    tue_assert(vec.keys().size() == vec.size());
    tue_assert(vec.values().size() == 1);
    {
        bool out_of_range_thrown = false;
        try {
            [[maybe_unused]] auto &v = vec[2];
        }
        catch (std::out_of_range &e) {
            out_of_range_thrown = true;
        }
        tue_assert(out_of_range_thrown);
    }

    vec.insert(2, std::string{"B"});
    tue_assert(vec.size() == 2);
    tue_assert(vec.keys().size() == vec.size());
    tue_assert(vec.values().size() == 2);
    tue_assert(vec[2] == "B");

    vec.erase(2);
    tue_assert(vec.size() == 1);
    tue_assert(vec.keys().size() == vec.size());
    tue_assert(vec.values().size() == 1);
    {
        bool out_of_range_thrown = false;
        try {
            [[maybe_unused]] auto &v = vec[2];
        }
        catch (std::out_of_range &e) {
            out_of_range_thrown = true;
        }
        tue_assert(out_of_range_thrown);
    }

    vec.erase(1);
    tue_assert(vec.size() == 0);
    tue_assert(vec.keys().size() == vec.size());
    tue_assert(vec.values().size() == 0);
    {
        bool out_of_range_thrown = false;
        try {
            [[maybe_unused]] auto &v2 = vec[2];
        }
        catch (std::out_of_range &e) {
            out_of_range_thrown = true;
        }
        tue_assert(out_of_range_thrown);
    }

    vec.insert(10, std::string{"A"});
    auto p = vec.insert(11, std::string{"X"});
    tue_assert(vec.size() == 2);
    tue_assert(vec.keys().size() == vec.size());
    tue_assert(vec.values().size() == 2, "Expect 2 distinct values");
    vec.insert(12, p); // so that 12 shares value with 11
    tue_assert(vec.size() == 3);
    tue_assert(vec.keys().size() == vec.size());
    tue_assert(vec.values().size() == 2, "Expect values unchanged");
    tue_assert(vec[10] == "A");
    tue_assert(vec[11] == "X");
    tue_assert(vec[12] == "X");
    tue_assert(vec.keys()[0] == 10);
    tue_assert(vec.keys()[1] == 11);
    tue_assert(vec.keys()[2] == 12);

    vec.mutable_values()[1] = "Y";
    tue_assert(vec[10] == "A", "Should be unchanged");
    tue_assert(vec[11] == "Y");
    tue_assert(vec[12] == "Y");

    vec.erase(11);
    tue_assert(vec.size() == 2);
    tue_assert(vec.keys().size() == vec.size());
    tue_assert(vec.values().size() == 2);
    tue_assert(vec.keys()[0] == 10);
    tue_assert(vec.keys()[1] == 12);

    vec.erase(10);
    tue_assert(vec.size() == 1);
    tue_assert(vec.keys().size() == vec.size());
    tue_assert(vec.values().size() == 1);
    tue_assert(vec.keys()[0] == 12);
    vec.erase(12);
    tue_assert(vec.size() == 0);
    tue_assert(vec.keys().size() == vec.size());
    tue_assert(vec.values().size() == 0);

    tue_assert(vec.empty());

    vec.insert(21, std::string{"A21"});
    vec.insert(22, std::string{"A22"});
    vec.insert(23, std::string{"A23"});
    for (const auto &[k, v] : vec) {
        std::println("{}:{}", k, v);
        tue_assert(vec[k] == v);
    }
    for (const auto &[k, v] : vec) {
        v += v;
        std::println("{}:{}", k, v);
        tue_assert(vec[k] == v);
    }

    return 0;
}
