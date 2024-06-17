# partDNA

partDNA is a library to partition input DNA sequences to receive a different representation that yields nearly the same BWT except for additional end markes. 

## Build

You can manually build everything by:

```
wget https://web.archive.org/web/20230309123010/https://sites.google.com/site/yuta256/sais-2.4.1.zip
unzip sais-2.4.1.zip
cd sais-2.4.1
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_INSTALL_PREFIX="/usr/local" -DBUILD_EXAMPLES=OFF -DBUILD_SAIS64=ON -DBUILD_SHARED_LIBS=OFF ..
make
sudo make install
cd ../..
git clone https://github.com/adlerenno/partDNA.git
cd partDNA
mkdir -p build
cd build
cmake ..
make
./exc -h
```

This installs the SA-IS suffix array construction algorithm and builds the current master from partDNA project.

## Command-Line Tool

```
Usage: ./exc
    -h      shows help.
    -i      input file path. See -d for file types.
    -o      output file path. Will always be fasta.
    -r      run-length of A sequence to partition. Default is 3.
    -d      specifies the used input parser:
                if -d is not used:  reads fastq.gz and fasta.gz files using kseq library and gzip
                if -d is used:      reads fasta files, also multiple lines per read
```
