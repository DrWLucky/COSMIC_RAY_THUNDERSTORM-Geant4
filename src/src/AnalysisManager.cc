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

#include "AnalysisManager.hh"

// class following singleton pattern

AnalysisManager *AnalysisManager::instance = nullptr;

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

AnalysisManager * AnalysisManager::getInstance() // singleton lazy initialization
{
  if (instance == nullptr)
  {
    instance = new AnalysisManager;
  }

  return instance;
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
AnalysisManager::AnalysisManager() // constructor
{
  // initialization of spectrums in energy and momentum direction
  memset(PART_SPEC,    0, sizeof(PART_SPEC));
  memset(PART_MOM_X,   0, sizeof(PART_MOM_X));
  memset(PART_MOM_Y,   0, sizeof(PART_MOM_Y));
  memset(PART_MOM_Z,   0, sizeof(PART_MOM_Z));

  memset(counter_up,   0, sizeof(counter_up));
  memset(counter_down, 0, sizeof(counter_down));

  // sanity checks
  if (settings->RECORD_ALTITUDES.size() > nbalt_max)
  {
    G4cout << "Error in AnalysisManager.hh/cc : the number of requested record altitudes is more than the pre-alocated memory of the array." << G4endl;
    std::abort();
  }

  if (ENER_GRID.size() != size_grid_ener)
  {
    G4cout << "Error in AnalysisManager.hh/cc : energy grid vector does not have 256 elements." << G4endl;
    std::abort();
  }

  if (MOM_GRID.size() != size_grid_mom)
  {
    G4cout << "Error in AnalysisManager.hh/cc : momentum-direction grid vector does not have 128 elements." << G4endl;
    std::abort();
  }

  //////

  RECORDED_OUTPUT_STRINGS.clear();

  G4String first_part        = "./output/Part_ener_mom_dists_";
  G4String potential_str     = "_" + std::to_string(int(settings->POTENTIAL_VALUE));
  G4String efield_alt_str    = "_" + std::to_string(int(settings->EFIELD_REGION_ALT_CENTER * 10.));
  G4String efield_length_str = "_" + std::to_string(int(settings->EFIELD_REGION_LEN * 100.));
  G4String rec_alt_part      = "_" + std::to_string(int(settings->RECORD_ALTITUDES[0] * 10.)) + "km";

  asciiFileName1 = first_part + std::to_string(settings->RANDOM_SEED) + rec_alt_part + potential_str + efield_alt_str + efield_length_str + ".out";

  G4cout << "Creating output file : " << asciiFileName1 << G4endl;

  // open / close with trunc to clean the file (if it does exist) or to create it

  asciiFile_analysis.open(asciiFileName1, std::ios::trunc);

  if (asciiFile_analysis.is_open())
  {
    asciiFile_analysis.close();
  }
  else
  {
    G4cout << G4endl << "ERROR : cannot open output file. Aborting" << G4endl;
    std::abort();
  }
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

AnalysisManager::~AnalysisManager() = default;

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4int AnalysisManager::NB_OUTPUT() const
{
  return NB_OUTPUT_;
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void AnalysisManager::add_NB_OUTPUT()
{
  NB_OUTPUT_++;

#ifndef NDEBUG // debug mode
  G4cout << "Number of outputs : " << NB_OUTPUT_ << G4endl;
#endif // ifndef NDEBUG
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void AnalysisManager::write_output_MATLAB_file_endOf_program()
{
  G4String str_rng = std::to_string(settings->RANDOM_SEED);
  G4String file    = "./output_mat/" + str_rng + ".mat";

  //  G4String file = "./output_mat/test.mat";

  G4cout << "Creating file " << file << G4endl;

  MATLAB_MATIO_output MATLAB_MATIO_outp(file);

  MATLAB_MATIO_outp.output_scalar(settings->RANDOM_SEED,                         "RANDOM_SEED");
  MATLAB_MATIO_outp.output_scalar(settings->EFIELD_REGION_ALT_CENTER,            "EFIELD_REGION_ALT_CENTER");
  MATLAB_MATIO_outp.output_scalar(settings->EFIELD_REGION_LEN,                   "EFIELD_REGION_LEN");
  MATLAB_MATIO_outp.output_scalar(settings->POTENTIAL_VALUE,                     "POTENTIAL_VALUE");
  MATLAB_MATIO_outp.output_scalar(settings->RREA_PART_NB_LIMIT_HAS_BEEN_REACHED, "RREA_PART_NB_LIMIT_HAS_BEEN_REACHED");

  MATLAB_MATIO_outp.output_vector(ENER_GRID, "ENERGY_GRID");
  MATLAB_MATIO_outp.output_vector(MOM_GRID, "MOMENTUM_GRID");

  MATLAB_MATIO_outp.output_vector(settings->RECORD_ALTITUDES, "RECORD_ALTITUDES");
  MATLAB_MATIO_outp.output_vector(settings->PDG_LIST,         "PDG_LIST");

  MATLAB_MATIO_outp.output_2D_matrix<nbp, nbalt_max>(counter_up,   "counter_up");
  MATLAB_MATIO_outp.output_2D_matrix<nbp, nbalt_max>(counter_down, "counter_down");

  MATLAB_MATIO_outp.output_3D_matrix<nbp, nbalt_max, ngride>(PART_SPEC,  "PART_SPEC");
  MATLAB_MATIO_outp.output_3D_matrix<nbp, nbalt_max, ngridm>(PART_MOM_X, "PART_MOM_X");
  MATLAB_MATIO_outp.output_3D_matrix<nbp, nbalt_max, ngridm>(PART_MOM_Y, "PART_MOM_Y");
  MATLAB_MATIO_outp.output_3D_matrix<nbp, nbalt_max, ngridm>(PART_MOM_Z, "PART_MOM_Z");

  G4cout << "Done" << G4endl;
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void AnalysisManager::write_output_file_endOf_program()
{
  asciiFile_analysis.open(asciiFileName1, std::ios::out | std::ios::app);

  if (asciiFile_analysis.is_open())
  {
    asciiFile_analysis << std::scientific << std::setprecision(6)
                       << settings->RANDOM_SEED
                       << " " << settings->NB_EVENT
                       << " " << settings->EFIELD_REGION_ALT_CENTER
                       << " " << settings->EFIELD_REGION_LEN
                       << " " << settings->POTENTIAL_VALUE
                       << " " << settings->RREA_PART_NB_LIMIT_HAS_BEEN_REACHED;

    for (uint i_alt = 0; i_alt < settings->RECORD_ALTITUDES.size(); ++i_alt)
    {
      for (uint i_part = 0; i_part < nbp; ++i_part)
      {
        asciiFile_analysis
          << " " << settings->PDG_LIST[i_part]
          << " " << settings->RECORD_ALTITUDES[i_alt]
          << " " << counter_up[i_part][i_alt]
          << " " << counter_down[i_part][i_alt];

        for (uint ii = 0; ii < ngride; ++ii)
        {
          asciiFile_analysis << " " << PART_SPEC[i_part][i_alt][ii];
        }

        for (uint ii = 0; ii < ngridm; ++ii)
        {
          asciiFile_analysis << " " << PART_MOM_X[i_part][i_alt][ii];
        }

        for (uint ii = 0; ii < ngridm; ++ii)
        {
          asciiFile_analysis << " " << PART_MOM_Y[i_part][i_alt][ii];
        }

        for (uint ii = 0; ii < ngridm; ++ii)
        {
          asciiFile_analysis << " " << PART_MOM_Z[i_part][i_alt][ii];
        }
      }
    }

    asciiFile_analysis << G4endl;
  }
  else
  {
    G4cout << G4endl << "ERROR : cannot open output file. Aborting." << G4endl;
    std::abort();
  }

  asciiFile_analysis.close();
}

// ======================================================================
// Returns interpolated value at x from parallel arrays ( xData, yData )
//   Assumes that xData has at least two elements, is sorted and is strictly monotonic increasing
//   boolean argument extrapolate determines behaviour beyond ends of array (if needed)
G4double AnalysisManager::interpolate(std::vector<G4double>& xData, std::vector<G4double>& yData, G4double x, bool extrapolate)
{
  int size = static_cast<int>(xData.size());

  int i = 0;                // find left end of interval for interpolation

  if (x >= xData[size - 2]) // special case: beyond right end
  {
    i = size - 2;
  }
  else
  {
    while (x > xData[i + 1]) i++;
  }

  double xL = xData[i], yL = yData[i], xR = xData[i + 1], yR = yData[i + 1]; // points on either side (unless beyond ends)

  if (!extrapolate)                                                          // if beyond ends of array and not extrapolating
  {
    if (x < xL)
    {
      yR = yL;
    }

    if (x > xR)
    {
      yL = yR;
    }
  }

  double dydx = (yR - yL) / (xR - xL); // gradient

  return yL + dydx * (x - xL);         // linear interpolation
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double AnalysisManager::get_scale(const G4double alt)
{
  // returns the atmospheric relative scale compared to sea level (>1)
  std::vector<G4double> alt_list =
  { 0,
    0.500000000000000,
    1.000000000000000,
    1.500000000000000,
    2.000000000000000,
    2.500000000000000,
    3.000000000000000,
    3.500000000000000,
    4.000000000000000,
    4.500000000000000,
    5.000000000000000,
    5.500000000000000,
    6.000000000000000,
    6.500000000000000,
    7.000000000000000,
    7.500000000000000,
    8.000000000000000,
    8.500000000000000,
    9.000000000000000,
    9.500000000000000,
    10.000000000000000,
    10.500000000000000,
    11.000000000000000,
    11.500000000000000,
    12.000000000000000,
    12.500000000000000,
    13.000000000000000,
    13.500000000000000,
    14.000000000000000,
    14.500000000000000,
    15.000000000000000,
    15.500000000000000,
    16.000000000000000,
    16.500000000000000,
    17.000000000000000,
    17.500000000000000,
    18.000000000000000,
    18.500000000000000,
    19.000000000000000,
    19.500000000000000,
    20.000000000000000, };

  std::vector<G4double> scale_list =
  { 1.000000000000000,
    1.059301380991064,
    1.121238177128117,
    1.184377838328792,
    1.249042145593870,
    1.317304778260430,
    1.388415672913118,
    1.463688404983724,
    1.543377914546100,
    1.628371628371629,
    1.719862833025587,
    1.818435364663227,
    1.925291598996014,
    2.041647095663066,
    2.168634624979211,
    2.307556184746062,
    2.460377358490566,
    2.628502318081032,
    2.813376483279396,
    3.018518518518519,
    3.245395719263315,
    3.496916063287745,
    3.774240231548481,
    4.080100125156446,
    4.414353419092755,
    4.781811514484781,
    5.180770758839889,
    5.613430908308223,
    6.082089552238807,
    6.589186457806973,
    7.133479212253830,
    7.720544701006513,
    8.348271446862997,
    9.024221453287199,
    9.753178758414361,
    10.541632983023444,
    11.398601398601400,
    12.325141776937620,
    13.334696799263730,
    14.431164231961047,
    15.627996164908916, };

  if ((alt > 20.) || (alt < 0.))
  {
    G4cout << "ERROR in get_scale : altitude of E field is not between 0 and 20. Aborting.";
    std::abort();
  }

  return interpolate(alt_list, scale_list, alt, false);
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnalysisManager::fill_histogram_E(const G4int idx_part, const G4int idx_alt, const G4double& value)
{
  for (uint ii = 0; ii < ngride - 1; ++ii)
  {
    if ((value >= ENER_GRID[ii]) && (value < ENER_GRID[ii + 1]))
    {
      PART_SPEC[idx_part][idx_alt][ii]++;
      return;
    }
  }
}

void AnalysisManager::fill_histogram_mX(const G4int idx_part, const G4int idx_alt, const G4double& value)
{
  for (uint ii = 0; ii < ngridm - 1; ++ii)
  {
    if ((value >= MOM_GRID[ii]) && (value < MOM_GRID[ii + 1]))
    {
      PART_MOM_X[idx_part][idx_alt][ii]++;
      return;
    }
  }
}

void AnalysisManager::fill_histogram_mY(const G4int idx_part, const G4int idx_alt, const G4double& value)
{
  for (uint ii = 0; ii < ngridm - 1; ++ii)
  {
    if ((value >= MOM_GRID[ii]) && (value < MOM_GRID[ii + 1]))
    {
      PART_MOM_Y[idx_part][idx_alt][ii]++;
      return;
    }
  }
}

void AnalysisManager::fill_histogram_mZ(const G4int idx_part, const G4int idx_alt, const G4double& value)
{
  for (uint ii = 0; ii < ngridm - 1; ++ii)
  {
    if ((value >= MOM_GRID[ii]) && (value < MOM_GRID[ii + 1]))
    {
      PART_MOM_Z[idx_part][idx_alt][ii]++;
      return;
    }
  }
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AnalysisManager::check_if_should_use_stacking_action()
{
  // checks if E-field is above RREA, and if so, sets settings->USE_STACKING_ACTION to true

  G4double EFIELD_VAL = (settings->POTENTIAL_VALUE * megavolt) / (settings->EFIELD_REGION_LEN * km);

  G4double margin_factor       = 1.0;
  G4double EFIELD_VAL_KV_PER_M = std::abs(EFIELD_VAL / (kilovolt / meter)) * margin_factor;

  G4double atmos_scale = get_scale(settings->EFIELD_REGION_ALT_CENTER);                 // should be > 1

  settings->DELTA_T = 27.3 / (EFIELD_VAL_KV_PER_M * atmos_scale - 277.0) * atmos_scale; // in micro second for the stacking action that set up the
                                                                                        // fake time oriented

  settings->DELTA_T = settings->DELTA_T / 10.0 * microsecond;

  double DELTA_T_tmp = settings->DELTA_T; // variable for debug mode check

  if (atmos_scale < 1.0)
  {
    G4cout << "ERROR in create_thunderstorm_electric_fields : atmos_scale is <1. Aborting" << G4endl;
    std::abort();
  }

  // if below RREA threshold, no need to have the stacking action for fake time oriented
  // and the calculed time step (DELTA_T) should be negative
  settings->USE_STACKING_ACTION = settings->DELTA_T >= 0.0;
}
