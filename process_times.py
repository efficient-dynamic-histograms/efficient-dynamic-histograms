import os
import json
import pandas as pd

reports_dir = "archive/reports_fixed_30_30"
all_records = []

for filename in os.listdir(reports_dir):
    filepath = os.path.join(reports_dir, filename)
    with open(filepath, "r") as f:
        content = json.load(f)
        for key, value in content.items():
            if isinstance(value, list):
                for row in value:
                    if isinstance(row, dict):
                        row["_source"] = key
                        row["file"] = filepath.split('/')[-1].removeprefix("result_")
                        all_records.append(row)

df = pd.DataFrame(all_records)
result = df.pivot_table(
    index='file',
    columns='_source',
    values='milliseconds',
    aggfunc='mean'
).reset_index().set_index("file")

print(result)
order = ['iforest', 'dado', 'dado_corrected', 'benhaim_simple', 'benhaim_advanced', 'benhaim_incorrect', 'kde_benhaim', 'limited_kde_benhaim']
result[order].round(2).to_csv("archive/times/times_fixed_30_30.csv")
# result[order].round(2).to_latex("data/times.tex", float_format='%.2f')