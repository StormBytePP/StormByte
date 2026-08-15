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
    // Other unices (BSD, etc.) if you ever need them
    #define UNIX
#else
    #error "Unsupported operating system"
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