# json_eval

A CLI program similar to `jq` to manipulate json.

## Features
- Tested on Windows with clang but should be portable
- Parses spec compliant JSON except for string escaping
- Allows simple `jq`-like expressions such as `a.b[0]` and `a.b[2].c`
- Allows nesting expressions such as `a.b[a.b[1]].c`
- Supports intrinsic functions `min()`, `max()`, `size()`
- Allows numeric literals in expressions such as `a.b[0]` and `min(a.b[3], 2.0)`
- Is unit tested with `GTest`

## Quickstart

> [!NOTE]
> The following is for Windows with clang, ninja and cmake

- Clone project
```
git clone https://github.com/advay168/json_eval && cd json_eval
```

- Make temporary build directory
```
mkdir build && cd build
```

- Configure CMake
```
cmake -G Ninja ..
```

- Build project
```
ninja
```
- Run executable
```
json_eval ..\test.json "max(a.b[0], 10, a.b[1], 15)"
> 15
json_eval ..\test.json "a.b[a.b[1]].c"
> "test"
```

- Run tests
```
ctest
> 100% tests passed
```
