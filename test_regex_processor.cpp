#define CATCH_CONFIG_MAIN
#include <limits>
#include <stdexcept>

#include "catch.hpp"
#include "regex_processor.h"

TEST_CASE("Valid expressions with expected results", "[valid]") {
  SECTION("Test Case 1") {
    std::string expression = "ab+c.aba.*.bac.+.+*";
    char x = 'a';
    int k = 2;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("Test Case 2") {
    std::string expression = "acb..bab.c.*.ab.ba.+.+*a.";
    char x = 'a';
    int k = 0;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == false);
  }

  SECTION("Expression with only one letter equal to x") {
    std::string expression = "a";
    char x = 'a';
    int k = 1;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("Expression with only one letter not equal to x") {
    std::string expression = "b";
    char x = 'a';
    int k = 0;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("Expression with Kleene star including zero occurrences") {
    std::string expression = "a*";
    char x = 'a';
    int k = 0;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("Expression with concatenation") {
    std::string expression = "aa.";
    char x = 'a';
    int k = 2;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("Expression with union") {
    std::string expression = "ab+";
    char x = 'a';
    int k = 1;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("Complex expression") {
    std::string expression = "ab+c.*";
    char x = 'a';
    int k = 5;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }
}

TEST_CASE("Invalid expressions that should throw exceptions", "[invalid]") {
  SECTION("Operator with insufficient operands") {
    std::string expression = "+";
    char x = 'a';
    int k = 1;
    REQUIRE_THROWS_AS(isKOccurrencesPossible(expression, x, k),
                      std::invalid_argument);
  }

  SECTION("Unknown symbol") {
    std::string expression = "a$";
    char x = 'a';
    int k = 1;
    REQUIRE_THROWS_AS(isKOccurrencesPossible(expression, x, k),
                      std::invalid_argument);
  }

  SECTION("Extra operands left in stack") {
    std::string expression = "aa";
    char x = 'a';
    int k = 2;
    REQUIRE_THROWS_AS(isKOccurrencesPossible(expression, x, k),
                      std::invalid_argument);
  }
}

TEST_CASE("Edge cases", "[edge]") {
  SECTION("Empty expression") {
    std::string expression = "";
    char x = 'a';
    int k = 0;
    REQUIRE_THROWS_AS(isKOccurrencesPossible(expression, x, k),
                      std::invalid_argument);
  }

  SECTION("Expression resulting in infinite max_count") {
    std::string expression = "a*";
    char x = 'a';
    int k = 1000;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("Expression with zero possible counts_mod") {
    std::string expression = "1";
    char x = 'a';
    int k = 0;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("k less than min_count") {
    std::string expression = "aa.";
    char x = 'a';
    int k = 1;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == false);
  }

  SECTION("k greater than max_count") {
    std::string expression = "a";
    char x = 'a';
    int k = 2;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == false);
  }
}

TEST_CASE("Expressions with multiple operators", "[operators]") {
  SECTION("Nested Kleene stars") {
    std::string expression = "a**";
    char x = 'a';
    int k = 0;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("Concatenation and union") {
    std::string expression = "ab+";
    char x = 'a';
    int k = 1;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("Complex nested expression") {
    std::string expression = "ab+ac+.";
    char x = 'a';
    int k = 1;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }
}

TEST_CASE("Expressions with varying counts_mod", "[counts_mod]") {
  SECTION("counts_mod equals zero") {
    std::string expression = "a*";
    char x = 'a';
    int k = 5;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("counts_mod greater than zero") {
    std::string expression = "aa*.";
    char x = 'a';
    int k = 3;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("counts_mod test with gcd") {
    std::string expression = "aa.a*.";
    char x = 'a';
    int k = 4;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("counts_mod mismatch") {
    std::string expression = "aa.a*.";
    char x = 'a';
    int k = 1;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == false);
  }
}

TEST_CASE("Expressions with zero_included variations", "[zero_included]") {
  SECTION("zero_included is true") {
    std::string expression = "a*";
    char x = 'a';
    int k = 0;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == true);
  }

  SECTION("zero_included is false") {
    std::string expression = "a";
    char x = 'a';
    int k = 0;
    bool result = isKOccurrencesPossible(expression, x, k);
    REQUIRE(result == false);
  }
}