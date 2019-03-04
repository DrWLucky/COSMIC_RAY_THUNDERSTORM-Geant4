//

// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

// E-field that will be set to zero the number of secondaries electrons with a given energy threshold is too high

#pragma once

#include "G4ElectricField.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "Settings.hh"
#include "globals.hh"

class G4UniformElectricField_timeCut: public G4ElectricField
{
public:

  // with description
  explicit G4UniformElectricField_timeCut(const G4ThreeVector FieldVector);

  // A field with value equal to FieldVector.

  G4UniformElectricField_timeCut(G4double vField, G4double vTheta, G4double vPhi);

  ~G4UniformElectricField_timeCut() override;

  G4UniformElectricField_timeCut& operator = (const G4UniformElectricField_timeCut &p);

  // Copy constructor and assignment operator

  void     GetFieldValue(const G4double pos[4],
                         G4double      *field) const override;

  G4Field* Clone() const override;

private:

  Settings *settings = Settings::getInstance();

  G4double fFieldComponents[6];

  const double EFIELD_XY_HALF_SIZE_mm = settings->EFIELD_XY_HALF_SIZE * km;
};
