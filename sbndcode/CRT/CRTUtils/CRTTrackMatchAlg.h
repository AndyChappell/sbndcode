#ifndef CRTTRACKMATCHALG_H_SEEN
#define CRTTRACKMATCHALG_H_SEEN


///////////////////////////////////////////////
// CRTTrackMatchAlg.h
//
// Functions for CRT TPC track matching
// T Brooks (tbrooks@fnal.gov), November 2018
///////////////////////////////////////////////

// framework
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "fhiclcpp/ParameterSet.h" 
#include "art/Framework/Principal/Handle.h" 
#include "canvas/Persistency/Common/Ptr.h" 
#include "canvas/Persistency/Common/PtrVector.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "messagefacility/MessageLogger/MessageLogger.h" 
#include "canvas/Persistency/Common/FindManyP.h"

// LArSoft
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Track.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/GeometryCore.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"

// Utility libraries
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/Atom.h"
#include "cetlib/pow.h" // cet::sum_of_squares()

#include "sbndcode/CRT/CRTProducts/CRTTrack.hh"
#include "sbndcode/CRT/CRTUtils/CRTBackTracker.h"
#include "sbndcode/Geometry/GeometryWrappers/TPCGeoAlg.h"

// c++
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <cmath> 
#include <memory>

// ROOT
#include "TVector3.h"
#include "TGeoManager.h"


namespace sbnd{
/*
  struct RecoCRTTrack{
    int crtID;
    int tpc;
    TVector3 start; //[cm]
    TVector3 end; //[cm]
    double trueTime; // [us]
    bool complete;
  };
*/
  class CRTTrackMatchAlg {
  public:

    struct Config {
      using Name = fhicl::Name;
      using Comment = fhicl::Comment;

      fhicl::Atom<double> MaxAngleDiff {
        Name("MaxAngleDiff"),
        Comment("")
      };

      fhicl::Atom<double> MaxDistance {
        Name("MaxDistance"),
        Comment("")
      };

      fhicl::Atom<bool> StitchAcrossCPA {
        Name("StitchAcrossCPA"),
        Comment("")
      };

      fhicl::Atom<art::InputTag> TPCTrackLabel {
        Name("TPCTrackLabel"),
        Comment("")
      };

    };

    CRTTrackMatchAlg(const Config& config);

    CRTTrackMatchAlg(const fhicl::ParameterSet& pset) :
      CRTTrackMatchAlg(fhicl::Table<Config>(pset, {})()) {}

    CRTTrackMatchAlg();

    ~CRTTrackMatchAlg();

    void reconfigure(const Config& config);

    // Calculate intersection between CRT track and TPC
    std::pair<TVector3, TVector3> TpcIntersection(const geo::TPCGeo& tpcGeo, crt::CRTTrack track);
/*
    // Function to transform a CRTTrack into an expected reconstructed track
    std::vector<RecoCRTTrack> CrtToRecoTrack(crt::CRTTrack track, int id);

    // Function to shift CRTTrack in X and work out how much is reconstructed
    std::vector<RecoCRTTrack> CreateRecoCRTTrack(TVector3 start, TVector3 end, double shift, 
                                                 int tpc, int id, double time, bool complete);
*/
    // Function to calculate if a CRTTrack crosses the TPC volume
    bool CrossesTPC(crt::CRTTrack track);

    // Function to calculate if a CRTTrack crosses the TPC volume
    bool CrossesAPA(crt::CRTTrack track);
/*
    int GetMatchedCRTTrackId(recob::Track tpcTrack, std::vector<crt::CRTTrack> crtTracks, int tpc);

    double T0FromCRTTracks(recob::Track tpcTrack, std::vector<crt::CRTTrack> crtTracks, int tpc);
*/
    double T0FromCRTTracks(recob::Track tpcTrack, std::vector<crt::CRTTrack> crtTracks, const art::Event& event);

    // Find the closest valid matching CRT track ID
    int GetMatchedCRTTrackId(recob::Track tpcTrack, std::vector<crt::CRTTrack> crtTracks, const art::Event& event);

    // Get all CRT tracks that cross the right TPC within an allowed time
    std::vector<crt::CRTTrack> AllPossibleCRTTracks(recob::Track tpcTrack, 
                                                    std::vector<crt::CRTTrack> crtTracks, 
                                                    const art::Event& event); 

    // Find the closest matching crt track by angle between tracks within angle and DCA limits
    std::pair<crt::CRTTrack, double> ClosestCRTTrackByAngle(recob::Track tpcTrack, 
                                                            std::vector<crt::CRTTrack> crtTracks, 
                                                            const art::Event& event,
                                                            double minDCA = 0.); 
    // Find the closest matching crt track by average DCA between tracks within angle and DCA limits
    std::pair<crt::CRTTrack, double> ClosestCRTTrackByDCA(recob::Track tpcTrack, 
                                                          std::vector<crt::CRTTrack> crtTracks, 
                                                          const art::Event& event,
                                                          double minAngle = 0.); 

    // Calculate the angle between tracks assuming start is at the largest Y
    double AngleBetweenTracks(recob::Track tpcTrack, crt::CRTTrack crtTrack);

    // Calculate the average DCA between tracks
    double AveDCABetweenTracks(recob::Track tpcTrack, crt::CRTTrack crtTrack, double shift);
    double AveDCABetweenTracks(recob::Track tpcTrack, crt::CRTTrack crtTrack, const art::Event& event);

  private:

    geo::GeometryCore const* fGeometryService;
    detinfo::DetectorProperties const* fDetectorProperties;
    detinfo::DetectorClocks const* fDetectorClocks;

    TPCGeoAlg fTpcGeo;
    CRTBackTracker fCrtBackTrack;

    double fMaxAngleDiff;
    double fMaxDistance;
    bool fStitchAcrossCPA;

    art::InputTag fTPCTrackLabel;

  };

}

#endif
