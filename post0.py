import math
import numpy as np
import glob
from pathlib import Path
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap

cmap_data = [
    (0.00, (0.78, 0.78, 0.75)),   # gray background (200,198,190)
    (0.04, (0.55, 0.54, 0.52)),   # mid gray
    (0.10, (0.09, 0.09, 0.08)),   # near black (23,22,21)
    (0.15, (0.21, 0.48, 0.65)),   # dark blue (53,122,167)
    (0.18, (0.49, 0.65, 0.74)),   # light cyan (124,165,189)
    (0.22, (0.60, 0.19, 0.07)),   # dark red (154,48,18)
    (0.30, (0.68, 0.16, 0.02)),   # red (173,42,4)
    (0.45, (0.79, 0.30, 0.02)),   # orange (202,76,4)
    (0.65, (0.78, 0.47, 0.00)),   # dark yellow (198,120,0)
    (1.00, (0.78, 0.64, 0.00)),   # yellow (198,164,0)
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
    if isnap == 0:
        vmax0 = 1.0

for isnap, attr_path in enumerate(paths):
    omega = omegas[isnap, :, :]
    out_path = attr_path.with_suffix("").with_suffix(".png")
    fig, ax = plt.subplots(1, 1, frameon=False)
    ax.set_axis_off()
    ax.imshow(omega, origin="lower", cmap=cmap_vortex, vmin=0, vmax=vmax0)
    fig.savefig(out_path, bbox_inches="tight", pad_inches=0, dpi=150)
    plt.close(fig)
