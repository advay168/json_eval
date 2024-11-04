#pragma once

#include <format>
#include <iostream>
#include <string>
#include <vector>

enum class ExprTokenType {
  MIN,
  MAX,
  SIZE,
  LEFT_SQUARE,
  RIGHT_SQUARE,
  LEFT_ROUND,
  RIGHT_ROUND,
  INT,
  NUMBER,
  DOT,
  COMMA,
  IDENT,
  EOF_,
};

struct ExprToken {
  ExprTokenType type;
  int start;
  int end;
};

inline std::string printExprToken(ExprTokenType t) {
  switch (t) {
  case ExprTokenType::MIN:
    return "MIN";
  case ExprTokenType::MAX:
    return "MAX";
  case ExprTokenType::SIZE:
    return "SIZE";
  case ExprTokenType::LEFT_SQUARE:
    return "LEFT_SQUARE";
  case ExprTokenType::RIGHT_SQUARE:
    return "RIGHT_SQUARE";
  case ExprTokenType::LEFT_ROUND:
    return "LEFT_ROUND";
  case ExprTokenType::RIGHT_ROUND:
    return "RIGHT_ROUND";
  case ExprTokenType::INT:
    return "INT";
  case ExprTokenType::NUMBER:
    return "NUMBER";
  case ExprTokenType::DOT:
    return "DOT";
  case ExprTokenType::COMMA:
    return "COMMA";
  case ExprTokenType::IDENT:
    return "IDENT";
  case ExprTokenType::EOF_:
    return "EOF";
  }
}

struct ExprParseError : std::runtime_error {
  inline ExprParseError(const std::string &key) : std::runtime_error(key) {}
};

struct ExprTokeniser {
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

      case 'm':
        if (input.substr(pos, 3) == "min") {
          tokens.emplace_back(ExprTokenType::MIN, pos, pos + 2);
        } else if (input.substr(pos, 3) == "max") {
          tokens.emplace_back(ExprTokenType::MAX, pos, pos + 2);
        } else {
          throw ExprParseError("Invalid keyword");
        }
        pos += 3;
        break;

      case 's':
        if (input.substr(pos, 4) != "size")
          throw ExprParseError("Invalid keyword");
        tokens.emplace_back(ExprTokenType::SIZE, pos, pos + 3);
        pos += 4;
        break;

      case '[':
        tokens.emplace_back(ExprTokenType::LEFT_SQUARE, pos, pos);
        pos++;
        break;

      case ']':
        tokens.emplace_back(ExprTokenType::RIGHT_SQUARE, pos, pos);
        pos++;
        break;

      case '(':
        tokens.emplace_back(ExprTokenType::LEFT_ROUND, pos, pos);
        pos++;
        break;

      case ')':
        tokens.emplace_back(ExprTokenType::RIGHT_ROUND, pos, pos);
        pos++;
        break;

      case '.':
        tokens.emplace_back(ExprTokenType::DOT, pos, pos);
        pos++;
        break;

      case ',':
        tokens.emplace_back(ExprTokenType::COMMA, pos, pos);
        pos++;
        break;

      default: {
        if (input[pos] == '-' || isdigit(input[pos])) {
          tokens.emplace_back(ExprTokenType::INT, pos, pos);
          pos++;
          tokenInt();
        } else if (isalpha(input[pos])) {
          tokens.emplace_back(ExprTokenType::IDENT, pos, pos);
          tokenIdent();
        } else {
          throw ExprParseError(std::string("Unexpected character: ") +
                               input[pos]);
        }
        break;
      }
      }
    }

    tokens.push_back({ExprTokenType::EOF_, pos, pos});
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
        tokens.back().type = ExprTokenType::NUMBER;
        pos++;
        tokenNum();
        return;
      }
      if (!isdigit(input[pos]))
        return;
      tokens.back().end = pos++;
    }
  }

  void tokenIdent() {
    while (pos < input.size() && isalpha(input[pos])) {
      tokens.back().end = pos++;
    }
  }

  std::vector<ExprToken> tokens;
  std::string input;
  int pos;
};
