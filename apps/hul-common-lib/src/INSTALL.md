## Install hul-common-lib
### Build example

```
mkdir hul-common-lib
cd  hul-common-lib
mkdir build install

git clone https://github.com/spadi-alliance/hul-common-lib.git hul-common-lib.src.git 

cmake \
   -S ./hul-common-lib.src.git \
   -B ./build \
   -DCMAKE_INSTALL_PREFIX=./install
cd build
make
make install
```
