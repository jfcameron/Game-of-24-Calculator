## 24 Game Calculator

### Motivation
I was introduced to this arithmetic game recently and was inspired to write an algorithim that could solve it generally. I took the brute force approach: it calculates all possible expressions for an input and records those that = 24. A live demo can be tried <a href="https://jfcameron.github.io/Game-of-24-Calculator/">here</a>. The demo requires a browser that supports webassembly.

### About the code
"src" directory contains the c++ code, this is the source code for the calculator itself.
"websrc" contains the frontend and javascript wrapper code for the web build.

### Building
The application can be built for linux, mac, windows as well as browsers that support webassembly. Native builds do have a performance advantage over web. printing to standard out on native is also much faster than web, so the set limit is lifted there. Building can be done from the "workspace" directory. During development I used gcc, clang and emscripten toolchains. For windows: gcc on windows linux submodule worked. I did not try mingw or msvc, but they probably work (famous last words) as no compiler-specific language extensions were used nor platform specific headers.

