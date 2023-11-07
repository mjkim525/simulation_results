#include "KOMAC_make.h"
#include <algorithm>
using namespace std;

void KOMAC_make_PostTrack()
{
    //gStyle->SetOptStat(0);

    TH1D *hPostTrackID = new TH1D("hPostTrackID","",1000,0,1000);

    TFile* G = new TFile("out.root", "recreate");

    TH1D *hProcessID = new TH1D("hProcessID","",10,0,10);

    const int Particle_arr = 4; // PID_Index -> p, n, g, e-, e+, deutron, 3He, alpha

    TH3D* hPhi_Theta_kE_corr[Particle_arr];
    TH2D* hSCVxVycorr[Particle_arr];

    for(int a  = 0; a < Particle_arr; a++)
    { 
        hPhi_Theta_kE_corr[a] = new TH3D(Form("%s_Phi_Theta_kE_corr",IndextoLabel(a).c_str()),"",360,-180,180,180,0,180,100,0,100);
        hSCVxVycorr[a] = new TH2D(Form("%s_PostVxVy_corr",IndextoLabel(a).c_str()),"",160,-8,8,400,166,206);
    }

    ifstream flist;
	char fname[300];
	sprintf(fname, "file.lst");
	flist.open(fname);

	while ( flist >> fname ){

		TFile* F = TFile::Open(fname);
		if (!F || F->IsZombie()){ 
			cout <<"Cannot open " <<fname <<endl; continue; 
		}else{
			cout << "OPEN: " << fname << endl;
		}
		TTree* T = (TTree*)F->Get("G4sim");

        const int nEvents = T -> GetEntriesFast();

        int nPostTrack;
        T->SetBranchAddress("nPostTrack", &nPostTrack);

        int DetectorID[20000]; //Post Track DetectorID
        T->SetBranchAddress("PostTrackDetID", DetectorID);
        int PDG[20000];
        T->SetBranchAddress("PostTrackPDG", PDG);
        double PX[20000];
        T->SetBranchAddress("PostTrackPX", PX);
        double PY[20000];
        T->SetBranchAddress("PostTrackPY", PY);
        double PZ[20000];
        T->SetBranchAddress("PostTrackPZ", PZ);

        double VX[20000];
        T->SetBranchAddress("PostTrackVX", VX);
        double VY[20000];
        T->SetBranchAddress("PostTrackVY", VY);
        double VZ[20000];
        T->SetBranchAddress("PostTrackVZ", VZ);


        for (int a=0; a < nEvents; a++)
        {
            T->GetEntry(a);


            vector<int> FromT;
            vector<int> FromTSC;

		    vector<float> PartPhi;
            vector<float> PartTheta;
            vector<float> kE1;

            vector<float> fVx1;
			vector<float> fVy1;


            for(int b=0; b<nPostTrack; b++){

                ROOT::Math::PxPyPzMVector LV(PX[b],PY[b],PZ[b],PDGtoMass(PDG[b]));
                const int fNumA = PDGtoAZ(PDG[b]).first;
                const double kE = fNumA>0?((LV.E()-LV.M())/fNumA):(LV.E()-LV.M()); 


                if(DetectorID[b] == 0)
                {
                    FromT.push_back(PDG[b]);
                    PartPhi.push_back(LV.Phi());
                    PartTheta.push_back(LV.Theta());

                    kE1.push_back(kE);


                    if(VZ[b] > -870.){
                        if(VX[b] >= -8. && VX[b] <= 8. && VY[b] >= 167. && VY[b] <= 205.){
                            FromTSC.push_back(PDG[b]);
                            fVx1.push_back(VX[b]);
                            fVy1.push_back(VY[b]);
                        }
                    }
                
                }


                


            }

            for (unsigned int b=0; b<FromT.size(); b++)
            {
                int PID_Index = PDGtoIndex(FromT[b]);

                float phi = PartPhi[b]*180./TMath::Pi();
                float theta = PartTheta[b]*180./TMath::Pi();

                const double kE = kE1[b];

                for(int c=0; c<Particle_arr; c++)
                {
                    if(PID_Index == c ){
                        // not exist slit -> no need cut
                        hPhi_Theta_kE_corr[c] -> Fill(phi,theta,kE);
                    }
                 
                }
            }

            for (unsigned int b=0; b<FromTSC.size(); b++)
            {
                int PID_IndexSC = PDGtoIndex(FromTSC[b]);

                for(int c=0; c<Particle_arr; c++)
                {
                    if(PID_IndexSC == c){
                     hSCVxVycorr[c] -> Fill(fVx1[b],fVy1[b]);
                    }
                }
            }


        }

    
    F->Close();

    }


    G->Write();
	G->Close();




	return;

}