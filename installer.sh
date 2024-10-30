git clone https://github.com/Kistler-Group/sdbus-cpp
cd sdbus-cpp

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release 
cmake --build .
sudo cmake --build . --target install

cd ../..

sudo apt update
sudo apt install sqlite3 libsqlite3-dev

mkdir build
cd build
cmake -S ../src -B .
cmake --build .

