DNest5
======

(c) 2020 Brendon J. Brewer.

Please Clone Recursively
========================

`$ git clone --recursive https://github.com/eggplantbren/DNest5`

This will pull in a submodule that I'm using.

LICENCE
=======

GNU General Public License version 3.

Dependencies
============

* Boost
* Python 3 with NumPy, matplotlib, apsw
* SQLite3
* yaml-cpp

Installation
============

To compile `DNest5`, run the build script. This generates a `Makefile` and
then runs `make` to compile everything.

`$ ./build.py`

By default, this compiles the StraightLine example from
<a href="https://www.jstatsoft.org/index.php/jss/article/view/v086i07/v86i07.pdf" target="_blank" rel="noopener noreferrer">DNest4</a>. The two executable
files created are `main`, which does the sampling, and `postprocess`, which
creates posterior samples and computes the marginal likelihood.

Running and Outputs
===================

First run `main`, then run `showresults.py` in Python. Showresults calls
`postprocess`, so you don't need to run `postprocess` on its own. It is safe
to run `showresults.py` while `main` is still running.

Compiling a Different Model
===========================

Edit `ModelType.hpp` and change the `#include` and `using` lines
to point to a different model. Then run `./build.py` again.

At this time, all example models in the `Example` directory are implemented
using inheritance from the `UniformModel` class template.
These follow the convention where the exploration is done in terms of a coordinate system that makes the prior Uniform(0, 1) for every coordinate.
The user/model writer has to write the member functions `us_to_params` to
convert from the `us` (Uniform(0, 1) parameters) to the actual parameters,
and `log_likelihood` in order to evaluate the likelihood function.

Specifying the Options
======================

The configuration file `options.yaml` plays the role of DNest4's `OPTIONS`
and its command line arguments.

Outputs
=======

All outputs are written to the `output` directory. The files are:

* `dnest5.db`: An SQLite3 database of output, similar to DNest4's `sample.txt`,
    `sample_info.txt`, and `levels.txt`, but combined into one binary file.
* `figure?.pdf`: Figures exported by `showresults.py`. Note that if you run
    `showresults.py' while the main process is running, some inconsistencies
    can appear in Figure 3 between the particles and the levels. These
    do not have any major consequences.
* `posterior.db`: An SQLite3 database of posterior samples. Does not store
    the samples themselves, but rather refers back to the appropriate rows
    in `dnest5.db`.
* `posterior.csv`: CSV file of posterior samples.
* `results.yaml`: YAML file (plain text) with marginal likelihood values and
    related things.
