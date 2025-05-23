# TrogonDB

A key-value database compatible with Redis.


## How to build

GNU/Linux:

```sh
cd build
conan install .. --profile:all ../conan/profiles/linux.x86_64.clang.release --build=missing
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++
cmake --build . --config Release --parallel
```
