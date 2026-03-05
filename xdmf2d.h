#include <stdlib.h>
#include <string.h>

typedef struct {
  float cx, cy;
  float xyz[12];
  float *attr;
} xdmf_cell_record;

static int xdmf_cmp_cell_record(const void *pa, const void *pb) {
  const xdmf_cell_record *a = pa;
  const xdmf_cell_record *b = pb;
  if (a->cy < b->cy) return -1;
  if (a->cy > b->cy) return 1;
  if (a->cx < b->cx) return -1;
  if (a->cx > b->cx) return 1;
  return 0;
}

static int output_xdmf(double t, scalar *list, vector *vlist,
                       const char *path) {
  float *xyz, *attr;
  long i, j, nattr, nvect, ncomp, ncell, ncell_total, nsize;
  char xyz_path[FILENAME_MAX], attr_path[FILENAME_MAX], xdmf_path[FILENAME_MAX],
      *vname, *xyz_base, *attr_base;
  FILE *file;
  xdmf_cell_record *records;
  const int shift[4][2] = {
      {0, 0},
      {0, 1},
      {1, 1},
      {1, 0},
  };

  snprintf(xyz_path, sizeof xyz_path, "%s.xyz.raw", path);
  snprintf(attr_path, sizeof attr_path, "%s.attr.raw", path);
  snprintf(xdmf_path, sizeof xdmf_path, "%s.xdmf2", path);

  xyz_base = xyz_path;
  attr_base = attr_path;
  for (j = 0; xyz_path[j] != '\0'; j++) {
    if (xyz_path[j] == '/' && xyz_path[j + 1] != '\0') {
      xyz_base = &xyz_path[j + 1];
      attr_base = &attr_path[j + 1];
    }
  }

  nattr = list_len(list);
  nvect = vectors_len(vlist);
  ncomp = nattr + 3 * nvect;

  nsize = 0;
  ncell = 0;
  records = NULL;
  foreach_cell() if (is_leaf(cell)) {
    long p;
    xdmf_cell_record *r;

    if (ncell >= nsize) {
      xdmf_cell_record *tmp;
      nsize = 2 * nsize + 1;
      tmp = realloc(records, nsize * sizeof *records);
      if (tmp == NULL) {
        fprintf(stderr, "%s:%d: realloc failed\n", __FILE__, __LINE__);
        for (i = 0; i < ncell; i++)
          free(records[i].attr);
        free(records);
        return 1;
      }
      records = tmp;
    }

    r = &records[ncell];
    r->cx = x;
    r->cy = y;
    if ((r->attr = malloc(ncomp * sizeof *r->attr)) == NULL) {
      fprintf(stderr, "%s:%d: malloc failed\n", __FILE__, __LINE__);
      for (i = 0; i < ncell; i++)
        free(records[i].attr);
      free(records);
      return 1;
    }

    p = 0;
    for (i = 0; i < 4; i++) {
      r->xyz[p++] = x + Delta * (shift[i][0] - 0.5);
      r->xyz[p++] = y + Delta * (shift[i][1] - 0.5);
      r->xyz[p++] = 0;
    }
    assert(p == 12);

    p = 0;
    for (scalar s in list)
      r->attr[p++] = val(s);
    for (vector v in vlist) {
      r->attr[p++] = val(v.x);
      r->attr[p++] = val(v.y);
      r->attr[p++] = 0;
    }
    assert(p == ncomp);
    ncell++;
  }

  qsort(records, ncell, sizeof *records, xdmf_cmp_cell_record);

  if ((xyz = malloc(3 * 4 * ncell * sizeof *xyz)) == NULL) {
    fprintf(stderr, "%s:%d: malloc failed\n", __FILE__, __LINE__);
    for (i = 0; i < ncell; i++)
      free(records[i].attr);
    free(records);
    return 1;
  }
  if ((attr = malloc(ncomp * ncell * sizeof *attr)) == NULL) {
    fprintf(stderr, "%s:%d: malloc failed\n", __FILE__, __LINE__);
    free(xyz);
    for (i = 0; i < ncell; i++)
      free(records[i].attr);
    free(records);
    return 1;
  }

  for (i = 0; i < ncell; i++) {
    memcpy(&xyz[12 * i], records[i].xyz, 12 * sizeof *xyz);
    memcpy(&attr[ncomp * i], records[i].attr, ncomp * sizeof *attr);
    free(records[i].attr);
  }
  free(records);

  if ((file = fopen(xyz_path, "wb")) == NULL) {
    fprintf(stderr, "%s:%d: fail to open '%s'\n", __FILE__, __LINE__, xyz_path);
    free(xyz);
    free(attr);
    return 1;
  }
  if (fwrite(xyz, sizeof *xyz, 3 * 4 * ncell, file) !=
      (size_t)(3 * 4 * ncell)) {
    fprintf(stderr, "%s:%d: fail to write '%s'\n", __FILE__, __LINE__, xyz_path);
    fclose(file);
    free(xyz);
    free(attr);
    return 1;
  }
  if (fclose(file) != 0) {
    fprintf(stderr, "%s:%d: error: fail to close '%s'\n", __FILE__, __LINE__,
            xyz_path);
    free(xyz);
    free(attr);
    return 1;
  }

  if ((file = fopen(attr_path, "wb")) == NULL) {
    fprintf(stderr, "%s:%d: fail to open '%s'\n", __FILE__, __LINE__, attr_path);
    free(xyz);
    free(attr);
    return 1;
  }
  if (fwrite(attr, sizeof *attr, ncomp * ncell, file) !=
      (size_t)(ncomp * ncell)) {
    fprintf(stderr, "%s:%d: fail to write '%s'\n", __FILE__, __LINE__, attr_path);
    fclose(file);
    free(xyz);
    free(attr);
    return 1;
  }
  if (fclose(file) != 0) {
    fprintf(stderr, "%s:%d: error: fail to close '%s'\n", __FILE__, __LINE__,
            attr_path);
    free(xyz);
    free(attr);
    return 1;
  }
  free(xyz);
  free(attr);

  ncell_total = ncell;
  if ((file = fopen(xdmf_path, "w")) == NULL) {
    fprintf(stderr, "%s:%d: fail to open '%s'\n", __FILE__, __LINE__, xdmf_path);
    return 1;
  }
  fprintf(file,
          "<Xdmf\n"
          "    Version=\"2\">\n"
          "  <Domain>\n"
          "    <Grid>\n"
          "      <Time\n"
          "          Value=\"%.16e\"/>\n"
          "      <Topology\n"
          "          TopologyType=\"Quadrilateral\"\n"
          "          Dimensions=\"%ld\"/>\n"
          "      <Geometry>\n"
          "        <DataItem\n"
          "            Dimensions=\"%ld 3\"\n"
          "            Format=\"Binary\">\n"
          "          %s\n"
          "        </DataItem>\n"
          "      </Geometry>\n",
          t, ncell_total, 4 * ncell_total, xyz_base);
  j = 0;
  for (scalar s in list)
    fprintf(file,
            "      <Attribute\n"
            "          Name=\"%s\"\n"
            "          Center=\"Cell\">\n"
            "        <DataItem\n"
            "            ItemType=\"HyperSlab\"\n"
            "            Dimensions=\"%ld\"\n"
            "            Type=\"HyperSlab\">\n"
            "          <DataItem Dimensions=\"3 1\">\n"
            "            %ld %ld %ld\n"
            "          </DataItem>\n"
            "          <DataItem\n"
            "              Dimensions=\"%ld\"\n"
            "              Format=\"Binary\">\n"
            "            %s\n"
            "          </DataItem>\n"
            "         </DataItem>\n"
            "      </Attribute>\n",
            s.name, ncell_total, j++, ncomp, ncell_total,
            ncomp * ncell_total, attr_base);
  for (vector v in vlist) {
    vname = strdup(v.x.name);
    *strrchr(vname, '.') = '\0';
    fprintf(file,
            "      <Attribute\n"
            "          Name=\"%s\"\n"
            "          AttributeType=\"Vector\"\n"
            "          Center=\"Cell\">\n"
            "        <DataItem\n"
            "            ItemType=\"HyperSlab\"\n"
            "            Dimensions=\"%ld 3\"\n"
            "            Type=\"HyperSlab\">\n"
            "          <DataItem Dimensions=\"3 2\">\n"
            "            0 %ld\n"
            "            1 1\n"
            "            %ld 3\n"
            "          </DataItem>\n"
            "          <DataItem\n"
            "              Dimensions=\"%ld %ld\"\n"
            "              Format=\"Binary\">\n"
            "            %s\n"
            "          </DataItem>\n"
            "         </DataItem>\n"
            "      </Attribute>\n",
            vname, ncell_total, j, ncell_total, ncell_total, ncomp, attr_base);
    free(vname);
    j += 3;
  }
  fprintf(file, "    </Grid>\n"
                "  </Domain>\n"
                "</Xdmf>\n");
  if (fclose(file) != 0) {
    fprintf(stderr, "%s:%d: error: fail to close '%s'\n", __FILE__, __LINE__,
            xdmf_path);
    return 1;
  }
  return 0;
}
