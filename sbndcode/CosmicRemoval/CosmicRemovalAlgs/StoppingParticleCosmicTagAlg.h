#ifndef STOPPINGPARTICLECOSMICTAGALG_H_SEEN
#define STOPPINGPARTICLECOSMICTAGALG_H_SEEN


///////////////////////////////////////////////
// StoppingParticleCosmicTagAlg.h
//
// Functions for fiducial volume cosmic tagger
// T Brooks (tbrooks@fnal.gov), November 2018
///////////////////////////////////////////////

#include "sbndcode/CosmicRemoval/CosmicRemovalUtils/CosmicRemovalUtils.h"

// framework
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "fhiclcpp/ParameterSet.h" 
#include "art/Framework/Principal/Handle.h" 
#include "canvas/Persistency/Common/Ptr.h" 
#include "canvas/Persistency/Common/PtrVector.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art/Framework/Services/Optional/TFileService.h" 
#include "art/Framework/Services/Optional/TFileDirectory.h" 
#include "messagefacility/MessageLogger/MessageLogger.h" 

// LArSoft
#include "lardataobj/RecoBase/Track.h"

// Utility libraries
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/Atom.h"

#include "TGraph.h"
#include "TF1.h"

// c++
#include <vector>


namespace sbnd{

  class StoppingParticleCosmicTagAlg {
  public:

    struct Config {
      using Name = fhicl::Name;
      using Comment = fhicl::Comment;

      fhicl::Atom<double> Fiducial {
        Name("Fiducial"),
        Comment("Fiducial volume cut (cm) to decide if track exits")
      };

      fhicl::Atom<double> ResRangeMin {
        Name("ResRangeMin"),
        Comment("Minumum residual range (cm) to fit")
      };

      fhicl::Atom<double> ResRangeMax {
        Name("ResRangeMax"),
        Comment("Maximum residual range (cm) to fit")
      };

      fhicl::Atom<double> DEdxMax {
        Name("DEdxMax"),
        Comment("Maximum dE/dx (MeV/cm) to fit")
      };

      fhicl::Atom<double> StoppingChi2Limit {
        Name("StoppingChi2Limit"),
        Comment("Limit of pol/exp chi2 ratio to cut on to determine if stopping")
      };

    };

    StoppingParticleCosmicTagAlg(const Config& config);

    StoppingParticleCosmicTagAlg(const fhicl::ParameterSet& pset) :
      StoppingParticleCosmicTagAlg(fhicl::Table<Config>(pset, {})()) {}

    StoppingParticleCosmicTagAlg();

    ~StoppingParticleCosmicTagAlg();

    void reconfigure(const Config& config);

    bool StoppingEnd(geo::Point_t end, std::vector<art::Ptr<anab::Calorimetry>> calos);

    bool StoppingParticleCosmicTag(recob::Track track, std::vector<art::Ptr<anab::Calorimetry>> calos);

    bool StoppingParticleCosmicTag(recob::Track track, recob::Track track2, std::vector<art::Ptr<anab::Calorimetry>> calos, std::vector<art::Ptr<anab::Calorimetry>> calos2);

  private:

    double fFiducial;
    double fResRangeMin;
    double fResRangeMax;
    double fDEdxMax;
    double fStoppingChi2Limit;

  };

}

#endif
