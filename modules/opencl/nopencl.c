/**
 * nopencl.c 
 * --------
 *
 * OpenCL wrapper for Nial.
 *
 */


#include "switches.h"

#define MATHLIB
#define STDLIB
#define IOLIB
#define LIMITLIB
#define SJLIB

#include "sysincl.h"

#include "qniallim.h"
#include "lib_main.h"
#include "absmach.h"
#include "ops.h"

#include <string.h>

#ifdef OSX
#include <OpenCL/opencl.h>
#endif
#ifdef LINUX
#include <CL/cl.h>
#endif


/**
 * OpenCL platform structure for Nial
 */
typedef struct NOCL_PLATFORM {
  cl_platform_id   p_id;
  cl_device_id     *devices;
  cl_uint          num_devices;
} NoclPlatform;
  


/**
 * This will hold a list of platform ids after initialisation
 */
#define MAX_OPENCL_PLATFORMS 1024
#define MAX_OPENCL_DEVICES   1024
static int nocl_num_platforms = -1;
static NoclPlatform *nocl_platforms;



/**
 * Initialise the system, store the details in the global structure and
 * return a count of the number of platforms
 */
void inocl_platforms(void) {
  cl_int clStatus;
  nialptr x = apop();
  cl_platform_id platforms[MAX_OPENCL_PLATFORMS];
  cl_uint num_platforms;
  int i;

  if (kind(x) != inttype) {
    apush(makefault("?args"));
  } else {
    /* Get the platform details */
    clStatus = clGetPlatformIDs(MAX_OPENCL_PLATFORMS, platforms, &num_platforms);

    if (clStatus == CL_SUCCESS) {
      /* Create space to hold the information */
      nocl_platforms = (NoclPlatform*)malloc(num_platforms*sizeof(NoclPlatform));
      if (nocl_platforms == NULL) {
	nocl_num_platforms = 0;
	apush(makefault("?opencl_no_mem"));
      } else {
	/* Copy across the platform details */
	for (i = 0; i < num_platforms; i++) {
	  nocl_platforms[i].p_id = platforms[i];
	  nocl_platforms[i].devices = NULL;
	  nocl_platforms[i].num_devices = 0;
	}

	/* Return the count */
	nocl_num_platforms = num_platforms;
	apush(createint((nialint)num_platforms));
      }
    } else {
      nocl_num_platforms = 0;
      apush(makefault("?opencl_initialisation_error"));
    }
  }
   
  freeup(x);
  return;
}


/** 
 * Currently recognized device types
 */
static int num_device_types = 5;
static cl_device_type device_types[] = { CL_DEVICE_TYPE_CPU,
                                         CL_DEVICE_TYPE_GPU,
                                         CL_DEVICE_TYPE_ACCELERATOR,
                                         CL_DEVICE_TYPE_DEFAULT,
                                         CL_DEVICE_TYPE_ALL
};



/**
 * Get the device information for a specific platform and nominated
 * device type and store with the platform table. Returns the number of
 * devices.
 */ 
void inocl_devices(void) {
  nialptr x = apop();
  cl_device_id devices[1024], *dvs;
  nialint *iptr, p_index, d_index;
  cl_int clStatus;
  cl_uint num_devices;
  char *err;
  int i;

  /* Check arguments */
  if (kind(x) != inttype || tally(x) != 2) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  iptr = pfirstint(x);
  p_index = *iptr++;
  d_index = *iptr++;

  if (p_index < 0 || p_index >= nocl_num_platforms) {
    apush(makefault("?invalid_platform_index"));
    freeup(x);
    return;
  }

  if (d_index < 0 || d_index >= num_device_types) {
    apush(makefault("?invalid_device_type"));
    freeup(x);
    return;
  }

  /* Free up any existing device table */
  if (nocl_platforms[p_index].devices != NULL) {
    free(nocl_platforms[p_index].devices);
    nocl_platforms[p_index].devices = NULL;
    nocl_platforms[p_index].num_devices = 0;
  }

  /* Collect the device information */
  clStatus = clGetDeviceIDs(nocl_platforms[p_index].p_id, device_types[d_index], 1024, devices, &num_devices);

  if (clStatus == CL_SUCCESS) {
    /* Sucessful , so copy across the device ids to the platform table */
    dvs = (cl_device_id*)malloc(num_devices*sizeof(cl_device_id));
    if (dvs == NULL) {
      apush(makefault("?no_mem"));
    } else {
      for (i = 0; i < num_devices; i++) {
	dvs[i] = devices[i];
      }

      /* Set up the globals */
      nocl_platforms[p_index].devices = dvs;
      nocl_platforms[p_index].num_devices = num_devices;

      /* Return the count */
      apush(createint(num_devices));
    }
  } else {
    switch(clStatus) {
    case CL_INVALID_PLATFORM: err = "?invalid_platform"; break;
    case CL_INVALID_DEVICE_TYPE: err = "?invalid_device_type"; break;
    case CL_INVALID_VALUE: err = "?invalid_value"; break;
    case CL_DEVICE_NOT_FOUND: err = "?device_not_found"; break;
    default: err = "?unknown_error"; break;
    }
    apush(makefault(err));
  }

  freeup(x);
  return;
}


static int cl_platform_info_count = 5;
static cl_platform_info cl_platform_info_codes[] = { CL_PLATFORM_PROFILE,
                                                     CL_PLATFORM_VERSION,
                                                     CL_PLATFORM_NAME,
                                                     CL_PLATFORM_VENDOR,
                                                     CL_PLATFORM_EXTENSIONS
};


/**
 * Return the info for a given platform index
 */ 
void inocl_platform_info(void) {
  nialptr x = apop();
  char info_text[1024];
  nialint *iptr, p_index, d_index;
  cl_int clStatus;
  size_t num_chars;
  char *err;
  nialptr res;
  nialint reslen;


  /* Check arguments */
  if (kind(x) != inttype || tally(x) != 2) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  iptr = pfirstint(x);
  p_index = *iptr++;
  d_index = *iptr++;

  if (p_index < 0 || p_index >= nocl_num_platforms) {
    apush(makefault("?invalid_platform_index"));
    freeup(x);
    return;
  }

  if (d_index < 0 || d_index >= cl_platform_info_count) {
    apush(makefault("?invalid_info_type"));
    freeup(x);
    return;
  }

  clStatus = clGetPlatformInfo(nocl_platforms[p_index].p_id, cl_platform_info_codes[d_index], 1024, (void*)info_text, &num_chars);

  if (clStatus == CL_SUCCESS) {
    reslen = num_chars;
    res = new_create_array(chartype, 1, 0, &reslen);
    memcpy(pfirstchar(res), info_text, num_chars);
    apush(res);
  } else {
    switch(clStatus) {
    case CL_INVALID_PLATFORM: err = "?invalid_platform"; break;
    case CL_INVALID_VALUE: err = "?invalid_value"; break;
    default: err = "?unknown_error"; break;
    }
    apush(makefault(err));
  }

  freeup(x);
  return;
}




