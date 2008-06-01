#! /bin/sh
############################################################################
## First check if correct syntax was used
############################################################################

if [ "$#" = "0" ]; then
    echo "Usage: cobol <cobol file> [cobcy options]"
    exit
fi

############################################################################
## Set up all files and programs
############################################################################
# Need to know what this script's name is
THIS_SCRIPT=`basename $0`

# Cannot use basename because path is needed
EXECUTABLE=${1%.cob}

# cobcy is assumed to be in the same directory as this script
RUN_PATH=`dirname $0`
LIB_PATH=${RUN_PATH}/lib

# Check if debugging flag is present
if [ "$2" = "-g" ]; then
    DEBUG=1
else
    DEBUG=0
fi

CC=gcc

if [ "$DEBUG" = "1" ]; then
    CFLAGS="-Wall -g -pipe -I${LIB_PATH} -L${LIB_PATH}"
else
    CFLAGS="-w -Os -pipe -I${LIB_PATH} -L${LIB_PATH}"
fi
COBOL_LIB=-lcobol

COBOL_SOURCE=${EXECUTABLE}.cob
C_SOURCE=${EXECUTABLE}.c

rm -f $C_SOURCE $EXECUTABLE

############################################################################
## Ready to compile. Display cobol source on terminal to let the user
## 	know what is being compiled. This is mostly for development.
############################################################################

echo "---------------------------| COBOL code |------------------------------------"
cat $COBOL_SOURCE
echo "-----------------------------------------------------------------------------"

############################################################################
## The actual compilation goes on here. Generated C code is displayed
##	cobcy gets all the options given after the filename to cobol
############################################################################

shift
$RUN_PATH/cobcy $* -o $C_SOURCE $COBOL_SOURCE

if [ ! -f $C_SOURCE ]; then
    echo Compilation failed.
    exit
fi

echo "---------------------------| C code |----------------------------------------"
cat $C_SOURCE
echo "-----------------------------------------------------------------------------"

############################################################################
## If there is C code, compile it with gcc
############################################################################

if [ -f $C_SOURCE ]; then
    echo Building executable ...
    $CC $CFLAGS -o $EXECUTABLE $C_SOURCE $COBOL_LIB
fi

if [ -x $EXECUTABLE ]; then
    if [ "$DEBUG" = "1" ]; then
	echo Not stripping for debugging.
    else
	strip $EXECUTABLE
    fi
    echo "---------------------------| Results |---------------------------------------"
    $EXECUTABLE
    echo "-----------------------------------------------------------------------------"
else
    rm -f $EXECUTABLE
    echo Aborting due to errors.
fi

############################################################################
## All done!
############################################################################

