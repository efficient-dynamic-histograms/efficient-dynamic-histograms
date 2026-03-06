import numpy as np

N_RANGE = 100000
# stream = np.random.normal(0,1,size=N_RANGE)
R = 30
streams = [np.random.uniform(i, 2*i, N_RANGE//R) for i in range(0,R)]
stream = np.concat(streams)
# stream = np.random.exponential(1.0/3, N_RANGE)

mixing_coef = 0.5
components = np.random.rand(N_RANGE) < mixing_coef
stream = np.empty(N_RANGE)
stream[components] = np.random.beta(1, 100, size=components.sum())
stream[~components] = np.random.beta(100, 1, size=(~components).sum())

print(min(stream), max(stream))
with open("beta.csv", "w") as file:
    file.write(f"{N_RANGE}\n")
    for x in stream:
        file.write(f"{x}\n")