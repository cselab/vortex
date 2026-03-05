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

Output is written to `a.*.attr.raw`; to read it in Python, see `post.py`.

Generate PNG frames:
```bash
python post.py
```

Create a video:
```bash
magick a.*.png a.mp4
```

Video: [img/a.mp4](img/a.mp4)
