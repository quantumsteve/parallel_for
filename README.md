parallel_for
============

Here is an attempt to create a generic parallel_for_each function for Mantid. The goal is to minimize changes to the current openmp macros while falling back to threading building blocks on platforms without openmp.

The CMake variable `MANTID_PARALLELISM_BACKEND` datermines the library used for parallelism. Valid options are  `TBB`, `OPENMP`, and `SERIAL`

