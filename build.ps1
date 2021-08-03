
# -----------------------------------------------------------------------------
# main program
#
# DISCLAIMER:
# Feabhas is furnishing this item "as is". Feabhas does not provide any
# warranty of the item whatsoever, whether express, implied, or statutory,
# including, but not limited to, any warranty of merchantability or fitness
# for a particular purpose or any warranty that the contents of the item will
# be error-free.
# In no respect shall Feabhas incur any liability for any damages, including,
# but limited to, direct, indirect, special, or consequential damages arising
# out of, resulting from, or any way connected to the use of the item, whether
# or not based upon warranty, contract, tort, or otherwise; whether or not
# injury was sustained by persons or property or otherwise; and whether or not
# loss was sustained from, or arose out of, the results of, the item, or any
# services that may be provided by Feabhas.
# -----------------------------------------------------------------------------

# Set-PSDebug -trace 1
Set-StrictMode -version latest

$SCRIPT = Split-Path $PSCommandPath -Leaf;
$USAGE = "Usage: $SCRIPT [-v | --verbose | --rtos] [ reset | clean | debug | release ]"

$CMAKE = 'C:\Program Files\CMake\bin\cmake.exe'
$MAKE = 'C:\msys64\usr\bin\make.exe'
$ARM_TOOLCHAIN = 'C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2020-q4-major\bin'
$env:PATH += ";$ARM_TOOLCHAIN"

$BUILD= 'build'
$BTYPE = 'DEBUG'
$BUILD_DIR = "$BUILD\debug"
$CLEAN = ''
$RESET = ''
$VERBOSE = ''
$RTOS = ''

switch -regex ($args)
{
    '^(--help|-h|)$'    { Write-Output "$USAGE"; exit 0 }
    '^(--verbose|-v)$'  { $VERBOSE = 'SHELL="/bin/sh -x"'  }
    '^--rtos$'          { $RTOS = '-DUSE_RTOS=ON'  }
    '^debug$'           { $BTYPE = 'DEBUG';   $BUILD_DIR = "$BUILD\debug" }
    '^release$'         { $BTYPE = 'RELEASE'; $BUILD_DIR = "$BUILD\release"  }
    '^clean$'           { $CLEAN = '1'  }
    '^reset$'           { $RESET = '1'  }
    default             { Write-Error "Unknown option $arg"; Show-Usage }
}

if ( $RESET -and (Test-Path $BUILD_DIR -PathType Container) ) {
    Remove-Item $BUILD_DIR -Recurse
}

$TOOLCHAIN = '-DCMAKE_TOOLCHAIN_FILE=toolchain-STM32F407.cmake'
$CMAKE_ARGS = '-G', 'Unix Makefiles', "-DCMAKE_MAKE_PROGRAM=$MAKE"
$BUILD_TYPE = "-DCMAKE_BUILD_TYPE=$BTYPE"

&$CMAKE -S . -B $BUILD_DIR $CMAKE_ARGS `
        --warn-uninitialized $BUILD_TYPE $TOOLCHAIN $RTOS

&$CMAKE $CMAKE_ARGS -S . -B $BUILD_DIR  

if ( $CLEAN  -ne '' ) {
  &$MAKE -C $BUILD_DIR clean
}

&$MAKE -C $BUILD_DIR $VERBOSE
