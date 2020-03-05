import matplotlib.pyplot as plt
import numpy as np

X = np.array([
    # [2, 37.329370],
    # [4, 536.377451],
    [8, 2539.492832],
    [16, 10688.631164],
    [32, 47541.105086],
    [64, 106669.710206],
    [128, 524492.935998],
])

plt.figure(figsize=(4,3))
plt.plot(X[:, 0], (X[:, 1] / 500), 'k*-')
plt.xlabel("Step Size")
plt.ylabel("AVG. MSQE")
plt.title("MSQE for Uniform Quantization")
plt.tight_layout()
plt.xticks(X[:, 0], [str(int(x)) for x in X[:, 0]])
plt.show()
