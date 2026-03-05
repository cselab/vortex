#include "grid/quadtree.h"
#include "navier-stokes/stream.h"
#include "diffusion.h"
#include "xdmf2d.h"

enum { level = 6 };
static double t_end = 30;


int main(void) {
  origin (-0.5, -0.5);
  init_grid(1 << level);
  run();
}

event init(t = 0) {
  double dd = 0.1, a = 1., b = 10.;
  foreach()
    omega[] = a*(exp(-(sq(x - dd) + sq(y))/(dd/b)) +
		 exp(-(sq(x + dd) + sq(y))/(dd/b)));  
}

event xdmf_output(t += 1.0) {
  static int tid = 0;
  char prefix[FILENAME_MAX];
  fields_stats();
  sprintf(prefix, "a.%06d", tid++);
  output_xdmf(t, {omega}, {u}, NULL, prefix);
}

event end(t = t_end) {}
