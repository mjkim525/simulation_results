#define max 10000

void MakeInput(const char* inFile = "out_temp.root")
{
	// define data container
	TFile* F = new TFile(inFile,"read");
	TList* L_para = (TList*) F -> Get("InputParameters");
	TTree* T = (TTree*) F -> Get("G4sim");

	// load parameters 
	map<string,string> map_parameters;
	for(int a=0; a<L_para->GetSize();a++)
	{
		TNamed* name = (TNamed*) L_para -> At(a);
		map_parameters.insert(make_pair(name->GetName(),name->GetTitle()));
	}

	// define container for MCPostTrack
	int nPostTrack;
	int PostTrackID[max], PostTrackPDG[max], PostTrackDetID[max];
	double PostTrackVX[max], PostTrackVY[max], PostTrackVZ[max];
	double PostTrackPX[max], PostTrackPY[max], PostTrackPZ[max];

	if(map_parameters["MCPostTrack"] == "true")
	{
		cout << "MCPostTrack was called" << endl;
		T -> SetBranchAddress("nPostTrack",&nPostTrack);
		T -> SetBranchAddress("PostTrackID",PostTrackID);
		T -> SetBranchAddress("PostTrackPDG",PostTrackPDG);
		T -> SetBranchAddress("PostTrackDetID",PostTrackDetID);
		T -> SetBranchAddress("PostTrackVX",PostTrackVX);
		T -> SetBranchAddress("PostTrackVY",PostTrackVY);
		T -> SetBranchAddress("PostTrackVZ",PostTrackVZ);
		T -> SetBranchAddress("PostTrackPX",PostTrackPX);
		T -> SetBranchAddress("PostTrackPY",PostTrackPY);
		T -> SetBranchAddress("PostTrackPZ",PostTrackPZ);
	}

	// define container for output("Input.dat")
	vector<int> vec_pid;
	vector<double> vec_px, vec_py, vec_pz;
	vector<double> vec_vx, vec_vy, vec_vz;

	// event loop
	for(int a=0; a<T->GetEntriesFast(); a++)
	{
		T -> GetEntry(a);
		for(int b=0; b<nPostTrack; b++)
		{
			vec_pid.push_back(PostTrackPDG[b]);
			vec_vx.push_back(PostTrackVX[b]);
			vec_vy.push_back(PostTrackVY[b]);
			vec_vz.push_back(PostTrackVZ[b]);
			vec_px.push_back(PostTrackPX[b]);
			vec_py.push_back(PostTrackPY[b]);
			vec_pz.push_back(PostTrackPZ[b]);
		}// track
	}// event

	double translation = 0;	// for translate the z position
	double z_pos = 0;				// for fixed z postion

	// write output
	ofstream output("input.dat");
	output << vec_pid.size() << endl;
	for(int a=0; a<vec_pid.size(); a++)
	{
		// eventID nTracks vx vy vz
		output << a << " 1 " << vec_vx[a] << " " << vec_vy[a] << " " << vec_vz[a] << endl;
		// pid px py pz
		output << vec_pid[a] << " " << vec_px[a] << " " << vec_py[a] << " " << vec_pz[a] << endl;
	}// write all tracks

	output.close();
}
