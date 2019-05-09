#ifndef CRTGEOALG_H_SEEN
#define CRTGEOALG_H_SEEN


///////////////////////////////////////////////
// CRTGeoAlg.h
//
// Wrapper for some awkward CRT geometry things
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
#include "art/Framework/Services/Optional/TFileService.h" 
#include "art/Framework/Services/Optional/TFileDirectory.h" 
#include "messagefacility/MessageLogger/MessageLogger.h" 

// LArSoft
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/GeometryCore.h"
#include "lardataobj/Simulation/AuxDetSimChannel.h"
#include "larcore/Geometry/AuxDetGeometry.h"
#include "nusimdata/SimulationBase/MCParticle.h"

// c++
#include <vector>

// ROOT
#include "TVector3.h"
#include "TGeoManager.h"

namespace sbnd{

  struct CRTStripGeo{
    std::string name;
    double minX;
    double maxX;
    double minY;
    double maxY;
    double minZ;
    double maxZ;
    std::string module;
    bool null;
  };

  struct CRTModuleGeo{
    std::string name;
    double minX;
    double maxX;
    double minY;
    double maxY;
    double minZ;
    double maxZ;
    size_t planeID;
    std::string tagger;
    std::map<std::string, CRTStripGeo> strips;
    bool null;
  };

  struct CRTTaggerGeo{
    std::string name;
    double minX;
    double maxX;
    double minY;
    double maxY;
    double minZ;
    double maxZ;
    // change to ref?
    std::map<std::string, CRTModuleGeo> modules;
    bool null;
  };


  class CRTGeoAlg {
  public:

    CRTGeoAlg();

    ~CRTGeoAlg();

    // Return the volume enclosed by the whole CRT system
    std::vector<double> CRTLimits();

    // Get the number of taggers in the geometry
    size_t NumTaggers() const;

    // Get the total number of modules in the geometry
    size_t NumModules() const;
    // Get the number of modules in a tagger by name
    size_t NumModules(std::string taggerName) const;
    // Get the number of modules in a tagger by index
    size_t NumModules(size_t tagger_i) const;

    // Get the total number of strips in the geometry
    size_t NumStrips() const;
    // Get the number of strips in module by name
    size_t NumStrips(std::string moduleName) const;
    // Get the number of strips in  module by global index
    size_t NumStrips(size_t module_i) const;
    // Get the number of strips in module by tagger index and local module index
    size_t NumStrips(size_t tagger_i, size_t module_i) const;

    // Get the tagger geometry object by name
    CRTTaggerGeo GetTagger(std::string taggerName) const;
    // Get the tagger geometry object by index
    CRTTaggerGeo GetTagger(size_t tagger_i) const;

    // Get the module geometry object by name
    CRTModuleGeo GetModule(std::string moduleName) const;
    // Get the module geometry object by global index
    CRTModuleGeo GetModule(size_t module_i) const;
    // Get the module geometry object by tagger index and local module index
    CRTModuleGeo GetModule(size_t tagger_i, size_t module_i) const;

    // Get the strip geometry object by name
    CRTStripGeo GetStrip(std::string stripName) const;
    // Get the strip geometry object by global index
    CRTStripGeo GetStrip(size_t strip_i) const;
    // Get the strip geometry object by global module index and local strip index
    CRTStripGeo GetStrip(size_t module_i, size_t strip_i) const;
    // Get the strip geometry object by tagger index, local module index and local strip index
    CRTStripGeo GetStrip(size_t tagger_i, size_t module_i, size_t strip_i) const;

    // Determine if a point is inside a tagger by name
    bool IsInsideTagger(std::string taggerName, geo::Point_t point);
    bool IsInsideTagger(const CRTTaggerGeo& tagger, geo::Point_t point);
    // Determine if a point is inside a module by name
    bool IsInsideModule(std::string moduleName, geo::Point_t point);
    bool IsInsideModule(const CRTModuleGeo& module, geo::Point_t point);
    // Determine if a point is inside a strip by name
    bool IsInsideStrip(std::string stripName, geo::Point_t point);
    bool IsInsideStrip(const CRTStripGeo& strip, geo::Point_t point);

    // Check if two strips overlap in 2D
    bool CheckOverlap(const CRTModuleGeo& module1, const CRTModuleGeo& module2);
    // Check is a strip overlaps with a strip in a perpendicular module
    bool HasOverlap(const CRTModuleGeo& module);

    // FIXME Truth stuff probably shouldn't be here!
    // Find the average of the tagger entry and exit points of a true particle trajectory
    geo::Point_t TaggerCrossingPoint(std::string taggerName, simb::MCParticle particle);
    // Find the average of the module entry and exit points of a true particle trajectory
    geo::Point_t ModuleCrossingPoint(std::string moduleName, simb::MCParticle particle);
    // Find the average of the strip entry and exit points of a true particle trajectory
    geo::Point_t StripCrossingPoint(std::string stripName, simb::MCParticle particle);

    // Check if a particle enters the CRT volume
    bool EntersVolume(simb::MCParticle particle);
    // Check if a particle crosses the CRT volume
    bool CrossesVolume(simb::MCParticle particle);

    // Determine if a particle would be able to produce a hit in a tagger
    bool ValidCrossingPoint(std::string taggerName, simb::MCParticle particle);


  private:

    std::map<std::string, CRTTaggerGeo> fTaggers;
    std::map<std::string, CRTModuleGeo> fModules;
    std::map<std::string, CRTStripGeo> fStrips;

    geo::GeometryCore const* fGeometryService;
    art::ServiceHandle<geo::AuxDetGeometry> fAuxDetGeoService;
    const geo::AuxDetGeometry* fAuxDetGeo;
    const geo::AuxDetGeometryCore* fAuxDetGeoCore;

  };

}

#endif
