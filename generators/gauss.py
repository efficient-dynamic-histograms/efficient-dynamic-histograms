import numpy as np
import pandas as pd

N_RANGE = 100000
D = 10
EPS = 0.90
p = EPS ** (1/D)


def generate_mixture_sample(N, N_RANGE, p):
    choices = np.random.rand(N_RANGE, N) > p
    choices = choices.astype(bool)
    label = np.any(choices, axis=1)
    label = np.reshape(label, (-1, 1)).astype(bool)
    normals_1 = np.random.normal(0, 1, (N_RANGE, N))
    normals_2 = np.random.normal(100000, 4, (N_RANGE, N))
    sample = np.where(choices, normals_2, normals_1)
    sample = np.concat([sample, label], axis=1)
    return sample

stream = generate_mixture_sample(D, N_RANGE, p)

df = pd.DataFrame(stream, columns=[f'dim_{i}' for i in range(D)]+ ['label'])
df['label'] = df['label'].astype("boolean")
df['label'] = df['label'].map({True: 'true', False: 'false'})
df.to_csv('mixture_sample.csv', index=False)