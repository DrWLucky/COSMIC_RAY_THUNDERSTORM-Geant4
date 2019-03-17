COSMIC_RAY_THUNDERSTORM-Gean4
=======
Geant4-based code for simulation effect of thunderstorms electric fields on cosmic ray fluxes.
=======

contact : <david.sarria@uib.no>

## Generalities
* Code based on Geant4 (particle propagation in arbitrary materials, with arbitrary electric-fields), NRL-MSISE-00 (atmosphere model) and PARMA (cosmic-ray model) to simulate the effect of thunderstorms electric fields on cosmic ray fluxes (electrons, positrons, muons, photons, etc.).
* Integrates the [NRL-MSISE-00 model](https://ccmc.gsfc.nasa.gov/pub/modelweb/atmospheric/msis/nrlmsise00/) for the atmosphere and [PARMA](https://phits.jaea.go.jp/expacs/) for the cosmic-ray generator.
* The code itself mostly uses Geant4 features. See [documentation](http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/index.html "Geant4 documentation").
* Produces an output file in the `.mat` format in the folder `build/output_mat`, that can be loaded not only in Matlab, but also Python (scipy library) and other languages. It contains the recorded spectra for the different particle types and the simulation settings. This output is also written as an ASCII file in the folder `build/output` but it is hard to interpret the data inside, and is used only for debugging.
* Feel free to clone, fork and suggest improvements for the code.

## Requirements
* Requires a Geant4 installation (library and headers), see [the website](http://geant4.web.cern.ch/) and [installation instructions](http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/InstallationGuide/html/index.html). Recommended version is 10.4.3. For Linux and Ubuntu users, easy installation scripts are provided in the [following repository](https://github.com/DavidSarria89/GEANT4-easy-install-script).
* Requires [MATIO](https://github.com/tbeu/matio) to be able to write the output .mat file storing the spectrums for the different particle types as well and the simulations settings. Some of the provided [easy install scripts](https://github.com/DavidSarria89/GEANT4-easy-install-script) will also include download, compilation and installation of it.

## Compilation
* The source code is localted in `src/` and the build should be done in the folder `build/`.
* The user should open a terminal in the build directory and type the commands `cmake ../` and then `make`. It produces the executable `build/mos_test`

## Usage
* The executable `mos_test` can run with default parameters, specified inside the code (see **Simulation Settings**).
* `mos_test` also accepts arguments containing the simulation configuration. For example `./mos_test 1000 10 2 100 9 10` will place and electric-field between 9 and 11 km altitude, with a potential of 100 MV, and will record particles at 9 and 10 km altitude (and stop the simulation when it has recorded at least 1000 particles)
* The python script `build/python_job_local.py` makes it possible to run the code on multiple threads (CPU cores) by running several times the executable (possibly with different settings). Implementation is straightforward since every initial particle is independent. It requires `mpi4py`, `numpy`, and possibly other python libraries. Communication between python script and executable is done with the help of the parameters `int argv` and `char** argc`  of the main function in `src/mos_test.cc`.

## Simulation Settings:
* Most of settings can be adjusted in `src/include/Settings.hh`. In particular:
  * `std::vector < int > PDG_LIST` : list of particles to be simulated (available are photons, electrons, positrons, positive muons, negative muons, neutrons, protons)
  * `WORLD_MAX_ALT` : the maximum altitude of the simulation world
  * `POTENTIAL_VALUE` : the required thnderstorm potential 
  * `EFIELD_REGION_LEN` : the altitude (direction Z) size of the E-field region
  * `EFIELD_REGION_ALT_CENTER` : the altitude center of the E-field region
  * `EFIELD_XY_HALF_SIZE` : the half size of the E-field region in the XZ plane (perpendicular to altitude direction)
  * `longitude`, `latitude` : coordinates were the simulation takes places
  * `year`, `month`, `day` : time when the simulation takes place
  * `CR_GENERATION_ALT_MIN`, `CR_GENERATION_ALT_MAX` : minimum and maximum altitude to generate the initial cosmic ray particles
  * `CR_SAMPLING_XY_HALF_SIZE` : half size in the XY plane (perpendicular to altitude) where the initial cosmic rays are generated
* other parameters are ine `src/mos_test.cc` :
  * `std::vector < G4double > RECORD_ALTITUDES` : list of altitudes to record particles. Maximum number is now 6. It is stored in `src/include/Settings.hh`, but the record altitude values should be added inside the main function (in the `src/mos_test.cc` file), using `settings->RECORD_ALTITUDES.push_back(11.0);` (here a record at 11 km is added).
  * `NB_PARTICLES_TO_GET` : number of particles to get before stopping the simulation.

## Non-exhaustive implementation details list
* By default, it uses the `G4EmStandardPhysics_option1` physics list (can be changed in `src/src/PhysicsList.cc`) with a change in the "Dr over R value". Maximum steps can also be used. Not using one of these two tweaks can lead to incorrect results for large electric fields. See [this article](https://www.geosci-model-dev.net/11/4515/2018/) for the reason why.
* The code is made so that each run will have a different random seed (that is a `long` integer storing the current time given by the `std::chrono::high_resolution_clock` function, in nanoseconds). It assumes that the program cannot be launched twice during the exact same nanosecond.

