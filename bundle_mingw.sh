
set -xe

if [ ! -d build ]; then
    mkdir build
fi;

if [ ! -d build/win64 ]; then
    mkdir build/win64
fi

x86_64-w64-mingw32-gcc -shared -fPIC -o ./libs/libtest.so ./src/test.c -lm
zip -9 ./build/pong-bang.love -r ./assets/ ./libs/libtest.so ./src main.lua
cat ./libs/win64/love.exe ./build/pong-bang.love > ./build/win64/pong-bang.exe
cp ./libs/win64/*.dll ./build/win64
