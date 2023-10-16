#ifndef ATLAS_CONF_2022_042_H_
#define ATLAS_CONF_2022_042_H_
// AUTHOR: Liangliang Shang
//  EMAIL: shangliangliang@htu.edu.cn
#include "AnalysisBase.h"

class Atlas_conf_2022_042 : public AnalysisBase {
  public:
    Atlas_conf_2022_042() : AnalysisBase()  {}               
    ~Atlas_conf_2022_042() {}
  
    void initialize();
    void analyze();        
    void finalize();

  private:
    std::vector<Electron*> overlapRemoval_electrons_jets(std::vector<Electron*> electrons, std::vector<Jet*> jets);
};

#endif
