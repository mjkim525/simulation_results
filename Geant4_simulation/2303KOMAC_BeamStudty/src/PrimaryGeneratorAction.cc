#include <random>
#include "PrimaryGeneratorAction.hh"
#include "ParameterContainer.hh"
#include "G4IonTable.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction(ParameterContainer* par)
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0),
	PC(par)
{
	fParticleGun  = new G4ParticleGun();
	if(PC->GetParInt("Beam_Input_Opt") == 1)
	{
		fInputName = PC -> GetParString("Input_File_Name");
		ReadInputFile();
	}
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
	if(PC->GetParInt("Beam_Input_Opt") == 1)
		fInputFile.close();
  delete fParticleGun;
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	if(PC->GetParInt("Beam_Input_Opt") == 0)
		GeneratePrimariesOpt0(anEvent);
	else if (PC->GetParInt("Beam_Input_Opt") == 1)	{
		GeneratePrimariesOpt1(anEvent);
	}	else	{
		G4ExceptionDescription out;
		out << "ParameterContainer::Beam_Input_Opt should be the 0 or 1";
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries","",FatalException,out);
	}
}

void PrimaryGeneratorAction::GeneratePrimariesOpt0(G4Event* anEvent)
{
	// default particle kinematic
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName = PC -> GetParString("Beam_particle");
	G4ParticleDefinition* particle
		= particleTable->FindParticle(particleName);
	fParticleGun->SetParticleDefinition(particle);
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
	fParticleGun->SetParticleEnergy(PC->GetParDouble("Beam_energy")*MeV);
	if (PC->GetParBool("Square_beam"))
	{
	   for(G4int n=0; n<PC -> GetParInt("NperEvent"); n++)
		{
			G4double x0 = 
				(PC->GetParDouble("Beam_x0")+PC->GetParDouble("Beam_dx")*(G4UniformRand()-0.5)) * mm;
			G4double y0 = 
				(PC->GetParDouble("Beam_y0")+PC->GetParDouble("Beam_dy")*(G4UniformRand()-0.5)) * mm;
			G4double z0 = 
				(PC->GetParDouble("Beam_z0")+PC->GetParDouble("Beam_dz")*(G4UniformRand()-0.5)) * mm;

			fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

			fParticleGun->GeneratePrimaryVertex(anEvent);
		}
	}

	if (PC->GetParBool("Radial_beam"))
	{

   	for(G4int n=0; n<PC -> GetParInt("NperEvent"); n++)
		{
			G4double z0 = (PC->GetParDouble("Beam_z0") + PC->GetParDouble("Beam_dz")*(G4UniformRand()-0.5)) * mm;

			// Add beam radius
			G4double beamRadius = PC->GetParDouble("Beam_radius")*((G4UniformRand())) * mm;
			G4double phi = 2. * CLHEP::pi *(G4UniformRand());
			G4double x0 = beamRadius * cos(phi);
			G4double y0 = beamRadius * sin(phi);

			fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
			fParticleGun->GeneratePrimaryVertex(anEvent);
		}
	}
}

void PrimaryGeneratorAction::GeneratePrimariesOpt1(G4Event* anEvent)
{

	// find event number(ID) 
	for(G4int n=0; n<PC -> GetParInt("NperEvent"); n++)
	{
		std::random_device rd; 
		std::mt19937 gen(rd()); 
		std::uniform_int_distribution<int> dis_0(0,vec_eventID.size()-1);
	
		int a = dis_0(gen);

		if(vec_PDG[a] > 1000000000){
			G4ParticleDefinition* particle = G4IonTable::GetIonTable()->GetIon(vec_PDG[a]);
			fParticleGun -> SetParticleDefinition(particle);
		}else{
			G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(vec_PDG[a]);
			fParticleGun -> SetParticleDefinition(particle);
		}

		G4ThreeVector mom(vec_px[a],vec_py[a],vec_pz[a]);
		G4ThreeVector pos(vec_vx[a],vec_vy[a],vec_vz[a]);
		fParticleGun -> SetParticleMomentumDirection(mom.unit());
		fParticleGun -> SetParticleMomentum(mom.mag()*MeV);
		fParticleGun -> SetParticlePosition(pos);
		fParticleGun -> GeneratePrimaryVertex(anEvent);
	}
}

void PrimaryGeneratorAction::ReadInputFile()
{
	fInputFile.open(fInputName.data());
	G4int nEvents = 0;
	// container 
	G4String line1, line2;
	G4int eventID, nTracks, pdg;
	G4double vx, vy, vz, px, py, pz;
	//getline(fInputFile,line1);	// dummy line
	 while (getline(fInputFile, line1) && getline(fInputFile, line2))
	{
		//getline(fInputFile,line1);
		//getline(fInputFile,line2);
		stringstream ss1(line1);
		stringstream ss2(line2);
		ss1 >> eventID >> nTracks >> vx >> vy >> vz;
		ss2 >> pdg >> px >> py >> pz;

		vec_eventID.push_back(eventID);
		vec_PDG.push_back(pdg);
		vec_vx.push_back(vx);
		vec_vy.push_back(vy);
		vec_vz.push_back(vz);
		vec_px.push_back(px);
		vec_py.push_back(py);
		vec_pz.push_back(pz);

		++nEvents;
	}

	G4cout << "Input: " << fInputName <<  " contains " << nEvents <<" "<< "events" << G4endl;

	G4cout << G4endl;
}
