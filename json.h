#pragma once
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

struct InvalidOperation : std::runtime_error {
  inline InvalidOperation(const std::string &key) : std::runtime_error(key) {}
};

using Json = std::shared_ptr<struct JsonValue>;

struct JsonValue {
  virtual ~JsonValue() = default;
  virtual std::string toString() = 0;
  virtual double getNumber() = 0;
  virtual int getInt() = 0;
  virtual Json &getIndex(int index) = 0;
  virtual Json &getKey(const std::string &key) = 0;
  virtual int size() = 0;
};

struct JsonNull : JsonValue {
  inline virtual std::string toString() { return "null"; }
  inline virtual double getNumber() {
    throw InvalidOperation("Cannot treat null as number");
  };
  inline virtual int getInt() {
    throw InvalidOperation("Cannot treat null as int");
  };
  inline virtual Json &getIndex(int index) {
    throw InvalidOperation("Cannot index null");
  };
  inline virtual Json &getKey(const std::string &key) {
    throw InvalidOperation("Cannot index null");
  };
  inline virtual int size() {
    throw InvalidOperation("Cannot take size of null");
  };
};

struct JsonBool : JsonValue {
  inline JsonBool(bool val) : val(val) {}
  inline virtual std::string toString() { return val ? "true" : "false"; }
  inline virtual double getNumber() {
    throw InvalidOperation("Cannot treat bool as number");
  };
  inline virtual int getInt() {
    throw InvalidOperation("Cannot treat bool as int");
  }
  inline virtual Json &getIndex(int index) {
    throw InvalidOperation("Cannot index bool");
  };
  inline virtual Json &getKey(const std::string &key) {
    throw InvalidOperation("Cannot index bool");
  };
  inline virtual int size() {
    throw InvalidOperation("Cannot take size of bool");
  };
  bool val;
};

struct JsonInt : JsonValue {
  inline JsonInt(int val) : val(val) {}
  inline virtual std::string toString() { return std::to_string(val); }
  inline virtual double getNumber() { return val; };
  inline virtual int getInt() { return val; }
  inline virtual Json &getIndex(int index) {
    throw InvalidOperation("Cannot index int");
  };
  inline virtual Json &getKey(const std::string &key) {
    throw InvalidOperation("Cannot index int");
  };
  inline virtual int size() {
    throw InvalidOperation("Cannot take size of int");
  };
  int val;
};

struct JsonNumber : JsonValue {
  inline JsonNumber(double val) : val(val) {}
  inline virtual std::string toString() { return std::to_string(val); }
  inline virtual double getNumber() { return val; };
  inline virtual int getInt() {
    throw InvalidOperation("Cannot treat double as int");
  }
  inline virtual Json &getIndex(int index) {
    throw InvalidOperation("Cannot index number");
  };
  inline virtual Json &getKey(const std::string &key) {
    throw InvalidOperation("Cannot index number");
  };
  inline virtual int size() {
    throw InvalidOperation("Cannot take size of number");
  };
  double val;
};

struct JsonString : JsonValue {
  inline JsonString(const std::string &val) : val(val) {}
  inline virtual std::string toString() { return '"' + val + '"'; }
  inline virtual double getNumber() {
    throw InvalidOperation("Cannot treat string as number");
  };
  inline virtual int getInt() {
    throw InvalidOperation("Cannot treat string as int");
  }
  inline virtual Json &getIndex(int index) {
    throw InvalidOperation("Cannot index string");
  };
  inline virtual Json &getKey(const std::string &key) {
    throw InvalidOperation("Cannot index string");
  };
  inline virtual int size() { return val.size(); };
  std::string val;
};

struct JsonArray : JsonValue {
  inline virtual std::string toString() {
    std::stringstream ss;
    ss << "[";
    bool first = true;
    for (auto &val : arr) {
      if (!first)
        ss << ", ";
      first = false;
      ss << val->toString();
    }
    ss << "]";
    return ss.str();
  }
  inline Json min() {
    if (arr.size() == 0) {
      throw InvalidOperation("Min called on empty array");
    }
    size_t index = 0;
    double mini = arr[0]->getNumber();
    for (int i = 1; i < arr.size(); i++) {
      double v = arr[i]->getNumber();
      if (v < mini) {
        mini = v;
        index = i;
      }
    }
    return arr[index];
  }
  inline Json max() {
    if (arr.size() == 0) {
      throw InvalidOperation("Min called on empty array");
    }
    size_t index = 0;
    double mini = arr[0]->getNumber();
    for (int i = 1; i < arr.size(); i++) {
      double v = arr[i]->getNumber();
      if (v > mini) {
        mini = v;
        index = i;
      }
    }
    return arr[index];
  }

  inline virtual double getNumber() {
    throw InvalidOperation("Cannot treat array as number");
  };
  inline virtual int getInt() {
    throw InvalidOperation("Cannot treat array as int");
  }
  inline virtual Json &getIndex(int index) {
    if (index < 0 || index > arr.size()) {
      throw InvalidOperation("Invalid index to array: " +
                             std::to_string(index));
    }
    return arr[index];
  };
  inline virtual Json &getKey(const std::string &key) {
    throw InvalidOperation("Cannot index array by key");
  };
  inline virtual int size() { return arr.size(); };
  std::vector<Json> arr;
};

struct JsonObject : JsonValue {
  inline virtual std::string toString() {
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (auto &[key, val] : mapping) {
      if (!first) {
        ss << ", ";
      }
      first = false;
      ss << '"' << key << "\": " << val->toString();
    }
    ss << "}";
    return ss.str();
  }
  inline virtual double getNumber() {
    throw InvalidOperation("Cannot treat object as number");
  };
  inline virtual int getInt() {
    throw InvalidOperation("Cannot treat object as int");
  }
  inline virtual Json &getIndex(int index) {
    throw InvalidOperation("Cannot index object by int");
  };
  inline virtual Json &getKey(const std::string &key) {
    if (auto it = mapping.find(key); it != mapping.end()) {
      return it->second;
    } else {
      throw InvalidOperation("Key not in object: " + key);
    }
  };
  inline virtual int size() { return mapping.size(); };
  std::unordered_map<std::string, Json> mapping;
};
