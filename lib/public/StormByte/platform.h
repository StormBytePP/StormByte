#pragma once

// ---------------------------------------------------------------------------
// Operating system
// ---------------------------------------------------------------------------
#if defined(_WIN32) || defined(__CYGWIN__)
    #define WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define MACOS
    #define UNIX
#elif defined(__linux__)
    #define LINUX
    #define UNIX
#elif defined(__unix__) || defined(__unix)
    #define UNIX
#else
    #error "Unsupported operating system"
#endif

// ---------------------------------------------------------------------------
// Architecture (pointer / ILP model)
// ---------------------------------------------------------------------------
#if defined(_WIN64) || defined(__x86_64__) || defined(__amd64__) \
    || defined(__aarch64__) || defined(_M_X64) || defined(_M_ARM64) \
    || (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8)
    #define BIT64
#elif defined(_WIN32) || defined(__i386__) || defined(__i386) \
    || defined(_M_IX86) || defined(__arm__) || defined(_M_ARM) \
    || (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4)
    #define BIT32
#else
    #error "Unsupported architecture (expected 32-bit or 64-bit)"
#endif

// ---------------------------------------------------------------------------
// Compiler
// ---------------------------------------------------------------------------
#if defined(__clang__)
    #define CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define GCC
#elif defined(_MSC_VER)
    #define MSVC
#endif