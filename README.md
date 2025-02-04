## yggdrasilBML

This is a collection of BioCro modules that call external models using yggdrasil. The models are included in this repository as git submodules under the 'models' directory, but they can be cloned independently if the appropriate paths are updated in the yaml files.

### Installation

#### Requirements

- The [R environment](https://cran.r-project.org/).
- On Windows, a version of [Rtools](https://cran.r-project.org/bin/windows/Rtools/)
  appropriate for your version of R.
- On Linux, gcc and g++ version 4.9.3 or greater (consult documentation for your
  distribution for installation instructions).
- On MacOS, Xcode.
- yggdrasil

#### Installation steps (yggdrasil integration)

0. Install mamba (micromamba works well)

1. Install yggdrasil & model dependencies

To create a conda environment named 'ygg' with yggdrasil & the model dependencies installed from conda-forge using mamba:

```
mamba env create -n ygg -f environment.yml
```

The yggdrasil repository contains utilities for setting up a yggdrasil conda environment with yggdrasil installed from source in development mode, but then the additional dependencies must also be installed:

```
python utils/setup_test_env.py devenv mamba 3.9 --env-name ygg -y
mamba install boost>=1.36.0 sundials>=5.7.0
```

2. Install the R module

For ease of use, I have provided an installation script `install.sh` that assumes yggdrasil is already installed within a mamba environment from conda-forge. Run it from within the top directory of this repository.

#### Yggdrasil integrations

The yaml files describing the example integrations can be found in the 'yamls' directory. They are:

- biocro.yml: For running the BioCro Soybean model in isolation with the built in photosynthesis model.
- ePhotosynthesis.yaml: For running the ePhotosynthesis model as part of integrations with BioCro.
- biocro_ephoto.yml: For running an integration of BioCro & the ePhotosynthesis model where ePhotosynthesis takes the place of the built-in BioCro photosynthesis model.

The above version all assumes that yggdrasil is installed from the most recent tagged release (either from source, conda-forge, or PyPI). I have also prepared versions that are compatible with my current development branch of yggdrasil ('topic/cache'). They begin with the 'dev_*' prefix.

#### Running a Yggdrasil integration

```
yggrun biocro_ephoto.yml
```

#### Installation steps (R module)

First, obtain a local copy of this repository, which can be accomplished using
either of two methods:
1. If you are new to Git, the easiest way to get a local copy is to install
   GitHub Desktop and use the "Open with GitHub Desktop option in the "Code"
   dropdown on the GitHub repository page.
2. Alternatively, clone this repository using Git on the command line in the
   usual fashion by running `git clone <repository URL>` where
   `<repository URL>` is the URL for this repository. This repository contains a
   Git submodule, so you will need to take the additional step of running
   `git submodule update --init` to obtain it.

After obtaining a local copy of the source code, install the package from the
command line or from within R using one of the following sets of commands. These
assume that the source files are in a directory named  'yggdrasilBML'.

- From the command line
```
R CMD INSTALL .
```

- Or from within R
```
setwd('path_to_unzipped_directory')
install.packages('yggdrasilBML', repos=NULL, type='SOURCE')
```

### An example (R module)

The following code will print a list of all modules available in this library,
return information about one of them (`yggdrasilBML:ephotosynthesis`), and then run that
module:
```
library(BioCro)
library(yggdrasilBML)
get_all_modules('yggdrasilBML')
module_info('yggdrasilBML:ephotosynthesis')
```
For more information about using BioCro modules in R, please see the
[BioCro framework R package](https://github.com/biocro/biocro).

### Source

This package was derived from the
[BioCro skeleton module library](https://github.com/biocro/skelBML). See
`skelBML_description` for more information about the version of the skeleton
library that was used.

### License

The `yggdrasilBML` R package is licensed under the MIT license, while the BioCro C++
framework is licensed under version 3 or greater of the GNU Lesser General
Public License (LGPL). This scheme allows people to freely develop models for
any use (public or private) under the MIT license, but any changes to the
framework that assembles and solves models must make source code changes
available to all users under the LGPL. See `LICENSE.note` for more details.
