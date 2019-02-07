#ifndef TGA_H
#define TGA_H

int loadtga(const char *fname, void **buf, int *sizex, int *sizey);
int savetga(const char *fname, unsigned int *data, int width, int height);

#endif
