#pragma once

#include "AnalysisManager.hh"
#include "G4UserEventAction.hh"
#include "Settings.hh"
#include "globals.hh"

#include <sys/time.h>
#include <time.h>

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction : public G4UserEventAction
{
public:
    EventAction();

    ~EventAction() override;

public:
    void
    BeginOfEventAction(const G4Event *) override;

    void
    EndOfEventAction(const G4Event *) override;

private:
    G4int print_nb = 1; // just initialisation

    AnalysisManager *analysis = AnalysisManager::getInstance();

    double
    get_wall_time() const;

    double time_begin_event = -5.;
    double time_end_event = -5.;

    double max_event_duration = -10.;
};

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
