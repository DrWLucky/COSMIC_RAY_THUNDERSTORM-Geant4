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


#pragma once

#include "globals.hh"
#include <vector>
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "g4csv.hh"
#include <array>
#include "Settings.hh"
#include <algorithm>
#include <stdio.h>
#include <string.h> /* For strcmp() */
#include <stdlib.h> /* For EXIT_FAILURE, EXIT_SUCCESS */
#include <vector> /* For STL */

// #include "g4root.hh"
// #include "g4xml.hh"

// following singleton pattern

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class AnalysisManager
{
private:

  AnalysisManager();

  static AnalysisManager *instance;

private:

  ////////////////////////////////////////////////////
  //        static const uint NB_ENER_BINS = 256;
  //        static const uint NB_MOM_BINS = 128;

public:

  ~AnalysisManager();

  static AnalysisManager* getInstance();

  void                    write_output_file_endOf_program();

  G4int                   NB_OUTPUT() const;

  void                    check_if_should_use_stacking_action();

  void                    add_NB_OUTPUT();

  void                    fill_histogram_E(const G4int     idx_part,
                                           const G4int     idx_alt,
                                           const G4double& value);
  void                    fill_histogram_mX(const G4int     idx_part,
                                            const G4int     idx_alt,
                                            const G4double& value);
  void                    fill_histogram_mY(const G4int     idx_part,
                                            const G4int     idx_alt,
                                            const G4double& value);
  void                    fill_histogram_mZ(const G4int     idx_part,
                                            const G4int     idx_alt,
                                            const G4double& value);

  static const uint nbp       = 7;
  static const uint nbalt_max = 6;
  static const uint ngride    = 256;
  static const uint ngridm    = 128;
  uint PART_SPEC[nbp][nbalt_max][ngride];
  uint PART_MOM_X[nbp][nbalt_max][ngridm];
  uint PART_MOM_Y[nbp][nbalt_max][ngridm];
  uint PART_MOM_Z[nbp][nbalt_max][ngridm];
  uint counter_up[nbp][nbalt_max];
  uint counter_down[nbp][nbalt_max];

private:

  Settings *settings = Settings::getInstance();

  G4String asciiFileName1;

  G4int NB_OUTPUT_ = 0;

  std::vector < G4String > RECORDED_OUTPUT_STRINGS;

  std::ofstream asciiFile_analysis;

  //    bool not_contains(const G4int &x, const std::vector<G4int> &v);

  G4double get_scale(const G4double alt);

  G4double interpolate(std::vector < G4double >& xData,
                       std::vector < G4double >& yData,
                       G4double                  x,
                       bool                      extrapolate);

  ////////////////////////////////////////////////////

  uint  size_grid_ener = 256;
  const std::vector < double >
  ENER_GRID { 0.0500, 0.0515, 0.0531, 0.0547, 0.0563, 0.0580, 0.0598, 0.0616, 0.0635, 0.0654, 0.0674, 0.0694, 0.0715, 0.0737, 0.0759, 0.0782, 0.0806, 0.0830, 0.0855, 0.0881,
              0.0908, 0.0935, 0.0963, 0.0992, 0.1022,
              0.1053, 0.1085, 0.1118, 0.1152, 0.1187, 0.1223, 0.1260, 0.1298, 0.1337, 0.1378, 0.1419, 0.1462, 0.1506, 0.1552, 0.1599, 0.1647,
              0.1697, 0.1749, 0.1801, 0.1856, 0.1912, 0.1970, 0.2030, 0.2091, 0.2154,
              0.2219, 0.2287, 0.2356, 0.2427, 0.2500, 0.2576, 0.2654, 0.2734, 0.2817, 0.2902, 0.2990, 0.3081, 0.3174, 0.3270, 0.3369, 0.3471,
              0.3576, 0.3684, 0.3795, 0.3910, 0.4028, 0.4150, 0.4276, 0.4405, 0.4539,
              0.4676, 0.4817, 0.4963, 0.5113, 0.5268, 0.5427, 0.5592, 0.5761, 0.5935, 0.6115, 0.6300, 0.6490, 0.6687, 0.6889, 0.7097, 0.7312,
              0.7533, 0.7761, 0.7996, 0.8238, 0.8487, 0.8744, 0.9009, 0.9281, 0.9562,
              0.9851, 1.0149, 1.0456, 1.0773, 1.1099, 1.1435, 1.1780, 1.2137, 1.2504, 1.2882, 1.3272, 1.3674, 1.4088, 1.4514, 1.4953, 1.5405,
              1.5871, 1.6352, 1.6846, 1.7356, 1.7881, 1.8422, 1.8980, 1.9554, 2.0145,
              2.0755, 2.1383, 2.2030, 2.2696, 2.3383, 2.4091, 2.4820, 2.5570, 2.6344, 2.7141, 2.7962, 2.8808, 2.9680, 3.0578, 3.1503, 3.2456,
              3.3438, 3.4450, 3.5492, 3.6566, 3.7673, 3.8812, 3.9987, 4.1197, 4.2443,
              4.3727, 4.5050, 4.6413, 4.7818, 4.9264, 5.0755, 5.2291, 5.3873, 5.5503, 5.7182, 5.8912, 6.0695, 6.2531, 6.4423, 6.6372, 6.8380,
              7.0449, 7.2581, 7.4777, 7.7039, 7.9370, 8.1771, 8.4246, 8.6795, 8.9421,
              9.2126, 9.4913, 9.7785, 10.0744, 10.3792, 10.6932, 11.0168, 11.3501, 11.6935, 12.0473, 12.4118, 12.7873, 13.1742, 13.5728, 13.9835,
              14.4066, 14.8425, 15.2915, 15.7542, 16.2309, 16.7220, 17.2279, 17.7491,
              18.2862, 18.8394, 19.4094, 19.9967, 20.6017, 21.2251, 21.8672, 22.5289, 23.2105, 23.9128, 24.6363, 25.3817, 26.1496, 26.9408,
              27.7559, 28.5957, 29.4609, 30.3523, 31.2706, 32.2168, 33.1915, 34.1958,
              35.2304, 36.2963, 37.3945, 38.5259, 39.6916, 40.8925, 42.1297, 43.4044, 44.7177, 46.0707, 47.4646, 48.9007, 50.3802, 51.9045,
              53.4750, 55.0929, 56.7598, 58.4771, 60.2464, 62.0693, 63.9472, 65.8820,
              67.8754, 69.9290, 72.0448, 74.2246, 76.4703, 78.7840, 81.1677, 83.6236, 86.1537, 88.7604, 91.4459, 94.2127, 97.0632, 100.0000 };

  uint  size_grid_mom = 128;
  const std::vector < double >
  MOM_GRID { -1.0000, -0.9843, -0.9685, -0.9528, -0.9370, -0.9213, -0.9055, -0.8898, -0.8740, -0.8583, -0.8425, -0.8268, -0.8110, -0.7953, -0.7795, -0.7638, -0.7480, -0.7323,
             -0.7165, -0.7008, -0.6850, -0.6693, -0.6535,
             -0.6378, -0.6220, -0.6063, -0.5906, -0.5748, -0.5591, -0.5433, -0.5276, -0.5118, -0.4961, -0.4803, -0.4646, -0.4488, -0.4331,
             -0.4173, -0.4016, -0.3858, -0.3701, -0.3543, -0.3386, -0.3228, -0.3071, -0.2913,
             -0.2756, -0.2598, -0.2441, -0.2283, -0.2126, -0.1969, -0.1811, -0.1654, -0.1496, -0.1339, -0.1181, -0.1024, -0.0866, -0.0709,
             -0.0551, -0.0394, -0.0236, -0.0079, 0.0079, 0.0236, 0.0394, 0.0551, 0.0709,
             0.0866, 0.1024, 0.1181, 0.1339, 0.1496, 0.1654, 0.1811, 0.1969, 0.2126, 0.2283, 0.2441, 0.2598, 0.2756, 0.2913, 0.3071, 0.3228,
             0.3386, 0.3543, 0.3701, 0.3858, 0.4016, 0.4173, 0.4331, 0.4488, 0.4646, 0.4803,
             0.4961, 0.5118, 0.5276, 0.5433, 0.5591, 0.5748, 0.5906, 0.6063, 0.6220, 0.6378, 0.6535, 0.6693, 0.6850, 0.7008, 0.7165, 0.7323,
             0.7480, 0.7638, 0.7795, 0.7953, 0.8110, 0.8268, 0.8425, 0.8583, 0.8740, 0.8898,
             0.9055, 0.9213, 0.9370, 0.9528, 0.9685, 0.9843, 1.0000 };
};
