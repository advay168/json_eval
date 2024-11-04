#pragma once

#include <format>
#include <iostream>
#include <string>
#include <vector>

enum class JsonTokenType {
  TRUE,
  FALSE,
  NULL_,
  INT,
  NUMBER,
  STRING,
  LEFT_SQUARE,
  RIGHT_SQUARE,
  LEFT_CURLY,
  RIGHT_CURLY,
  COMMA,
  COLON,
  EOF_,
};

struct JsonToken {
  JsonTokenType type;
  int start;
  int end;
};

inline std::string p(JsonTokenType t) {
  switch (t) {
  case JsonTokenType::TRUE:
    return "TRUE";
  case JsonTokenType::FALSE:
    return "FALSE";
  case JsonTokenType::NULL_:
    return "NULL";
  case JsonTokenType::INT:
    return "INT";
  case JsonTokenType::NUMBER:
    return "NUMBER";
  case JsonTokenType::STRING:
    return "STRING";
  case JsonTokenType::LEFT_SQUARE:
    return "LEFT_SQUARE";
  case JsonTokenType::RIGHT_SQUARE:
    return "RIGHT_SQUARE";
  case JsonTokenType::LEFT_CURLY:
    return "LEFT_CURLY";
  case JsonTokenType::RIGHT_CURLY:
    return "RIGHT_CURLY";
  case JsonTokenType::COMMA:
    return "COMMA";
  case JsonTokenType::COLON:
    return "COLON";
  case JsonTokenType::EOF_:
    return "EOF";
  }
}

struct JsonParseError : std::runtime_error {
  inline JsonParseError(const std::string &key) : std::runtime_error(key) {}
};

struct JsonTokeniser {
  void tokenise(const std::string &input_) {
    input = input_;
    pos = 0;
    tokens.clear();
    while (pos < input.size()) {
      switch (input[pos]) {
      case '\t':
      case '\n':
      case '\r':
      case ' ':
        pos++;
        break;

      case 't':
        if (input.substr(pos, 4) != "true")
          throw JsonParseError("Invalid keyword");
        tokens.emplace_back(JsonTokenType::TRUE, pos, pos + 3);
        pos += 4;
        break;

      case 'f':
        if (input.substr(pos, 5) != "false")
          throw JsonParseError("Invalid keyword");
        tokens.emplace_back(JsonTokenType::FALSE, pos, pos + 4);
        pos += 5;
        break;

      case 'n':
        if (input.substr(pos, 4) != "null")
          throw JsonParseError("Invalid keyword");
        tokens.emplace_back(JsonTokenType::NULL_, pos, pos + 3);
        pos += 4;
        break;

      case '[':
        tokens.emplace_back(JsonTokenType::LEFT_SQUARE, pos, pos);
        pos++;
        break;

      case ']':
        tokens.emplace_back(JsonTokenType::RIGHT_SQUARE, pos, pos);
        pos++;
        break;

      case '{':
        tokens.emplace_back(JsonTokenType::LEFT_CURLY, pos, pos);
        pos++;
        break;

      case '}':
        tokens.emplace_back(JsonTokenType::RIGHT_CURLY, pos, pos);
        pos++;
        break;

      case ':':
        tokens.emplace_back(JsonTokenType::COLON, pos, pos);
        pos++;
        break;

      case ',':
        tokens.emplace_back(JsonTokenType::COMMA, pos, pos);
        pos++;
        break;

      case '"':
        tokens.emplace_back(JsonTokenType::STRING, pos, 0);
        pos++;
        tokenString();
        break;

      default: {
        if (input[pos] == '-' || isdigit(input[pos])) {
          tokens.emplace_back(JsonTokenType::INT, pos, pos);
          pos++;
          tokenInt();
        } else {
          throw JsonParseError(std::string("Unexpected character: ") +
                               input[pos]);
        }
        break;
      }
      }
    }

    tokens.push_back({JsonTokenType::EOF_, pos, pos});
  }

  void tokenNum() {
    while (pos < input.size()) {
      if (!isdigit(input[pos]))
        return;
      tokens.back().end = pos++;
    }
  }

  void tokenInt() {
    while (pos < input.size()) {
      if (input[pos] == '.') {
        tokens.back().type = JsonTokenType::NUMBER;
        pos++;
        tokenNum();
        return;
      }
      if (!isdigit(input[pos]))
        return;
      tokens.back().end = pos++;
    }
  }

  void tokenString() {
    while (pos < input.size()) {
      tokens.back().end = pos;
      if (input[pos++] == '"') {
        return;
      }
    }
    throw JsonParseError("String not terminated");
  }

  std::vector<JsonToken> tokens;
  std::string input;
  int pos;
};
