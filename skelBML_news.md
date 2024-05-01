<!--
This file should document all pull requests and all user-visible changes.

When a pull request is completed, changes made should be added to a section at
the top of this file called "# UNRELEASED". All changes should be categorized
under "## MAJOR CHANGES", "## MINOR CHANGES", or "## BUG FIXES" following the
major.minor.patch structure of semantic versioning. When applicable, entries
should include direct links to the relevant pull requests.

Then, when a new release is made, "# UNRELEASED" should be replaced by a heading
with the new version number, such as "# CHANGES IN skelBML VERSION 2.0.0." This
section will combine all of the release notes from all of the pull requests
merged in since the previous release.

Subsequent commits will then include a new "UNRELEASED" section in preparation
for the next release.
-->

# skelBML VERSION 2.1.4

- Several changes were made to better comply with CRAN policies and to better
  match the BioCro framework R package:
  - The included boost library was moved to `src/inc` from `inc`
  - Copyright holders of included libraries were added to the description
  - Citation guidelines were added
  - The minimum supported version of R was changed to 3.6.0

# skelBML VERSION 2.1.3

- The boost library is now included as a git submodule, and has been updated to
  address a few compiler warnings

# skelBML VERSION 2.1.2

- The C++ framework has been updated to v1.1.3

# skelBML VERSION 2.1.1

- Updated some links in the package documentation to point to the new stable
  BioCro R package repository location
- The C++ framework has been updated to v1.1.1
- Any module libraries that were based on earlier versions of `skelBML` should
  run `script/module_library_setup.R` when updating to version 2.1.1 to help
  remove any outdated links in their own documentation.

# skelBML VERSION 2.1.0 (2023-06-15)

- This version uses the latest BioCro C++ framework and adds two new unexported
  functions to any derived R packages: `framework_version` and
  `skeleton_version`.
- Any module libraries that were based on earlier versions of `skelBML` will
  need to run `script/module_library_setup.R` when updating to version 2.0.1.

# skelBML VERSION 2.0.0

- This version adds a startup script that can be used to initialize a new BioCro
  module library.
- Any module libraries that were based on earlier versions of `skelBML` will
  need to run `script/module_library_setup.R` when updating to version 2.0.0.

# skelBML VERSION 1.0.0

- This is the initial release of the package.
