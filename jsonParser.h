#pragma once
#include <format>
#include <iostream>
#include <string>

#include "json.h"
#include "jsonTokeniser.h"

struct JsonParser {
  Json parse(const std::string &input_) {
    input = input_;
    tokeniser.tokenise(input);

    pos = 0;
    Json expr = parseHelper();
    if (tokeniser.tokens[pos].type != JsonTokenType::EOF_) {
      throw JsonParseError("Unexpected token");
    }
    return expr;
  }

  Json parseHelper() {
    while (pos < tokeniser.tokens.size()) {
      auto [type, start, end] = tokeniser.tokens[pos++];
      switch (type) {
      case JsonTokenType::TRUE:
        return std::make_shared<JsonBool>(true);

      case JsonTokenType::FALSE:
        return std::make_shared<JsonBool>(false);

      case JsonTokenType::NULL_:
        return std::make_shared<JsonNull>();

      case JsonTokenType::INT:
        return std::make_shared<JsonInt>(
            std::stoi(input.substr(start, end - start + 1)));

      case JsonTokenType::NUMBER:
        return std::make_shared<JsonNumber>(
            std::stod(input.substr(start, end - start + 1)));

      case JsonTokenType::STRING:
        return std::make_shared<JsonString>(
            input.substr(start + 1, end - start - 1));

      case JsonTokenType::LEFT_SQUARE: {
        auto jsonArray = std::make_shared<JsonArray>();
        parseArray(jsonArray->arr);
        return jsonArray;
      }

      case JsonTokenType::RIGHT_SQUARE:
        throw JsonParseError("Unexpected closing list bracket");

      case JsonTokenType::LEFT_CURLY: {
        auto jsonObj = std::make_shared<JsonObject>();
        parseObject(jsonObj->mapping);
        return jsonObj;
      }

      case JsonTokenType::RIGHT_CURLY:
        throw JsonParseError("Unexpected closing object bracket");

      case JsonTokenType::COMMA:
        throw JsonParseError("Unexpected comma");

      case JsonTokenType::COLON:
        throw JsonParseError("Unexpected colon");

      case JsonTokenType::EOF_:
        throw JsonParseError("Unexpected end of file");
      }
    }
    throw; // Unreachable
  }

  void parseArray(std::vector<Json> &arr) {
    if (tokeniser.tokens.back().type == JsonTokenType::RIGHT_SQUARE)
      return;

    // Trailing commas not allowed
    while (pos < tokeniser.tokens.size()) {
      arr.push_back(parseHelper());
      auto type = tokeniser.tokens[pos++].type;
      if (type == JsonTokenType::RIGHT_SQUARE)
        return;
      if (type != JsonTokenType::COMMA)
        throw JsonParseError("Comma expected between elements of array");
    }
  }

  void parseObject(std::unordered_map<std::string, Json> &map) {
    if (tokeniser.tokens.back().type == JsonTokenType::RIGHT_CURLY)
      return;

    // Trailing commas not allowed
    while (pos < tokeniser.tokens.size()) {
      auto type = tokeniser.tokens[pos].type;
      if (type != JsonTokenType::STRING)
        throw JsonParseError("Expected string key");

      auto key = std::dynamic_pointer_cast<JsonString>(parseHelper())->val;

      if (tokeniser.tokens[pos++].type != JsonTokenType::COLON)
        throw JsonParseError("Colon expected after key in object");

      map[key] = parseHelper();

      auto type2 = tokeniser.tokens[pos++].type;
      if (type2 == JsonTokenType::RIGHT_CURLY)
        return;
      if (type2 != JsonTokenType::COMMA)
        throw JsonParseError("Comma expected between elements of object");
    }
  }

  std::string input;
  int pos;
  JsonTokeniser tokeniser;
};
