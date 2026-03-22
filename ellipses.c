#include "navier-stokes/stream.h"
#include "xdmf2d.h"

enum { level = 9 };
static const double t_end = 400;
static double *vals;
static int nv;
enum { NPAR = 6 };
int main(int argc, char **argv) {
  int i;
  char *end;
  vals = NULL;
  argv++;
  for (nv = 0; *argv != NULL; nv++) {
    vals = realloc(vals, NPAR * (nv + 1) * sizeof *vals);
    if (vals == NULL) {
      fprintf(stderr, "ellipses.c: error: realloc failed\n");
      exit(1);
    }
    for (i = 0; i < NPAR; i++) {
      if (*argv == NULL) {
        fprintf(stderr,
                "ellipses.c: error: wrong number of arguments\n"
                "usage: x y om ax ay ang [x y om ax ay ang ...]\n");
        exit(1);
      }
      vals[NPAR * nv + i] = strtod(*argv, &end);
      if (*end != '\0') {
        fprintf(stderr, "ellipses.c: error: '%s' is not a number\n", *argv);
        exit(1);
      }
      argv++;
    }
  }
  if (nv == 0) {
    fprintf(stderr,
            "ellipses.c: error: no ellipses given\n"
            "usage: x y om ax ay ang [x y om ax ay ang ...]\n");
    exit(1);
  }
  init_grid(1 << level);
  run();
  free(vals);
}
event init(t = 0) {
  foreach () {
    double ans = 0;
    for (int i = 0; i < nv; i++) {
      double xc = vals[NPAR * i];
      double yc = vals[NPAR * i + 1];
      double om = vals[NPAR * i + 2];
      double ax = vals[NPAR * i + 3];
      double ay = vals[NPAR * i + 4];
      double ang = vals[NPAR * i + 5];
      double cs = cos(ang), sn = sin(ang);
      double dx = x - xc, dy = y - yc;
      double u = cs * dx + sn * dy;
      double v = -sn * dx + cs * dy;
      double r2 = sq(u / ax) + sq(v / ay);
      ans += om * exp(-r2);
    }
    omega[] = ans;
  }
}
event xdmf_output(t += 1.0) {
  static int tid = 0;
  char prefix[FILENAME_MAX];
  sprintf(prefix, "a.%06d", tid++);
  if (output_xdmf(t, {omega, psi}, {u}, prefix) != 0) {
    fprintf(stderr, "ellipses.c: error: fail to dump: %s\n", prefix);
    exit(1);
  }
}
event end(t = t_end) {}
