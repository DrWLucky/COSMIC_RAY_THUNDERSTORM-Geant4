#pragma once

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include <vector>

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class Settings
{
private:

  Settings() = default; // Private so that it can not be called

  Settings(Settings const&)
  {}

  // copy constructor is private
  // assignment operator is private
  static Settings *instance;

public:

  static Settings* getInstance();

public:

  enum PDG_nb : int {
    pdg_phot = 22,
    pdg_elec = 11,
    pdg_posi = -11,
    pdg_muP  = -13,
    pdg_muN  = 13,
    pdg_neut = 2112,
    pdg_prot = 2212
  };

  //////////////// Parameters are listed below ////////////////

  enum Efield_State
  {
    efield_ON, efield_OFF
  };

  // list of PDG number of particles that we want to generate from parma and be recorded
  const std::vector < int > PDG_LIST = { pdg_phot, pdg_elec, pdg_posi, pdg_muP, pdg_muN, pdg_neut, pdg_prot };

  const double WORLD_MAX_ALT = 30;                      // km

  const double GLOBAL_MAX_STEP     = 50.0 * meter;      // in Geant4 units (mm)
  const bool   USE_GLOBAL_MAX_STEP = true;

  const bool CR_GENRATOR_write_output_FOR_TEST = false; // test or not the CR particle generator (i.e. output the list of particles)
  const bool ATMOS_LAYERS_OUTPUT_TO_FILE       = false; // write info about atmosphere layers on a file (for test / debug)
  const bool WRITE_MOM_OUTPUT_FOR_TEST         = false; // write info about momentum (for test / debug)
  bool USE_STACKING_ACTION                     = false; // will mimic time oriented simulation if set to true
  // can be a bad idea to set it on, because it can use a lot of memory compared to default G4 behaviour
  G4double   DELTA_T                       = -77.88;    // microsecond , value just for initialization
  const bool USE_WALL_TIME_LIMIT_FOR_EVENT = false;
  const bool TIME_EVENT_DURATIONS          = false;

  // simulation records
  G4long NB_EVENT                = 0; // initialisation is important here
  const double ENERGY_MIN_RECORD = 50. * keV;
  const double ENERGY_MAX_RECORD = 100. * MeV;

  // variables storing simulation input parameters
  G4long RANDOM_SEED = 250;                  // just initialization, will be replace at beginning of main

  //// electric field parameters
  G4double POTENTIAL_VALUE          = 120.0; // MV, may be override by input argument
  G4double EFIELD_REGION_LEN        = 2.0;   // km
  G4double EFIELD_REGION_ALT_CENTER = 5.0;   // km

  std::vector < G4double > RECORD_ALTITUDES; // km

  Efield_State current_efield_status = efield_ON;
  Efield_State initial_efield_status = efield_ON;

  const G4double EFIELD_XY_HALF_SIZE = 8.0; // km
  const G4double TILT                = 0.0 * degree;

  ////
  const G4double CR_SAMPLING_XY_HALF_SIZE = 50.0; // km

  //    const G4double longitude = 130.5; // ILDAS positron event coordinates
  //    (Australia)
  //    const G4double latitude = -13.5;  // ILDAS positron event coordinates
  //    (Australia)
  const G4double drOverR = 0.001;

  //// cosmic ray sampling parameters
  const G4double longitude              = -103.5; // deg, FEGS glow coordinates (Colorado)
  const G4double latitude               = 39.5;   // deg, FEGS glow coordinates (Colorado)
  const G4double CR_GENERATION_ENER_MIN = 0.02;   // MeV
  const G4double CR_GENERATION_ENER_MAX = 0.95e6; // MeV
  const int year                        = 2018;
  const int month                       = 5;
  const int day                         = 20;
  const  double CR_GENERATION_ALT_MIN   = 11; // km
  const  double CR_GENERATION_ALT_MAX   = 12; // km

  ////

  double wall_T_begin_event  = 0;
  double VARIABLE_TIME_LIMIT = 0;

  G4double Y_SIZE_RECORD_LAYER = 10 * km;

  //
  int RREA_PART_NB_LIMIT_HAS_BEEN_REACHED = 0; // a flag indication if the limit of particles for RREA has been reached
  // meaning that real multiplciation factor is higher than the one obtained
};

//
// namespace Settings
// {
//
//    enum Efield_State
//    {
//        ON, OFF
//    };
//
//    // All these variables are put here to be shared amounts source files
//    // (not very c++ but easier to implement)
//
//    // simulation records
//    extern G4long NB_EVENT;
//
//    extern const double ENERGY_MIN_RECORD;
//    extern const double ENERGY_MAX_RECORD;
//
//    // variables storing simulation input parameters
//    extern G4long RANDOM_SEED;
//    extern const G4double ALTITUDE_CR_PROTON_SAMPLE; // meters
//
//    extern G4double POTENTIAL_VALUE;
//
//    extern std::vector<G4double> RECORD_ALTITUDES;
//
//    extern G4double EFIELD_REGION_LEN;
//    extern G4double EFIELD_REGION_ALT_CENTER;
//
//    extern Efield_State current_efield_status;
//    extern Efield_State initial_efield_status;
//
//    extern const G4double longitude;
//    extern const G4double latitude;
//
//    extern const G4double min_cr_sp_ener; // MeV
//    extern const G4double max_cr_sp_ener; // MeV
//    extern const int year ;
//    extern const int month ;
//    extern const int day ;
//
//    extern const G4double drOverR;
//
//    extern const G4double TILT;
//
//    extern const bool USE_MAX_STEP_FOR_EFIELD;
//    extern const G4double MAX_STEP_INSIDE_EFIELD;
//
//    extern const bool USE_MAX_STEP_FOR_RECORD;
//
//    extern const bool CR_GENRATOR_write_output_FOR_TEST;
//    extern const bool ATMOS_LAYERS_OUTPUT_TO_FILE;
//    extern const bool WRITE_MOM_OUTPUT_FOR_TEST;
//
//    extern bool USE_STACKING_ACTION;
//    extern G4double DELTA_T;
//
//    extern const bool USE_WALL_TIME_LIMIT_FOR_EVENT;
//
//    extern double wall_T_begin_event;
//
//    extern double VARIABLE_TIME_LIMIT;
//
//    extern G4double SIZE_RECORD_LAYER;
//
//    extern const G4double CR_SAMPLING_XY_HALF_SIZE;
//    extern const G4double EFIELD_XY_HALF_SIZE;
//
//    extern int RREA_PART_NB_LIMIT_HAS_BEEN_REACHED;
//
//    extern const bool TIME_EVENT_DURATIONS;
// }
