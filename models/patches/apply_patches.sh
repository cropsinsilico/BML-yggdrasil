#!/bin/bash
set -e
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH/../ePhotosynthesis_C
git apply $SCRIPTPATH/ephoto_CMakeLists.patch
