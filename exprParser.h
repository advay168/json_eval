#pragma once
#include <format>
#include <iostream>
#include <string>

#include "exprTokeniser.h"
#include "json.h"

struct ExprParser {
  Json parse(Json json, const std::string &input_) {
    input = input_;
    tokeniser.tokenise(input);
    // for (auto [type, a, b] : tokeniser.tokens) {
    //   std::cout << std::format("{}\n", printExprToken(type));
    // }
    pos = 0;
    global = json;
    Json ret = parseHelper();
    if (tokeniser.tokens[pos].type != ExprTokenType::EOF_) {
      // std::cout << pos << ' ' << printExprToken(tokeniser.tokens[pos].type)
      //           << std::endl;
      throw ExprParseError("Unexpected token");
    }
    if (!ret)
      throw ExprParseError("Empty expression");
    return ret;
  }

  Json parseMinMax(bool isMin) {
    if (tokeniser.tokens[pos++].type != ExprTokenType::LEFT_ROUND)
      throw ExprParseError("Expected bracket after min/max");
    std::vector<Json> args;
    while (true) {
      auto arg = parseHelper();
      if (!arg)
        throw ExprParseError("Empty expression");
      args.push_back(arg);
      ExprTokenType type = tokeniser.tokens[pos++].type;
      if (type == ExprTokenType::RIGHT_ROUND)
        break;
      if (type != ExprTokenType::COMMA)
        throw ExprParseError("Expected comma after argument");
    }
    if (args.size() == 1) {
      std::shared_ptr<JsonArray> arr =
          std::dynamic_pointer_cast<JsonArray>(args[0]);
      if (!arr)
        throw InvalidOperation("Can only take min/max of array");
      return isMin ? arr->min() : arr->max();
    } else {
      Json best_obj = args[0];
      double best_val = best_obj->getNumber();
      for (int i = 1; i < args.size(); i++) {
        Json obj = args[i];
        double val = obj->getNumber();
        if (isMin) {
          if (val < best_val) {
            best_val = val;
            best_obj = obj;
          }
        } else {
          if (val > best_val) {
            best_val = val;
            best_obj = obj;
          }
        }
      }
      return best_obj;
    }
  }
  Json parseSize() {
    if (tokeniser.tokens[pos++].type != ExprTokenType::LEFT_ROUND)
      throw ExprParseError("Expected opening bracket after size");
    Json val = parseHelper();
    if (!val)
      throw ExprParseError("Empty expression");
    if (tokeniser.tokens[pos++].type != ExprTokenType::RIGHT_ROUND)
      throw ExprParseError("Expected closing bracket after argument to size");
    return std::make_shared<JsonInt>(val->size());
  }

  Json parseHelper() {
    Json current = nullptr;
    int first = pos;
    while (pos < tokeniser.tokens.size()) {
      auto [type, start, end] = tokeniser.tokens[pos];
      switch (type) {
      case ExprTokenType::MIN: {
        if (pos != first)
          throw ExprParseError("Unexpected min");
        pos++;
        current = parseMinMax(true);
        pos--;
        break;
      }
      case ExprTokenType::MAX: {
        if (pos != first)
          throw ExprParseError("Unexpected max");
        pos++;
        current = parseMinMax(false);
        pos--;
        break;
      }
      case ExprTokenType::SIZE: {
        if (pos != first)
          throw ExprParseError("Unexpected size");
        pos++;
        current = parseSize();
        pos--;
        break;
      }
      case ExprTokenType::LEFT_SQUARE: {
        pos++;
        Json index = parseHelper();
        if (pos == tokeniser.tokens.size() ||
            tokeniser.tokens[pos].type != ExprTokenType::RIGHT_SQUARE)
          throw ExprParseError("Expected closing bracket for subscript");
        if (!index)
          throw ExprParseError("Expected index");
        if (!current)
          current = global;
        current = current->getIndex(index->getInt());
        break;
      }
      case ExprTokenType::RIGHT_SQUARE:
        return current;
      case ExprTokenType::LEFT_ROUND:
        return current;
      case ExprTokenType::RIGHT_ROUND:
        return current;
      case ExprTokenType::INT:
        current = std::make_shared<JsonInt>(
            std::stoi(input.substr(start, end - start + 1)));
        break;
      case ExprTokenType::NUMBER:
        current = std::make_shared<JsonNumber>(
            std::stod(input.substr(start, end - start + 1)));
        break;
      case ExprTokenType::DOT:
        if (!current)
          throw ExprParseError("Unexpected dot");
        if (pos + 1 == tokeniser.tokens.size() ||
            tokeniser.tokens[pos + 1].type != ExprTokenType::IDENT)
          throw ExprParseError("Expected identifier after dot");
        break;
      case ExprTokenType::COMMA:
        return current;
      case ExprTokenType::IDENT: {
        std::string ident = input.substr(start, end - start + 1);
        if (!current)
          current = global;
        else {
          if (pos > 0 && tokeniser.tokens[pos - 1].type != ExprTokenType::DOT)
            throw ExprParseError("Unexpected identifier");
        }
        current = current->getKey(ident);
        break;
      }
      case ExprTokenType::EOF_:
        return current;
      }
      pos++;
    }
    return current;
    throw; // Unreachable
  }

  void parseArray(std::vector<Json> &arr) {
    if (tokeniser.tokens.back().type == ExprTokenType::RIGHT_ROUND)
      return;

    // Trailing commas not allowed
    while (pos < tokeniser.tokens.size()) {
      // arr.push_back(parseHelper());
      auto type = tokeniser.tokens[pos++].type;
      if (type == ExprTokenType::RIGHT_ROUND)
        return;
      if (type != ExprTokenType::COMMA)
        throw ExprParseError("Comma expected between elements of array");
    }
  }

  std::string input;
  Json global;
  int pos;
  ExprTokeniser tokeniser;
};
