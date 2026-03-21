import math
import numpy as np
import glob
from pathlib import Path
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap

cmap_data = [
    (0.00, (0.78, 0.76, 0.74)),  # warm gray (zero)
    (0.05, (0.45, 0.43, 0.41)),  # dark gray
    (0.12, (0.15, 0.15, 0.15)),  # near black
    (0.20, (0.0, 0.1, 0.3)),     # dark navy
    (0.30, (0.0, 0.5, 0.8)),     # cyan-blue
    (0.38, (0.7, 0.0, 0.0)),     # red
    (0.55, (0.9, 0.3, 0.0)),     # orange
    (0.75, (1.0, 0.7, 0.0)),     # yellow-orange
    (1.00, (1.0, 0.9, 0.3)),     # bright yellow
]
cmap_vortex = LinearSegmentedColormap.from_list(
    "vortex", [(p, c) for p, c in cmap_data]
)

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
    fig, ax = plt.subplots(1, 1, frameon=False)
    ax.set_axis_off()
    ax.imshow(omega, origin="lower", cmap=cmap_vortex)
    fig.savefig(out_path, bbox_inches="tight", pad_inches=0, dpi=150)
    plt.close(fig)
