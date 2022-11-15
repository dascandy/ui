#include <catch2/catch_all.hpp>
#include <Var.h>

TEST_CASE("basic") {
  Val<int> a = val(1);
  Val<int> b = val(2);
  Val<int> c = varfunc<int>([](int a, int b) { return a+b; }, a, varref(b));
  Val<int> d = varfunc<int>([](int a, int b) { return a + b; }, 42, varref(b));
  Val<double> e = varfunc<double>([](double d) { return d / 10.0; }, convert<double>(varref(b)));
  REQUIRE(**c == 3);
  REQUIRE(**e == 0.2);
  b = val(6); // by ref
  REQUIRE(**c == 7);
  REQUIRE(**e == 0.6);
  a = val(9); // by val
  REQUIRE(**c == 7);
  REQUIRE(**d == 48);
}


