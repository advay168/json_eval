#include <gtest/gtest.h>

#include "eval.h"

std::string testJson =
    R"delim^^(
{
  "a": {
    "b": [
      1,
      2,
      {
        "c": "test"
      },
      [
        11,
        12
      ]
    ]
  }
}
)delim^^";

TEST(JSONEvalTest, BareBones) {
  std::string result = evaluate("{\"a\": 1}", "a")->toString();
  EXPECT_STREQ(result.c_str(), "1");
}

TEST(JSONEvalTest, Sequential1) {
  std::string result = evaluate(testJson, "a.b[1]")->toString();
  EXPECT_STREQ(result.c_str(), "2");
}

TEST(JSONEvalTest, Sequential2) {
  std::string result = evaluate(testJson, "a.b[2].c")->toString();
  EXPECT_STREQ(result.c_str(), "\"test\"");
}

TEST(JSONEvalTest, Nested) {
  std::string result = evaluate(testJson, "a.b[a.b[1]].c")->toString();
  EXPECT_STREQ(result.c_str(), "\"test\"");
}

TEST(JSONEvalTest, MAX) {
  std::string result = evaluate(testJson, "max(a.b[0], a.b[1])")->toString();
  EXPECT_STREQ(result.c_str(), "2");
}

TEST(JSONEvalTest, MIN) {
  std::string result = evaluate(testJson, "min(a.b[3])")->toString();
  EXPECT_STREQ(result.c_str(), "11");
}

TEST(JSONEvalTest, SIZE1) {
  std::string result = evaluate(testJson, "size(a)")->toString();
  EXPECT_STREQ(result.c_str(), "1");
}

TEST(JSONEvalTest, SIZE2) {
  std::string result = evaluate(testJson, "size(a.b)")->toString();
  EXPECT_STREQ(result.c_str(), "4");
}

TEST(JSONEvalTest, SIZE3) {
  std::string result = evaluate(testJson, "size(a.b[a.b[1]].c)")->toString();
  EXPECT_STREQ(result.c_str(), "4");
}

TEST(JSONEvalTest, Literals) {
  std::string result = evaluate(testJson, "max(a.b[0], 10, a.b[1], 15)")->toString();
  EXPECT_STREQ(result.c_str(), "15");
}

TEST(JSONEvalTest, JSONParseFail1) {
  EXPECT_THROW(evaluate("{1}", "a"), JsonParseError);
}

TEST(JSONEvalTest, JSONParseFail2) {
  EXPECT_THROW(evaluate("{abc}", "a"), JsonParseError);
}

TEST(JSONEvalTest, ExprParseFail1) {
  EXPECT_THROW(evaluate(testJson, "max[]"), ExprParseError);
}

TEST(JSONEvalTest, ExprParseFail2) {
  EXPECT_THROW(evaluate(testJson, "a()"), ExprParseError);
}

TEST(JSONEvalTest, ExprParseFail3) {
  EXPECT_THROW(evaluate(testJson, "min(,)"), ExprParseError);
}

TEST(JSONEvalTest, InvalidOperation1) {
  EXPECT_THROW(evaluate(testJson, "min(a)"), InvalidOperation);
}

TEST(JSONEvalTest, InvalidOperation2) {
  EXPECT_THROW(evaluate(testJson, "size(a.b[0])"), InvalidOperation);
}

TEST(JSONEvalTest, InvalidOperation3) {
  EXPECT_THROW(evaluate(testJson, "a.b[x]"), InvalidOperation);
}
