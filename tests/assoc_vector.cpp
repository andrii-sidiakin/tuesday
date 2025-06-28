#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <tuesday/assert.hpp>
#include <tuesday/ecs/assoc_vector.hpp>

#include <print>
#include <string>

TEST_SUITE("assoc_vector") {
    using intstr_vec_t = tue::ecs::assoc_vector<int, std::string>;

    TEST_CASE("empty") {
        intstr_vec_t vec;
        CHECK(vec.empty());
        CHECK_EQ(vec.size(), 0);
        CHECK_EQ(vec.keys().size(), vec.size());
        CHECK_EQ(vec.values().size(), 0);
    }

    TEST_CASE("insert") {
        intstr_vec_t vec;

        vec.insert(1, std::string{"A"});
        CHECK_EQ(vec.size(), 1);
        CHECK_EQ(vec.keys().size(), vec.size());
        CHECK_EQ(vec.values().size(), 1);
        CHECK_EQ(vec[1], "A");
        CHECK_THROWS_AS(vec[2], const std::exception &);

        vec.insert(2, std::string{"B"});
        CHECK_EQ(vec.size(), 2);
        CHECK_EQ(vec.keys().size(), vec.size());
        CHECK_EQ(vec.values().size(), 2);
        CHECK_EQ(vec[1], "A");
        CHECK_EQ(vec[2], "B");
    }

    TEST_CASE("insert existing key") {
        intstr_vec_t vec;
        CHECK(vec.insert(1, std::string{"A"}));
        CHECK_FALSE(vec.insert(1, std::string{"B"}));
    }

    TEST_CASE("insert (resuse value)") {
        intstr_vec_t vec;
        auto a1 = vec.insert(1, std::string{"A"});
        vec.insert(2, std::string{"B"});
        vec.insert(3, a1);

        CHECK_EQ(vec.size(), 3);
        CHECK_EQ(vec.values().size(), 2);

        CHECK_EQ(vec[1], "A");
        CHECK_EQ(vec[2], "B");
        CHECK_EQ(vec[3], "A");
        vec[1] = "C";
        CHECK_EQ(vec[1], "C");
        CHECK_EQ(vec[2], "B");
        CHECK_EQ(vec[3], "C");

        vec.erase(1);                     // erase key=1
        CHECK_EQ(vec.values().size(), 2); // values unchanged
        CHECK_EQ(vec.size(), 2);          // keys decreased
        CHECK_THROWS_AS(vec[1], const std::out_of_range &);
        CHECK_EQ(vec[2], "B");
        CHECK_EQ(vec[3], "C");

        vec.erase(3);                     // erase key=3
        CHECK_EQ(vec.values().size(), 1); // values decreased
        CHECK_EQ(vec.size(), 1);          // keys decreased
        CHECK_EQ(vec[2], "B");
        CHECK_THROWS_AS(vec[3], const std::out_of_range &);
    }

    TEST_CASE("erase (front)") {
        intstr_vec_t vec;
        vec.insert(1, std::string{"A"});
        vec.insert(2, std::string{"B"});

        vec.erase(1);
        CHECK_EQ(vec.size(), 1);
        CHECK_EQ(vec.keys().size(), vec.size());
        CHECK_EQ(vec.values().size(), 1);

        vec.erase(2);
        CHECK_EQ(vec.size(), 0);
        CHECK_EQ(vec.keys().size(), vec.size());
        CHECK_EQ(vec.values().size(), 0);
    }

    TEST_CASE("erase (back)") {
        intstr_vec_t vec;
        vec.insert(1, std::string{"A"});
        vec.insert(2, std::string{"B"});

        vec.erase(2);
        CHECK_EQ(vec.size(), 1);
        CHECK_EQ(vec.keys().size(), vec.size());
        CHECK_EQ(vec.values().size(), 1);

        vec.erase(1);
        CHECK_EQ(vec.size(), 0);
        CHECK_EQ(vec.keys().size(), vec.size());
        CHECK_EQ(vec.values().size(), 0);
    }

    TEST_CASE("for-loop") {
        intstr_vec_t vec;
        vec.insert(1, "A");
        vec.insert(2, "B");
        vec.insert(3, "C");

        int k = 0;
        std::string v{};

        for (const auto &e : vec) {
            k += e.first;
            v += e.second;
        }

        CHECK_EQ(k, 6);
        CHECK_EQ(v, "ABC");
    }
}
