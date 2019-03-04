#pragma once

#include "G4SDManager.hh"
#include <G4AssemblyVolume.hh>
#include <G4String.hh>
#include <G4Types.hh>
#include <G4VUserDetectorConstruction.hh>
#include "G4VPhysicalVolume.hh"

#include <CLHEP/Units/SystemOfUnits.h>
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4UnitsTable.hh"

// #include "E_Field.hh"

#include "Settings.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4EqMagElectricField.hh"
#include "G4ElectroMagneticField.hh"
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4FieldManager.hh"
#include "G4ElectroMagneticField.hh"
#include "G4TransportationManager.hh"
#include "G4ClassicalRK4.hh"
#include "G4UniformElectricField.hh"
#include "G4UniformElectricField_timeCut.hh"
#include "G4UserLimits.hh"
#include "G4DormandPrince745.hh"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include "G4MagIntegratorDriver.hh"

#include "SD.hh"

// #include "E_Field.hh"

extern "C" {
#include <coordinates_conversions.h>
}

class G4LogicalVolume;

class G4Material;

class G4Box;

class G4Cons;

class G4Tubs;

class G4VPhysicalVolume;

class G4BooleanSolid;

class G4UnionSolid;

class G4UserLimits;

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction: public G4VUserDetectorConstruction
{
public:

  DetectorConstruction();

  ~DetectorConstruction() override;

public:

  G4VPhysicalVolume* Construct() override;

private:

  Settings *settings = Settings::getInstance();

  std::vector < double > altitudes;

  // usefull null position and rotation
  G4RotationMatrix *rotation_null    = new G4RotationMatrix;
  G4ThreeVector     translation_null = G4ThreeVector(0, 0, 0);

  // World volume
  G4double World_XZ_half_size = 50. * CLHEP::km;
  G4double World_Y_size       = settings->WORLD_MAX_ALT * CLHEP::km;
  G4int    nb_altitudes       = 256;
  G4Box   *sWorld;           // pointer to the solid envelope
  G4LogicalVolume   *lWorld; // pointer to the logical envelope
  G4VPhysicalVolume *pWorld; // pointer to the physical envelope
  std::vector < G4Material * > Construct_Atmos_layers_Materials_simple(const vector < G4double > &altitudes_);

  void create_thunderstorm_electric_fields();

  G4FieldManager *globalfieldMgr = nullptr;
  G4FieldManager *localfieldMgr  = nullptr;
  G4double field_val_temp        = 0.0; // just for debug

  G4double interpolate(const vector < G4double >& xData,
                       const vector < G4double >& yData,
                       const G4double           & x,
                       const bool                 extrapolate);

  //        std::vector < G4Material * > Construct_Atmos_layers_Materials(const std::vector < G4double > altitudes_);
  std::vector < G4double > calculate_altitudes_list(G4double alt_min, G4double alt_max_construction, G4int nb_altitudes);

  void                          build_air_layers();


  G4Region *EFIELD_Region = new G4Region("EFIELD");

  std::ofstream asciiFile;

  G4bool contains(const vector < G4double >& v,
                  const G4double           & x);

  std::vector < SensitiveDet * > sens_det_List;

  bool                 hasDuplicates(const std::vector < G4double >& arr);

  G4double             get_scale(const G4double& alt);

  G4Element *elN = new G4Element("Nitrogen",
                                 "N",
                                 7.,
                                 14.01 *g / mole);
  G4Element *elO = new G4Element("Oxygen",
                                 "O",
                                 8.,
                                 16.00 *g / mole);
};

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
