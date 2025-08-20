//How many experiments are we running?
#define NUM_PACKING_PARAMS 9

/* Define our struct to hold the packing parameters. This are populated
   in the define_params function below, allowing a neat way of iterating
   over experiments */
struct PackingParams {
  int compress; // perform compression, 1 for compression, 0 for not
  int deflate_level; // deflate level, 0-9
  int shuffle; // use shuffle, 1 for yes, 0 for no
  int compress_mode;  
                /* Chose compression mode:
                    1) ZLIB
                    2) ZSTD - not yet implemented
                */
  int do_quantize; // perform quantization, 1 for quantization, 0 for not
  int netcdf_quantize_mode; /* Chose netcdf quantization mode, choices:
			       1) NC_QUANTIZE_BITGROOM
			       2) NC_QUANTIZE_GRANULARBR
			       3) NC_QUANTIZE_BITROUND 
                   
                   It seems wrong that this is an int, but the docs specify the quantization
                   function takes an int, but expects one of the above values.  netcdf.h defines:
                    #define NC_NOQUANTIZE 0     
                    #define NC_QUANTIZE_BITGROOM 1 
                    #define NC_QUANTIZE_GRANULARBR 2 
                    #define NC_QUANTIZE_BITROUND 3
                   */
  int netcdf_nsd; /* Number of significant digits to preserve. For
		     NC_QUANTIZE_BITGROOM and NC_QUANTIZE_GRANULARBR these are
		     decimal significant figures, for NC_QUANTIZE_BITROUND
		     these are binary signficant figures.
		     Note: 1 decimal sf requires ~3.32 bits. */
  char filename[200]; // Name of file to write to
  char fieldname[200]; // Name of field to write to file
};

/* Define parameter sets */
// Not sure if we want to continue manually writing tests or if we should programmatically generate them
// For example, we might run all tests with shuffle on and off, or sweep all deflate levels over all quantization types etc
// If I do want to test all config options possible, I will create a few lists of options and for-loops traversing each
struct PackingParams *define_params() {

  //Allocate our array of parameters
  struct PackingParams* param_array;
  param_array = (struct PackingParams*) \
    malloc(NUM_PACKING_PARAMS*sizeof(struct PackingParams));

  // define our parameters for this experiment
  // Reference
  param_array[0].compress = 0;
  param_array[0].do_quantize = 0;
  strcpy(param_array[0].filename, "reference.nc");
  strcpy(param_array[0].fieldname, "field");

//   Thinking about a param sweep loop:
//   Indexing the params needs some thought.
//   for (int i = 0; i < 2; i++) {  // Loop over compression modes
//      char type = (i == 0) ? "zlib" : "zstd";
//      for (int k = 0; k < 4; k++) { // Loop over quant modes
//          char quant = (k == 0) ? "no_quant" : "quant";
//          char quant_mode = (k == 1) ? "NC_QUANTIZE_BITGROOM" : (k == 2) ? "NC_QUANTIZE_GRANULARBR" : "NC_QUANTIZE_BITROUND";
//          param_array[1].compress = 1;
//          param_array[1].compress_mode = i;
//          param_array[1].do_quantize = (k > 0) ? 1 : 0;
//          param_array[1].netcdf_quantize_mode = k;
//          int nsd = (k == 1) ? 3 : (k == 2) ? 3 : 10; // Recreating the original nsd values, we might want to sweep over these too
//          param_array[1].netcdf_nsd = nsd
//          strcpy(param_array[1].filename, "comp_{type}_{quant}_{quant_mode}_{nsd}.nc");
//          strcpy(param_array[1].fieldname, "field");

  // Reference compress
  param_array[1].compress = 1;
  param_array[1].compress_mode = 1; // ZLIB
  param_array[1].do_quantize = 0;
  // strcpy(param_array[1].filename, "reference_comp_zlib.nc");
  strcpy(param_array[1].filename, "reference_comp.nc");
  strcpy(param_array[1].fieldname, "field");

  // Bitgroom pack and compress, nsd = 3
  param_array[2].compress = 1;
  param_array[2].compress_mode = 1; // ZLIB
  param_array[2].do_quantize = 1;
  param_array[2].netcdf_quantize_mode = NC_QUANTIZE_BITGROOM;
  param_array[2].netcdf_nsd = 3;
  // strcpy(param_array[2].filename, "quant_bg_3_comp_zlib.nc");
  strcpy(param_array[2].filename, "quant_bg_3_comp.nc");
  strcpy(param_array[2].fieldname, "field");

  // Granular pack and compress, nsd = 3
  param_array[3].compress = 1;
  param_array[3].compress_mode = 1; // ZLIB
  param_array[3].do_quantize = 1;
  param_array[3].netcdf_quantize_mode = NC_QUANTIZE_GRANULARBR;
  param_array[3].netcdf_nsd = 3;
  // strcpy(param_array[3].filename, "quant_gran_3_comp_zlib.nc");
  strcpy(param_array[3].filename, "quant_gran_3_comp.nc");
  strcpy(param_array[3].fieldname, "field");

  // Bitround pack and compress, nsd = 10bit (3dec)
  param_array[4].compress = 1;
  param_array[4].compress_mode = 1; // ZLIB
  param_array[4].do_quantize = 1;
  param_array[4].netcdf_quantize_mode = NC_QUANTIZE_BITROUND;
  param_array[4].netcdf_nsd = 10;
  // strcpy(param_array[4].filename, "quant_br_10b_comp_zlib.nc");
  strcpy(param_array[4].filename, "quant_br_10b_comp.nc");
  strcpy(param_array[4].fieldname, "field");

//   // ZSTD set:
//   // Reference compress
//   param_array[5].compress = 1;
//   param_array[5].compress_mode = 2; // ZSTD
//   param_array[5].do_quantize = 0;
//   strcpy(param_array[5].filename, "reference_comp_zstd.nc");
//   strcpy(param_array[5].fieldname, "field");

//   // Bitgroom pack and compress, nsd = 3
//   param_array[6].compress = 1;
//   param_array[6].compress_mode = 2; // ZSTD
//   param_array[6].do_quantize = 1;
//   param_array[6].netcdf_quantize_mode = NC_QUANTIZE_BITGROOM;
//   param_array[6].netcdf_nsd = 3;
//   strcpy(param_array[6].filename, "quant_bg_3_comp_zstd.nc");
//   strcpy(param_array[6].fieldname, "field");

//   // Granular pack and compress, nsd = 3
//   param_array[7].compress = 1;
//   param_array[7].compress_mode = 2; // ZSTD
//   param_array[7].do_quantize = 1;
//   param_array[7].netcdf_quantize_mode = NC_QUANTIZE_GRANULARBR;
//   param_array[7].netcdf_nsd = 3;
//   strcpy(param_array[7].filename, "quant_gran_3_comp_zstd.nc");
//   strcpy(param_array[7].fieldname, "field");

//   // Bitround pack and compress, nsd = 10bit (3dec)
//   param_array[8].compress = 1;
//   param_array[8].compress_mode = 2; // ZSTD
//   param_array[8].do_quantize = 1;
//   param_array[8].netcdf_quantize_mode = NC_QUANTIZE_BITROUND;
//   param_array[8].netcdf_nsd = 10;
//   strcpy(param_array[8].filename, "quant_br_10b_comp_zstd.nc");
//   strcpy(param_array[8].fieldname, "field");

  return param_array;
}
