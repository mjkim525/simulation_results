#include "SteppingAction.hh"
#include "RunAction.hh"
#include "ParameterContainer.hh"

#include "G4Step.hh"

SteppingAction::SteppingAction(RunAction* runAction)
: G4UserSteppingAction(),
  fRunAction(runAction)
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
	// information of each steps
	G4StepStatus stat = step -> GetPostStepPoint() -> GetStepStatus();
	G4int trackID = step -> GetTrack() -> GetTrackID();
	G4int trackPDG = step -> GetTrack() -> GetDefinition() -> GetPDGEncoding();
	G4ThreeVector pos = step -> GetPreStepPoint() -> GetPosition();
	G4double edep = step -> GetTotalEnergyDeposit();
	G4int prevNo = step -> GetPreStepPoint()  -> GetPhysicalVolume() -> GetCopyNo(); //Get pre. volumeID (step to material inside)
	G4double Step_dt = step -> GetPreStepPoint() -> GetLocalTime();
	G4int postNo = 0;
	if(stat == fWorldBoundary)
		return;
	else
		postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();

	if(ParameterContainer::GetInstance()->GetParBool("MCStep") && prevNo == ParameterContainer::GetInstance()->GetParInt("SC1_ID"))
		fRunAction -> FillStep(trackID, trackPDG, prevNo, postNo, pos, edep, Step_dt);

	if(ParameterContainer::GetInstance()->GetParBool("MCStep") && prevNo == ParameterContainer::GetInstance()->GetParInt("SC2_ID"))
		fRunAction -> FillStep(trackID, trackPDG, prevNo, postNo, pos, edep, Step_dt);
	
}
