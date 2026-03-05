import argparse
import numpy as np
import matplotlib.pyplot as plt
parser = argparse.ArgumentParser()
parser.add_argument("--path", default="sample/omegas.npy")
parser.add_argument("--i", type=int, default=0, help="snapshot index")
parser.add_argument("--cmap", default="jet")
args = parser.parse_args()
omega = np.load(args.path, mmap_mode="r")
assert omega.ndim == 3, f"expected (nsnap, n, n), got {omega.shape}"
nsnap = omega.shape[0]
i = max(0, min(args.i, nsnap - 1))
plt.figure()
plt.imshow(omega[args.i], origin="lower", cmap=args.cmap)
plt.colorbar(label="omega")
plt.title(f"{args.path}  snapshot {i}/{nsnap-1}")
plt.tight_layout()
plt.show()
