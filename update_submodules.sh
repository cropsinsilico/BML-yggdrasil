#!/bin/bash
set -e
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
# 1. Stash the changes caused by patches
bash $SCRIPTPATH/models/patches/apply_patches.sh --stash
# 2. Update the submodules
git submodule update --remote $SCRIPTPATH/models/ePhotosynthesis_C
git submodule update --remote $SCRIPTPATH/models/BML-ePhotosynthesis
# 3. Apply the stashed changes
bash $SCRIPTPATH/models/patches/apply_patches.sh --pop-stash
# 4. Regenerate the patches
bash $SCRIPTPATH/models/patches/make_patches.sh
