include(CheckCXXSourceCompiles)
include(CheckTypeSize)

macro(CheckFeature which_define variable)
    check_cxx_source_compiles("
    int main() {
    #ifndef ${which_define}
    #error
    #endif
    #if !${which_define}
    #error
    #endif
    return 0;
    }" ${variable})
endmacro()

check_cxx_source_compiles("
int main() {
#if defined(__linux__) || defined(__gnu_linux__)
#else
#error
#endif
return 0; }" LINUX)

check_cxx_source_compiles("
int main() {
#if defined(__APPLE__) && defined(__MACH__)
#else
#error
#endif
return 0; }" MACOS)

check_cxx_source_compiles("
int main() {
#if defined(_WIN32) || defined(__WIN32__)
#else
#error
#endif
return 0; }" WIN32)

check_cxx_source_compiles("
int main() {
#if defined(__i386__)
#else
#error
#endif
return 0; }" X86_32)

check_cxx_source_compiles("
int main() {
#if defined(__amd64__) || defined(__x86_64__)
#else
#error
#endif
return 0; }" X86_64)

check_cxx_source_compiles("
int main() {
#if defined(__arm64__) && defined(__aarch64__)
#else
#error
#endif
return 0; }" ARM64)

if(X86_32)
    set(ARCH_NAME "i386")
elseif(X86_64)
    set(ARCH_NAME "x86_64")
elseif(ARM64)
    set(ARCH_NAME "arm64")
endif()

if(NOT (LINUX AND X86_64 OR
        LINUX AND ARM64 OR
        MACOS AND X86_64 OR
        MACOS AND ARM64 OR
        WIN32 AND X86_64))
    message(FATAL_ERROR "Platform not supported")
endif()

string(TOLOWER "${CMAKE_CXX_COMPILER_ID}" CXX_COMPILER_NAME)

configure_file("cmake/helpers/platform_defines.h.in" "${CMAKE_BINARY_DIR}/include/trogondb/platform_defines.h")
