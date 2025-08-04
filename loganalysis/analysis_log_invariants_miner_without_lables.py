#!/usr/bin/env python
# -*- coding: utf-8 -*-
''' This is a demo file for the Invariants Mining model.
    API usage:
        dataloader.load_HDFS(): load HDFS dataset
        feature_extractor.fit_transform(): fit and transform features
        feature_extractor.transform(): feature transform after fitting
        model.fit(): fit the model
        model.predict(): predict anomalies on given data
        model.evaluate(): evaluate model accuracy with labeled data
'''

import sys
sys.path.append('../')
from loganalysis.models import InvariantsMiner
from loganalysis import dataloader, preprocessing

struct_log = '../data/result/logmesh_structured.csv' # The structured log file
label_file = '../data/result/anomaly_label.csv' # The anomaly label file
epsilon = 0.5 # threshold for estimating invariant space

if __name__ == '__main__':
    # Load structured log without label info
    (x_train, _), (x_test, _), _ = dataloader.load_wifi_log(struct_log, train_ratio=1.0,
                                                           split_type='sequential', save_csv=False)
    # Feature extraction
    feature_extractor = preprocessing.FeatureExtractor()
    x_train = feature_extractor.fit_transform(x_train)

    # Model initialization and training
    model = InvariantsMiner(epsilon=epsilon)
    model.fit(x_train)

    # Predict anomalies on the training set offline, and manually check for correctness
    y_train = model.predict(x_train)

    # Predict anomalies on the test set to simulate the online mode
    # x_test may be loaded from another log file
    x_test = feature_extractor.transform(x_test)
    y_test = model.predict(x_test)

