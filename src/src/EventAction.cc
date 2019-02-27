// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EventAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "Run.hh"

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction() : G4UserEventAction()
{
    if (std::abs(settings->POTENTIAL_VALUE / settings->EFIELD_REGION_LEN) < 51.0)
    {
        print_nb = 100;
    }
    else
    {
        print_nb = 100;
    }
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{
}

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event *)
{
    settings->NB_EVENT++;

    if (settings->USE_WALL_TIME_LIMIT_FOR_EVENT)
    {
        settings->wall_T_begin_event = get_wall_time();
    }

#ifndef NDEBUG // debug mode

    if (settings->NB_EVENT % print_nb == 0)
    {
        G4cout << "Begin of event : " << settings->NB_EVENT << G4endl;
    }

#endif

    settings->RREA_PART_NB_LIMIT_HAS_BEEN_REACHED = 0;

    if (settings->TIME_EVENT_DURATIONS)
    {
        time_begin_event = get_wall_time();
    }

} // EventAction::BeginOfEventAction

// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event *)
{
    //    analysis->WriteOutputFile_endOfEvent();

    if (settings->TIME_EVENT_DURATIONS)
    {
        time_end_event = get_wall_time();

        double duration = time_end_event - time_begin_event;

        if (duration > max_event_duration)
        {
            max_event_duration = duration;
        }

        G4cout << "Max event duration: " << max_event_duration << G4endl;
    }

} // EventAction::EndOfEventAction

// ------------------------------------------------------------------------

double EventAction::get_wall_time() const
// returns time in seconds
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec + (tv.tv_usec / 1000000.0);
}
