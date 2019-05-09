#include "CRTGeoAlg.h"

namespace sbnd{


CRTGeoAlg::CRTGeoAlg(){

  fGeometryService = lar::providerFrom<geo::Geometry>();
  fAuxDetGeo = &(*fAuxDetGeoService);
  fAuxDetGeoCore = fAuxDetGeo->GetProviderPtr();

  // Keep track of whichobjects we've recorded
  std::vector<std::string> usedTaggers;
  std::vector<std::string> usedModules;
  std::vector<std::string> usedStrips;

  // Get the auxdets (strip arrays for some reason)
  const std::vector<geo::AuxDetGeo*> auxDets = fAuxDetGeoCore->AuxDetGeoVec();

  // Loop over them
  for(auto const auxDet : auxDets){

    // Loop over the strips in the arrays
    for(size_t i = 0; i < auxDet->NSensitiveVolume(); i++){

      // Get the geometry object for the strip
      geo::AuxDetSensitiveGeo const& auxDetSensitive = auxDet->SensitiveVolume(i);
      std::set<std::string> volNames = {auxDetSensitive.TotalVolume()->GetName()};
      std::vector<std::vector<TGeoNode const*>> paths = fGeometryService->FindAllVolumePaths(volNames);

      // Build up a path that ROOT understands
      std::string path = "";
      for (size_t inode = 0; inode < paths.at(0).size(); inode++){
        path += paths.at(0).at(inode)->GetName();
        if(inode < paths.at(0).size() - 1){
          path += "/";
        }
      }

      // Access the path
      TGeoManager* manager = fGeometryService->ROOTGeoManager();
      manager->cd(path.c_str());

      // Get all of the things we're intersted in in the path
      TGeoNode* nodeStrip = manager->GetCurrentNode();
      TGeoNode* nodeModule = manager->GetMother(2);
      TGeoNode* nodeTagger = manager->GetMother(3);
      TGeoNode* nodeDet = manager->GetMother(4);

      // Fill the tagger information
      std::string taggerName = nodeTagger->GetName();
      if(std::find(usedTaggers.begin(), usedTaggers.end(), taggerName) == usedTaggers.end()){
        usedTaggers.push_back(taggerName);

        // Get the limits in local coords
        double halfWidth = ((TGeoBBox*)nodeTagger->GetVolume()->GetShape())->GetDX();
        double halfHeight = ((TGeoBBox*)nodeTagger->GetVolume()->GetShape())->GetDY();
        double halfLength = ((TGeoBBox*)nodeTagger->GetVolume()->GetShape())->GetDZ()/2;

        // Transform those limits to world coords
        double limits[3] = {halfWidth, halfHeight, halfLength};
        double limitsDet[3];
        nodeTagger->LocalToMaster(limits, limitsDet);
        double limitsWorld[3];
        nodeDet->LocalToMaster(limitsDet, limitsWorld);

        double limits2[3] = {-halfWidth, -halfHeight, -halfLength};
        double limitsDet2[3];
        nodeTagger->LocalToMaster(limits2, limitsDet2);
        double limitsWorld2[3];
        nodeDet->LocalToMaster(limitsDet2, limitsWorld2);

        // Create a tagger geometry object
        CRTTaggerGeo tagger = {};
        tagger.name = taggerName;
        tagger.minX = std::min(limitsWorld[0], limitsWorld2[0]);
        tagger.maxX = std::max(limitsWorld[0], limitsWorld2[0]);
        tagger.minY = std::min(limitsWorld[1], limitsWorld2[1]);
        tagger.maxY = std::max(limitsWorld[1], limitsWorld2[1]);
        tagger.minZ = std::min(limitsWorld[2], limitsWorld2[2]);
        tagger.maxZ = std::max(limitsWorld[2], limitsWorld2[2]);
        tagger.null = false;
        fTaggers[taggerName] = tagger;
      }

      // Fill the module information
      std::string moduleName = nodeModule->GetName();
      if(std::find(usedModules.begin(), usedModules.end(), moduleName) == usedModules.end()){
        usedModules.push_back(moduleName);

        // Technically the auxdet is the strip array but this is basically the same as the module
        // Get the limits in local coordinates
        double halfWidth = auxDet->HalfWidth1();
        double halfHeight = auxDet->HalfHeight();
        double halfLength = auxDet->Length()/2;

        // Transform to world coordinates
        double limits[3] = {halfWidth, halfHeight, halfLength};
        double limitsWorld[3];
        auxDet->LocalToWorld(limits, limitsWorld);

        double limits2[3] = {-halfWidth, -halfHeight, -halfLength};
        double limitsWorld2[3];
        auxDet->LocalToWorld(limits2, limitsWorld2);

        // Determine which plane the module is in in the tagger (XY configuration)
        double origin[3] = {0, 0, 0};
        double modulePosMother[3];
        nodeModule->LocalToMaster(origin, modulePosMother);
        size_t planeID = (modulePosMother[2] > 0);

        // Create a module geometry object
        CRTModuleGeo module = {};
        module.name = moduleName;
        module.minX = std::min(limitsWorld[0], limitsWorld2[0]);
        module.maxX = std::max(limitsWorld[0], limitsWorld2[0]);
        module.minY = std::min(limitsWorld[1], limitsWorld2[1]);
        module.maxY = std::max(limitsWorld[1], limitsWorld2[1]);
        module.minZ = std::min(limitsWorld[2], limitsWorld2[2]);
        module.maxZ = std::max(limitsWorld[2], limitsWorld2[2]);
        module.null = false;
        module.planeID = planeID;
        module.tagger = taggerName;
        fModules[moduleName] = module;
      }

      // Fill the strip information
      std::string stripName = nodeStrip->GetName();
      if(std::find(usedStrips.begin(), usedStrips.end(), stripName) == usedStrips.end()){
        usedStrips.push_back(stripName);

        // Get the limits in local coordinates
        double halfWidth = auxDetSensitive.HalfWidth1();
        double halfHeight = auxDetSensitive.HalfHeight();
        double halfLength = auxDetSensitive.Length()/2;

        // Transform to world coordinates
        double limits[3] = {halfWidth, halfHeight, halfLength};
        double limitsWorld[3];
        auxDetSensitive.LocalToWorld(limits, limitsWorld);

        double limits2[3] = {-halfWidth, -halfHeight, -halfLength};
        double limitsWorld2[3];
        auxDetSensitive.LocalToWorld(limits2, limitsWorld2);

        // Create a strip geometry object
        CRTStripGeo strip = {};
        strip.name = stripName;
        strip.minX = std::min(limitsWorld[0], limitsWorld2[0]);
        strip.maxX = std::max(limitsWorld[0], limitsWorld2[0]);
        strip.minY = std::min(limitsWorld[1], limitsWorld2[1]);
        strip.maxY = std::max(limitsWorld[1], limitsWorld2[1]);
        strip.minZ = std::min(limitsWorld[2], limitsWorld2[2]);
        strip.maxZ = std::max(limitsWorld[2], limitsWorld2[2]);
        strip.null = false;
        strip.module = moduleName;
        fStrips[stripName] = strip;

        // Add the strip to the relevant module
        fModules[moduleName].strips[stripName] = strip;
      }
    }
  }

  // Need to fill the tagger modules after all the strip associations have been made
  // Loop over the modules
  for(auto const& module : fModules){
    // Fill the tagger map
    std::string taggerName = module.second.tagger;
    std::string moduleName = module.second.name;
    fTaggers[taggerName].modules[moduleName] = module.second;
  }

}


CRTGeoAlg::~CRTGeoAlg(){

}

size_t CRTGeoAlg::NumTaggers() const{
  return fTaggers.size();
}


size_t CRTGeoAlg::NumModules() const{
  return fModules.size();
}

size_t CRTGeoAlg::NumModules(std::string taggerName) const{
  CRTTaggerGeo tagger = GetTagger(taggerName);
  if(!tagger.null) return tagger.modules.size();
  return 0;
}

size_t CRTGeoAlg::NumModules(size_t tagger_i) const{
  CRTTaggerGeo tagger = GetTagger(tagger_i);
  if(!tagger.null) return tagger.modules.size();
  return 0;
}


size_t CRTGeoAlg::NumStrips() const{
  return fStrips.size();
}

size_t CRTGeoAlg::NumStrips(std::string moduleName) const{
  CRTModuleGeo module = GetModule(moduleName);
  if(!module.null) return module.strips.size();
  return 0;
}

size_t CRTGeoAlg::NumStrips(size_t module_i) const{
  CRTModuleGeo module = GetModule(module_i);
  if(!module.null) return module.strips.size();
  return 0;
}

size_t CRTGeoAlg::NumStrips(size_t tagger_i, size_t module_i) const{
  CRTModuleGeo module = GetModule(tagger_i, module_i);
  if(!module.null) return module.strips.size();
  return 0;
}

CRTTaggerGeo CRTGeoAlg::GetTagger(std::string taggerName) const{
  for(auto const& tagger : fTaggers){
    if(taggerName == tagger.first) return tagger.second;
  }
  CRTTaggerGeo nullTagger = {};
  nullTagger.null = true;
  return nullTagger;
}

CRTTaggerGeo CRTGeoAlg::GetTagger(size_t tagger_i) const{
  size_t index = 0;
  for(auto const& tagger : fTaggers){
    if(tagger_i == index) return tagger.second;
    index++;
  }
  CRTTaggerGeo nullTagger = {};
  nullTagger.null = true;
  return nullTagger;
}


CRTModuleGeo CRTGeoAlg::GetModule(std::string moduleName) const{
  for(auto const& module : fModules){
    if(moduleName == module.first) return module.second;
  }
  CRTModuleGeo nullModule = {};
  nullModule.null = true;
  return nullModule;
}

CRTModuleGeo CRTGeoAlg::GetModule(size_t module_i) const{
  size_t index = 0;
  for(auto const& module : fModules){
    if(module_i == index) return module.second;
    index++;
  }
  CRTModuleGeo nullModule = {};
  nullModule.null = true;
  return nullModule;
}

CRTModuleGeo CRTGeoAlg::GetModule(size_t tagger_i, size_t module_i) const{
  CRTModuleGeo nullModule = {};
  nullModule.null = true;

  CRTTaggerGeo tagger = GetTagger(tagger_i);
  if(tagger.null) return nullModule;

  size_t index = 0;
  for(auto const& module : tagger.modules){
    if(module_i == index) return module.second;
    index++;
  }
  return nullModule;
}


CRTStripGeo CRTGeoAlg::GetStrip(std::string stripName) const{
  for(auto const& strip : fStrips){
    if(stripName == strip.first) return strip.second;
  }
  CRTStripGeo nullStrip = {};
  nullStrip.null = true;
  return nullStrip;
}

CRTStripGeo CRTGeoAlg::GetStrip(size_t strip_i) const{
  size_t index = 0;
  for(auto const& strip : fStrips){
    if(strip_i == index) return strip.second;
    index++;
  }
  CRTStripGeo nullStrip = {};
  nullStrip.null = true;
  return nullStrip;
}

CRTStripGeo CRTGeoAlg::GetStrip(size_t module_i, size_t strip_i) const{
  CRTStripGeo nullStrip = {};
  nullStrip.null = true;

  CRTModuleGeo module = GetModule(module_i);
  if(module.null) return nullStrip;

  size_t index = 0;
  for(auto const& strip : module.strips){
    if(strip_i == index) return strip.second;
    index++;
  }
  return nullStrip;
}

CRTStripGeo CRTGeoAlg::GetStrip(size_t tagger_i, size_t module_i, size_t strip_i) const{
  CRTStripGeo nullStrip = {};
  nullStrip.null = true;

  CRTModuleGeo module = GetModule(tagger_i, module_i);
  if(module.null) return nullStrip;

  size_t index = 0;
  for(auto const& strip : module.strips){
    if(strip_i == index) return strip.second;
    index++;
  }
  return nullStrip;
}

bool CRTGeoAlg::IsInsideTagger(std::string taggerName, geo::Point_t point){
  CRTTaggerGeo tagger = GetTagger(taggerName);
  return IsInsideTagger(tagger, point);
}

bool CRTGeoAlg::IsInsideTagger(const CRTTaggerGeo& tagger, geo::Point_t point){
  if(tagger.null) return false;
  double x = point.X();
  double y = point.Y();
  double z = point.Z();
  double xmin = tagger.minX;
  double ymin = tagger.minY;
  double zmin = tagger.minZ;
  double xmax = tagger.maxX;
  double ymax = tagger.maxY;
  double zmax = tagger.maxZ;
  if(x > xmin && x < xmax && y > ymin && y < ymax && z > zmin && z < zmax) return true;
  return false;
}

bool CRTGeoAlg::IsInsideModule(std::string moduleName, geo::Point_t point){
  CRTModuleGeo module = GetModule(moduleName);
  return IsInsideModule(module, point);
}

bool CRTGeoAlg::IsInsideModule(const CRTModuleGeo& module, geo::Point_t point){
  if(module.null) return false;
  double x = point.X();
  double y = point.Y();
  double z = point.Z();
  double xmin = module.minX;
  double ymin = module.minY;
  double zmin = module.minZ;
  double xmax = module.maxX;
  double ymax = module.maxY;
  double zmax = module.maxZ;
  // Make the width limits a bit more generous to account for steps in true trajectory
  if(std::abs(xmax-xmin)==1){ xmin-=1; xmax+=1; }
  if(std::abs(ymax-ymin)==1){ ymin-=1; ymax+=1; }
  if(std::abs(zmax-zmin)==1){ zmin-=1; zmax+=1; }
  if(x > xmin && x < xmax && y > ymin && y < ymax && z > zmin && z < zmax) return true;
  return false;
}

bool CRTGeoAlg::IsInsideStrip(std::string stripName, geo::Point_t point){
  CRTStripGeo strip = GetStrip(stripName);
  return IsInsideStrip(strip, point);
}

bool CRTGeoAlg::IsInsideStrip(const CRTStripGeo& strip, geo::Point_t point){
  if(strip.null) return false;
  double x = point.X();
  double y = point.Y();
  double z = point.Z();
  double xmin = strip.minX;
  double ymin = strip.minY;
  double zmin = strip.minZ;
  double xmax = strip.maxX;
  double ymax = strip.maxY;
  double zmax = strip.maxZ;
  if(x > xmin && x < xmax && y > ymin && y < ymax && z > zmin && z < zmax) return true;
  return false;
}

geo::Point_t CRTGeoAlg::TaggerCrossingPoint(std::string taggerName, simb::MCParticle particle){
  geo::Point_t entry {-99999, -99999, -99999};
  geo::Point_t exit {-99999, -99999, -99999};
  CRTTaggerGeo tagger = GetTagger(taggerName);

  bool first = true;
  for(size_t i = 0; i < particle.NumberTrajectoryPoints(); i++){
    geo::Point_t point {particle.Vx(i), particle.Vy(i), particle.Vz(i)};
    if(!IsInsideTagger(tagger, point)) continue;
    if(first){
      entry = point;
      first = false;
    }
    exit = point;
  }

  geo::Point_t cross {(entry.X()+exit.X())/2., (entry.Y()+exit.Y())/2., (entry.Z()+exit.Z())/2};
  return cross;
}

geo::Point_t CRTGeoAlg::ModuleCrossingPoint(std::string moduleName, simb::MCParticle particle){
  geo::Point_t entry {-99999, -99999, -99999};
  geo::Point_t exit {-99999, -99999, -99999};
  CRTModuleGeo module = GetModule(moduleName);

  bool first = true;
  for(size_t i = 0; i < particle.NumberTrajectoryPoints(); i++){
    geo::Point_t point {particle.Vx(i), particle.Vy(i), particle.Vz(i)};
    if(!IsInsideModule(module, point)) continue;
    if(first){
      entry = point;
      first = false;
    }
    exit = point;
  }

  geo::Point_t cross {(entry.X()+exit.X())/2., (entry.Y()+exit.Y())/2., (entry.Z()+exit.Z())/2};
  return cross;
}

geo::Point_t CRTGeoAlg::StripCrossingPoint(std::string stripName, simb::MCParticle particle){
  geo::Point_t entry {-99999, -99999, -99999};
  geo::Point_t exit {-99999, -99999, -99999};
  CRTStripGeo strip = GetStrip(stripName);

  bool first = true;
  for(size_t i = 0; i < particle.NumberTrajectoryPoints(); i++){
    geo::Point_t point {particle.Vx(i), particle.Vy(i), particle.Vz(i)};
    if(!IsInsideStrip(strip, point)) continue;
    if(first){
      entry = point;
      first = false;
    }
    exit = point;
  }

  geo::Point_t cross {(entry.X()+exit.X())/2., (entry.Y()+exit.Y())/2., (entry.Z()+exit.Z())/2};
  return cross;
}

std::vector<double> CRTGeoAlg::CRTLimits(){
  std::vector<double> limits;

  std::vector<double> minXs;
  std::vector<double> minYs;
  std::vector<double> minZs;
  std::vector<double> maxXs;
  std::vector<double> maxYs;
  std::vector<double> maxZs;
  for(auto const& tagger : fTaggers){
    minXs.push_back(tagger.second.minX);
    minYs.push_back(tagger.second.minY);
    minZs.push_back(tagger.second.minZ);
    maxXs.push_back(tagger.second.maxX);
    maxYs.push_back(tagger.second.maxY);
    maxZs.push_back(tagger.second.maxZ);
  }
  limits.push_back(*std::min_element(minXs.begin(), minXs.end()));
  limits.push_back(*std::min_element(minYs.begin(), minYs.end()));
  limits.push_back(*std::min_element(minZs.begin(), minZs.end()));
  limits.push_back(*std::max_element(maxXs.begin(), maxXs.end()));
  limits.push_back(*std::max_element(maxYs.begin(), maxYs.end()));
  limits.push_back(*std::max_element(maxZs.begin(), maxZs.end()));

  return limits;
}

bool CRTGeoAlg::EntersVolume(simb::MCParticle particle){
  bool enters = false;
  bool startOutside = false;
  bool endOutside = false;
  std::vector<double> limits = CRTLimits();
  for(size_t i = 0; i < particle.NumberTrajectoryPoints(); i++){
    double x = particle.Vx(i); 
    double y = particle.Vy(i);
    double z = particle.Vz(i);
    if(x > limits[0] && y > limits[1] && z > limits[2] && x < limits[3] && y < limits[4] && z < limits[5]){
      enters = true;
    }
    else if(i == 0) startOutside = true;
    else if(i == particle.NumberTrajectoryPoints()-1) endOutside = true;
  }
  if(enters && (startOutside || endOutside)) return true;
  return false;
}

bool CRTGeoAlg::CrossesVolume(simb::MCParticle particle){
  bool enters = false;
  bool startOutside = false;
  bool endOutside = false;
  std::vector<double> limits = CRTLimits();
  for(size_t i = 0; i < particle.NumberTrajectoryPoints(); i++){
    double x = particle.Vx(i); 
    double y = particle.Vy(i);
    double z = particle.Vz(i);
    if(x > limits[0] && y > limits[1] && z > limits[2] && x < limits[3] && y < limits[4] && z < limits[5]){
      enters = true;
    }
    else if(i == 0) startOutside = true;
    else if(i == particle.NumberTrajectoryPoints()-1) endOutside = true;
  }
  if(startOutside && enters && endOutside) return true;
  return false;
}

bool CRTGeoAlg::CheckOverlap(const CRTModuleGeo& module1, const CRTModuleGeo& module2){
  // Get the minimum and maximum X, Y, Z coordinates
  double minX = std::max(module1.minX, module2.minX);
  double maxX = std::min(module1.maxX, module2.maxY);
  double minY = std::max(module1.minY, module2.minY);
  double maxY = std::min(module1.maxY, module2.maxY);
  double minZ = std::max(module1.minZ, module2.minZ);
  double maxZ = std::min(module1.maxZ, module2.maxZ);

  // If the two strips overlap in 2 dimensions then return true
  if ((minX<maxX && minY<maxY) || (minX<maxX && minZ<maxZ) || (minY<maxY && minZ<maxZ)) return true;
  // Otherwise return a "null" value
  return false;
}

bool CRTGeoAlg::HasOverlap(const CRTModuleGeo& module){
  // Record plane of mother module
  size_t planeID = module.planeID;
  // Get mother tagger of module
  std::string taggerName = module.tagger;
  // Loop over other modules in tagger
  for(auto const& module2 : fTaggers[taggerName].modules){
    // If in other plane loop over strips
    if(module2.second.planeID == planeID) continue;
    // Check for overlaps
    if(CheckOverlap(module, module2.second)) return true;
  }
  return false;
}

bool CRTGeoAlg::ValidCrossingPoint(std::string taggerName, simb::MCParticle particle){

  // Get all the crossed strips in the tagger
  std::vector<std::string> crossedModules;
  for(auto const& module : fTaggers[taggerName].modules){
    geo::Point_t crossPoint = ModuleCrossingPoint(module.second.name, particle);
    if(crossPoint.X() != -99999) crossedModules.push_back(module.second.name);
  }

  // Check if the strip has a possible overlap, return true if not
  for(size_t i = 0; i < crossedModules.size(); i++){
    if(!HasOverlap(fModules[crossedModules[i]])) return true;
    // Check if any of the crossed strips overlap, return true if they do
    for(size_t j = i; j < crossedModules.size(); j++){
      if(CheckOverlap(fModules[crossedModules[i]], fModules[crossedModules[j]])) return true;
    }
  }
  return false;
}

}
