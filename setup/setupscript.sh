#!/bin/bash
# TODO 1:  change this, so we can take a parameter for a dir to cd in to...
cd /home/cx/dev/packages
# /TODO

wget https://github.com/mongodb/mongo-c-driver/releases/download/1.15.3/mongo-c-driver-1.15.3.tar.gz
tar xzf mongo-c-driver-1.15.3.tar.gz
mv mongo-c-driver-1.15.3.tar.gz ./archives
cd mongo-c-driver-1.15.3
mkdir cmake-build
cd cmake-build
cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
make
sudo make install
# TODO 1:  change this, so we can take a parameter for a dir to cd in to...
cd /home/cx/dev/packages/
# /TODO
git clone https://github.com/mongodb/mongo-cxx-driver.git \
    --branch releases/stable --depth 1
cd mongo-cxx-driver/build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_COMPILER=gcc-8 -DBUILD_SHARED_AND_STATIC_LIBS=ON
