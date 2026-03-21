import math
import numpy as np
import glob
from pathlib import Path
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap, PowerNorm

cmap_data = [
    (0.00, (0.55, 0.54, 0.52)),   # slight shadow
    (0.08, (0.30, 0.30, 0.29)),   # darker shadow
    (0.14, (0.09, 0.09, 0.08)),   # dark
    (0.18, (0.21, 0.48, 0.65)),   # dark blue
    (0.21, (0.16, 0.54, 0.73)),   # medium blue
    (0.24, (0.47, 0.63, 0.74)),   # light cyan
    (0.27, (0.70, 0.67, 0.60)),   # near-white transition
    (0.30, (0.60, 0.12, 0.02)),   # dark red
    (0.34, (0.55, 0.05, 0.00)),   # deep red
    (0.38, (0.63, 0.10, 0.00)),   # red
    (0.42, (0.68, 0.16, 0.02)),   # bright red
    (0.48, (0.74, 0.24, 0.00)),   # red-orange
    (0.54, (0.77, 0.30, 0.00)),   # orange
    (0.60, (0.78, 0.38, 0.01)),   # dark amber
    (0.67, (0.78, 0.42, 0.01)),   # amber
    (0.74, (0.77, 0.53, 0.00)),   # light amber
    (0.82, (0.78, 0.58, 0.00)),   # dark yellow
    (0.90, (0.78, 0.61, 0.00)),   # medium yellow
    (0.96, (0.78, 0.63, 0.00)),   # yellow
    (1.00, (0.78, 0.64, 0.00)),   # bright yellow
]
cmap_vortex = LinearSegmentedColormap.from_list(
    "vortex", [(p, c) for p, c in cmap_data]
)
cmap_vortex.set_under((0.78, 0.78, 0.75))  # gray background

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
    ax.imshow(omega, origin="lower", cmap=cmap_vortex, vmin=0.01, vmax=1.0)
    fig.savefig(out_path, bbox_inches="tight", pad_inches=0, dpi=300)
    plt.close(fig)
