import math
import numpy as np
import glob
from pathlib import Path
import matplotlib.pyplot as plt

for attr_path in glob.glob("a.*.attr.raw"):
    attr_path = Path(attr_path)
    nfields = 5 # omega, psi, ux, uy, uz
    dtype = np.float32
    itemsize = np.dtype(dtype).itemsize
    size = attr_path.stat().st_size
    ncells = size // (itemsize * nfields)
    attr = np.fromfile(attr_path, dtype=dtype).reshape(ncells, nfields)
    n = math.isqrt(ncells)
    assert n * n == ncells, f"ncells={ncells} is not a perfect square"
    fields = attr.reshape(n, n, nfields)
    omega, psi, ux, uy, uz  = fields.T
    assert np.all(uz == 0), "uz contains non-zero bytes"
    out_path = attr_path.with_suffix("").with_suffix(".png")
    plt.figure()
    plt.imshow(omega, origin="lower", cmap="jet")
    plt.savefig(out_path)
    plt.close()
