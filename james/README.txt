This is James Levitt's section of the repo. Do not make changes here, please
contact james.levitt@ni.com if you have questions.

The files EV3_Output, EV3_Sound, and EV3_Ui make functionality from lms2012
available to Vireo, the LabVIEW VM.  They are similar to some files found in xander's subdirectory.  The main difference is these functions make use of a
parameter-passing function similar to PrimParPointer instead of passing
parameters in the function call.  If we want to share this code, it makes
sense to use something in the style of xander's code.

The file EV3Entry contains functions VireoInit and VireoStep, which are
associated with new opcodes opVIREO_INIT and opVIREO_STEP.

The file speed1.lms is a simple program written in lms2012 bytecode.  The
program sets up the LabVIEW VM with the VIREO_INIT instruction and enters a
loop which services the VM with the VIREO_STEP instruction.

