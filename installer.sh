sudo apt install git build-essential cmake libsystemd-dev sqlite3 libsqlite3-dev

git clone https://github.com/Kistler-Group/sdbus-cpp
cd sdbus-cpp

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DSDBUSCPP_BUILD_LIBSYSTEMD=OFF 
cmake --build .
sudo cmake --build . --target install

cd ../..

mkdir build
cd build
cmake -S ../src -B .
cmake --build .

