# 1. Install older version of zeromq as version 4.3.5 is missing
#    _zmq_ctx_get_ext that is expected by the conda-forge distribution
#    of libczmq
mamba install -y zeromq==4.3.4
# 2. Configure the OpenSimRoot repository path (added for future use)
yggconfig --osr-repository-path $(pwd)/models/OpenSimRoot
# 3. Compile the yggdrasil C & C++ interfaces
yggcompile c cpp
# 4. Install BioCro
R CMD BATCH install.R
# 5. Install yggdrasilBML wrapper modules
R CMD INSTALL .
# For compat with yggdrasil on main which assumes no scope when
#   wrapping CMakeLists.txt and linking target to yggdrasil library
#   via target_include_directories. yggdrasil on param branch fixes this.
# To update the patch, make the desired changes in the ePhotosynthesis_C
#   submodule and then run the following from that directory:
#     git diff &> ../../ephoto_CMakeLists.patch
cd $(pwd)/models/ePhotosynthesis_C
git apply ../../ephoto_CMakeLists.patch
