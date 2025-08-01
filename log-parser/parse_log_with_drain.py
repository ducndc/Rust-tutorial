#!/usr/bin/env python

import sys
sys.path.append('../../')
from logparser.Drain import LogParser

input_dir = '../data/input_log/' # The input directory of log file
output_dir = '../data/result/'  # The output directory of parsing results
log_file = 'logmesh'  # The input log file name
#log_format = '<Date> <Time> <Level> <Component>: <Content>'  # wifi log format
log_format = '<Timestamp> <Level> <Component>: <Code> <Tag> <Content>'

# Regular expression list for optional preprocessing (default: [])
# regex      = [
#     r'blk_(|-)[0-9]+' , # block id
#     r'(/|)([0-9]+\.){3}[0-9]+(:[0-9]+|)(:|)', # IP
#     r'(?<=[^A-Za-z0-9])(\-?\+?\d+)(?=[^A-Za-z0-9])|[0-9]+$', # Numbers
# ]
regex = [
    r'blk_(|-)[0-9]+',                            # block id
    r'(/|)([0-9]+\.){3}[0-9]+(:[0-9]+|)(:|)',     # IP
    r'(?<=[^A-Za-z0-9])[-+]?[0-9]+(?=[^A-Za-z0-9])|[0-9]+$', # Numbers
    r'\[[0-9]+\]',                                # các đoạn như [887]
    r'\[[A-Z]+\]',                                # các đoạn như [TOPO], [INFO]
]

st         = 0.5  # Similarity threshold
depth      = 6  # Depth of all leaf nodes

parser = LogParser(log_format, indir=input_dir, outdir=output_dir,  depth=depth, st=st, rex=regex)
parser.parse(log_file)
