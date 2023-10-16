#include "atlas_conf_2022_042.h"
// AUTHOR: Liangliang Shang
//  EMAIL: shangliangliang@htu.edu.cn
void Atlas_conf_2022_042::initialize() {
  setAnalysisName("atlas_conf_2022_042");          
  setInformation(""
    "# ATLAS\n"
    "# ATLAS-CONF-2022-042\n"
    "# 2||4 taus, etmiss\n"
    "# sqrt(s) = 13 TeV\n"
    "# int(L) = 139 fb^-1\n"
  "");
  setLuminosity(139.0*units::INVFB);      
  bookSignalRegions("SR-C1C1-LM;SR-C1C1-HM;SR-C1N2OS-LM;SR-C1N2OS-HM;SR-C1N2SS-LM;SR-C1N2SS-HM;SR-Wh-LM;SR-Wh-HM");
  // bookCutflowRegions("01_overlap removal;");
  // You can also book cutflow regions with bookCutflowRegions("CR1;CR2;..."). Note that the regions are
  //  always ordered alphabetically in the cutflow output files.

  // You should initialize any declared variables here
}

void Atlas_conf_2022_042::analyze() {
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
  
  // std::vector<Jet*> LooseTauJets
  std::vector<Jet*> MediumTauJets, TightTauJets;
  std::vector<Jet*> AllJets;
  std::vector<Jet*> BJets;
  std::vector<Jet*> LightJets;

  for(int i = 0; i < jets.size(); i++){
    if(checkTauTag(jets[i], "tight"))
      TightTauJets.push_back(jets[i]);
    if(checkTauTag(jets[i], "medium"))
      MediumTauJets.push_back(jets[i]);
    // if(checkTauTag(jets[i], "loose")){
    //   LooseTauJets.push_back(jets[i]);
    // }
    else if(checkBTag(jets[i])){
      BJets.push_back(jets[i]);
    }
    else{
      LightJets.push_back(jets[i]);
    }
  }

  LightJets = filterPhaseSpace(LightJets, 20., -2.8, 2.8);
  BJets = filterPhaseSpace(BJets, 20., -2.5, 2.5);
  electronsLoose = filterPhaseSpace(electronsLoose, 10., -2.47, 2.47);
  muonsCombined = filterPhaseSpace(muonsCombined, 10., -2.7, 2.7);
  // LooseTauJets = filterPhaseSpace(LooseTauJets, 10., -2.5, 2.5);
  // LooseTauJets = filterPhaseSpace(LooseTauJets, 20., -2.47, 2.47, true);
  MediumTauJets = filterPhaseSpace(MediumTauJets, 10., -2.5, 2.5);
  MediumTauJets = filterPhaseSpace(MediumTauJets, 20., -2.47, 2.47, true);
  TightTauJets = filterPhaseSpace(TightTauJets, 10., -2.5, 2.5);
  TightTauJets = filterPhaseSpace(TightTauJets, 20., -2.47, 2.47, true);

  // LooseTauJets = overlapRemoval(LooseTauJets, electronsLoose, 0.2);
  // LooseTauJets = overlapRemoval(LooseTauJets, electronsLoose, 0.2);
  MediumTauJets = overlapRemoval(MediumTauJets, electronsLoose, 0.2);
  MediumTauJets = overlapRemoval(MediumTauJets, muonsCombined, 0.2);
  TightTauJets = overlapRemoval(TightTauJets, electronsLoose, 0.2);
  TightTauJets = overlapRemoval(TightTauJets, muonsCombined, 0.2);

  LightJets = overlapRemoval(LightJets, electronsLoose, 0.2);
  electronsLoose = overlapRemoval(electronsLoose, LightJets, 0.4);

  electronsLoose = overlapRemoval(electronsLoose, BJets, 0.4);
  LightJets = overlapRemoval(LightJets, muonsCombined, 0.4);
  // LightJets = overlapRemoval(LightJets, LooseTauJets, 0.4);
  LightJets = overlapRemoval(LightJets, MediumTauJets, 0.4);
  LightJets = overlapRemoval(LightJets, TightTauJets, 0.4);
  BJets = overlapRemoval(BJets, muonsCombined, 0.4);
  // BJets = overlapRemoval(BJets, LooseTauJets, 0.4);
  BJets = overlapRemoval(BJets, MediumTauJets, 0.4);
  BJets = overlapRemoval(BJets, TightTauJets, 0.4);

  countCutflowEvent("01_overlap removal");
  
  if (MediumTauJets.size() < 2)
    return;
  countCutflowEvent("02_MediumTauJets < 2");

  if (BJets.size() != 0)
    return;
  countCutflowEvent("03_b jets veto");

  std::vector<FinalStateObject*> leptons;
  std::vector<FinalStateObject*> light_leptons;
  for(int e = 0; e < electronsLoose.size(); e++) {
    FinalStateObject* lep = newFinalStateObject(electronsLoose[e]);
    leptons.push_back(lep);
    light_leptons.push_back(lep);
  }
  for(int m = 0; m < muonsCombined.size(); m++) {
    FinalStateObject* lep = newFinalStateObject(muonsCombined[m]);
    leptons.push_back(lep);
    light_leptons.push_back(lep);
  }
  for(int t = 0; t < MediumTauJets.size(); t++) {
    FinalStateObject* lep = newFinalStateObject(MediumTauJets[t]);
    leptons.push_back(lep);
  }
  
  int leptons_size = leptons.size();
  std::vector<double> lep_mT2s;
  for(int i = 0; i < (leptons_size - 1); i++){
    for(int j = (i + 1); j < leptons_size; j++){
      double mT2_lep_temp =  mT2(leptons[i]->P4(), leptons[j]->P4(), 0.);
      lep_mT2s.push_back(mT2_lep_temp);
    }
  }  

  int max_lep_mT2_index = 0;
  for(int i = 1; i < lep_mT2s.size(); i++){
    if (lep_mT2s[max_lep_mT2_index] < lep_mT2s[i])
      max_lep_mT2_index = i;
  }
  double lep_mT2 = lep_mT2s[max_lep_mT2_index];

  int light_leptons_size = light_leptons.size();
  int MediumTauJets_size = MediumTauJets.size();
  std::vector<double> Wh_mT2s;
  for(int i = 0; i < (MediumTauJets_size - 1); i++){
    for(int j = (i + 1); j < MediumTauJets_size; j++){
      double mT2_Wh_temp =  mT2(MediumTauJets[i]->P4(), MediumTauJets[j]->P4(), 0.);
      Wh_mT2s.push_back(mT2_Wh_temp);
    }
  }  
  for(int i = 0; i < light_leptons_size; i++){
    for(int j = 0; j < MediumTauJets_size; j++){
      double mT2_Wh_temp =  mT2(light_leptons[i]->P4(), MediumTauJets[j]->P4(), 0.);
      Wh_mT2s.push_back(mT2_Wh_temp);
    }
  }

  int max_Wh_mT2_index = 0;
  for(int i = 1; i < Wh_mT2s.size(); i++){
    if (Wh_mT2s[max_Wh_mT2_index] < Wh_mT2s[i])
      max_Wh_mT2_index = i;
  }
  double Wh_mT2 = Wh_mT2s[max_Wh_mT2_index];

  double mT_1 = mT(MediumTauJets[0]->P4(), missingET->P4());
  double mT_2 = mT(MediumTauJets[1]->P4(), missingET->P4());
  double sum_mT = mT_1 + mT_2;
  double mT_lep;
  double sum_mT_Wh;
  if(light_leptons.size() > 0) {
    mT_lep = mT(light_leptons[0]->P4(), missingET->P4());
    sum_mT_Wh = mT_1 + mT_2 + mT_lep;
  }

  double dR = MediumTauJets[0]->P4().DeltaR(MediumTauJets[1]->P4());
  double mtautau = (MediumTauJets[0]->P4() + MediumTauJets[1]->P4()).M();
  double dphi = fabs(MediumTauJets[0]->P4().DeltaPhi(MediumTauJets[1]->P4()));

  double met = missingET->P4().Et();
  bool ditau_trigger = (MediumTauJets[0]->PT > 95 && MediumTauJets[1]->PT > 60);
  bool etmiss_trigger = (MediumTauJets[0]->PT > 50 && MediumTauJets[1]->PT > 40 && met > 150);

  bool OS_tau = false;
  bool SS_tau = false;

  if(MediumTauJets.size() >= 2){
    if(MediumTauJets[0]->Charge != MediumTauJets[1]->Charge){
      OS_tau = true;}
    else{
      SS_tau = true;}
  }

  // SR-C1C1-LM 8
  if(MediumTauJets.size() == 2 && OS_tau){
    countCutflowEvent("04_01SRC1C1LM01_MediumTauOS.size == 2");
    if(TightTauJets.size() >= 1){
      countCutflowEvent("04_01SRC1C1LM02_TightTau.size >= 1");
      if(ditau_trigger){
        countCutflowEvent("04_01SRC1C1LM03_ditau_trigger");
        if(met < 150){
          countCutflowEvent("04_01SRC1C1LM04_met < 150");
          if(mtautau > 120.){
            countCutflowEvent("04_01SRC1C1LM05_Zh_veto");
            if(dphi > 1.5){
              countCutflowEvent("04_01SRC1C1LM06_dphi > 1.5");
              if(met > 60){
                countCutflowEvent("04_01SRC1C1LM07_met > 60");
                if(lep_mT2 > 80){
                  countCutflowEvent("04_01SRC1C1LM08_mT2 > 80");
                  countSignalEvent("SR-C1C1-LM");
                }
              }
            }
          }
        }
      }
    }
  }

  // SR-C1C1-HM 6
  if(MediumTauJets.size() == 2 && OS_tau){
    countCutflowEvent("04_02SRC1C1HM01_MediumTauOS.size == 2");
    if(etmiss_trigger){
      countCutflowEvent("04_02SRC1C1HM02_etmiss_trigger");
      if(met > 150){
        countCutflowEvent("04_02SRC1C1HM03_met > 150");
        if(mtautau > 120.){
          countCutflowEvent("04_02SRC1C1HM04_Zh_veto");
          if(sum_mT > 400){
            countCutflowEvent("04_02SRC1C1HM05_sum_mT > 400");
            if(lep_mT2 > 85){
              countCutflowEvent("04_02SRC1C1HM06_mT2 > 85");
              countSignalEvent("SR-C1C1-HM");
            }
          }
        }
      }
    }
  }

  // SR-C1N2OS-LM 8
  if(MediumTauJets.size() >= 2 && OS_tau){
    countCutflowEvent("04_03SRC1N2OSLM01_MediumTauOS.size > 2");
    if(TightTauJets.size() >= 1){
      countCutflowEvent("04_03SRC1N2OSLM02_TightTauJets.size > 1");
      if(ditau_trigger){
        countCutflowEvent("04_03SRC1N2OSLM03_ditau_trigger");
        if(met < 150){
          countCutflowEvent("04_03SRC1N2OSLM04_met < 150");
          if(mtautau > 120.){
            countCutflowEvent("04_03SRC1N2OSLM05_Zh_veto");
            if(LightJets.size() < 3){
              countCutflowEvent("04_03SRC1N2OSLM06_Nj < 3");
              if(met > 60){
                countCutflowEvent("04_03SRC1N2OSLM07_met > 60");
                if(lep_mT2 > 70){
                  countCutflowEvent("04_03SRC1N2OSLM08_mT2 > 70");
                  countSignalEvent("SR-C1N2OS-LM");
                }
              }
            }
          }
        }
      }
    }
  }

  // SR-C1N2OS-HM 6
  if(MediumTauJets.size() >= 2 && OS_tau){
    countCutflowEvent("04_04SRC1N2OSHM01_MediumTauOS.size > 2");
    if(etmiss_trigger){
      countCutflowEvent("04_04SRC1N2OSHM02_etmiss_trigger");
      if(met > 150){
        countCutflowEvent("04_04SRC1N2OSHM03_met > 150");
        if(mtautau > 120.){
          countCutflowEvent("04_04SRC1N2OSHM04_Zh_veto");
          if(sum_mT > 400){
            countCutflowEvent("04_04SRC1N2OSHM05_sum_mT > 400");
            if(lep_mT2 > 85){
              countCutflowEvent("04_04SRC1N2OSHM06_mT2 > 85");
              countSignalEvent("SR-C1N2OS-HM");
            }
          }
        }
      }
    }
  }

  // SR-C1N2SS-LM 7
  if(MediumTauJets.size() >= 2 && SS_tau){
    countCutflowEvent("04_05SRC1N2SSLM01_MediumTauSS.size > 2");
    if(ditau_trigger){
      countCutflowEvent("04_05SRC1N2SSLM02_ditau_trigger");
      if(met < 150){
        countCutflowEvent("04_05SRC1N2SSLM03_met < 150");
        if(dphi > 1.6){
          countCutflowEvent("04_05SRC1N2SSLM04_dphi > 1.6");
          if(LightJets.size() < 3){
            countCutflowEvent("04_05SRC1N2SSLM05_LightJets.size() < 3");
            if(sum_mT > 200){
              countCutflowEvent("04_05SRC1N2SSLM06_sum_mT > 200");
              if(lep_mT2 > 80){
                countCutflowEvent("04_05SRC1N2SSLM07_lep_mT2 > 80");
                countSignalEvent("SR-C1N2SS-LM");
              }
            }
          }
        }
      }
    }
  }

  // SR-C1N2SS-HM 5
  if(MediumTauJets.size() >= 2 && SS_tau){
    countCutflowEvent("04_06SRC1N2SSHM01_MediumTauSS.size > 2");
    if(etmiss_trigger){
      countCutflowEvent("04_06SRC1N2SSHM02_etmiss_trigger");
      if(met > 150){
        countCutflowEvent("04_06SRC1N2SSHM03_met > 150");
        if(sum_mT > 450){
          countCutflowEvent("04_06SRC1N2SSHM04_sum_mT > 450");
          if(lep_mT2 > 80){
            countCutflowEvent("04_06SRC1N2SSHM05_lep_mT2 > 80");
            countSignalEvent("SR-C1N2SS-HM");
          }
        }
      }
    }
  }

  // SR-Wh-LM 5
  if(MediumTauJets.size() >= 2 && OS_tau){
    countCutflowEvent("04_07SRWhLM01_MediumTauOS.size > 2");
    if(light_leptons.size() == 1){
      countCutflowEvent("04_07SRWhLM02_light_leptons.size() == 1");
      if(dphi < 3){
        countCutflowEvent("04_07SRWhLM03_dphi < 3");
        if(mtautau > 90. && mtautau < 130.){
          countCutflowEvent("04_07SRWhLM04_90 < mtautau < 130");
          if(Wh_mT2 > 100){
            countCutflowEvent("04_07SRWhLM05_Wh_mT2 > 100");
            countSignalEvent("SR-Wh-LM");
          }
        }
      }
    }
  }

  // SR-Wh-HM 8
  if(MediumTauJets.size() >= 2 && OS_tau){
    countCutflowEvent("04_08SRWhHM01_MediumTauOS.size > 2");
    if(light_leptons.size() == 1){
      countCutflowEvent("04_08SRWhHM02_light_leptons.size() == 1");
      if(dphi < 3){
        countCutflowEvent("04_08SRWhHM03_dphi < 3");
        if(dR < 2.2){
          countCutflowEvent("04_08SRWhHM04_dR < 2.2");
          if(mtautau > 80 && mtautau < 160){
            countCutflowEvent("04_08SRWhHM05_80 < mtautau < 160");
            if(Wh_mT2 > 80){
              countCutflowEvent("04_08SRWhHM06_Wh_mT2 > 80");
              if(mT_lep > 80){
                countCutflowEvent("04_08SRWhHM07_mT_lep > 80");
                if(sum_mT_Wh > 450){
                  countCutflowEvent("04_08SRWhHM08_sum_mT_Wh > 450");
                  countSignalEvent("SR-Wh-HM");
                }
              }
            }
          }
        }
      }
    }
  }
}

void Atlas_conf_2022_042::finalize() {
  // Whatever should be done after the run goes here
}       

std::vector<Electron*> Atlas_conf_2022_042::overlapRemoval_electrons_jets(std::vector<Electron*> electrons, std::vector<Jet*> jets){
  std::vector<bool> del_electrons;
  //fill bool vectors:
  for(int i=0;i<electrons.size();i++){
    del_electrons.push_back(false);
  }

  for(int i=0;i<electrons.size();i++){
    for(int j=0;j<jets.size();j++){

      double dR=electrons[i]->P4().DeltaR(jets[j]->P4());
      if (dR > 0.2 and dR < 0.4) {
        del_electrons[i]=true; //discard electron, keep jet
      }
    }
  }
  //delete the corresponding entries
  std::vector<Electron*> filtered_electrons;
  for(int i=0;i<electrons.size();i++){
    if(del_electrons[i]==false)
      filtered_electrons.push_back(electrons[i]);
  }
  return filtered_electrons;
}
