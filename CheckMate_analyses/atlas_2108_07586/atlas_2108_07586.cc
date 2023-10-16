#include "atlas_2108_07586.h"
#include "fastjet/tools/Filter.hh"
// AUTHOR: jxl
//  EMAIL: 1111
void Atlas_2108_07586::initialize()
{
  setAnalysisName("atlas_2108_07586");
  setInformation(""
                 "# ATLAS\n"
                 "# arXiv-2108-07586\n"
                 "# 0 lepton, 2 more jets, etmiss\n"
                 "# sqrt(s) = 13 TeV\n"
                 "# int(L) = 139 fb^-1\n"
                 "");
  setLuminosity(139.0 * units::INVFB);
  bookSignalRegions("SR-4Q-WW;SR-4Q-WZ;SR-4Q-ZZ;SR-4Q-VV;SR-2B2Q-WZ;SR-2B2Q-Wh;SR-2B2Q-ZZ;SR-2B2Q-Zh;SR-2B2Q-VZ;SR-2B2Q-Vh");
  // You can also book cutflow regions with bookCutflowRegions("CR1;CR2;..."). Note that the regions are
  //  always ordered alphabetically in the cutflow output files.

  // You should initialize any declared variables here
}

void Atlas_2108_07586::analyze()
{
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

  // Event reconstruction
  missingET->addMuons(muonsCombined); // Adds muons to missing ET. This should almost always be done which is why this line is not commented out.

  std::vector<Jet*> b_jets_pre;
  std::vector<Jet*> small_R_jets_pre;
  b_jets_pre = filterPhaseSpace(jets, 20., -2.5, 2.5);
  electronsMedium = filterPhaseSpace(electronsMedium, 4.5, -2.47, 2.47);
  muonsCombined = filterPhaseSpace(muonsCombined, 3., -2.7, 2.7);
  small_R_jets_pre = filterPhaseSpace(jets, 30., -4.5, 4.5);
  photonsMedium = filterPhaseSpace(photonsMedium, 50., -2.4, 2.4);

  std::vector<Jet*> b_jets;
  for (int i = 0; i < b_jets_pre.size(); i++){
    if (checkBTag(b_jets_pre[i])){
      b_jets.push_back(b_jets_pre[i]);
    }
  }

  std::vector<Jet*> small_R_jets;
  for (int i = 0; i < small_R_jets_pre.size(); i++){
    if (!checkBTag(small_R_jets_pre[i])){
      small_R_jets.push_back(small_R_jets_pre[i]);
    }
  }

  std::vector<fastjet::PseudoJet> convertJets;
  for ( int i = 0; i < jets.size(); i++ ) {
    double tmppx = jets[i]->P4().Px();
    double tmppy = jets[i]->P4().Py();
    double tmppz = jets[i]->P4().Pz();
    double tmpe  = jets[i]->P4().E();
    convertJets.push_back( fastjet::PseudoJet(tmppx, tmppy, tmppz, tmpe) );
  }

  fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, 1.0);
  fastjet::ClusterSequence cs(convertJets, jet_def);
  std::vector<fastjet::PseudoJet> largeRJets = sorted_by_pt(cs.inclusive_jets());

  std::vector<fastjet::PseudoJet> largeRJets_trimmed;
  double Rfilt = 0.2;
  fastjet::Selector selector_trimmer = fastjet::SelectorPtFractionMin(0.05);
  fastjet::Filter trimmer(Rfilt, selector_trimmer);
  for(int i=0; i<largeRJets.size(); i++) {
    fastjet::PseudoJet trimmed_jet = trimmer(largeRJets[i]);
    largeRJets_trimmed.push_back(trimmed_jet);
  }

  largeRJets_trimmed = largeRJets;

  std::vector<fastjet::PseudoJet> largeRJets_candidate;
  for (std::vector<fastjet::PseudoJet>::iterator it = largeRJets_trimmed.begin(); it != largeRJets_trimmed.end(); it++)
  {
    if (it->pt() < 200)
    {
     continue;
    }
    if (fabs(it->eta()) >= 2.0)
    {
      continue;
    }
    if (it->m() <= 40)
    {
      continue;
    }
    largeRJets_candidate.push_back(*it);
  }

  photonsMedium = overlapRemoval(photonsMedium, electronsMedium, 0.4);
  photonsMedium = overlapRemoval(photonsMedium, muonsCombined, 0.4);
  small_R_jets = overlapRemoval(small_R_jets, electronsMedium, 0.2);
  small_R_jets = overlapRemoval(small_R_jets, photonsMedium, 0.2);
  small_R_jets = overlapRemoval(small_R_jets, muonsCombined, 0.4);
  electronsMedium = overlapRemoval(electronsMedium, small_R_jets, 0.4);
  muonsCombined = overlapRemoval(muonsCombined, small_R_jets, 0.4);
  photonsMedium = overlapRemoval(photonsMedium, small_R_jets, 0.4);

  // largeRJets_candidate = overlapRemoval(largeRJets_candidate, electronsMedium, 1.0); 
  std::vector<fastjet::PseudoJet> large_R_Jets;
  for (int j = 0; j < largeRJets_candidate.size(); j++){
    bool good_jet = true;
    TLorentzVector largeRJet_p4(largeRJets_candidate[j].px(), largeRJets_candidate[j].py(), largeRJets_candidate[j].pz(), largeRJets_candidate[j].e());
   for (int e = 0; e < electronsMedium.size(); e++){
      double Je_dR = largeRJet_p4.DeltaR(electronsMedium[e]->P4());
      if (Je_dR <= 1.0){
        good_jet = false;
        break;
      }
    }
    if (good_jet){
      large_R_Jets.push_back(largeRJets_candidate[j]);
    }
  }

  electronsMedium = filterPhaseSpace(electronsMedium, 10., -2.47, 2.47);
  muonsCombined = filterPhaseSpace(muonsCombined, 10., -2.7, 2.7);
  small_R_jets = filterPhaseSpace(small_R_jets, 30., -2.8, 2.8);
  photonsMedium = filterPhaseSpace(photonsMedium, 200., -2.4, 2.4);
  

  // Common preselection
  double MET = missingET->P4().Et();

  // MET > 200
  if (MET < 200) {
    return;
  }
  countCutflowEvent("01_missingET > 200");
  
  // Lepton veto
  if(!electronsMedium.empty() || !muonsCombined.empty()){
    return;
  }
  countCutflowEvent("02_Lepton_veto");

  // n_large_R_Jets
  if (large_R_Jets.size() < 2){
    return;
  }
  countCutflowEvent("03_n_large_R_jets >= 2");

  std::vector<fastjet::PseudoJet> large_R_Jqq;
  std::vector<fastjet::PseudoJet> large_R_Jbb;
  for (int i = 0; i < 2; i++) {
    int b_jet_multiplicity = 0;
    for (int j = 0; j < b_jets.size(); j++) {
      TLorentzVector large_R_Jet_p4(large_R_Jets[i].px(), large_R_Jets[i].py(), large_R_Jets[i].pz(), large_R_Jets[i].e());
      double Jb_dR = large_R_Jet_p4.DeltaR(b_jets[j]->P4());
      if (Jb_dR < 1.0) {
        b_jet_multiplicity++;
      }
    }
    if (b_jet_multiplicity == 2) {
      large_R_Jbb.push_back(large_R_Jets[i]);
    }
    else if (b_jet_multiplicity < 2){
      large_R_Jqq.push_back(large_R_Jets[i]);
    }
  }

  // unmatched_b_jets
  std::vector<Jet*> unmatched_b_jets;
  for (int i = 0; i < b_jets.size(); i++) {
    bool is_contained = false;
    for (int j = 0; j < large_R_Jqq.size(); j++) {
      TLorentzVector Jqq_p4(large_R_Jqq[j].px(), large_R_Jqq[j].py(), large_R_Jqq[j].pz(), large_R_Jqq[j].e());
      double Jqqb_dR = Jqq_p4.DeltaR(b_jets[i]->P4());
        if (Jqqb_dR < 1.0) {
          is_contained = true;
          break;
        }
    }
    for (int j = 0; j < large_R_Jbb.size(); j++) {
      TLorentzVector Jbb_p4(large_R_Jbb[j].px(), large_R_Jbb[j].py(), large_R_Jbb[j].pz(), large_R_Jbb[j].e());
      double Jbbb_dR = Jbb_p4.DeltaR(b_jets[i]->P4());
        if (Jbbb_dR < 1.0) {
          is_contained = true;
          break;
        }
    }
    if (!is_contained) {
      unmatched_b_jets.push_back(b_jets[i]);
    }
  }
  if(unmatched_b_jets.size() > 0){
    return;
  }
  countCutflowEvent("04_unmatched_b_jet = 0");

  // deltaphi_MET_small_R_Jets
  for (int i = 0; i < small_R_jets.size(); i++){
    if (fabs(small_R_jets[i]->P4().DeltaPhi(missingET->P4())) <= 1.0){
      return;
    }
  }
  countCutflowEvent("05_small_R_jets_missingET_DeltaPhi > 1.0");

  // 4Q or 2B2Q
  bool SR_4Q = false;
  bool SR_4Q_WW = false;
  bool SR_4Q_WZ = false;
  bool SR_4Q_ZZ = false;
  bool SR_4Q_VV = false;
  bool SR_2B2Q = false;
  bool SR_2B2Q_WZ = false;
  bool SR_2B2Q_ZZ = false;
  bool SR_2B2Q_VZ = false;
  bool SR_2B2Q_Wh = false;
  bool SR_2B2Q_Zh = false;
  bool SR_2B2Q_Vh = false;
  if(large_R_Jbb.size() == 0){
    SR_4Q = true;
    if(large_R_Jqq[0].m() >= 65 && large_R_Jqq[0].m() <= 105 && large_R_Jqq[1].m() >= 65 && large_R_Jqq[1].m() <= 105){
      countCutflowEvent("99_test0");
      SR_4Q_VV = true;
    }
    if(large_R_Jqq[0].m() >= 65 && large_R_Jqq[0].m() <= 75 && large_R_Jqq[1].m() >= 65 && large_R_Jqq[1].m() <= 75){
      countCutflowEvent("99_test1");
      SR_4Q_WW = true;
    }
    if(large_R_Jqq[0].m() >= 95 && large_R_Jqq[0].m() <= 105 && large_R_Jqq[1].m() >= 95 && large_R_Jqq[1].m() <= 105){
      countCutflowEvent("99_test2");
      SR_4Q_ZZ = true;
    }
    if(large_R_Jqq[0].m() >= 95 && large_R_Jqq[0].m() <= 105 && large_R_Jqq[1].m() >= 65 && large_R_Jqq[1].m() <= 75){
      countCutflowEvent("99_test3");
      SR_4Q_WZ = true;
    }
    if(large_R_Jqq[0].m() >= 65 && large_R_Jqq[0].m() <= 75 && large_R_Jqq[1].m() >= 95 && large_R_Jqq[1].m() <= 105){
      countCutflowEvent("99_test3");
      SR_4Q_WZ = true;
    }
  }
  if(large_R_Jbb.size() == 1){
    SR_2B2Q = true;
    if(large_R_Jbb[0].m() >= 70 && large_R_Jbb[0].m() <= 100 && large_R_Jqq[0].m() >= 65 && large_R_Jqq[0].m() <= 105){
      countCutflowEvent("99_test4");
      SR_2B2Q_VZ = true;
    }
    if(large_R_Jbb[0].m() >= 70 && large_R_Jbb[0].m() <= 100 && large_R_Jqq[0].m() >= 65 && large_R_Jqq[0].m() <= 75){
      countCutflowEvent("99_test5");
      SR_2B2Q_WZ = true;
    }
    if(large_R_Jbb[0].m() >= 70 && large_R_Jbb[0].m() <= 100 && large_R_Jqq[0].m() >= 95 && large_R_Jqq[0].m() <= 105){
      countCutflowEvent("99_test6");
      SR_2B2Q_ZZ = true;
    }
    if(large_R_Jbb[0].m() >= 100 && large_R_Jbb[0].m() <= 135 && large_R_Jqq[0].m() >= 65 && large_R_Jqq[0].m() <= 105){
      countCutflowEvent("99_test7");
      SR_2B2Q_Vh = true;
    }
    if(large_R_Jbb[0].m() >= 100 && large_R_Jbb[0].m() <= 135 && large_R_Jqq[0].m() >= 65 && large_R_Jqq[0].m() <= 75){
      countCutflowEvent("99_test8");
      SR_2B2Q_Wh = true;
    }
    if(large_R_Jbb[0].m() >= 100 && large_R_Jbb[0].m() <= 135 && large_R_Jqq[0].m() >= 95 && large_R_Jqq[0].m() <= 105){
      countCutflowEvent("99_test9");
      SR_2B2Q_Zh = true;
    }
  }

  // countSignalEvent
  if(SR_4Q){
    double m_eff = missingET->PT + large_R_Jqq[0].pt() + large_R_Jqq[1].pt();
    
    // n_bjets <=1 
    if(b_jets.size() > 1){
      return;
    }
    countCutflowEvent("06_SR4QVV_b_jets.size() <= 1");

    // missingET > 300
    if(MET <= 300){
      return;
    }
    countCutflowEvent("07_SR4QVV_MET > 300");

    // meff > 1300
    if(m_eff <= 1300){
      return;
    }
    countCutflowEvent("08_SR4QVV_meff > 1300");

    // n(Vqq) = 2
    if (!SR_4Q_VV){
      return;
    }
    countCutflowEvent("09_SR4QVV_n(Vqq) = 2");

    // SR_4Q count Signal
    if(SR_4Q_WW){
      countCutflowEvent("90_SR-4Q-WW");
      countSignalEvent("SR-4Q-WW");
    }
    else if(SR_4Q_WZ){
      countCutflowEvent("91_SR-4Q-WZ");
      countSignalEvent("SR-4Q-WZ");
    }
    else if(SR_4Q_ZZ){
      countCutflowEvent("92_SR-4Q-ZZ");
      countSignalEvent("SR-4Q-ZZ");
    }
    else if(SR_4Q_VV){
      countCutflowEvent("93_SR-4Q-VV");
      countSignalEvent("SR-4Q-VV");
    }
  }
  else {
    // n(Jbb) = 1
    if (!SR_2B2Q){
      return;
    }
    countCutflowEvent("10_SR2B2Q_n(Jbb) = 1");

    // SR-2B2Q mT2
    TLorentzVector Jqq0_p4(large_R_Jqq[0].px(), large_R_Jqq[0].py(), large_R_Jqq[0].pz(), large_R_Jqq[0].e());
    TLorentzVector Jbb0_p4(large_R_Jbb[0].px(), large_R_Jbb[0].py(), large_R_Jbb[0].pz(), large_R_Jbb[0].e());
    double mT2_2B2Q = mT2(Jqq0_p4, Jbb0_p4, 0.); 

    if(mT2_2B2Q <= 250){
      return;
    }
    countCutflowEvent("11_SR2B2Q_mT2 > 250");
    
    // meff > 1000
    double m_eff = missingET->PT + large_R_Jqq[0].pt() + large_R_Jbb[0].pt();
    if(m_eff <= 1000){
      return;
    }
    countCutflowEvent("12_SR2B2Q_m_eff > 1000");

    // n(Vqq) = 1
    if(!SR_2B2Q_VZ && !SR_2B2Q_Vh){
      return;
    }
    countCutflowEvent("13_SR2B2Q_n(Vqq) = 1");

    // SR_2B2Q count Signal
    if(SR_2B2Q_WZ){
      countCutflowEvent("94_SR-2B2Q-WZ");
      countSignalEvent("SR-2B2Q-WZ");
    }
    if(SR_2B2Q_ZZ){
      countCutflowEvent("95_SR-2B2Q-ZZ");
      countSignalEvent("SR-2B2Q-ZZ");
    }
    if(SR_2B2Q_VZ){
      countCutflowEvent("96_SR-2B2Q-VZ");
      countSignalEvent("SR-2B2Q-VZ");
    }
    if(SR_2B2Q_Wh){
      countCutflowEvent("97_SR-2B2Q-Wh");
      countSignalEvent("SR-2B2Q-Wh");
    }
    if(SR_2B2Q_Zh){
      countCutflowEvent("98_SR-2B2Q-Zh");
      countSignalEvent("SR-2B2Q-Zh");
    }
    if(SR_2B2Q_Vh){
      countCutflowEvent("99_SR-2B2Q-Vh");
      countSignalEvent("SR-2B2Q-Vh");
    }
  }
}

void Atlas_2108_07586::finalize()
{
  // Whatever should be done after the run goes here
}


// static std::vector<fastjet::PseudoJet> convertJets(std::vector<Jet*> jets) {
//     std::vector<fastjet::PseudoJet> converted;
//     for (std::vector<Jet*>::iterator it=jets.begin(); it!=jets.end(); it++) {
//         TLorentzVector p4 = (*it)->P4();
//         converted.push_back(fastjet::PseudoJet(p4.Px(), p4.Py(), p4.Pz(), p4.E()));
//     }
//     return converted;
// }

// static fastjet::PseudoJet trim_jet(fastjet::PseudoJet jet, double fcut, double lambdaHard) {
//     fastjet::PseudoJet trimmed(0,0,0,0);
//     fastjet::PseudoJet zero(0,0,0,0);
//     int n_const=0;
//     std::vector<fastjet::PseudoJet> constituents = jet.constituents();
//     for (std::vector<fastjet::PseudoJet>::iterator it=constituents.begin(); it!=constituents.end(); it++) {
//         if (it->pt() < fcut*lambdaHard) {
//             continue;
//         }
//         trimmed += (*it);
//         n_const++;
//     }
//     if (n_const < 2)
//     	return zero;
//     return trimmed;
// }

// static std::vector<fastjet::PseudoJet> recluster_jets(std::vector<Jet*> jets, double R, double fcut) {
//     std::vector<fastjet::PseudoJet> pjets = convertJets(jets);
//     fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);
//     fastjet::ClusterSequence cs(pjets, jet_def);
//     std::vector<fastjet::PseudoJet> largeJets = fastjet::sorted_by_pt(cs.inclusive_jets());

//     std::vector<fastjet::PseudoJet> trimmedJets;
//     for (std::vector<fastjet::PseudoJet>::iterator it = largeJets.begin(); it!=largeJets.end(); it++) {
//         trimmedJets.push_back(trim_jet(*it, fcut, it->pt()));
//     }
//     return trimmedJets;
// }

// void Atlas_2108_07586::recluster_large_jets() {
//   std::vector<fastjet::PseudoJet> reclustered_jets = recluster_jets(jets, 1.0, 0.05);
// }
