////////////////////////////////////////////////////////////////////////
// Class:       PandoraValidation
// Plugin Type: analyzer (Unknown Unknown)
// File:        PandoraValidation_module.cc
//
// Generated at Wed Mar  5 07:49:26 2025 by Andrew Chappell using cetskelgen
// from cetlib version 3.18.02.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "canvas/Persistency/Common/FindManyP.h"
#include "lardataobj/RecoBase/Slice.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/PFParticleMetadata.h"
#include "lardataobj/RecoBase/Vertex.h"

#include <TTree.h>

namespace validation {
    class PandoraValidation;
}


class validation::PandoraValidation : public art::EDAnalyzer {
    public:
        explicit PandoraValidation(fhicl::ParameterSet const& p);
        // The compiler-generated destructor is fine for non-base
        // classes without bare pointers or other resource use.

        // Plugins should not be copied or assigned.
        PandoraValidation(PandoraValidation const&) = delete;
        PandoraValidation(PandoraValidation&&) = delete;
        PandoraValidation& operator=(PandoraValidation const&) = delete;
        PandoraValidation& operator=(PandoraValidation&&) = delete;

        // Required functions.
        void analyze(art::Event const& e) override;

        // Selected optional functions.
        void beginJob() override;
        void endJob() override;

    private:
        void Reset();

        template<typename T>
        void AddPropertyToVector(const larpandoraobj::PFParticleMetadata::PropertiesMap &properties, const std::string &key, std::vector<T> &vector);

        TTree *fTree;
        unsigned int fEventID;
        std::string fSliceLabel;
        std::string fPFParticleLabel;
        std::string fTrackScoreKey;
        bool fIsNuSlice;
        float fNuVertexX;
        float fNuVertexY;
        float fNuVertexZ;
        unsigned int fNPFParticle;
        std::vector<float> fTrackScore;
        std::vector<float> fConeChargeConcentration;
        std::vector<float> fConeChargeConicalness;
        std::vector<float> fConeChargeHaloTotRatio;
        std::vector<float> fChargeEndFrac;
        std::vector<float> fChargeFracSpread;
        std::vector<float> fLinearDiffStraightLineMean;
        std::vector<float> fLinearLength;
        std::vector<float> fLinearMaxFitGapLength;
        std::vector<float> fLinearFitRMS;
        std::vector<float> fOpenAngleDiff;
        std::vector<float> fPCASecondaryRatio;
        std::vector<float> fPCATertiaryRatio;
        std::vector<float> fVertexDistance;
        std::vector<float> fNuScore;
        std::vector<float> fNuEigenRatioInSphere;
        std::vector<float> fNuNFinalStatePfos;
        std::vector<float> fNuNHitsTotal;
        std::vector<float> fNuNSpacePointsInSphere;
        std::vector<float> fNuWeightedDirZ;
        std::vector<float> fCRFracHitsInLongestTrack;
        std::vector<float> fCRLongestTrackDeflection;
        std::vector<float> fCRLongestTrackDirY;
        std::vector<float> fCRNHitsMax;
};


validation::PandoraValidation::PandoraValidation(fhicl::ParameterSet const& p)
    : EDAnalyzer{p},
    fSliceLabel{p.get<std::string>("SliceLabel")},
    fPFParticleLabel{p.get<std::string>("PFParticleLabel")}
{
    // Call appropriate consumes<>() for any products to be retrieved by this module.
}

void validation::PandoraValidation::analyze(art::Event const& e)
{
    this->Reset();
    fEventID = e.id().event();

    art::ValidHandle<std::vector<recob::Slice>> sliceHandle{e.getValidHandle<std::vector<recob::Slice>>(fSliceLabel)};
    if (!sliceHandle.isValid())
        return;
    std::vector<art::Ptr<recob::Slice>> sliceVector;
    art::fill_ptr_vector(sliceVector, sliceHandle);
    art::FindManyP<recob::PFParticle> slicePFPAssoc(sliceHandle, e, fSliceLabel);

    art::ValidHandle<std::vector<recob::PFParticle>> pfpHandle{e.getValidHandle<std::vector<recob::PFParticle>>(fPFParticleLabel)};
    if (!pfpHandle.isValid())
        return;
    art::FindManyP<larpandoraobj::PFParticleMetadata> pfpMetadataAssoc(pfpHandle, e, fPFParticleLabel);
    art::FindManyP<recob::Vertex> pfpVertexAssoc(pfpHandle, e, fPFParticleLabel);

    for (const art::Ptr<recob::Slice> &slice : sliceVector)
    {
        std::vector<art::Ptr<recob::PFParticle>> slicePFPs{slicePFPAssoc.at(slice.key())};
        for (const art::Ptr<recob::PFParticle> &pfp : slicePFPs)
        {
            const int pdg{std::abs(pfp->PdgCode())};
            const bool isNeutrino{pdg == 12 || pdg ==14};

            if (isNeutrino)
            {
                std::vector<art::Ptr<recob::Vertex>> pfpVertices{pfpVertexAssoc.at(pfp.key())};
                if (pfpVertices.size() == 1)
                {
                    art::Ptr<recob::Vertex> pfpVertex{pfpVertices.front()};
                    fNuVertexX = pfpVertex->position().x();
                    fNuVertexY = pfpVertex->position().y();
                    fNuVertexZ = pfpVertex->position().z();
                }
                fIsNuSlice = true;
            }

            fNPFParticle = slicePFPs.size();
                auto metadataVec{pfpMetadataAssoc.at(pfp.key())};

            std::cout << "PDG code: " << pdg << std::endl;
            if (!metadataVec.empty())
            {
                auto metadata{metadataVec.front()};
                const auto &properties{metadata->GetPropertiesMap()};
                for (const auto &[key, value] : properties)
                    std::cout << "  " << key << ": " << value << std::endl;
                this->AddPropertyToVector(properties, "TrackScore", fTrackScore);
                this->AddPropertyToVector(properties, "LArConeChargeFeatureTool_Concentration", fConeChargeConcentration);
                this->AddPropertyToVector(properties, "LArConeChargeFeatureTool_Conicalness", fConeChargeConicalness);
                this->AddPropertyToVector(properties, "LArConeChargeFeatureTool_HaloTotalRatio", fConeChargeHaloTotRatio);
                this->AddPropertyToVector(properties, "LArThreeDChargeFeatureTool_EndFraction", fChargeEndFrac);
                this->AddPropertyToVector(properties, "LArThreeDChargeFeatureTool_FractionalSpread", fChargeFracSpread);
                this->AddPropertyToVector(properties, "LArThreeDLinearFitFeatureTool_DiffStraightLineM", fLinearDiffStraightLineMean);
                this->AddPropertyToVector(properties, "LArThreeDLinearFitFeatureTool_Length", fLinearLength);
                this->AddPropertyToVector(properties, "LArThreeDLinearFitFeatureTool_MaxFitGapLength", fLinearMaxFitGapLength);
                this->AddPropertyToVector(properties, "LArThreeDLinearFitFeatureTool_SlidingLinearFitRMS", fLinearFitRMS);
                this->AddPropertyToVector(properties, "LArThreeDOpeningAngleFeatureTool_AngleDiff", fOpenAngleDiff);
                this->AddPropertyToVector(properties, "LArThreeDPCAFeatureTool_SecondaryPCARatio", fPCASecondaryRatio);
                this->AddPropertyToVector(properties, "LArThreeDPCAFeatureTool_TertiaryPCARatio", fPCATertiaryRatio);
                this->AddPropertyToVector(properties, "LArThreeDVertexDistanceFeatureTool_VertexDistance", fVertexDistance);

                if (isNeutrino)
                {
                    this->AddPropertyToVector(properties, "NuScore", fNuScore);
                    this->AddPropertyToVector(properties, "NuEigenRatioInSphere", fNuEigenRatioInSphere);
                    this->AddPropertyToVector(properties, "NuNFinalStatePfos", fNuNFinalStatePfos);
                    this->AddPropertyToVector(properties, "NuNHitsTotal", fNuNHitsTotal);
                    this->AddPropertyToVector(properties, "NuNSpacePointsInSphere", fNuNSpacePointsInSphere);
                    this->AddPropertyToVector(properties, "NuWeightedDirZ", fNuWeightedDirZ);
                    this->AddPropertyToVector(properties, "CRFracHitsInLongestTrack", fCRFracHitsInLongestTrack);
                    this->AddPropertyToVector(properties, "CRLongestTrackDeflection", fCRLongestTrackDeflection);
                    this->AddPropertyToVector(properties, "CRLongestTrackDirY", fCRLongestTrackDirY);
                    this->AddPropertyToVector(properties, "CRNHitsMax", fCRNHitsMax);
                }
            }
        }
        fTree->Fill();
    }
}

void validation::PandoraValidation::beginJob()
{
    art::ServiceHandle<art::TFileService> tfs;
    fTree = tfs->make<TTree>("tree", "Pandora Validation Tree");

    fTree->Branch("eventID", &fEventID);
    fTree->Branch("isNuSlice", &fIsNuSlice);
    fTree->Branch("fNuVertexX", &fNuVertexX);
    fTree->Branch("fNuVertexY", &fNuVertexY);
    fTree->Branch("fNuVertexZ", &fNuVertexZ);
    fTree->Branch("nPFParticle", &fNPFParticle);
    fTree->Branch("fTrackScore", &fTrackScore);
    fTree->Branch("fConeChargeConcentratio", &fConeChargeConcentration);
    fTree->Branch("fConeChargeConicalness", &fConeChargeConicalness);
    fTree->Branch("fConeChargeHaloTotRatio", &fConeChargeHaloTotRatio);
    fTree->Branch("fChargeEndFrac", &fChargeEndFrac);
    fTree->Branch("fChargeFracSpread", &fChargeFracSpread);
    fTree->Branch("fLinearDiffStraightLineMean", &fLinearDiffStraightLineMean);
    fTree->Branch("fLinearLength", &fLinearLength);
    fTree->Branch("fLinearMaxFitGapLength", &fLinearMaxFitGapLength);
    fTree->Branch("fLinearFitRMS", &fLinearFitRMS);
    fTree->Branch("fOpenAngleDiff", &fOpenAngleDiff);
    fTree->Branch("fPCASecondaryRatio", &fPCASecondaryRatio);
    fTree->Branch("fPCATertiaryRatio", &fPCATertiaryRatio);
    fTree->Branch("fVertexDistance", &fVertexDistance);
    fTree->Branch("fNuScore", &fNuScore);
    fTree->Branch("fNuEigenRatioInSphere", &fNuEigenRatioInSphere);
    fTree->Branch("fNuNFinalStatePfos", &fNuNFinalStatePfos);
    fTree->Branch("fNuNHitsTotal", &fNuNHitsTotal);
    fTree->Branch("fNuNSpacePointsInSphere", &fNuNSpacePointsInSphere);
    fTree->Branch("fNuWeightedDirZ", &fNuWeightedDirZ);
    fTree->Branch("fCRFracHitsInLongestTrack", &fCRFracHitsInLongestTrack);
    fTree->Branch("fCRLongestTrackDeflection", &fCRLongestTrackDeflection);
    fTree->Branch("fCRLongestTrackDirY", &fCRLongestTrackDirY);
    fTree->Branch("fCRNHitsMax", &fCRNHitsMax);
}

void validation::PandoraValidation::endJob()
{
    // Implementation of optional member function here.
}

void validation::PandoraValidation::Reset()
{
    fNPFParticle = 0;
    fIsNuSlice = false;
    fNuVertexX = -999;
    fNuVertexY = -999;
    fNuVertexZ = -999;
    fTrackScore.clear();
    fConeChargeConcentration.clear();
    fConeChargeConicalness.clear();
    fConeChargeHaloTotRatio.clear();
    fChargeEndFrac.clear();
    fChargeFracSpread.clear();
    fLinearDiffStraightLineMean.clear();
    fLinearLength.clear();
    fLinearMaxFitGapLength.clear();
    fLinearFitRMS.clear();
    fOpenAngleDiff.clear();
    fPCASecondaryRatio.clear();
    fPCATertiaryRatio.clear();
    fVertexDistance.clear();
    fNuScore.clear();
    fNuEigenRatioInSphere.clear();
    fNuNFinalStatePfos.clear();
    fNuNHitsTotal.clear();
    fNuNSpacePointsInSphere.clear();
    fNuWeightedDirZ.clear();
    fCRFracHitsInLongestTrack.clear();
    fCRLongestTrackDeflection.clear();
    fCRLongestTrackDirY.clear();
    fCRNHitsMax.clear();
}

template<typename T>
void validation::PandoraValidation::AddPropertyToVector(const larpandoraobj::PFParticleMetadata::PropertiesMap &properties, const std::string &key, std::vector<T> &vector)
{
    if (properties.find(key) != properties.end())
        vector.emplace_back(properties.at(key));
}

DEFINE_ART_MODULE(validation::PandoraValidation)
