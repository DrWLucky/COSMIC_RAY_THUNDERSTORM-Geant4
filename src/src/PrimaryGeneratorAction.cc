// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4IonTable.hh"
#include "Randomize.hh"


// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction(), fParticleGun(nullptr)
{
  G4int n_particle = 1;

  fParticleGun = new G4ParticleGun(n_particle);
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
{
  //    G4cout << energy / keV << G4endl;

  // pos_dir is pair with 3 vector positon and 3 vector momentum direction
  //    position_ini = G4ThreeVector(0, settings->ALTITUDE_PARAMETER, 0);
  //    momentum_ini = isotropic_direction_rand_sample();
  //    momentum_ini = sample_isotropic_beam_direction(position_ini, OpeningAngle);

  geant4_initial_cosmic_ray out_cosmic = cosmic_ray_gene->generate_Cosmic_ray();

  fParticleGun->SetParticleEnergy(out_cosmic.energy);
  fParticleGun->SetParticlePosition(out_cosmic.position_ini);
  fParticleGun->SetParticleMomentumDirection(out_cosmic.momentum_ini);
  fParticleGun->SetParticleTime(out_cosmic.time);
  fParticleGun->SetParticleDefinition(out_cosmic.g4_particle);
  fParticleGun->GeneratePrimaryVertex(anEvent);
} // PrimaryGeneratorAction::GeneratePrimaries
