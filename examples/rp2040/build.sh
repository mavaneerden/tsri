mkdir -p build
cmake -S . -B build -DPICO_TOOLCHAIN_PATH=../toolchain/bin -DTSRI_DIRECTORY=../.. -DTSRI_SVD_FILE=rp2040.svd -DTSRI_NAMESPACE=test -DTSRI_PRETTY_CODE=OFF
cmake --build build --parallel 8
