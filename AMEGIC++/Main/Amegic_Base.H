#ifndef AMEGIC_Main_Amegic_Base_H
#define AMEGIC_Main_Amegic_Base_H

#include "AMEGIC++/Main/Process_Tags.H"
#include "PHASIC++/Process/Process_Info.H"


namespace AMEGIC {

  class Amegic_Base {
  protected:
    Process_Tags *p_pinfo;
    size_t m_ntchanmin, m_ntchanmax;
  public:
    static int s_partcommit;
    Amegic_Base();
    virtual ~Amegic_Base();
    virtual int PerformTests() = 0;
    virtual bool NewLibs() = 0;
    virtual void Minimize() = 0;

    inline void SetNTchanmin(const size_t &ntch) { m_ntchanmin=ntch; }
    inline void SetNTchanmax(const size_t &ntch) { m_ntchanmax=ntch; }

    inline size_t NTchanMin() const  { return m_ntchanmin; }
    inline size_t NTchanMax() const  { return m_ntchanmax; }

    Pol_Info ExtractPolInfo(const PHASIC::Subprocess_Info &spi);

    void TranslateDecay(Process_Tags &info,const PHASIC::Subprocess_Info &spi);
    Process_Tags *Translate(const PHASIC::Process_Info &pi);
  };
}

#endif
