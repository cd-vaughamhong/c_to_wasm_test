Setup
================================================================================

* Install emscripten - https://emscripten.org/docs/getting_started/downloads.html
* Install Python3 - https://www.python.org/downloads

How to run tests:
================================================================================

1. Run build script to generate html/js/wasm from c file (main.c).

    ./build.sh - builds without optimizations.
    or
    ./build_optimized - builds with optimizations.

2. Start local (python) server.

    ./start_local_server.sh

3. Open browser.

    http://localhost:8000/main.html.