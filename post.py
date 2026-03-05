import math
import numpy as np
import glob
from pathlib import Path
import matplotlib.pyplot as plt

paths = [Path(p) for p in sorted(glob.glob("a.*.attr.raw"))]
assert paths, "no input files matching a.*.attr.raw"

nfields = 5
dtype = np.float32
itemsize = np.dtype(dtype).itemsize

first_size = paths[0].stat().st_size
first_ncells = first_size // (itemsize * nfields)
n = math.isqrt(first_ncells)
assert n * n == first_ncells, f"ncells={first_ncells} is not a perfect square"

omegas = np.lib.format.open_memmap(
    "omegas.npy",
    mode="w+",
    dtype=dtype,
    shape=(len(paths), n, n),
)

for isnap, attr_path in enumerate(paths):
    size = attr_path.stat().st_size
    ncells = size // (itemsize * nfields)
    assert ncells == first_ncells, f"inconsistent ncells in {attr_path}"
    attr = np.fromfile(attr_path, dtype=dtype).reshape(ncells, nfields)
    fields = attr.reshape(n, n, nfields)
    omega = fields[:, :, 0]
    uz = fields[:, :, 4]
    assert np.all(uz == 0), "uz contains non-zero"
    omegas[isnap, :, :] = omega
    out_path = attr_path.with_suffix("").with_suffix(".png")
    plt.figure()
    plt.imshow(omega, origin="lower", cmap="jet")
    plt.savefig(out_path)
    plt.close()
