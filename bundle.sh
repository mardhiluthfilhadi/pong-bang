set -xe

if [ ! -d build ]; then
    mkdir build
fi;

if [ ! -d build/linux64 ]; then
    mkdir build/linux64
    mkdir build/linux64/libs
fi

gcc -shared -fPIC -o ./libs/libtest.so ./src/test.c -lm
zip -9 ./build/pong-bang.love -r ./assets/ ./src main.lua
cat ./libs/linux64/love ./build/pong-bang.love > ./build/linux64/pong-bang
cp ./libs/libtest.so ./build/linux64/libs/
chmod +x ./build/linux64/pong-bang
cd build/linux64/ && zip -9 pong-bang-linux-x64.zip ./pong-bang libs/libtest.so
