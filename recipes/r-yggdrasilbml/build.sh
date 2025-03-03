#!/bin/bash
export DISABLE_AUTOBREW=1
yggconfig
yggcompile c cpp
${R} CMD INSTALL --build . ${R_ARGS}
