<!-- WARNING: This file was included in this package by the the skeleton BioCro
     module library and should not be manually edited -->

## skelBML

This repository contains the **skel**eton **B**ioCro **m**odule **l**ibrary
(`skelBML`), which is a framework for creating an R package. Running the setup
script on a copy of this repository will create a basic BioCro module library R
package with a single example module. Then, additional modules can be added. See
below for detailed instructions explaining how to do this.

### Using this repository to initialize a working module library

By default, the code here will not compile and does not define a functional R
package. Instead, the following steps should be taken to initialize a working R
package:
1. Duplicate this repository, making sure to choose a name for the new copy
   corresponding to the name of your new module library (see Step 3). The
   easiest way to duplicate a repository is to use the
   [GitHub importer tool](https://github.com/new/import). In this case, just
   type the URL for this respository (`https://github.com/biocro/skelBML`) as
   the "old repository clone URL" and then specify the new name and visibility
   settings. Repositories can also be duplicated using Git from the command
   line, as described in the official GitHub instructions for
   [duplicating a repository](https://docs.github.com/en/repositories/creating-and-managing-repositories/duplicating-a-repository).
2. Obtain a local copy of your new repository, making sure to include the Git
   submodule code. This can be accomplished using either of two methods:
   1. If you are new to Git, the easiest way to get a local copy is to install
      GitHub Desktop and use the "Open with GitHub Desktop" option in the "Code"
      dropdown on the GitHub page for the repository you created in step 1.
   2. Alternatively, clone your repository using Git on the command
      line in the usual fashion by running `git clone <repository URL>` where
      `<repository URL>` is the URL for your repository. The repository
      contains a Git submodule, so you will need to take the additional step of
      running `git submodule update --init` to obtain it.
3. Run the setup script, which can be accomplished using either of two
   methods:
   1. Open an R session, set the working directory to `script`, and type
      `source('module_library_setup.R')`.
   2. From a terminal running in the `script` directory, type
      `Rscript module_library_setup.R`.

   In either case, you will be prompted for a module library name, which should
   be 16 or fewer characters long and not contain any underscores; ideally this
   would match the name of the newly-created repository. (If you might ever want
   to submit your package to CRAN, make sure your name is unique on CRAN and
   BioConductor; one way to help ensure this is to include `BML` in the name.)
   This script will generate several files that are required to complete the
   package.
5. To confirm that everything worked, try checking your new package. This can be
   done from a terminal using the following commands:
   ```
   R CMD build path_to_package_directory
   R CMD check package_tarball
   ```
   where `path_to_package_directory` is the path to the package's root directory
   and `package_tarball` is the tarball created by `R CMD build`, which is
   usually formatted like `packageName_version.tar.gz` (for example,
   `BioCro_3.0.0.tar.gz`).
6. As another confirmation, you can try following the installation instructions
   example in the `README.md` file of your new repository.
7. Commit the new files to your new repository.

### Customizing your new module library

Now you are ready to start adding new modules to the package! Adding new modules
requires the addition of new module classes in the `src/module_library`
directory, and modifications to the `#include` directives and table in
`src/module_library/module_library.cpp`.

You may also want to update `README.md`, `NEWS.md`, `DESCRIPTION`, and
`man/module_library.Rd` to include information about your new module library and
its authors.

By default, R packages derived from `skelBML` are licensed under the MIT
license. If you wish to use a different license for distributing your package,
you will need to modify the `DESCRIPTION` and `README.md` files, and replace
`LICENSE.md` as appropriate. (Please keep in mind that any works derived
from `skelBML` must be licensed in accordance with the terms of the `skelBML`
license.)

Another optional step is to implement one or more GitHub workflows/actions/pages
using the following functions from the `usethis` R package:
- If you have written any documentation for your package, such as a `.Rd` file
  that describes a data set or a long-form vignette, it might be helpful to use
  `pkgdown` to build an automatically-updated website for your package. This
  will make your documentation more accessible to current or potential users.
  For an example, see the [public BioCro Documentation
  website](https://biocro.github.io). A `pkgdown` website workflow can be
  initialized by calling `usethis::use_pkgdown_github_pages()` from an R session
  running in the main directory of your package repository. This would be most
  useful for a public module library repository.
- If you want to make sure that your package tests pass on multiple operating
  systems, it might be helpful to automatically run `R CMD check` on development
  branches before merging them into your main branch. Such a workflow can be
  initialized by calling `usethis::use_github_action_check_standard()` from an
  R session running in the main directory of your package repository.

While working on your module library, please avoid modifying the following
files; doing so may cause difficulties in the future when updating your
repository from the skeleton module library:
- Any files directly in the `src` directory.
- Any files in the `src/framework` directory.
- Any files describing the skeleton module library: `docs/README.md`,
  `skelBML_license.md`, and `skelBML_description`.
- Any files in the `script` directory.
Most files that should not be modified include the following text at the start
of the file: "WARNING: This file was included in this package by the BioCro
skeleton module library and should not be manually edited." The major exceptions
are the files in `src/framework`, which should not be edited but do not include
this warning.

### Updating your module library from the skeleton

The BioCro development team may make changes to the BioCro framework and/or the
skeleton module library in the future; every effort will be made to limit the
frequency of these updates, but they will nevertheless occur. When there is an
update to this repository, you can update your module library from it by taking
the following steps:
1. Make a new "development" branch for your repository (based on the main
   branch) and make sure this branch is checked out in your working copy.
2. Make sure you have a remote named `upstream` that points to the
   `biocro/skelBML` repository (_this_ repository). This can be done with the
   following command:
   ```
   git remote add upstream https://github.com/biocro/skelBML.git
   ```
   If the remote already exists, you will get an error, but there is no harm in
   running the command.
3. Update from the remote using the commands
   ```
   git fetch upstream
   git merge upstream/main
   ```
   You may need to address one or more merge conflicts at this point.
4. Complete the update as follows:
   1. Rerun the setup script, running it as described above.
   2. Any files that would be changed by the script will be backed up; for
      example, if the script would modify
      `src/module_library/module_library.cpp`, it will first store the contents
      of the original file in a new file called something like
      `src/module_library/module_library.cpp-36185ed74f94.bak` (where
      `36185ed74f94` is a randomly generated alphanumeric string).
   3. For any files with corresponding `.bak` versions, check for important
      customizations that should be retained in the new version of those files,
      such as the table of modules, the package title, etc. Delete backups as
      you see fit.
5. To confirm that everything worked, try building and checking the package on
   the development branch with `R CMD build` and `R CMD check`.
6. When everything is working, commit the changed files to the development
   branch, and then merge the branch into the main branch of your repository
   (possibly by first creating a pull request if you are working with a team).

Updating your repository on a development branch as described in these steps
will allow you to test out the changes before committing them to your main
branch.

### Making contributions

Please see the contribution guidelines before submitting changes.
These may be found in Chapter One of the _Developer's Manual_ on the
[public BioCro Documentation web site](https://biocro.github.io).

### Software Documentation

See the [public BioCro Documentation web
site](https://biocro.github.io), which includes documentation for the C++
framework, the BioCro R package, and the standard module library.

### License

`skelBML` is licensed under the MIT license, while the BioCro C++ framework is
licensed under version 3 or greater of the GNU Lesser General Public License
(LGPL). This scheme allows people to freely develop models for any use (public
or private) under the MIT license, but any changes to the framework that
assembles and solves models must make source code changes available to all users
under the LGPL. See `LICENSE.note` for more details.
