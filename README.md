# partDNA

partDNA is a library to partition input DNA sequences to receive a different representation that yields nearly the same BWT except for additional end markes. 

## Build

You can build everything by:

```
git clone https://github.com/adlerenno/partDNA.git
cd partDNA
unzip sais-2.4.1.zip
cd sais-2.4.1
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_INSTALL_PREFIX="/usr/local" -DBUILD_EXAMPLES=OFF -DBUILD_SAIS64=ON -DBUILD_SHARED_LIBS=OFF ..
make
sudo make install
cd ../..
mkdir -p build
cd build
cmake ..
make
./exc -h
```

This installs the SA-IS suffix array construction algorithm and builds the current master from partDNA project.

## Dependencies

- ZLIB

## Command-Line Tool

```
Usage: ./exc
    -h      shows help.
    -i      input file path. See -d for file types.
    -o      output file path. The output file format is fasta.
    -r      run-length of A sequence to partition. Default is 3.
    -d      specifies the used input parser:
                if -d is not used:  reads fastq.gz and fasta.gz files using kseq library and gzip
                if -d is used:      reads fasta files, also multiple lines per read
```

## Test files

To perform the tests of the [partDNA](https://arxiv.org/abs/2406.10610) Paper, see the [partDNAtest](https://github.com/adlerenno/partDNAtest) repository.
