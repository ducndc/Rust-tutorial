#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import pandas as pd
sys.path.append('../')

from loganalysis.models import PCA
from loganalysis import dataloader, preprocessing

struct_log = '../data/result/logmesh_structured.csv'
anomalies_file = './results/anomalies_detected.csv'
templates_file = '../data/result/logmesh_templates.csv'

train_ratio = 0.7

if __name__ == '__main__':
    (x_all, _), (_, _), full_df = dataloader.load_wifi_log(struct_log, train_ratio=1.0,
                                                           split_type='sequential', save_csv=False)

    num_train = int(len(x_all) * train_ratio)
    x_train = x_all[:num_train]
    x_test = x_all[num_train:]
    test_df = full_df.iloc[num_train:].reset_index(drop=True)

    feature_extractor = preprocessing.FeatureExtractor()
    x_train = feature_extractor.fit_transform(x_train, term_weighting='tf-idf', normalization='zero-mean')

    print('Train phase:')
    model = PCA()
    model.fit(x_train)
    y_train = model.predict(x_train)

    print('Test phase:')
    x_test = feature_extractor.transform(x_test)
    y_test = model.predict(x_test)

    test_df['Anomaly'] = y_test
    anomalies = test_df[test_df['Anomaly'] == 1]
    anomalies.to_csv(anomalies_file, index=False)

    anomalies_df = pd.read_csv(anomalies_file)
    templates_df = pd.read_csv(templates_file)

    anomalies_df['EventId'] = anomalies_df['EventSequence'].str.extract(r"\['(.+?)'\]")
    merged_df = pd.merge(anomalies_df, templates_df[['EventId', 'EventTemplate']], on='EventId', how='left')

    unique_rows = merged_df.drop_duplicates(subset=['EventSequence'])
    unique_rows.to_csv('./results/anomalous_events_with_templates.csv', index=False)
