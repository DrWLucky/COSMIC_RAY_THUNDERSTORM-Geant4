#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "Run.hh"

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction *det, EventAction *event) : G4UserSteppingAction(), fDetector(det), fEventAction(event)
{}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction() = default;

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step *aStep)
{
  //////// AVOID PUTTING THIS LINE, it will produce incorrect energy record
  //        if (aStep->GetTrack()->GetTrackStatus() != fAlive) return;
  ////////////////////////////////////////////


#ifndef NDEBUG // debug mode

  if (aStep->GetStepLength() > settings->GLOBAL_MAX_STEP)
  {
    G4cout << "Current step length : " << aStep->GetStepLength() / meter << " meter" << G4endl;
    G4cout << "Error in SteppingAction : step is lager than the maximum allowed." << G4endl;
    std::abort();
  }
#endif // ifndef NDEBUG

  if (aStep->GetPreStepPoint()->GetGlobalTime() > 6.0 * second)
  {
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }

  if (aStep->GetPreStepPoint()->GetPosition().y() < 0.0)
  {
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }

  if (aStep->GetPreStepPoint()->GetPosition().y() > settings->WORLD_MAX_ALT * km)
  {
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }

  if (std::abs(aStep->GetPreStepPoint()->GetPosition().x()) > settings->CR_SAMPLING_XY_HALF_SIZE * km)
  {
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }

  if (std::abs(aStep->GetPreStepPoint()->GetPosition().z()) > settings->CR_SAMPLING_XY_HALF_SIZE * km)
  {
    aStep->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }

  ////////////////////////////
  // turn off electric field if event is too long (i.e. requires too much
  // computation time)

  if (settings->USE_WALL_TIME_LIMIT_FOR_EVENT)
  {
    nb_skip++;

    if (nb_skip > 1600000)
    {
      double WT = get_wall_time();

      if (abs(WT - settings->wall_T_begin_event) > computation_time_length_for_event_limit)
      {
        settings->current_efield_status               = settings->efield_OFF;
        settings->RREA_PART_NB_LIMIT_HAS_BEEN_REACHED = 1;
      }

      nb_skip = 0;
#ifndef NDEBUG // debug mode
      G4cout << "nb skip reset" << G4endl;
#endif // ifndef NDEBUG
    }
  }

  ////////////////////////////
  if (std::abs(aStep->GetPreStepPoint()->GetPosition().x()) > settings->EFIELD_XY_HALF_SIZE * km)
  {
    return;
  }

  if (std::abs(aStep->GetPreStepPoint()->GetPosition().z()) > settings->EFIELD_XY_HALF_SIZE * km)
  {
    return;
  }

  ///////////////////////
  const int PDG_num = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

  ///////////////////////

  // cleaning low energy particles to improve performance
  if (aStep->GetPreStepPoint())
  {
    // avoid removing positron below threshold to make sure we still have
    // annihilation

    if (aStep->GetPreStepPoint()->GetKineticEnergy() < 8.0 * keV)
    {
      if (PDG_num != Settings::pdg_posi)
      {
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        return;
      }

      // using fStopAndAlive for positrons makes a bug that saturates the RAM...
    }
  }

  ////////////////////////////

  std::vector<int>::iterator it = std::find(PDG_LST.begin(), PDG_LST.end(), PDG_num);

  if (it == PDG_LST.end()) // skiping the rest if not in list of wanted particles
  {
    return;
  }

  /// stacking action check, if pseudo time oriented simulation is activated (slow)
  if (settings->USE_STACKING_ACTION)
  {
    if (aStep->GetPreStepPoint())
    {
      if ((settings->current_efield_status != settings->efield_OFF) &&
          (is_inside_eField_region(aStep->GetPreStepPoint()->GetPosition().y() / km, aStep->GetPreStepPoint()->GetPosition().x() / km,
                                   aStep->GetPreStepPoint()->GetPosition().z() / km)))
      {
        if (aStep->GetTrack()->GetGlobalTime() > settings->VARIABLE_TIME_LIMIT)
        {
          aStep->GetTrack()->SetTrackStatus(fSuspend);
        }
      }
    }
  }

  thePrePoint = aStep->GetPreStepPoint();

  const G4double pre_y  = aStep->GetPreStepPoint()->GetPosition().y() / km;
  const G4double post_y = aStep->GetPostStepPoint()->GetPosition().y() / km;

  for (uint i_alt = 0; i_alt < settings->RECORD_ALTITUDES.size(); ++i_alt)
  {
    G4double rec_alt = settings->RECORD_ALTITUDES[i_alt];

    if (((pre_y >= rec_alt) && (post_y < rec_alt)) || ((pre_y <= rec_alt) && (post_y > rec_alt))) // particle is crossing rec_alt layer
    {
      const G4double ener = thePrePoint->GetKineticEnergy();

      const G4double momy = thePrePoint->GetMomentumDirection().y();

      // WARNING : PARTICLES ARE ALLOWED TO BE RECORED SEVERAL TIMES if they cross the detection altitude several times

      if ((ener > settings->ENERGY_MIN_RECORD) && (ener < settings->ENERGY_MAX_RECORD))
      {
        analysis->add_NB_OUTPUT();

        // Get index of element from iterator
        int i_part = std::distance(PDG_LST.begin(), it);
        analysis->fill_histogram_E(i_part, i_alt, ener);
        analysis->fill_histogram_mX(i_part, i_alt, thePrePoint->GetMomentumDirection().x());
        analysis->fill_histogram_mY(i_part, i_alt, thePrePoint->GetMomentumDirection().y());
        analysis->fill_histogram_mZ(i_part, i_alt, thePrePoint->GetMomentumDirection().z());

        if (momy > 0.0)
        {
          analysis->counter_up[i_part][i_alt]++;
        }
        else
        {
          analysis->counter_down[i_part][i_alt]++;
        }
      }
    }
  }
} // SteppingAction::UserSteppingAction

bool SteppingAction::is_inside_eField_region(const G4double& alt, const G4double& xx, const G4double& zz)

// alt, xx, zz assumed in km
{
  return alt > alt_min && alt < alt_max && std::fabs(xx) < settings->EFIELD_XY_HALF_SIZE && std::fabs(zz) < settings->EFIELD_XY_HALF_SIZE;
}

// ------------------------------------------------------------------------

double SteppingAction::get_wall_time()

// returns time in seconds
{
  struct timeval tv {};

  gettimeofday(&tv, nullptr);
  return tv.tv_sec + (tv.tv_usec / 1000000.0);
}
