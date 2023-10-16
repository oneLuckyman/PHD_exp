#ifndef ATLAS_2108_07586_H_
#define ATLAS_2108_07586_H_
// AUTHOR: jxl
//  EMAIL: 1111
#include "AnalysisBase.h"

class Atlas_2108_07586 : public AnalysisBase {
  public:
    Atlas_2108_07586() : AnalysisBase()  {}               
    ~Atlas_2108_07586() {}
  
    void initialize();
    void analyze();        
    void finalize();

  private:
    // void recluster_large_jets();
};

#endif
