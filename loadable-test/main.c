/* vim: set tabstop=2 shiftwidth=2 expandtab autoindent: */
 
/**
 * Configurable module loading test program
 *
 * Changelog:
 * - 0.1: Initial release
 *
 * License: restricted for now
 *
 * Xander Soldaat <xander@robotc.net>
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>

#define CONFIG_FILE         "firmwares.conf"
#define MAX_VM_FILENAME     64
#define MAX_ENTRYPOINT_NAME 64
#define MAX_VMS              2
#define MAX_BUFSIZE         256

typedef void (*tEntryPointFunc)(void);

struct tVirtualMachineInfo
{
  char fileName[MAX_VM_FILENAME];
  char entryPointName[MAX_ENTRYPOINT_NAME];
  tEntryPointFunc entryPointFunc;
};

struct tVirtualMachineInfo virtualMachineInfo[MAX_VMS];

// function prototypes
int loadVirtualMachine(struct tVirtualMachineInfo *);
int readConfigFile(char *);


// let the code begin


// Taken from dlopen man page and modified
int loadVirtualMachine(struct tVirtualMachineInfo *vm)
{
  void *handle;
  char *error;

  fprintf(stderr, "Loading %s\n", vm->fileName);

  // You can change this to another type of binding.
  // This one only resolves the required symbols.
  handle = dlopen(vm->fileName, RTLD_LAZY);
  if (!handle) {
    fprintf(stderr, "%s\n", dlerror());
    return 1;
  }

  dlerror();    /* Clear any existing error */

  // Setup our pointer to function.  The *(void **) construction
  // is taken from the dlopen man page, apparently it's required.
  *(void **) (&vm->entryPointFunc) = dlsym(handle, vm->entryPointName);

  // If an error occured revolving our entry point, shout about it.
  if ((error = dlerror()) != NULL)  
  {
    fprintf(stderr, "%s\n", error);
    dlclose(handle);
    return 1;
  }

  // Execute our entry point function
  (vm->entryPointFunc)();

  // Close the shared object.  Normally you wouldn't do 
  // that until the whole VM exits
  dlclose(handle);
  return 0;
}


// Read the config file and populate our info structures
int readConfigFile(char *filename)
{
  FILE *vmConfigFile;
  char buff[MAX_BUFSIZE];
  int index = 0;
  int retval = 0;

  vmConfigFile = fopen(filename, "r" );

  if ( vmConfigFile == NULL )
  {
    printf("Could not open configuration file: %s\n", filename);
    retval = 1;
  }
  else 
  {
    while (fgets( buff, sizeof buff, vmConfigFile) != NULL ) 
    {
      if (sscanf( buff, "%s %s", virtualMachineInfo[index].fileName, virtualMachineInfo[index].entryPointName) == 2) 
      {
        printf("Virtual machine: %s, entryPointName: %s\n", virtualMachineInfo[index].fileName, virtualMachineInfo[index].entryPointName); 
      }
      else
      {
        printf ("Malformed config file\n");
        retval = 1;
        break;
      }
      index++;
    }
    fclose( vmConfigFile );
  }
  return retval;
}

int main (int argc, char *argv[])
{
  int index = 0;

  if ( argc != 2 ) 
  {
    printf("Usage: %s <config file>\n", argv[0]);
    return 1;
  }

  // Read the config file
  else 
  {
    if (readConfigFile(argv[1]) != 0)
      return 1;
  }

  // Load our VMs and execute the enry point functions
  for (index = 0; index < MAX_VMS; index++)
  {
    if (loadVirtualMachine(&virtualMachineInfo[index]) != 0)
    {
      printf ("Could not load virtual machine %s\n", virtualMachineInfo[index].fileName);
    }
  }
  return 0;
}
