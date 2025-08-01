#!/bin/bash

cd ./log-parser

python parse_log_with_drain.py

cd ../loganalysis

python test.py

libreoffice --calc ./results/anomalous_events_with_templates.csv
