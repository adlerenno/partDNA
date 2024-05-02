# BWTSplit

You should be able to compile the whole project running using snakemake:

```
snakemake build
```

You can manually build everything by:

```
wget https://web.archive.org/web/20230309123010/https://sites.google.com/site/yuta256/sais-2.4.1.zip
unzip sais-2.4.1.zip
cd sais-2.4.1
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_INSTALL_PREFIX="/usr/local/lib" -DBUILD_EXAMPLES=OFF -DBUILD_SAIS64=ON -DBUILD_SHARED_LIBS=OFF ..
make
sudo make install
cd ../..
git clone https://github.com/adlerenno/BWTSplit.git
mkdir -p build
cd build
cmake ..
make
./exc -h
```

This installs the sais suffix array construction and builds the current master from BWTSplit project.
