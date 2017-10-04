import numpy as np
from matplotlib import pyplot as plt
import argparse

## Parse Command Line Args
ap = argparse.ArgumentParser()
ap.add_argument('-f', '--file', help='csv file', type=str, required=True)
args = ap.parse_args()

data = np.loadtxt(args.file, delimiter=",")
plt.imshow(data, aspect="auto", interpolation = "none", cmap="coolwarm", vmin=0.0, vmax=15.0)
plt.colorbar()
plt.show()
