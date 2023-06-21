source ../emsdk/emsdk_env.sh
emcc main.c -O3 -msimd128 -o main.html
