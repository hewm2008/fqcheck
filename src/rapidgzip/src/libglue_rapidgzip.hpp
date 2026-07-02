#ifndef LIBGLUE_RAPIDGZIP_H
#define LIBGLUE_RAPIDGZIP_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Decompress gzip file to output fd. Returns bytes written, or < 0 on error. */
long long glue_rapidgzip_decompress_fd(const char *input_path,
                                       int output_fd,
                                       int num_threads);

/* Open gzip file as a readable FILE* via fopencookie (streaming decompress).
 * The returned FILE* must be closed with fclose(). Returns NULL on error. */
FILE *glue_rapidgzip_fopen(const char *input_path, int num_threads);

/* Get version string */
const char *glue_rapidgzip_version(void);

#ifdef __cplusplus
}
#endif

#endif /* LIBGLUE_RAPIDGZIP_H */
