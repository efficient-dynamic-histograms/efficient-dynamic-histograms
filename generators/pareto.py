import numpy as np

def pareto_type1(alpha, x_m, size=1):
    # Generate Lomax samples and transform to Pareto Type I
    return x_m * (1 + np.random.pareto(alpha, size))

size=100000
alpha = 1.0
x_m = 1.0
stream = pareto_type1(alpha, x_m, size=size)

with open("pareto.csv", "w") as file:
    file.write(f"{size}\n")
    for x in stream:
        file.write(f"{x}\n")