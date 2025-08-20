#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <math.h>
#include <netcdf.h>
#include <time.h>
// #include <netcdf_zstd.h> // zstd library
#include "compress.h"
#include "compress_params.c"

double clock_to_second(clock_t t) {
  // Return double precision time in seconds
  double time_taken;
  time_taken = ((double) t) / CLOCKS_PER_SEC;
  return time_taken;
}

// Replace with a function to load a dataset into memory
void populateData(float* data_ptr, int dim1_size, int dim2_size) {
  /* populate the data at address data_ptr with a 2d analytic function,
     a sine wave along the j dimension, that scales along the i direction */
  int ii, jj;
  float dbl_dim2_size;
  dbl_dim2_size = (float) dim2_size;
  for (ii = 0; ii < dim1_size; ii++) {
    for (jj = 0; jj < dim2_size; jj++) {
      *(data_ptr + (ii * dim1_size) + jj) = \
	sin(((2.0*PI)/dbl_dim2_size)*jj) *ii;
    }
  }
}
  

int main() {
  int i;
  
  // Define the number of dimensions and size of data
  int ndims = 2;
  int dim1_size = 10;
  int dim2_size = 10;

  float* data_ptr;
  struct PackingParams* packing_params;
  struct PackingParams my_param;

  // Struct for results
  struct Results {
    float write_time;
    float close_time;
    float ratio;
    char filename[200];
  };
  struct Results* results = (struct Results*) calloc(NUM_PACKING_PARAMS, sizeof(struct Results));

  /* NetCDF related variables */
  int ncid;
  int x_dimid, y_dimid, varid;
  int dimids[ndims];

  // Time variable
  clock_t t;
  double write_time, close_time;

  // Define our packing params from quantize_params.h
  packing_params = define_params();

  // Create data only once
  data_ptr = (float*) calloc(dim1_size*dim2_size, sizeof(float));
  populateData(data_ptr, dim1_size, dim2_size);
  

  /* Loop over the array of packing_params defined in quantize_params.h,
     setting up and writing the variable in data_ptr depending on the
     individual parameter setting */
  for (i = 0; i<NUM_PACKING_PARAMS; i++) {
    my_param = *(packing_params + i);

    // set up the netcdf file
    nc_create(my_param.filename, NC_NETCDF4, &ncid);
    
    nc_def_dim(ncid, "x", dim1_size, &x_dimid);
    nc_def_dim(ncid, "y", dim2_size, &y_dimid);
    dimids[0] = x_dimid;
    dimids[1] = y_dimid;

    // Define the variable
    nc_def_var(ncid, my_param.fieldname, NC_FLOAT, ndims, dimids, &varid);

    // Set up quantization if appropriate
    if (my_param.do_quantize > 0) {
      nc_def_var_quantize(ncid, varid, my_param.netcdf_quantize_mode, \
			  my_param.netcdf_nsd);
    }

    // Set up compression if appropriate
    // Uses zlib compression (int ncid, int varid, int shuffle[0-1], int deflate[0-1], int deflate_level[0-9])
    if (my_param.compress > 0) {
      if (my_param.compress_mode == 1) {
          // ZLIB compression
          nc_def_var_deflate(ncid, varid, 0, my_param.shuffle, my_param.deflate_level);
      } else if (my_param.compress_mode == 2) {
          // ZSTD compression
          // Uncomment the next line if using ZSTD compression
          // nc_def_var_zstd(ncid, varid, 0, 1, 1);
      }
    }
    
    // End definitition
    nc_enddef(ncid);
    
    // write the data into the file
    t = clock();
    nc_put_var_float(ncid, varid, data_ptr);
    write_time = clock_to_second(clock() - t);

    printf("File %s nc_put_var_float() takes %.4f s\n",
	   my_param.filename, write_time);

    //close the file
    t = clock();
    nc_close(ncid);
    close_time = clock_to_second(clock() - t);
    printf("File %s nc_close() takes %.4f s \n",
	   my_param.filename, close_time);

    printf("File %s total time: %.4f\n\n", my_param.filename,
	   write_time + close_time);

    // Measure file size
    long original_size = dim1_size * dim2_size * sizeof(float); // Replace with actual data once obtained
    long compressed_size = get_file_size(my_param.filename);
    double compression_ratio = (double)original_size / compressed_size;

    // Store the results
    results[i].write_time = write_time;
    results[i].close_time = close_time;
    results[i].ratio = compression_ratio;
    strcpy(results[i].filename, my_param.filename);
  }

  // Write results to a file
  FILE *result_file = fopen("compression_results.txt", "w");
  if (result_file == NULL) {
    fprintf(stderr, "Error opening results file for writing.\n");
    return 1;
  }
  fprintf(result_file, "Filename\tWrite Time (s)\tClose Time (s)\tCompression Ratio\n");
  for (i = 0; i < NUM_PACKING_PARAMS; i++) {
    fprintf(result_file, "%s\t%.4f\t%.4f\t%.4f\n",
            results[i].filename,
            results[i].write_time,
            results[i].close_time,
            results[i].ratio);
  }
  fclose(result_file);
  
  // Free allocated memory
  free(data_ptr);
  free(packing_params);
  free(results);
  return 0;
}
