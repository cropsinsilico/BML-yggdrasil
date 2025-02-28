#!/bin/bash
set -e
ACTION=""
while [[ $# -gt 0 ]]; do
    case $1 in
        -R )
            ACTION="REVERT"
            shift
            ;;
        --stash )
            ACTION="STASH"
            shift
            ;;
        --pop-stash )
            ACTION="POP"
            shift
            ;;
    esac
done
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
cd $SCRIPTPATH/../ePhotosynthesis_C
if [ "$ACTION" = "STASH" ]; then
    git stash
elif [ "$ACTION" = "POP" ]; then
    git stash pop
elif [ "$ACTION" = "REVERT" ]; then
    git apply -R $SCRIPTPATH/ePhotosynthesis_C.patch
else
    git apply $SCRIPTPATH/ePhotosynthesis_C.patch
fi
cd $SCRIPTPATH/../BML-ePhotosynthesis
if [ "$ACTION" = "STASH" ]; then
    git stash
elif [ "$ACTION" = "POP" ]; then
    git stash pop
elif [ "$ACTION" = "REVERT" ]; then
    git apply -R $SCRIPTPATH/BML-ePhotosynthesis.patch
else
    git apply $SCRIPTPATH/BML-ePhotosynthesis.patch
fi
