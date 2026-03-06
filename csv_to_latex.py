import sys
import pandas as pd

input_file = sys.argv[1]
output_file = sys.argv[2]
mode = sys.argv[3]
FRAC = 0.95
if len(sys.argv) >=5:
    FRAC = float(sys.argv[4])

dataframe = pd.read_csv(input_file)
dataframe = dataframe.sort_values(by=dataframe.columns[0])


numeric_dataframe = dataframe.apply(pd.to_numeric, errors="coerce")

def round_sig(value, digits=3):
    if pd.isna(value):
        return value
    if value == 0:
        return 0
    y = float(f"{value:.{digits}g}")
    return int(y) if y.is_integer() else y

def bold_near_max(row, numeric_row):
    max_value = numeric_row.max()
    threshold = max_value * FRAC
    result = []
    for value, numeric_value in zip(row, numeric_row):
        rounded_value = round_sig(numeric_value)
        display_value = value if pd.isna(numeric_value) else rounded_value
        if pd.notna(numeric_value) and numeric_value >= threshold:
            result.append(f"\\textbf{{{display_value}}}")
        else:
            result.append(f"{display_value}")
    return result


def bold_near_min(row, numeric_row):
    min_value = numeric_row.min()
    threshold = min_value * (1/FRAC)
    result = []
    for value, numeric_value in zip(row, numeric_row):
        rounded_value = round_sig(numeric_value)
        display_value = value if pd.isna(numeric_value) else rounded_value
        if pd.notna(numeric_value) and numeric_value <= threshold:
            result.append(f"\\textbf{{{display_value}}}")
        else:
            result.append(f"{display_value}")
    return result

if mode=="max":
    bold_function = bold_near_max
elif mode=="min":
    bold_function = bold_near_min

formatted_rows = [
    bold_function(dataframe.iloc[i], numeric_dataframe.iloc[i])
    for i in range(len(dataframe))
]

formatted_dataframe = pd.DataFrame(formatted_rows, columns=dataframe.columns)

latex_table = formatted_dataframe.to_latex(index=False, escape=False)

with open(output_file, "w", encoding="utf-8") as file:
    file.write(latex_table)
