set -xe

if [ ! -d build ]; then
    mkdir build
fi;

if [ ! -d build/linux64 ]; then
    mkdir build/linux64
fi

gcc -shared -fPIC -o ./libs/libtest.so ./src/test.c -lm
zip -9 ./build/pong-bang.love -r ./assets/ ./libs/libtest.so ./src main.lua
cat ./libs/linux64/love ./build/pong-bang.love > ./build/linux64/pong-bang
chmod +x ./build/linux64/pong-bang
