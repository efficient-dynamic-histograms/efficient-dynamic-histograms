import os
import pathlib
import numpy as np
import pandas as pd
from collections import defaultdict
from tqdm import tqdm
from stats import calc_ad, calc_cm,calc_dkl,calc_ks
results_dir = "archive/distributions_memory/results"
data = defaultdict(lambda: defaultdict(list))

final_dkl = defaultdict(lambda: defaultdict(float))
final_cm = defaultdict(lambda: defaultdict(float))
final_ks = defaultdict(lambda: defaultdict(float))
final_ad = defaultdict(lambda: defaultdict(float))

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
                df = pd.read_csv(full_path, names=['x','est']).replace(np.nan, np.finfo(np.float64).max)
                data[dataset][method].append((iteration, df))
            except ValueError:
                continue

for dataset in data:
    for method in data[dataset]:
        data[dataset][method].sort(key=lambda x: x[0])
        data[dataset][method] = [df for _, df in data[dataset][method]]

data = dict(data)
# print(data['pareto']['benhaim_simple'])
for dataset,d in tqdm(data.items()):
    for method, result_list in d.items():
        dkls = []
        cms = []
        kss = []
        ads = []
        for result in result_list:
            dkl = calc_dkl(dataset, result['x'], result['est'])
            dkls.append(dkl)
            cm = calc_cm(dataset, result['x'], result['est'])
            cms.append(cm)
            ks = calc_ks(dataset, result['x'], result['est'])
            kss.append(ks)
            ad = calc_ad(dataset, result['x'], result['est'])
            ads.append(ad)
        final_dkl[dataset][method] = np.mean(dkls)
        final_cm[dataset][method] = np.mean(cms)
        final_ks[dataset][method] = np.mean(kss)
        final_ad[dataset][method] = np.mean(ads)

df_dkl = pd.DataFrame(final_dkl).transpose()
df_cm = pd.DataFrame(final_cm).transpose()
df_ks = pd.DataFrame(final_ks).transpose()
df_ad = pd.DataFrame(final_ad).transpose()
order = ['dado', 'dado_corrected', 'benhaim_simple', 'benhaim_advanced', 'benhaim_incorrect', 'kde_benhaim', 'limited_kde_benhaim']
base_path = pathlib.Path("archive/distributions_memory/reports")


base_path.mkdir(parents=True, exist_ok=True)
df_dkl[order].to_csv(base_path / "dkl.csv")
df_cm[order].to_csv(base_path / "cm.csv")
df_ks[order].to_csv(base_path / "ks.csv")
df_ad[order].to_csv(base_path / "ad.csv")