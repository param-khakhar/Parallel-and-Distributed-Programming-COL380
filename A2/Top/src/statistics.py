import numpy as np
import sys

if __name__ == "__main__":

    array = np.loadtxt(sys.argv[1], delimiter = "\n")
    print("Mean: ",np.mean(array))
    print("Std: ",np.std(array))
    print("Median: ",np.median(array))
    print("Quartiles: ",np.quantile(array, [0.25, 0.5, 0.75]))