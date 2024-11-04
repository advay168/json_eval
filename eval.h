#pragma once

#include "exprParser.h"
#include "json.h"
#include "jsonParser.h"

inline Json evaluate(const std::string &jsonInput, const std::string &exprInput) {
  JsonParser jsonParser;
  // std::cout << jsonInput << std::endl;
  // std::cout << "Parsing JSON..." << std::endl;
  Json parsedJson = jsonParser.parse(jsonInput);
  ExprParser exprParser;
  // std::cout << exprInput << std::endl;
  // std::cout << "Parsing expr..." << std::endl;
  Json result = exprParser.parse(parsedJson, exprInput);
  return result;
}
