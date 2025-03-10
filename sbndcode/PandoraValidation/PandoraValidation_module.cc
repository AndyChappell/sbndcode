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

        TTree *fTree;
        unsigned int fEventID;
        std::string fSliceLabel;
        std::string fPFParticleLabel;
        std::string fTrackScoreKey;
        bool fIsNuSlice;
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

    for (const art::Ptr<recob::Slice> &slice : sliceVector)
    {
        std::vector<art::Ptr<recob::PFParticle>> slicePFPs{slicePFPAssoc.at(slice.key())};
        for (const art::Ptr<recob::PFParticle> &slicePFP : slicePFPs)
        {
            const int pdg{std::abs(slicePFP->PdgCode())};
            const bool isNeutrino{pdg == 12 || pdg ==14};

            if (isNeutrino)
                fIsNuSlice = true;

            fNPFParticle = slicePFPs.size();
            for (const art::Ptr<recob::PFParticle> &pfp : slicePFPs)
            {
                auto metadataVec{pfpMetadataAssoc.at(pfp.key())};

                if (!metadataVec.empty())
                {
                    auto metadata{metadataVec.front()};
                    const auto &properties{metadata->GetPropertiesMap()};
                    if (properties.find("TrackScore") != properties.end())
                        fTrackScore.emplace_back(properties.at("TrackScore"));
                    if (properties.find("LArConeChargeFeatureTool_Concentration") != properties.end())
                        fConeChargeConcentration.emplace_back(properties.at("LArConeChargeFeatureTool_Concentration"));
                    if (properties.find("LArConeChargeFeatureTool_Conicalness") != properties.end())
                        fConeChargeConicalness.emplace_back(properties.at("LArConeChargeFeatureTool_Conicalness"));
                    if (properties.find("LArConeChargeFeatureTool_HaloTotalRatio") != properties.end())
                        fConeChargeHaloTotRatio.emplace_back(properties.at("LArConeChargeFeatureTool_HaloTotalRatio"));
                    if (properties.find("LArThreeDChargeFeatureTool_EndFraction") != properties.end())
                        fChargeEndFrac.emplace_back(properties.at("LArThreeDChargeFeatureTool_EndFraction"));
                    if (properties.find("LArThreeDChargeFeatureTool_FractionalSpread") != properties.end())
                        fChargeFracSpread.emplace_back(properties.at("LArThreeDChargeFeatureTool_FractionalSpread"));
                    if (properties.find("LArThreeDLinearFitFeatureTool_DiffStraightLineMean") != properties.end())
                        fLinearDiffStraightLineMean.emplace_back(properties.at("LArThreeDLinearFitFeatureTool_DiffStraightLineMean"));
                    if (properties.find("LArThreeDLinearFitFeatureTool_Length") != properties.end())
                        fLinearLength.emplace_back(properties.at("LArThreeDLinearFitFeatureTool_Length"));
                    if (properties.find("LArThreeDLinearFitFeatureTool_MaxFitGapLength") != properties.end())
                        fLinearMaxFitGapLength.emplace_back(properties.at("LArThreeDLinearFitFeatureTool_MaxFitGapLength"));
                    if (properties.find("LArThreeDLinearFitFeatureTool_SlidingLinearFitRMS") != properties.end())
                        fLinearFitRMS.emplace_back(properties.at("LArThreeDLinearFitFeatureTool_SlidingLinearFitRMS"));
                    if (properties.find("LArThreeDOpeningAngleFeatureTool_AngleDiff") != properties.end())
                        fOpenAngleDiff.emplace_back(properties.at("LArThreeDOpeningAngleFeatureTool_AngleDiff"));
                    if (properties.find("LArThreeDPCAFeatureTool_SecondaryPCARatio") != properties.end())
                        fPCASecondaryRatio.emplace_back(properties.at("LArThreeDPCAFeatureTool_SecondaryPCARatio"));
                    if (properties.find("LArThreeDPCAFeatureTool_TertiaryPCARatio") != properties.end())
                        fPCATertiaryRatio.emplace_back(properties.at("LArThreeDPCAFeatureTool_TertiaryPCARatio"));
                    if (properties.find("LArThreeDVertexDistanceFeatureTool_VertexDistance") != properties.end())
                        fVertexDistance.emplace_back(properties.at("LArThreeDVertexDistanceFeatureTool_VertexDistance"));
                }
            }
        }
    }

    fTree->Fill();
}

void validation::PandoraValidation::beginJob()
{
    art::ServiceHandle<art::TFileService> tfs;
    fTree = tfs->make<TTree>("tree", "Pandora Validation Tree");

    fTree->Branch("eventID", &fEventID);
    fTree->Branch("isNuSlice", &fIsNuSlice);
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
}

void validation::PandoraValidation::endJob()
{
    // Implementation of optional member function here.
}

void validation::PandoraValidation::Reset()
{
    fNPFParticle = 0;
    fIsNuSlice = false;
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
}

DEFINE_ART_MODULE(validation::PandoraValidation)
