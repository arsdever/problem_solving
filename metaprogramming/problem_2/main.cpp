#include <algorithm>
#include <iostream>
#include <type_traits>

namespace {
#pragma region Problem 1: Fibonacci
template <size_t N> struct Fibonacci {
  static constexpr auto value =
      Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template <> struct Fibonacci<0> {
  static constexpr auto value = 0;
};

template <> struct Fibonacci<1> {
  static constexpr auto value = 1;
};

static_assert(Fibonacci<0>::value == 0);
static_assert(Fibonacci<1>::value == 1);
static_assert(Fibonacci<2>::value == 1);
static_assert(Fibonacci<3>::value == 2);
static_assert(Fibonacci<4>::value == 3);
static_assert(Fibonacci<5>::value == 5);
static_assert(Fibonacci<6>::value == 8);
static_assert(Fibonacci<7>::value == 13);
static_assert(
    Fibonacci<41>::value % 2 ==
    1); // Useful for benchmarking, make sure your compiler calculates LHS.
#pragma endregion

#pragma region Problem 2: Vector concatenation
template <int...> struct Vector {};

template <typename V, typename V1> struct Concat;

template <int... V, int... V1> struct Concat<Vector<V...>, Vector<V1...>> {
  using type = Vector<V..., V1...>;
};

static_assert(std::is_same_v<Concat<Vector<1, 2>, Vector<3, 4>>::type,
                             Vector<1, 2, 3, 4>>);
#pragma endregion

#pragma region Problem 3: Run Length Encoding
template <typename> struct RLENext;

template <int F, int... I> struct RLENext<Vector<F, F, I...>> {
  static constexpr size_t count = RLENext<Vector<F, I...>>::count + 1;
};

template <int F, int S, int... I> struct RLENext<Vector<F, S, I...>> {
  static constexpr size_t count = 1;
};

template <int F> struct RLENext<Vector<F>> {
  static constexpr size_t count = 1;
};

template <typename V, size_t P> struct SubVector;

template <size_t P, int F, int... I> struct SubVector<Vector<F, I...>, P> {
  using type = SubVector<Vector<I...>, P - 1>::type;
};

template <int F, int... I> struct SubVector<Vector<F, I...>, 0> {
  using type = Vector<F, I...>;
};

template <int F> struct SubVector<Vector<>, F> {
  using type = Vector<>;
};

template <typename> struct RLEHelper;

template <int F, int... I> struct RLEHelper<Vector<F, I...>> {
  static constexpr size_t next_pos = RLENext<Vector<F, I...>>::count;
  using type =
      Concat<Vector<next_pos, F>,
             RLEHelper<SubVector<Vector<F, I...>, next_pos>::type>::type>::type;
};

template <> struct RLEHelper<Vector<>> {
  using type = Vector<>;
};

template <int... I> struct RLE {
  using type = RLEHelper<Vector<I...>>::type;
};

static_assert(
    std::is_same_v<SubVector<Vector<0, 0, 0, 0, 1, 1, 1, 0, 0>, 0>::type,
                   Vector<0, 0, 0, 0, 1, 1, 1, 0, 0>>);
static_assert(
    std::is_same_v<SubVector<Vector<0, 0, 0, 0, 1, 1, 1, 0, 0>, 3>::type,
                   Vector<0, 1, 1, 1, 0, 0>>);
static_assert(
    RLENext<Vector<0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0>>::count == 7);

static_assert(std::is_same_v<RLE<0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 1>::type,
                             Vector<5, 0, 2, 1, 12, 0, 1, 1>>);
static_assert(std::is_same_v<RLE<1, 1, 1, 1, 9, 9, 9, 2>::type,
                             Vector<4, 1, 3, 9, 1, 2>>);
#pragma endregion

#pragma region Problem 4: Minimum Subset Sum Difference

// static_assert(MSSD<>::value == 0);
// static_assert(MSSD<1>::value == 1);
// static_assert(MSSD<1, 1>::value == 0);
// static_assert(MSSD<1, 1, 1>::value == 1);
// static_assert(MSSD<20, 30, 10, 10, 20>::value == 10);
// static_assert(MSSD<5, 20, 25, 10, 10, 20>::value == 0);
// static_assert(MSSD<5, 10, 15, 20, 25, 10, 10, 20>::value == 5);
#pragma endregion
} // namespace

int main() {}
