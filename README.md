Vortex example using Basilisk.

Install Basilisk and enter the source directory:
```bash
wget -q http://basilisk.fr/basilisk/basilisk.tar.gz
tar zxf basilisk.tar.gz
cd basilisk/src
```

On Linux use:
```bash
cp config.gcc config
```

On macOS use:
```bash
cp config.osx config
```

Build and install `qcc`
```bash
make ast && make qcc
cp qcc "$HOME/.local/bin/"
```

Build:
```bash
qcc -disable-dimensions main.c -O2 -lm
```

Run with parameters `xc0 yc0 omg0 xc1 yc1 omg1`:
```bash
./a.out 0.5 0.5 1.0 0.6 0.8 -1.0
```

`main.c` writes per-snapshot raw fields to `a.*.attr.raw`; `omega` is the first field in each record.
`post.py` reads these files, writes per-snapshot images `a.*.png`, and creates `omegas.npy` with shape `(nsnap, n, n)`.

Generate PNG frames:
```bash
python post.py
```

Create an animation:
```bash
magick a.*.png img/a.gif
```

![Vortex animation](img/a.gif)
