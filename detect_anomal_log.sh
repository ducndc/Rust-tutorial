#!/bin/bash

echo "Parse log"
echo "************* GET LOG **************"
cd ./data/original_log
tar -xvf $1
cat logmesh.old logmesh > logmesh
cat messages.old messages > messages

cp logmesh ../input_log/logmesh
cp messages ../input_log/messages 
rm -rf *
cd ../..

echo "************* CHECK LOG **************"
cd ./log-parser
python parse_log_with_drain.py

echo "************* ANALAYSIS LOG **************"
cd ../loganalysis
python test_PCA_without_label.py

echo "************* VIEW RESULTS **************"
libreoffice --calc ./results/anomalous_events_with_templates.csv
