// https://www.slamecka.cz/posts/2021-03-17-cpp-metaprogramming-exercises-1

#include <iostream>
#include <type_traits>

namespace {
template <int... N> struct Vector {};

template <int... N> void print(Vector<N...> v) {
  ((std::cout << N << ' '), ...);
  std::cout << std::endl;
}

template <int P, typename V> struct Prepend;

template <int P, int... N> struct Prepend<P, Vector<N...>> {
  using type = Vector<P, N...>;
};

template <int P, typename V> using PrependT = Prepend<P, V>::type;

template <int A, typename V> struct Append;

template <int A, int... N> struct Append<A, Vector<N...>> {
  using type = Vector<N..., A>;
};

template <int A, typename V> using AppendT = Append<A, V>::type;

template <typename V> struct PopBack;

template <int L, int... N> struct PopBack<Vector<L, N...>> {
  using type = Prepend<L, typename PopBack<Vector<N...>>::type>::type;
};

template <int P, int L> struct PopBack<Vector<P, L>> {
  using type = Vector<P>;
};

template <int R, typename V> struct RemoveFirst;

template <int R, int F, int... N> struct RemoveFirst<R, Vector<F, N...>> {
  using type = Prepend<F, typename RemoveFirst<R, Vector<N...>>::type>::type;
};

template <int R, int... N> struct RemoveFirst<R, Vector<R, N...>> {
  using type = Vector<N...>;
};

template <int R, typename V> struct RemoveAll;

template <int R, int F, int... N> struct RemoveAll<R, Vector<F, N...>> {
  using type = Prepend<F, typename RemoveAll<R, Vector<N...>>::type>::type;
};

template <int R, int... N> struct RemoveAll<R, Vector<R, N...>> {
  using type = RemoveAll<R, Vector<N...>>::type;
};

template <int R> struct RemoveAll<R, Vector<>> {
  using type = Vector<>;
};

template <typename V> struct Length;

template <int... N> struct Length<Vector<N...>> {
  static constexpr auto value = sizeof...(N);
};

template <typename V> static constexpr auto length = Length<V>::value;

template <typename T> struct Min;

template <int F, int... N> struct Min<Vector<F, N...>> {
  static constexpr auto value = std::min(F, Min<Vector<N...>>::value);
};

template <int N, int N1> struct Min<Vector<N, N1>> {
  static constexpr auto value = std::min(N, N1);
};

template <typename V> struct Sort;

template <int... N> struct Sort<Vector<N...>> {
  using type =
      Prepend<Min<Vector<N...>>::value,
              typename Sort<typename RemoveFirst<
                  Min<Vector<N...>>::value, Vector<N...>>::type>::type>::type;
};

template <int N> struct Sort<Vector<N>> {
  using type = Vector<N>;
};

template <typename T> struct Uniq;

template <int I, int J, int... N> struct Uniq<Vector<I, J, N...>> {
  using type = Prepend<I, typename Uniq<Vector<J, N...>>::type>::type;
};

template <int I, int... N> struct Uniq<Vector<I, I, N...>> {
  using type = Uniq<Vector<I, N...>>::type;
};

template <int I> struct Uniq<Vector<I>> {
  using type = Vector<I>;
};

template <int... N> struct Set {
  using type = Uniq<typename Sort<Vector<N...>>::type>::type;
};

template <typename V> struct SetFrom;

template <int... N> struct SetFrom<Vector<N...>> {
  using type = Set<N...>::type;
};

template <int I, typename V> struct Get;

template <int I, int F, int... N> struct Get<I, Vector<F, N...>> {
  static_assert(I < sizeof...(N) + 1, "Index out of the bounds");
  static constexpr auto value = Get<I - 1, Vector<N...>>::value;
};

template <int F, int... N> struct Get<0, Vector<F, N...>> {
  static constexpr auto value = F;
};

template <int B, typename V> struct BisectLeft;

template <int B, int I, int... N> struct BisectLeft<B, Vector<I, N...>> {
  static constexpr auto value =
      B > I ? BisectLeft<B, Vector<N...>>::value + 1 : 0;
};

template <int B> struct BisectLeft<B, Vector<>> {
  static constexpr auto value = 0;
};

#pragma region Failed attempt 1
// Failed attempt 1: using partial initialization
// template <int P, int I, typename V> struct Insert;

// template <int P, int I, int F, int... N> struct Insert<P, I, Vector<F, N...>>
// {
//   using type = Prepend<F, typename Insert<P - 1, I,
//   Vector<N...>>::type>::type;
// };

// template <int I, int... N> struct Insert<0, I, Vector<N...>> {
//   using type = typename Prepend<I, Vector<N...>>::type;
// };
#pragma endregion

#pragma region Failed attempt 2
// Failed attempt 1: using std::conditional
// template <int P, int I, typename V> struct Insert;

// template <int P, int I, int F, int... N> struct Insert<P, I, Vector<F, N...>>
// {
//   using type = std::conditional_t<
//       P == 0, typename Prepend<I, Vector<N...>>::type,
//       typename Prepend<F, typename Insert<P - 1, I,
//       Vector<N...>>::type>::type>;
// };
#pragma endregion

#pragma region Succeeded attempt 3
template <int P, int I, typename V, typename E = void> struct InsertHelper;

template <int P, int I, int F, int... N>
struct InsertHelper<P, I, Vector<F, N...>, std::enable_if_t<P == 0>> {
  using type = typename Prepend<I, Vector<F, N...>>::type;
};

template <int P, int I>
struct InsertHelper<P, I, Vector<>, std::enable_if_t<P == 0>> {
  using type = Vector<I>;
};

template <int P, int I, int F, int... N>
struct InsertHelper<P, I, Vector<F, N...>, std::enable_if_t<P != 0>> {
  using type =
      Prepend<F, typename InsertHelper<P - 1, I, Vector<N...>>::type>::type;
};

template <int P, int I, typename V> struct Insert {
  using type = InsertHelper<P, I, V>::type;
};

static_assert(std::is_same_v<Vector<1, 2>, Vector<1, 2>>);
static_assert(std::is_same_v<Prepend<1, Vector<2, 3>>::type, Vector<1, 2, 3>>);
static_assert(std::is_same_v<PrependT<1, Vector<2, 3>>, Vector<1, 2, 3>>);
static_assert(std::is_same_v<Append<4, Vector<>>::type, Vector<4>>);
static_assert(
    std::is_same_v<Append<4, Vector<1, 2, 3>>::type, Vector<1, 2, 3, 4>>);
static_assert(std::is_same_v<AppendT<4, Vector<1, 2, 3>>, Vector<1, 2, 3, 4>>);
static_assert(
    std::is_same_v<PopBack<Vector<1, 2, 3, 4>>::type, Vector<1, 2, 3>>);
static_assert(
    std::is_same_v<RemoveFirst<1, Vector<1, 1, 2>>::type, Vector<1, 2>>);
static_assert(std::is_same_v<RemoveAll<9, Vector<1, 9, 2, 9, 3, 9>>::type,
                             Vector<1, 2, 3>>);
static_assert(Length<Vector<1, 2, 3>>::value == 3);
static_assert(length<Vector<>> == 0);
static_assert(length<Vector<1, 2, 3>> == 3);
static_assert(Min<Vector<3, 1, 2>>::value == 1);
static_assert(Min<Vector<1, 2, 3>>::value == 1);
static_assert(Min<Vector<3, 2, 1>>::value == 1);
static_assert(std::is_same_v<Sort<Vector<4, 1, 2, 5, 6, 3>>::type,
                             Vector<1, 2, 3, 4, 5, 6>>);
static_assert(std::is_same_v<Sort<Vector<3, 3, 1, 1, 2, 2>>::type,
                             Vector<1, 1, 2, 2, 3, 3>>);
static_assert(std::is_same_v<Sort<Vector<2, 2, 1, 1, 3, 3>>::type,
                             Vector<1, 1, 2, 2, 3, 3>>);
static_assert(
    std::is_same_v<Uniq<Vector<1, 1, 2, 2, 1, 1>>::type, Vector<1, 2, 1>>);
static_assert(std::is_same_v<Set<2, 1, 3, 1, 2, 3>::type, Set<1, 2, 3>::type>);
static_assert(std::is_same_v<SetFrom<Vector<2, 1, 3, 1, 2, 3>>::type,
                             Set<1, 2, 3>::type>);
static_assert(Get<0, Vector<0, 1, 2>>::value == 0);
static_assert(Get<1, Vector<0, 1, 2>>::value == 1);
static_assert(Get<2, Vector<0, 1, 2>>::value == 2);
// static_assert(Get<9, Vector<0,1,2>>::value == 2); // How good is your error
// message?
static_assert(BisectLeft<3, Vector<0, 1, 2, 3, 4>>::value == 3);
static_assert(BisectLeft<3, Vector<0, 1, 2, 4, 5>>::value == 3);
static_assert(BisectLeft<9, Vector<0, 1, 2, 4, 5>>::value == 5);
static_assert(BisectLeft<-1, Vector<0, 1, 2, 4, 5>>::value == 0);
static_assert(BisectLeft<2, Vector<0, 2, 2, 2, 2, 2>>::value == 1);
static_assert(
    std::is_same_v<Insert<0, 3, Vector<4, 5, 6>>::type, Vector<3, 4, 5, 6>>);
static_assert(
    std::is_same_v<Insert<1, 3, Vector<4, 5, 6>>::type, Vector<4, 3, 5, 6>>);
static_assert(
    std::is_same_v<Insert<2, 3, Vector<4, 5, 6>>::type, Vector<4, 5, 3, 6>>);
static_assert(
    std::is_same_v<Insert<3, 3, Vector<4, 5, 6>>::type, Vector<4, 5, 6, 3>>);
} // namespace

int main() {
  print(Vector<>{});
  print(Vector<1>{});
  print(Vector<1, 2, 3, 4, 5, 6>{});
  std::cout << typeid(Vector<1, 2, 3, 4, 5, 6>{}).name() << '\n';
}
