import os
import random
import math

random.seed(122345)

def good(x, y):
    for i in range(n):
        if x[i] < cutoff or x[i] > 1 - cutoff:
            return False
        if y[i] < cutoff or y[i] > 1 - cutoff:
            return False
        for j in range(i + 1, n):
            if math.hypot(x[i] - x[j], y[i] - y[j]) < cutoff:
                return False
    return True

cutoff = 0.1
n = 10
cnt = 0
while cnt < n:
    x = [random.uniform(0, 1) for _ in range(n)]
    y = [random.uniform(0, 1) for _ in range(n)]
    om = [random.choice([-1, 1]) * random.uniform(0.5, 1) for _ in range(n)]
    if good(x, y):
        d = f"{cnt:08d}"
        os.makedirs(d, exist_ok=True)
        with open(os.path.join(d, "args"), "w") as f:
            for xi, yi, omi in zip(x, y, om):
                f.write(f"{xi:.16e} {yi:.16e} {omi:.16e}\n")
        cnt += 1
        print(cnt)
