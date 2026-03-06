import os
import numpy as np
import pandas as pd
from collections import defaultdict
from tqdm import tqdm

results_dir = "archive/results_fixed_15_50"
data = defaultdict(lambda: defaultdict(list))
final_aucs = defaultdict(lambda: defaultdict(float))
from sklearn.metrics import roc_auc_score
for root, _, files in os.walk(results_dir):
    for file in files:
        if file.endswith(".csv"):
            full_path = os.path.join(root, file)
            rel_path = os.path.relpath(full_path, results_dir).replace("\\", "/")
            parts = rel_path.split("/")
            if len(parts) != 2:
                continue
            dataset = parts[0]
            filename = parts[1]
            if "_" not in filename:
                continue
            method_iter = filename.rsplit(".", 1)[0]
            method, iteration = method_iter.rsplit("_", 1)
            try:
                iteration = int(iteration)
                df = pd.read_csv(full_path, names=['pred', 'true']).replace(np.nan, np.finfo(np.float64).max).replace(np.inf, np.finfo(np.float64).max).replace(-np.inf, np.finfo(np.float64).max)
                data[dataset][method].append((iteration, df))
            except ValueError as e:
                print(e)
                continue

for dataset in data:
    for method in data[dataset]:
        data[dataset][method].sort(key=lambda x: x[0])
        data[dataset][method] = [df for _, df in data[dataset][method]]

data = dict(data)
for dataset,d in tqdm(data.items()):
    for method, result_list in d.items():
        aucs = []
        for idx, result in enumerate(result_list):
            try:
                auc = roc_auc_score(result['true'], result['pred'])
            except ValueError as e:
                print(dataset, method, idx)
                raise e
            aucs.append(auc)
        final_aucs[dataset][method] = np.mean(aucs)

df = pd.DataFrame(final_aucs).transpose()
order = ['iforest', 'dado', 'dado_corrected', 'benhaim_simple', 'benhaim_advanced', 'benhaim_incorrect', 'kde_benhaim', 'limited_kde_benhaim']
# df[order].to_latex("archive/aucs/50.tex", float_format='%.3f')
df[order].to_csv("archive/aucs/results_fixed_15_50.csv")
