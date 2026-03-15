Unpack [Basilisk](https://basilisk.fr) and enter the source directory:
```
wget -q http://basilisk.fr/basilisk/basilisk.tar.gz
tar zxf basilisk.tar.gz
cd basilisk/src
```

On Linux use:
```
cp config.gcc config
```

On macOS use:
```
cp config.osx config
```

Build and install `qcc`
```
make ast && make qcc
mkdir -p "$HOME/.local/bin"
cp qcc "$HOME/.local/bin/"
```

Build `a.aut`:
```
qcc -disable-dimensions main.c -O2 -lm
```

Run with one or more vortices; each vortex is three numbers: `x y omg`. Domain is `[0,1] x [0,1]`; strength `omg` can be negative.
```
./a.out 0.3 0.3 1.0  0.5 0.5 -0.5  0.7 0.4 1.0
```

`main.c` writes per-snapshot raw fields to `a.*.attr.raw`; `omega` is the first field in each record.
`post.py` reads these files, writes per-snapshot images `a.*.png`, and creates `omegas.npy` with shape `(nsnap, n, n)`.

Generate PNG frames:
```
python post.py
```

Create an animation with [ImageMagic](https://imagemagick.org), comand can be `convert` in older versions:
```
magick a.*.png img/a.gif
```

<p align="center">
  <img src="img/a.gif" alt="Vortex animation" width="480" />
</p>
