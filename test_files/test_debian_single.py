import os
import sys
import time
import subprocess
from stats import *

# This File is used to make the tests on debian (bwt.cs.upb.de)
dryrun = False

# Use the values in the comments to run the other test
filenames_to_split = [
    ['GRCh38_chromosom_1_singleline.fasta', True, 0],
    #['GRCh38_chromosom_all.fasta', False, 0],
]
SPLIT_RANGE_START = 1  # 3
SPLIT_RANGE_END = 11  # 4

filenames = [
]


def system(cmd: str):
    """Returns time in nanoseconds (Remove _ns from time to return seconds)"""
    if cmd == '':
        raise ValueError('Command is only an empty string, please check that...')
    print(cmd)
    cmd_list = cmd.split(' ')
    t = 0
    if not dryrun:
        t = time.time_ns()  # process_time()
        proc = subprocess.run(cmd_list)
        t = (time.time_ns() - t)
        # if proc != 0:
        #    print("Command returned %d" % res)
    return t


def mkdir(savedir):
    try:
        os.makedirs(savedir)
    except FileExistsError:
        pass


def filesize(path):
    ret = 0
    appendixes = ['', '.ebwt', '.rl_bwt', '.bwt', '.rlebwt']
    for appendix in appendixes:
        if os.path.exists(path + appendix):
            # print(f'added {appendix} to overall file size of {path}')
            ret += os.stat(path + appendix).st_size
    return ret


def size_folder(path):
    if str(path).endswith('.DS_Store'):
        return 0
    files = os.listdir(path)
    ret = 0
    for file in files:
        file = path + '/' + file
        if os.path.isdir(file):
            ret += size_folder(file)
            print(file, 'folder', ret)
        else:
            ret += os.stat(file).st_size
            print(file, 'file', ret)
    return ret


def csv_writer(filepath, names, data):
    # avoid overwriting files.
    if dryrun:
        return
    filepath = f'{RESULTS}{filepath}'
    mkdir('results')
    if os.path.exists(filepath):
        with open(filepath, 'a') as f:
            for d in data:
                f.write(f'{TIMESTAMP}; ')
                f.write('; '.join(map(str, d)) + '\n')
            f.flush()
    else:
        with open(filepath, 'w') as f:
            f.write('test_date; ')
            f.write('; '.join(names) + '\n')
            for d in data:
                f.write(f'{TIMESTAMP}; ')
                f.write('; '.join(map(str, d)) + '\n')
            f.flush()


def bcr(source: str, destination: str, multiword: bool):
    #if multiword:
    command = f'{PATH_BCR}BCR_LCP_GSA {source} {destination}'
    #else:
    #    command = f'{PATH_BCR_SINGLE_WORD}BCR_LCP_GSA {source} {destination}'
    return system(command)


def beetl(source: str, destination: str, multiword: bool):
    """Can only process files of equal read length"""
    command = f'{PATH_BEETL}beetl-bwt -i {source} -o {destination} --output-format=ascii'
    return system(command)


def bwtdisk(source: str, destination: str, multiword: bool):
    """Use param -t1 for gzip files."""
    command = f'{PATH_BWTDISK}bwte {source}'
    t = system(command)
    system(f'mv {source}.bwt {destination}')
    system(f'mv {source}.bwt.aux {destination}.aux')
    return t


def ropebwt(source: str, destination: str, multiword: bool):
    """ -t for threading. -R discard reverse strang, -N discard ambiguous bases."""
    command = f'{PATH_ROPEBWT}ropebwt -t -R -o {destination} {source}'
    return system(command)


def ropebwt2(source: str, destination: str, multiword: bool):
    """Use parameter -brm{MAX_MAIN_MEMORY}g for multithreading and 4G Main Memory. Use -P for single thread."""
    command = f'{PATH_ROPEBWT2}ropebwt2 -R -o {destination} {source}'
    return system(command)


def ropebwt3(source: str, destination: str, multiword: bool):
    """Use parameter -o for outputfile -t for threads. """
    command = f"{PATH_ROPEBWT3}ropebwt3 build -R -t{NUMBER_OF_PROCESSORS} -do {destination} {source}"
    return system(command)


def bigbwt(source: str, destionation: str, multiword: bool):
    """Adjust window -w and modulus -m. -t {NUMBER_OF_PROCESSORS} for experimental multithreading."""
    command = f'{PATH_BIGBWT}bigbwt {source} -t {NUMBER_OF_PROCESSORS}'
    t = system(command)
    for ending in ['log', 'bwt']:
        system(f'mv {source}.{ending} {destionation}.{ending}')
    return t


def grlbwt(source: str, destination: str, multiword: bool):
    command = f'{PATH_GRLBWT}grlbwt-cli {source} -t {NUMBER_OF_PROCESSORS} -T {PATH_GRLBWT_TEMP_FOLDER}'
    t = system(command)
    command = f'{PATH_GRLBWT}grl2plain {DIR}/{source.split("/")[-1].split(".")[0]}.rl_bwt {destination}'
    t += system(command)
    system(f'rm {DIR}/{source.split("/")[-1].split(".")[0]}.rl_bwt')
    return t


def egap(source: str, destination: str, multiword: bool):
    """--lcp for LCP construction. --em for external memory force, --se for semi-external and --im for internal memory."""
    command = f'{PATH_EGAP}eGap {source} -o {destination}'
    return system(command)


def gsufsort(source: str, destination: str, multiword: bool):
    """--upper converts all chars to upper case. """
    if multiword:
        command = f'{PATH_GSUFSORT}gsufsort-64 {source} --upper --bwt --output {destination}'
    else:
        command = f'{PATH_GSUFSORT}gsufsort-64 {source} --upper --txt --bwt --output {destination}'
    return system(command)


def pfpbwt(source: str, destination: str, multiword: bool):
    """Helper threads are definable. --reads for multiword, without for a single input"""
    command = f'python3 {PATH_PFP_BWT}pfpebwt {source} -v -w 10 -p 100 -t {NUMBER_OF_PROCESSORS} {"--reads" if multiword else ""}'
    t = system(command)
    system(f'mv {source}.ebwt {destination}.ebwt')
    system(f'mv {source}.I {destination}.I')
    system(f'mv {source}.log {destination}.log')
    system(f'mv {source}.ebwt.r {destination}.ebwt.r')
    system(f'mv {source}.*.* {destination}.*.*')
    return t


def r_pfpbwt(source: str, destination: str, multiword: bool):
    """Does this work? Outputs rle-compressed bwt."""
    command = f'{PATH_PFP}pfp++ -f {source} -w 10 -p 100 --output-occurrences'
    t = system(command)
    command = f'{PATH_PFP}pfp++ -i {source}.parse -w 5 -p 11'
    t += system(command)
    command = f'{PATH_R_PFP_BWT}rpfbwt --l1-prefix {source} --w1 10 --w2 5 --threads {NUMBER_OF_PROCESSORS} --tmp-dir {PATH_R_PFP_BWT_TEMP_DIR} --bwt-only'
    t += system(command)
    for ending in ['dict', 'occ', 'parse', 'parse.dict', 'parse.parse', 'rlebwt', 'rlebwt.meta']:
        system(f'mv {source}.{ending} {destination}.{ending}')
    return t


def divsufsort(source: str, destination: str, multiword: bool):
    """divsufsort testing"""
    command = f'{PATH_LIBDIVSUFSORT}dss -i {source} -o {destination}'
    t = system(command)
    return t


def split_file(source: str, destination: str, run_length: int, multiline: bool):
    # print(f'Splitting -{source}- to -{destination}-')
    command = f'{PATH_SPLIT}exc -i {source} -o {destination} -r {run_length}' + (' -d' if not multiline else '')
    command += f' >> {RESULTS}file_stats.txt'
    return system(command)


def prepare_files(file: str, multiline: bool, output_basename: str):
    # check for .gz file
    if file.endswith('.gz'):
        if os.path.isfile(file) and not os.path.isfile(file[:-3]):
            system(f'gzip -dk {file}')  # use -dk to decompress and keep the old file
        file = file[:-3]
        if not os.path.isfile(file):
            raise FileNotFoundError(f"Input file does not exist: {file}")

    is_fasta = file.split('.')[-1] in {'fa', 'fasta'}
    # check for fasta and create one word per line file
    line_number = 0
    written_chars = 0
    status = 1  # used to process multiline files correctly.
    if not os.path.isfile(output_basename + '.owpl'):
        print(f'Preparing file: {file}')
        with open(file) as input_file:
            with open(output_basename + '.owpl', 'w') as output_owpl:  # owpl = one word per line
                with open(output_basename + '.fa', 'w') as output_fasta:
                    with open(output_basename + '.fq', 'w') as output_fastq:
                        for line in input_file:
                            line = line.upper()
                            if (multiline and is_fasta and line_number % 2 == 1) \
                                    or (multiline and not is_fasta and line_number % 4 == 1):
                                assert line[0] not in {'>', '+', '@', ':', ';', 9, 8}
                                line = ''.join(filter(lambda x: x in {'A', 'C', 'G', 'T'}, line))

                                output_owpl.write(line + '\n')

                                output_fasta.write(f'>S{line_number}\n')
                                output_fasta.write(line + '\n')

                                output_fastq.write(f'@S{line_number} length={len(line)}\n')
                                output_fastq.write(line + '\n')
                                output_fastq.write(f'+ length={len(line)}\n')
                                output_fastq.write(':' * len(line) + '\n')
                            if not multiline:
                                if line[0] == '>':
                                    output_fasta.write(line + '\n')
                                    if status != 1:
                                        output_fastq.write(f'+ length={written_chars}\n')
                                        while written_chars > 0:
                                            to_write = max(80, written_chars)
                                            output_fastq.write(':'*to_write + '\n')
                                            written_chars -= to_write
                                    output_fastq.write(line + '\n')
                                    output_owpl.write('\n')
                                    status = 2
                                elif line[0] == '+':
                                    status = 4
                                elif status == 2:
                                    line = ''.join(filter(lambda x: x in {'A', 'C', 'G', 'T'}, line))

                                    output_owpl.write(line)

                                    output_fasta.write(line + '\n')
                                    output_fastq.write(line + '\n')
                                    written_chars += len(line)
                            line_number += 1
                        if multiline:
                            output_fasta.write('\n')
                            output_fastq.write(f'+ length={written_chars}\n')
                            while written_chars > 0:
                                to_write = max(80, written_chars)
                                output_fastq.write(':' * to_write + '\n')
                                written_chars -= to_write
                        output_owpl.flush()
                        output_fasta.flush()
                        output_fastq.flush()

    # create fasta.gz and fastq.gz files
    if not os.path.isfile(f'{output_basename}.fa.gz'):
        system(f'gzip -k {output_basename}.fa')
    if not os.path.isfile(f'{output_basename}.fq.gz'):
        system(f'gzip -k {output_basename}.fq')

    # all 5 files contain equal words now.


def test():
    # Possible, methods
    methods = [
        # ('beetl', beetl, '.fa', {}),  # .fa file, only if all inputs are of equal length.
        # ('bwtdisk', bwtdisk, '.fq.gz', {}),  # .gz file, much slower
        ('ropebwt', ropebwt, '.fq.gz', {}),  # .gz file
        ('ropebwt2', ropebwt2, '.fq.gz', {}),  # .gz file
        ('ropebwt3', ropebwt3, '.fq.gz', {}),  # .gz file
        ('bigBWT', bigbwt, '.fa', {}),  # .fa
        ('grlBWT', grlbwt, '.owpl', {}),  # one word per line
        ('eGap', egap, '.fa', {}),  # .fa
        ('gsufsort', gsufsort, '.fq.gz', {}),  # .gz
        ('r-pfbwt', r_pfpbwt, '.fa', {}),  # .fa
        ('divsufsort', divsufsort, '.fa', {}),
        ('BCR', bcr, '.fq.gz', {})  # .gz file
    ]

    savedirs = [OUTPUT + '/' + name for (name, _, _, _) in methods]
    # ['SRR006041.split.fasta.gz', True, 1]

    # Split files.
    for i in range(len(filenames_to_split)):
        for j in range(SPLIT_RANGE_START, SPLIT_RANGE_END):
            new_file_name = filenames_to_split[i][0].split('.')[0] + f'_split_{j}.fasta'
            t = split_file(SOURCE + filenames_to_split[i][0], SOURCE + new_file_name, j, filenames_to_split[i][1])
            filenames.append([new_file_name, True, j])

            data_split = [(filenames[i][0], str(filenames[i][1]), filenames[i][2], t, 'ns')]
            csv_writer('result_split.csv', ('data_set', 'multiline', 'run_length', 'time', 'time_unit'), data_split)
        filenames.append(filenames_to_split[i])

    # Prepare files for testing
    for i in range(len(filenames)):
        try:
            prepare_files(SOURCE + filenames[i][0], filenames[i][1], INPUT + '/' + filenames[i][0].split('.')[0])
            filenames[i][0] = filenames[i][0].split('.')[0]  # Used to append the correct file ending according to the approach.
        except FileNotFoundError:
            print(f'File {SOURCE}{filenames[i][0]} not found.')
            filenames[i][0] = 'n/a'

    for (methodname, method, file_extension, params), savedir in zip(methods, savedirs):
        for file, multiword, _ in filenames:
            try:
                # print('compress', file, 'by', methodname)
                if file == 'n/a':
                    raise ValueError(f'File not exists. Skipping {methodname}')
                from_file = INPUT + '/' + file + file_extension
                mkdir(savedir)
                to_file = savedir + '/' + file + file_extension
                t = method(from_file, to_file, multiword, **params)

                data = [(file, methodname, str(params), str(t), 'ns',
                             str(filesize(from_file)), str(filesize(to_file)))]
                csv_writer('result.csv', (
                    'data_set', 'method', 'params', 'time', 'time_unit', 'original_file_size', 'compressed_file_size'),
                           data)
                # print('compressed', file, 'by', methodname, 'in', t, 'nanoseconds')
            except ValueError as e:
                print(e)
                print(f'Continue with next, cause of an internal error of {methodname}.')
                csv_writer('result.csv', (
                    'data_set', 'method', 'params', 'time', 'time_unit', 'original_file_size', 'compressed_file_size'),
                           [(file, methodname, str(params), 'failed', '', '', '')])


if __name__ == '__main__':
    if '--dryrun' in sys.argv:
        dryrun = True
    test()
