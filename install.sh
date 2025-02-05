#!/bin/bash
set -e
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
# 1. Install older version of zeromq as version 4.3.5 is missing
#    _zmq_ctx_get_ext that is expected by the conda-forge distribution
#    of libczmq
mamba install -y zeromq==4.3.4
# 2. Configure the OpenSimRoot repository path (added for future use)
yggconfig --osr-repository-path $SCRIPTPATH/models/OpenSimRoot
# 3. Compile the yggdrasil C & C++ interfaces
yggcompile c cpp
# 4. Install BioCro (will be done at runtime if not here)
Rscript -e "install.packages('BioCro', repos='https://cran.rstudio.com')"
# R CMD BATCH install.R
# 5. Install yggdrasilBML wrapper modules
R CMD INSTALL $SCRIPTPATH/.
# yggcc .  # This does not work on ubuntu w/ yggdrasil from conda-forge
# For compat with yggdrasil on main which assumes no scope when
#   wrapping CMakeLists.txt and linking target to yggdrasil library
#   via target_include_directories. yggdrasil on param branch fixes this.
# To update the patch, make the desired changes in the ePhotosynthesis_C
#   submodule run the following:
#     ./models/patches/make_patches.sh
bash $SCRIPTPATH/models/patches/apply_patches.sh
