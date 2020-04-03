#!/bin/bash

#
# Builds all the FunnyOS binaries into the current directory.
# Usage ./build.sh [SOURCES_DIRECTORY]
#
# Environmental variables:
# - FOS_RELEASE:  Either Debug or Release [Default: Debug]
#

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Parameters
SOURCES=${1:-$DIR/../}
RELEASE=${FOS_RELEASE:-Debug}

SOURCES=$(realpath $SOURCES)

# Do build
echo "Sources directory: " $SOURCES
cmake -DCMAKE_BUILD_TYPE=$RELEASE -G "CodeBlocks - Unix Makefiles"  $SOURCES || exit 1
make FunnyOS_BuildImage || exit 1