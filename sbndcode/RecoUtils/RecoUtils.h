#ifndef FDSELECTIONUTILS_H_SEEN
#define FDSELECTIONUTILS_H_SEEN


///////////////////////////////////////////////
// RecoUtils.h
//
// A few reco utilities like truth matching
// D Brailsford (adapted from work by D Brailsford and M Wallbank), October 2017
///////////////////////////////////////////////

// framework
#include "art/Framework/Principal/Event.h"
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
#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Track.h"
//#include "lardataobj/RecoBase/Track.h"
//#include "lardataobj/RecoBase/Shower.h"
//#include "lardataobj/AnalysisBase/MVAPIDResult.h"
//#include "lardataobj/AnalysisBase/ParticleID.h"
#include "larsim/MCCheater/BackTrackerService.h"
#include "larsim/MCCheater/ParticleInventoryService.h"
#include "larcore/Geometry/Geometry.h"
namespace detinfo { class DetectorClocksData; }


// c++
#include <vector>
#include <map>

// ROOT
#include "TTree.h"


namespace RecoUtils{
  int TrueParticleID(detinfo::DetectorClocksData const& clockData, const art::Ptr<recob::Hit> hit, bool rollup_unsaved_ids=1); //Returns the geant4 ID which contributes the most to a single reco hit.  The matching method looks for true particle which deposits the most true energy in the reco hit.  If rollup_unsaved_ids is set to true, any unsaved daughter than contributed energy to the hit has its energy included in its closest ancestor that was saved.
  int TrueParticleIDFromTotalTrueEnergy(detinfo::DetectorClocksData const& clockData, const std::vector<art::Ptr<recob::Hit> >& hits, bool rollup_unsaved_ids=1); //Returns the geant4 ID which contributes the most to the vector of hits.  The matching method looks for which true particle deposits the most true energy in the reco hits
  int TrueParticleIDFromTotalRecoCharge(detinfo::DetectorClocksData const& clockData, const std::vector<art::Ptr<recob::Hit> >& hits, bool rollup_unsaved_ids=1);  //Returns the geant4 ID which contributes the most to the vector of hits.  The matching method looks for which true particle contributes the most reconstructed charge to the hit selection (the reco charge of each hit is correlated with each maximally contributing true particle and summed)
  int TrueParticleIDFromTotalRecoHits(detinfo::DetectorClocksData const& clockData, const std::vector<art::Ptr<recob::Hit> >& hits, bool rollup_unsaved_ids=1);  //Returns the geant4 ID which contributes the most to the vector of hits.  The matching method looks for which true particle maximally contributes to the most reco hits
  bool IsInsideTPC(TVector3 position, double distance_buffer); //Checks if a position is within any of the TPCs in the geometry (user can define some distance buffer from the TPC walls)
  double CalculateTrackLength(const art::Ptr<recob::Track> track); //Calculates the total length of a recob::track by summing up the distances between adjacent traj. points

  int NumberofHitsFromTrack(int TrackID, const std::vector<art::Ptr<recob::Hit> >& hits); //Returns the number of hits in the vector that are associated to the MC track.

  int NumberofPrimaryHitsFromTrack(int TrackID, const std::vector<art::Ptr<recob::Hit> >& hits); //Returns the number of hits in the vector that are associated to the MC track.

  std::map<geo::PlaneID,int> NumberofPlaneHitsFromTrack(int TrackID, const std::vector<art::Ptr<recob::Hit> >& hits); //Returns the number of hits in the vector that are ssociated to the MC trakc split into planes.

  std::map<int,std::map<geo::PlaneID,int> > NumberofPlaneHitsPerTrack(const std::vector<art::Ptr<recob::Hit> >& hits); //Returns a map of all the number of hits and the respetive track id they are associated to.


  float TrueEnergyDepositedFromMCTrack(int TrackID,const std::vector<art::Ptr<sim::SimChannel> >& simchannels); //Returns the total energy deposited from the track id given.
  std::map<geo::PlaneID,int> NumberofMCWiresHit(int TrackID,const std::vector<art::Ptr<sim::SimChannel> > & simchannels); // Returns the number of Wires that saw an energy deposit in Monte Carlo from a track.Might be useful to add an energy cut on this.

  std::map<geo::PlaneID,int> NumberofHitsThatContainEnergyDepositedByTrack(int TrackID, const std::vector<art::Ptr<recob::Hit> >& hits); //Returns the number of hits in the reconstruction that saw an energy deposition by the a track. Might be useful to add an energy cut on this.

  float TotalEnergyDepinHits(const std::vector<art::Ptr<recob::Hit> >& hits, int Plane); //Returns the amount of energy deposited in the detector (before recombination and lifetime effects) in the hits.

  float TotalEnergyDepinHitsFromTrack(const std::vector<art::Ptr<recob::Hit> >& hits, int TrackID, int Plane); //Returns the amount of energy deposited in the detector (before recombination and lifetime effects)in the hits from a given particle.

  std::map<int,float> TrueEnergyDepositedFromMCTracks(const std::vector<art::Ptr<sim::SimChannel> >& simchannels); //Returns a map of the Energies depsoited by each track id.

  std::map<int,std::map<geo::PlaneID,int> > NumberofMCWiresHitMap(const std::vector<art::Ptr<sim::SimChannel> >& simchannels); //Returns of map of the number of wires hit in each plane for each track.

  std::map<geo::PlaneID,int> NumberofHitsThatContainEnergyDepositedByTracks(std::vector<int> TrackIDs, const std::vector<art::Ptr<recob::Hit> >& hits);//Number of Hits that containt energy from the list of tracks.

  int NumberofPrimaryHitsWithAllTracks(std::vector<int>& TrackIDs, const std::vector<art::Ptr<recob::Hit> >& hits);

}

#endif
