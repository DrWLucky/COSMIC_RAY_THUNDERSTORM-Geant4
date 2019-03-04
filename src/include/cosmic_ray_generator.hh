#pragma once

#include "AnalysisManager.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "fortran.hh"
#include "globals.hh"
#include <locale.h>
#include <numeric>
#include <stdlib.h>
#include "Settings.hh"
#include <random>

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

struct cosmic_ray_parma_output
{
  int    type;
  double cos_zenith_angle;
  double altitude;
  double energy;
};

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

struct geant4_initial_cosmic_ray
{
  G4ThreeVector         momentum_ini;
  G4ThreeVector         position_ini;
  double                time;
  double                energy;
  G4ParticleDefinition *g4_particle;
};

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// int random number generator between 0 and 1000000
class random_number_generator_int {
  std::uniform_int_distribution < int > dis;
  std::mt19937_64 gen;

public:

  random_number_generator_int(int lower = 0, int upper = 1000000)
    : gen(std::random_device()()), dis(lower, upper) {}

  double operator()() {
    return dis(gen);
  }
};

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// double random number generator between 0 and 1
class random_number_generator_double {
  std::uniform_real_distribution < double > dis;
  std::mt19937_64 gen;

public:

  random_number_generator_double(double lower = 0.0, double upper = 1.0)
    : gen(std::random_device()()), dis(lower, upper) {}

  double operator()() {
    return dis(gen);
  }
};

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class Cosmic_Ray_Generator
{
public:

  Cosmic_Ray_Generator();
  ~Cosmic_Ray_Generator();

  geant4_initial_cosmic_ray generate_Cosmic_ray();

private:

  random_number_generator_int rand_int;
  random_number_generator_double rand_double;

  Settings *settings = Settings::getInstance();

  // all the particle types that it is possible to have
  G4ParticleDefinition *Gamma    = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  G4ParticleDefinition *Electron = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  G4ParticleDefinition *Positron = G4ParticleTable::GetParticleTable()->FindParticle("e+");
  G4ParticleDefinition *Neutron  = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
  G4ParticleDefinition *Proton   = G4ParticleTable::GetParticleTable()->FindParticle("proton");
  G4ParticleDefinition *MuonP    = G4ParticleTable::GetParticleTable()->FindParticle("mu+");
  G4ParticleDefinition *MuonN    = G4ParticleTable::GetParticleTable()->FindParticle("mu-");

  char ___; // PDG

  enum Parma_ID : int {
    parma_phot = 33,
    parma_elec = 31,
    parma_posi = 32,
    parma_muP  = 29,
    parma_muN  = 30,
    parma_neut = 0,
    parma_prot = 1
  };

  int nb_parmaID_total = 7;

  ////////////////////////////////////////////////

  const std::vector < int > PDG_LIST = settings->PDG_LIST;      // the list of particles types we want

  int nb_part_type_wanted = 0;                                  // initialization, will have the size of ID_list_wanted and particles_wanted,
  // determined at runtime
  std::vector<int> parmaID_list_wanted;                                     // the particles (parma ID) we want
  std::vector < G4ParticleDefinition * > particles_type_wanted; // the particles types we want

  // arrays that may be reduced if less particles are requested
  Parma_ID ID_list_total[7] =
  { parma_phot, parma_elec, parma_posi, parma_neut, parma_prot, parma_muP, parma_muN };
  std::vector < G4ParticleDefinition * > all_particles_types =  // all the particle types that it is possible to have
  { Gamma, Electron, Positron, Neutron, Proton, MuonP, MuonN }; // not used

  G4double min_cr_ener = settings->CR_GENERATION_ENER_MIN;      // MeV
  G4double max_cr_ener = settings->CR_GENERATION_ENER_MAX;      // MeV
  G4double min_alt     = settings->CR_GENERATION_ALT_MIN;       // km
  G4double max_alt     = settings->CR_GENERATION_ALT_MAX;       // km
  G4double min_cosAng  = -1.0;
  G4double max_cosAng  = 1.0;

  int iyear    = settings->year;
  int imonth   = settings->month;
  int iday     = settings->day;
  double glat  = settings->latitude;
  double glong = settings->longitude;

  int nebin   = 512; // size of energy mesh (will be log)
  int nabin   = 128; // size of angle mesh (linear)
  int naltbin = 2;   // size of altitude mesh (linear)
  // the bigger the numbers, more precise will be the sampling, but the more memory it will take
  //        int nebin = 768; // size of energy mesh (will be log)
  //        int nabin = 128; // size of angle mesh (linear)
  //        int naltbin = 256; // size of altitude mesh (linear)

  int counter             = 0;      // indexing of the sampled cosmic rays
  int seed_cr_smpl        = 123456; // dummy value, random seed of CR generator
  const static int nb_int = 1000000;
  double output_energies[nb_int];
  double output_cosangles[nb_int];
  double output_altitudes[nb_int];
  int    output_types[nb_int];

  // Functions

  std::vector < double > logspace(double& xmin, double& xmax, int nbb);

  G4ThreeVector           CR_direction_rand_sample(const double& cos_Sampled);
  G4ThreeVector           sample_CR_secondary_position(const double& altitude);

  cosmic_ray_parma_output sample_parameter_set_from_PARMA_distrutions_cumu();

  void                    generator_output_for_test();
  void                    generator_output_for_test_momentum(const G4ThreeVector& mom);
  void                    generate_samples_from_Parma();
  Parma_ID                find_parma_ID_from_PDG(const int PDG_in);

  G4String name_outFile_mom          = "./tests/cr_sampl_test_mom.txt";
  G4String filename_cr_sampling_test = "./tests/cr_sampl_test_";
};
