DATA_SETS = ['SRR006041.recal']

rule convert_targets:
    input:
        'testdata/split_SRR006041.recal.fastq.gz'

rule convert:
    input:
        script = "build/exc",
        in_file = "{filename}"
    output:
        out_file = "split_{filename}"
    benchmark:
        "benchmark.csv"
    shell:
        """
        {input.script} -i {input.in_file} -o {output.out_file} -r 3
        """

rule build:
    input:
        libsais = "/usr/local/lib/libsais64.a"
    output:
        script = "build/exc"
    shell:
        """
        mkdir -p build
        cd build
        cmake ..
        make
        """

rule build_sais:
    output:
        libsais = "/usr/local/lib/libsais64.a"
    shell:
        """
        wget https://web.archive.org/web/20230309123010/https://sites.google.com/site/yuta256/sais-2.4.1.zip
        unzip sais-2.4.1.zip
        cd sais-2.4.1
        mkdir -p build
        cd build
        cmake -DCMAKE_BUILD_TYPE="Release" -DCMAKE_INSTALL_PREFIX="/usr/local" -DBUILD_EXAMPLES=OFF -DBUILD_SAIS64=ON -DBUILD_SHARED_LIBS=OFF ..
        make
        sudo make install
        """
