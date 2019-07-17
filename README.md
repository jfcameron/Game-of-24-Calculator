## 24 Game Calculator

# About the code
"src" directory contains the c++ code, this is the source code for the calculator itself. "websrc" contains the frontend and javascript wrapper code for the web build. The application can be built for linux, mac, windows as well as browsers that support webassembly. Native builds do have a performance advantage over web. printing to standard out on native is also much faster than web, so the set limit is lifted there. Building can be done from the "workspace" directory. During development I used gcc, clang and emscripten toolchains. For windows: gcc on windows linux submodule worked. I did not try mingw or msvc, but they probably work (famous last words).

