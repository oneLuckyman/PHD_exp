#include "cms_sus_21_001.h"
// AUTHOR: Liangliang Shang
//  EMAIL: shangliangliang@htu.edu.cn
void Cms_sus_21_001::initialize() {
  setAnalysisName("cms_sus_21_001");          
  setInformation(""
    "# CMS\n"
    "# CMS-SUS-21-001\n"
    "# 0 leptons, 2 staus, etmiss\n"
    "# sqrt(s) = 13 TeV\n"
    "# int(L) = 137 fb^-1\n"
  "");
  setLuminosity(137.0*units::INVFB);      
  bookSignalRegions("SR_1;SR_2;SR_3;SR_4;SR_5;SR_6;SR_7;SR_8;SR_9;SR_10;SR_11;SR_12;SR_13;SR_14;SR_15;SR_16;SR_17;SR_18;SR_19;SR_20;SR_21;SR_22;SR_23;SR_24;SR_25;SR_26;SR_27;SR_28;SR_29;SR_30;SR_31");
  bookCutflowRegions("CR00_All;CR01_TauJets != 2;CR02_Trigger;CR03_TauJets_Charge;CR04_veto_ele_mu;CR05_AddTauJets_PT;CR06_veto_Btag;CR07_tau1tau2_DeltaPhi;CR08_PTmiss > 50;CR09_PTmiss_jets_DeltaPhi;CR10_mT2 > 25;CR11_sum_mT > 200");
  // You can also book cutflow regions with bookCutflowRegions("CR1;CR2;..."). Note that the regions are
  //  always ordered alphabetically in the cutflow output files.

  // You should initialize any declared variables here
}

void Cms_sus_21_001::analyze() {
  // Your eventwise analysis code goes here
  // The following objects are always defined unless they are 'ignored' above. They form std::vector objects of the respective Delphes class type (except for Etmiss which is a single object)
  // All std::vector members and etmiss have the common properties PT, Eta, Phi and P4() with the latter giving access to the full ROOT TLorentzVector.
  // Within a std::vector, all members are ordered with highest pt coming first.

  // electronsLoose, electronsMedium, electronsTight   are list of electrons that passed respective efficiency and reconstruction cuts
  // muonsCombinedPlus, muonsCombined                  as above for muons
  // photonsMedium                                     as above for photons
  // jets are all reconstructed jets                   as above for jets. Note that electrons are most certainly also reconstructed as a jet -> overlap removal do avoid double counting necessary!
  // tracks, towers                                    calorimeter and tracker information. Usually not needed.
  // missingET                                         rec missing ET EXCLUDING muons.

  
  // Here is a couple of useful functions and lines:  
  //------------Phase Space Cuts (defined for jets, electronsXYZ, muonsXYZ, photonsXYZ)
  // jets = filterPhaseSpace(jets, 20., -2.8, 2.8)  // The vector 'jets' only contains jets with pt >= 20 GeV and -2.8 < eta < 2.8. This function is applicable to other particles too (electronsMedium, ... ).
  // jets = overlapRemoval(jets, electronsLoose, 0.2) Removes all jets for which there exists any electron in 'electronsLoose' with deltaR < 0.2.
  // jets = overlapRemovel(jets, 0.2) If two jets overlap within deltaR < 0.2, only the harder jet is stored.
  
  //------------Isolation Checks (defined for electronsXYZ, muonsXYZ, photonsXYZ
  //------------        For each object, if the user entered N isolation conditions, they can be
  //------------        checked individually be the second argument (running from 0 to N-1).
  // electronsMedium = filterIsolation(electronsMedium, 0)            Removes electrons that do not pass the first isolation condition entered into the AnalysisManager by the user
  // std::vector<int> flags; flags.push_back(0); flags.push_back(2);
  // electronsMedium = filterIsolation(electronsMedium, flags)        Same as above, but both the first and the third condition have to be fulfilled
  // electronsMedium = filterIsolation(electronsMedium)               Same as above, but all conditions have to be fulfilled.
  
  //-----------Flavour Tag Checks (defined for jets only)
  //----------          Tau tags "loose", "medium" or "tight" can be individually checked if the user enabled tau tagging in the AM.
  //----------          For b-tags, if N working points have been defined, the ith condition can be tested by putting i-1 into the second argument (if there is only one, the argument can be omitted)
  // if checkTauTag(jets[0], "tight") leadingJetIsTagged = True;
  // if checkBTag(jets[0], 0) leadingJetIsBTagged = True;


  //-----------Auxiliary Information
  // - Always ensure that you don't access vectors out of bounds. E.g. 'if(jets[1]->PT > 150)' should rather be if (jets.size() > 1 && jets[1]->PT > 150). 
  // - Use rand()/(RAND_MAX+1.) for random numbers between 0 and 1. The random seed is determined from system time or by the RandomSeed parameter in CheckMATE.
  // - The 'return' statement will end this function for the current event and hence should be called whenever the current event is to be vetoed.
  // - Many advanced kinematical functions like mT2 are implemented. Check the manual for more information.
  // - If you need output to be stored in other files than the cutflow/signal files we provide, check the manual for how to do this conveniently.  

  missingET->addMuons(muonsCombined);  // Adds muons to missing ET. This should almost always be done which is why this line is not commented out.
  electronsLoose = filterPhaseSpace(electronsLoose, 20., -2.5, 2.5);
  muonsCombined = filterPhaseSpace(muonsCombined, 20., -2.4, 2.4);

  std::vector<Jet*> TauJets;
  std::vector<Jet*> LightJets;

  for(int j = 0; j < jets.size(); j++) {
    if( checkTauTag(jets[j], "loose"))
      TauJets.push_back(jets[j]);
    else 
      LightJets.push_back(jets[j]);
  }

  LightJets = filterPhaseSpace(LightJets, 30., -2.4, 2.4);
  LightJets = overlapRemoval(LightJets, TauJets, 0.4);

  TauJets = filterPhaseSpace(TauJets, 40., -2.1, 2.1);

  countCutflowEvent("CR00_All");

  if (TauJets.size() != 2)
    return;
  countCutflowEvent("CR01_TauJets != 2");

  bool trigger = false;
  if (missingET->PT < 200 && TauJets[0]->PT > 35 && TauJets[1]->PT > 35)
    trigger = true;
  else if (missingET->PT > 200 && TauJets[0]->PT > 100 && TauJets[0]->PT < 140 && TauJets[1]->PT > 100 && TauJets[1]->PT < 140)
    trigger = true;
  if ( !trigger )
    return;
  countCutflowEvent("CR02_Trigger");

  if (TauJets[0]->Charge == TauJets[1]->Charge)
    return;
  countCutflowEvent("CR03_TauJets_Charge");

  if (!electronsLoose.empty() || !muonsCombined.empty())
    return;
  countCutflowEvent("CR04_veto_ele_mu");

  for (int i = 2; i < TauJets.size(); i++) {
    if (TauJets[i]->PT > 30)
      return;
  }
  countCutflowEvent("CR05_AddTauJets_PT");

  for (int i = 0; i < jets.size(); i++) {
    if( checkBTag(jets[i]))
      return;
  }
  countCutflowEvent("CR06_veto_Btag");

  if (fabs(TauJets[0]->P4().DeltaPhi(TauJets[1]->P4())) <= 1.5)
    return;
  countCutflowEvent("CR07_tau1tau2_DeltaPhi");

  if (missingET->PT <= 50.0)
    return;
  countCutflowEvent("CR08_PTmiss > 50");

  for(int j = 0; j < LightJets.size(); j++) {
    if (fabs(LightJets[j]->P4().DeltaPhi(missingET->P4())) < 0.25)
      return;
  }
  countCutflowEvent("CR09_PTmiss_jets_DeltaPhi");

  double mT_1 = mT(TauJets[0]->P4(), missingET->P4());
  double mT_2 = mT(TauJets[1]->P4(), missingET->P4());
  double sum_mT = mT_1 + mT_2;
  double mT2_tauJets = mT2(TauJets[0]->P4(), TauJets[1]->P4(), 0.);

  if (mT2_tauJets <= 25)
    return;
  countCutflowEvent("CR10_mT2 > 25");

  if (sum_mT <= 200)
    return;
  countCutflowEvent("CR11_sum_mT > 200");

  if (LightJets.size() == 0){
    if (sum_mT > 200 && sum_mT < 250 && mT2_tauJets > 25 && mT2_tauJets < 50 && TauJets[0]->PT < 90){
      countSignalEvent("SR_1");}
    if (sum_mT > 200 && sum_mT < 250 && mT2_tauJets > 25 && mT2_tauJets < 50 && TauJets[0]->PT > 90){
      countSignalEvent("SR_2");}
    if (sum_mT > 200 && sum_mT < 250 && mT2_tauJets > 50 && mT2_tauJets < 75 && TauJets[0]->PT < 90){
      countSignalEvent("SR_3");}
    if (sum_mT > 200 && sum_mT < 250 && mT2_tauJets > 50 && mT2_tauJets < 75 && TauJets[0]->PT > 90){
      countSignalEvent("SR_4");}
    if (sum_mT > 200 && sum_mT < 250 && mT2_tauJets > 75){
      countSignalEvent("SR_5");}
    if (sum_mT > 250 && sum_mT < 300 && mT2_tauJets > 25 && mT2_tauJets < 50 && TauJets[0]->PT < 90){
      countSignalEvent("SR_6");}
    if (sum_mT > 250 && sum_mT < 300 && mT2_tauJets > 25 && mT2_tauJets < 50 && TauJets[0]->PT > 90){
      countSignalEvent("SR_7");}
    if (sum_mT > 250 && sum_mT < 300 && mT2_tauJets > 50 && mT2_tauJets < 75 && TauJets[0]->PT < 90){
      countSignalEvent("SR_8");}
    if (sum_mT > 250 && sum_mT < 300 && mT2_tauJets > 50 && mT2_tauJets < 75 && TauJets[0]->PT > 90){
      countSignalEvent("SR_9");}
    if (sum_mT > 250 && sum_mT < 300 && mT2_tauJets > 75){
      countSignalEvent("SR_10");}
    if (sum_mT > 300 && sum_mT < 350 && mT2_tauJets > 25 && mT2_tauJets < 50){
      countSignalEvent("SR_11");}
    if (sum_mT > 300 && sum_mT < 350 && mT2_tauJets > 50 && mT2_tauJets < 75){
      countSignalEvent("SR_12");}
    if (sum_mT > 300 && sum_mT < 350 && mT2_tauJets > 75 && mT2_tauJets < 100){
      countSignalEvent("SR_13");}
    if (sum_mT > 300 && sum_mT < 350 && mT2_tauJets > 100){
      countSignalEvent("SR_14");}
    if (sum_mT > 350 && mT2_tauJets > 25 && mT2_tauJets < 50){
      countSignalEvent("SR_15");}
    if (sum_mT > 350 && mT2_tauJets > 50 && mT2_tauJets < 75){
      countSignalEvent("SR_16");}
    if (sum_mT > 350 && mT2_tauJets > 75 && mT2_tauJets < 100){
      countSignalEvent("SR_17");}
    if (sum_mT > 350 && mT2_tauJets > 100){
      countSignalEvent("SR_18");}}
  if (LightJets.size() >= 1){
    if (sum_mT > 200 && sum_mT < 250 && mT2_tauJets > 25 && mT2_tauJets < 50){
      countSignalEvent("SR_19");}
    if (sum_mT > 200 && sum_mT < 250 && mT2_tauJets > 50){
      countSignalEvent("SR_20");}
    if (sum_mT > 250 && sum_mT < 300 && mT2_tauJets > 25 && mT2_tauJets < 50){
      countSignalEvent("SR_21");}
    if (sum_mT > 250 && sum_mT < 300 && mT2_tauJets > 50 && mT2_tauJets < 75){
      countSignalEvent("SR_22");}
    if (sum_mT > 250 && sum_mT < 300 && mT2_tauJets > 75){
      countSignalEvent("SR_23");}
    if (sum_mT > 300 && sum_mT < 350 && mT2_tauJets > 25 && mT2_tauJets < 50){
      countSignalEvent("SR_24");}
    if (sum_mT > 300 && sum_mT < 350 && mT2_tauJets > 50 && mT2_tauJets < 75){
      countSignalEvent("SR_25");}
    if (sum_mT > 300 && sum_mT < 350 && mT2_tauJets > 75){
      countSignalEvent("SR_26");}
    if (sum_mT > 350 && mT2_tauJets > 25 && mT2_tauJets < 75){
      countSignalEvent("SR_27");}
    if (sum_mT > 350 && mT2_tauJets > 75 && mT2_tauJets < 100){
      countSignalEvent("SR_28");}
    if (sum_mT > 350 && mT2_tauJets > 100){
      countSignalEvent("SR_29");}}
  // if (fabs(TauJets[0]->P4().DeltaPhi(TauJets[1]->P4())) > 1.75){
  //   if (TauJets[1]->PT < 110){
  //     countSignalEvent("SR_30");}
  //   if (TauJets[1]->PT > 110){
  //     countSignalEvent("SR_31");}}
}

void Cms_sus_21_001::finalize() {
  // Whatever should be done after the run goes here
}       
