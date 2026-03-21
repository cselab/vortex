#include "navier-stokes/stream.h"
#include "xdmf2d.h"

enum { level = 8 };
static const double t_end = 50;
static const double core = 0.01;
static double *vals;
static int nv;
int main(int argc, char **argv) {
  int i;
  char *end;
  vals = NULL;
  argv++;
  for (nv = 0; *argv != NULL; nv++) {
    vals = realloc(vals, 3 * (nv + 1) * sizeof *vals);
    if (vals == NULL) {
      fprintf(stderr, "main.c: error: realloc failed\n");
      exit(1);
    }
    for (i = 0; i < 3; i++) {
      if (*argv == NULL) {
        fprintf(stderr, "main.c: error: wrong number of arguments\n");
        exit(1);
      }
      vals[3 * nv + i] = strtod(*argv, &end);
      if (*end != '\0') {
        fprintf(stderr, "main.c: error: '%s' is not a number\n", *argv);
        exit(1);
      }
      argv++;
    }
  }
  init_grid(1 << level);
  run();
}
event init(t = 0) {
  foreach () {
    double ans = 0;
    for (int i = 0; i < nv; i++) {
      double xc, yc, om;
      xc = vals[3 * i];
      yc = vals[3 * i + 1];
      om = vals[3 * i + 2];
      ans += om * exp(-(sq(x - xc) + sq(y - yc)) / core);
    }
    omega[] = ans;
  }
}
event xdmf_output(t += 1.0) {
  static int tid = 0;
  char prefix[FILENAME_MAX];
  sprintf(prefix, "a.%06d", tid++);
  if (output_xdmf(t, {omega, psi}, {u}, prefix) != 0) {
    fprintf(stderr, "main.c: error: fail to dump: %s\n", prefix);
    exit(1);
  }
}
event end(t = t_end) {}
