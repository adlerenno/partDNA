count_smaller_symbols = 0
symbols = 0
with open('./GRCh38_latest_genomic.fna') as f:
    with open('./GRCh38_chromosom_all.fasta', 'w') as out:
        for line in f:
            if line.startswith('>'):
                if count_smaller_symbols == 0:
                    out.write(line)
                count_smaller_symbols += 1
                continue
            #if count_smaller_symbols > 50:
            #    break
            line = ''.join(filter(lambda x: x in {'A', 'C', 'G', 'T'}, line.upper()))
            symbols += len(line)
            if len(line) > 0:
                out.write(line + '\n')
print(symbols)
