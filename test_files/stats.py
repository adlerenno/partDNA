# This File is used to make the tests on debian (bwt.cs.upb.de)
import datetime
DIR = "/data"
SOURCE = f'{DIR}/source/'
INPUT = f"{DIR}/data"
OUTPUT = f"{DIR}/data_bwt"
RESULTS = f"{DIR}/results/"
MAX_MAIN_MEMORY = 128
NUMBER_OF_PROCESSORS = 32
TIMESTAMP = str(datetime.datetime.now())

# The ending / is necessary for all paths.
PATH_BCR = f'{DIR}/BCR/'
PATH_BEETL = f'{DIR}/BEETL/build/bin/'
PATH_BWTDISK = f'{DIR}/bwtdisk/'
PATH_ROPEBWT = f'{DIR}/ropebwt/'
PATH_ROPEBWT2 = f'{DIR}/ropebwt2-master/'
PATH_ROPEBWT3 = f'{DIR}/ropebwt3/'
PATH_CMSBWT = f'{DIR}/cms-bwt/CMS-BWT/'
PATH_BIGBWT = f'{DIR}/bigBWT/Big-BWT/'
PATH_GRLBWT = f'{DIR}/grlBWT/grlBWT/build/'
PATH_GRLBWT_TEMP_FOLDER = f'{DIR}/tmp/'
PATH_EGAP = f'{DIR}/egap/'
PATH_GSUFSORT = f'{DIR}/gsufsort/'
PATH_PFP_BWT = f'{DIR}/PFP-eBWT/build/'
PATH_R_PFP_BWT = f'{DIR}/r-pfbwt/build/'
PATH_PFP = f'{DIR}/pfp/'
PATH_R_PFP_BWT_TEMP_DIR = f'{DIR}/tmp/'
PATH_SPLIT = f'{DIR}/partDNA/build/'
PATH_LIBDIVSUFSORT = f'{DIR}/libdivsufsort/build/'
