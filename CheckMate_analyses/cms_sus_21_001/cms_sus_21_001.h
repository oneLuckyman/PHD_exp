#ifndef CMS_SUS_21_001_H_
#define CMS_SUS_21_001_H_
// AUTHOR: Liangliang Shang
//  EMAIL: shangliangliang@htu.edu.cn
#include "AnalysisBase.h"

class Cms_sus_21_001 : public AnalysisBase {
  public:
    Cms_sus_21_001() : AnalysisBase()  {}               
    ~Cms_sus_21_001() {}
  
    void initialize();
    void analyze();        
    void finalize();

  private:
};

#endif
