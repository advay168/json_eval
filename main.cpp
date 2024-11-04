#include <fstream>
#include <iostream>

#include "eval.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " [json_file] [expression]"
              << std::endl;
    return 0;
  }

  std::string jsonPath = argv[1];

  std::ifstream file(jsonPath);
  if (file.fail()) {
    std::cout << "Error in opening file: " << jsonPath << std::endl;
    return 1;
  }

  std::stringstream jsonInput;
  jsonInput << file.rdbuf();

  try {
    Json result = evaluate(jsonInput.str(), argv[2]);
    std::cout << result->toString() << std::endl;
  } catch (JsonParseError x) {
    std::cerr << "Json Parse Error: " << x.what();
  } catch (ExprParseError x) {
    std::cerr << "Expr Parse Error: " << x.what();
  } catch (InvalidOperation x) {
    std::cerr << "Invalid Operation: " << x.what();
  }
}
