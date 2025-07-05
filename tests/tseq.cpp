#include <tuesday/mp/tseq.hpp>
#include <tuesday/mp/tseq_ops.hpp>

namespace mp = tue::mp;

static_assert(mp::tseq<int>::has(mp::meta_for<int>));
static_assert(not mp::tseq<int>::has(mp::meta_for<char>));

template <bool YesNo, class T, class TS> consteval void test_has_T(TS ts) {
    static_assert(YesNo == ts.has(mp::meta_for<T>));
}

template <class BaseTS, class... TS>
consteval void test_all_same_as([[maybe_unused]] BaseTS base,
                                [[maybe_unused]] TS... ts) {
    static_assert((... and std::same_as<BaseTS, TS>));
}

consteval auto test_tseq_has() {
    test_has_T<true, int>(mp::tseq<int>{});
    test_has_T<true, int>(mp::tseq<int, int>{});
    test_has_T<true, int>(mp::tseq<char, int>{});
    test_has_T<true, int>(mp::tseq<char, int, char>{});

    test_has_T<false, float>(mp::tseq<int>{});
    test_has_T<false, float>(mp::tseq<int, int>{});
    test_has_T<false, float>(mp::tseq<char, int>{});
    test_has_T<false, float>(mp::tseq<char, int, char>{});
}

consteval auto test_tseq_unique() {
    test_all_same_as(mp::tseq<int, char, float>{}, // base
                     mp::unique(mp::tseq<int, int, char, float>{}),
                     mp::unique(mp::tseq<int, char, char, float>{}),
                     mp::unique(mp::tseq<int, char, int, float, int>{}),
                     mp::unique(mp::tseq<int, char, float>{}));
}

consteval auto test_tseq_join_unique() {
    test_all_same_as(
        mp::tseq<int, char>{}, // base
        mp::join_unique(mp::tseq<int>{}, mp::tseq<char>{}),
        mp::join_unique(mp::tseq<int, int>{}, mp::tseq<char, int>{}),
        mp::join_unique(mp::tseq<int, char>{}, mp::tseq<>{}),
        mp::join_unique(int{}, int{}, char{}, int{}, char{}),
        mp::join_unique(int{}, mp::tseq<char>{}, char{}),
        mp::join_unique(int{}, mp::tseq<char, char>{}),
        mp::join_unique(mp::tseq<>{}, mp::tseq<int, char>{}, mp::tseq<>{}));
}

int main() {
    test_tseq_has();
    test_tseq_unique();
    test_tseq_join_unique();
    return 0;
}
