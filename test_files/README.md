# partDNA test files

In this directory, all files necessary to perform the tests of the partDNA Paper are presented.
They are not working on themselves, as you need to clone and build the BWT construction approaches yourself.

In stats.py, edit the file path to the implementations you want to test. Also adjust the SOURCE, INPUT and OUTPUT paths.
In the source directory, you should put the original input file you want to test.
Be aware that files containing multiple dots may cause problems, better rename them.
In the input directory, the test_debian script will put the partitioned and the original file,
in fasta and fastq file format, in a fasta.gz and fasta.gz and as one read per line file (owpl files).
All files should contain equal strings such that the construction is comparable.
In the output directory, there will be subdirectories for each approach where they put their output files

Note that only some approaches are able to specify a working directory.
BCR, for example, will use the directory of the test_debian_single skript.

In the RESULTS path, the time results will be written.
Adjust the filenames in test_debian_single, if you do not like the name,
but be aware that the name is important and can lead to mismatches.
The results are appended if you already performed some tests.
The timestamp for each test will be equal to identify one execution of test_debian_single.

The tested filenames are listed in filenames_to_split.
It is a list of lists,
where the inner lists contain of the filename in SOURCE, a bool value indicating which parser to use
(if approaches have different parser available) and the third value is not used anymore.
Instead of the third value, there are SPLIT_RANGE_START and SPLIT_RANGE_END.
They are the interval for the value run_length of the partDNA implementation
that the given file should be partitioned with (start is included, end is excluded).
If a file should only be used in BWT construction and not partitioned,
put it (as a list of three elements as above) into the filenames list instead of filenames_to_split.
Do not delete the filenames list, as it will be autopopulated from the filenames_to_split list.

In the test-function, there is the list of tested approaches.
You can comment out approaches you do not want to test or include additional ones.
Each approach consists of a name, a method to perform the approach,
a file extension that its parser is able to read and dictionary with additional static parameters.
The additional static parameters are helpful
if you want to test an approach using different sets of parameters, but you only want a single method to test them.
They will be passed to the given method as keyword parameters.
The method you pass needs the following position parameters: source: str, destination: str, multiword: bool.
source and destination are full file paths.

Last, you have the option dryrun at skript execution time.
This will only print out the commands that will be executed by the script,
it will neither write to the result files nor execute any of the commands, the idea is to check the commands.

The convert_grc_long file was used to concatenate the GRC38 file to a single long genome.

If you need any help, feel free to contact us.
