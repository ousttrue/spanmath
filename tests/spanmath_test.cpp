#include <catch2/catch_test_macros.hpp>
#include <spanmath.h>

TEST_CASE("mat3 test", "[mat3]") {

  float lhs[] = {
      1, 1, 1, //
      2, 2, 2, //
      3, 3, 3, //
  };

  float rhs[] = {
      2, 0, 0, //
      0, 3, 0, //
      0, 0, 4, //
  };

  auto mult = spanmath::Mat3(lhs) * spanmath::Mat3(rhs);

  std::array<float, 9> result = {
      2, 3, 4,  //
      4, 6, 8,  //
      6, 9, 12, //
  };

  REQUIRE(mult == result);
}
