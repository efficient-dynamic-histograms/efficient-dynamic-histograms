from collections import defaultdict
import pandas as pd

sketch_size = [5,10,15,20,30, 50]
bin_size = [30,50]

def get(ss,bs):
    with open(f"auc_{ss}s_{bs}.tex") as f:
        latex_data = f.read()
    lines = latex_data.strip().splitlines()
    lines = [line for line in lines if not line.startswith('\\')]
    header_line = lines[0]
    data_lines = lines[1:]

    header = [x.strip() for x in header_line.strip().strip("\\").split("&")]
    data = []

    for line in data_lines:
        parts = [x.strip() for x in line.strip().strip("\\").split("&")]
        data.append(parts)

    df = pd.DataFrame(data, columns=["dataset"] + header[1:])
    df.iloc[:, 1:] = df.iloc[:, 1:].astype(float)
    df = df[['dado', 'dado_corrected', 'benhaim_simple', 'benhaim_advanced', 'kde_benhaim', 'limited_kde_benhaim']]
    print(df.head())
    mean_values = df.mean(axis=None)
    return mean_values

d = defaultdict(lambda: defaultdict(float))
for ss in sketch_size:
    for bs in bin_size:
        d[ss][bs] = get(ss,bs)

dff = pd.DataFrame(d)
print(dff)
dff.to_csv("auc_avg_results.csv")