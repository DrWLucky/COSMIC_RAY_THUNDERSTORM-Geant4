// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#pragma once

#include "AnalysisManager.hh"
#include "G4Box.hh"
#include "G4Navigator.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "fortran.hh"
#include "globals.hh"
#include <locale.h>
#include <numeric>
#include <stdlib.h>
#include "cosmic_ray_generator.hh"
#include "Settings.hh"

class G4Event;

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:

  PrimaryGeneratorAction();

  ~PrimaryGeneratorAction() override;

public:

  void           GeneratePrimaries(G4Event *) override;

  G4ParticleGun* GetParticleGun()
  {
    return fParticleGun;
  }

  G4double Sample_one_RREA_gammaray_energy(const G4double& MinEner,
                                           const G4double& MaxEner,
                                           const G4double& E_cut);

  G4double Sample_one_PL_energy(const G4double& MinEner,
                                const G4double& MaxEner,
                                const G4double& index);

private:

  Settings *settings = Settings::getInstance();

  Cosmic_Ray_Generator *cosmic_ray_gene = new Cosmic_Ray_Generator();
  G4ParticleGun        *fParticleGun; // pointer a to G4 service class
  G4ParticleTable      *particleTable = G4ParticleTable::GetParticleTable();
  AnalysisManager      *analysis      = AnalysisManager::getInstance();
};

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
