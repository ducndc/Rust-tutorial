#!/bin/bash

echo "Parse log"
cd ./log-parser
python parse_log_with_drain.py

echo "analaysis log"
cd ../loganalysis
python test.py

echo "view result"
libreoffice --calc ./results/anomalous_events_with_templates.csv
