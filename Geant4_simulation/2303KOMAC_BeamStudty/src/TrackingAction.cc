#include "TrackingAction.hh"
#include "ParameterContainer.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

TrackingAction::TrackingAction(RunAction* runAction)
:G4UserTrackingAction(),
 fRunAction(runAction)
{
}

TrackingAction::~TrackingAction()
{}

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
	G4int trkID = track -> GetTrackID();
	G4int parentID = track -> GetParentID();
	G4int pdg = track -> GetDefinition() -> GetPDGEncoding();
	G4int detID = track -> GetVolume() -> GetCopyNo();

	G4ThreeVector p = track -> GetMomentum();
	G4ThreeVector v = track -> GetPosition();

	G4double totenergy = track -> GetTotalEnergy();
	G4double kinenergy = track -> GetKineticEnergy();
	if(ParameterContainer::GetInstance() -> GetParBool("MCTrack"))
		fRunAction -> FillTrack(MCTrack, trkID, parentID, pdg, detID, p, v, totenergy, kinenergy);
}

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
	G4int trkID = track -> GetTrackID();
	G4int parentID = track -> GetParentID();
	G4int pdg = track -> GetDefinition() -> GetPDGEncoding();
	G4int detID = track -> GetVolume() -> GetCopyNo();

	G4ThreeVector p = track -> GetMomentum();
	G4ThreeVector v = track -> GetPosition();

	G4double totenergy = track -> GetTotalEnergy();
	G4double kinenergy = track -> GetKineticEnergy();
	if(ParameterContainer::GetInstance() -> GetParBool("MCPostTrack"))
		fRunAction -> FillTrack(MCPostTrack, trkID, parentID, pdg, detID, p, v, totenergy,kinenergy);
}
