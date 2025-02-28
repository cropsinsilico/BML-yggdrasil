#!/bin/bash
set -e
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH/../ePhotosynthesis_C
git diff &> $SCRIPTPATH/ePhotosynthesis_C.patch
cd $SCRIPTPATH/../BML-ePhotosynthesis
git diff &> $SCRIPTPATH/BML-ePhotosynthesis.patch
