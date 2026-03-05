#include "grid/quadtree.h"
#include "navier-stokes/stream.h"
#include "diffusion.h"
#include "xdmf2d.h"

enum { level = 9 };
static const double t_end = 30;
static const double core = 0.01;
static double xc0, yc0, omg0, xc1, yc1, omg1;
int main(int argc, char **argv) {
  int i;
  char *end;
  double *vals[] = {&xc0, &yc0, &omg0, &xc1, &yc1, &omg1};
  argv++;
  for (i = 0; i < sizeof vals / sizeof *vals; i++) {
    if (*argv == NULL) {
      fprintf(stderr, "main: error: not enough arguments\n");
      exit(1);
    }
    *vals[i] = strtod(*argv, &end);
    if (*end != '\0') {
      fprintf(stderr, "main: error: '%s' is not a number\n", *argv);
      exit(1);
    }
    argv++;
  }
  if (*argv != NULL) {
    fprintf(stderr, "main: error: unused argument: %s\n", *argv);
    exit(1);
  }
  init_grid(1 << level);
  run();
}
event init(t = 0) {
  foreach()
    omega[] =
      omg0 * exp(-(sq(x - xc0) + sq(y - yc0))/core) +
      omg1 * exp(-(sq(x - xc1) + sq(y - yc1))/core);
}
event xdmf_output(t += 1.0) {
  static int tid = 0;
  char prefix[FILENAME_MAX];
  fields_stats();
  sprintf(prefix, "a.%06d", tid++);
  output_xdmf(t, {omega}, {u}, NULL, prefix);
}
event end(t = t_end) {}
