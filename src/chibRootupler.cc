#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <DataFormats/PatCandidates/interface/UserData.h> 
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "TLorentzVector.h"
#include "TVector3.h"
#include "TTree.h"
#include <vector>
#include <sstream>

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

class chibRootupler:public edm::EDAnalyzer {
      public:
	explicit chibRootupler(const edm::ParameterSet &);
	~chibRootupler() override {};
	static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
        bool isAncestor(const reco::Candidate * ancestor, const reco::Candidate * particle);

      private:
	void analyze(const edm::Event &, const edm::EventSetup &) override;

	std::string file_name;
        edm::EDGetTokenT<pat::CompositeCandidateCollection> chi_;
        edm::EDGetTokenT<pat::CompositeCandidateCollection> ups_;
        edm::EDGetTokenT<pat::CompositeCandidateCollection> refit1_;
        edm::EDGetTokenT<pat::CompositeCandidateCollection> refit2_;
        edm::EDGetTokenT<pat::CompositeCandidateCollection> refit3_;
        edm::EDGetTokenT<reco::VertexCollection>            primaryVertices_;
        edm::EDGetTokenT<edm::TriggerResults>               triggerResults_;

	bool isMC_;

	UInt_t    run;
        ULong64_t event;
        UInt_t    lumiblock;

	TLorentzVector chi_p4;
	TLorentzVector dimuon_p4;
	TLorentzVector muonP_p4;
	TLorentzVector muonM_p4;
  	TLorentzVector photon_p4;
  	TLorentzVector ele_trk0;
  	TLorentzVector ele_trk1;

	TLorentzVector rf1S_chi_p4,rf2S_chi_p4,rf3S_chi_p4;
	Double_t invm1S,invm2S,invm3S;
        Double_t probFit1S,probFit2S,probFit3S;
	Double_t y1S_nsigma,y2S_nsigma,y3S_nsigma;

  	Double_t ele_mass;
	Double_t ele_lowerPt_pt;
	Double_t ele_higherPt_pt;
	Double_t ctpv;
	Double_t ctpv_error;
  	Double_t conv_vertex;
  	Double_t conv_vertex_x;
  	Double_t conv_vertex_y;
  	Double_t conv_vertex_z;
  	Double_t conv_vertex_r;
  	Double_t conv_vertex_eta;
  	Double_t conv_vertex_theta;
  	Double_t conv_vertex_phi;
	Double_t dz;

	UInt_t photon_flags;
	UInt_t numPrimaryVertices;
	UInt_t trigger;
	UInt_t rf1S_rank;

	TTree *chib_tree;

	Int_t chi_pdgId;
	Int_t yns_pdgId;
	TLorentzVector gen_chi_p4;
	TLorentzVector gen_yns_p4;
        TLorentzVector gen_dimuon_p4;
	TLorentzVector gen_photon_p4;
	TLorentzVector gen_muonP_p4;
	TLorentzVector gen_muonM_p4;

        edm::EDGetTokenT<reco::GenParticleCollection> genCands_;

        TTree *upsilon_tree;
        TLorentzVector mumu_p4, muP_p4, muM_p4;
        UInt_t mumu_rank;

};

static const double pi0_mass =  0.134977;
static const double y1SMass  =  9.46030;
static const double y2SMass  = 10.02326;
static const double y3SMass  = 10.35520;
static const double ele_mass = 0.000511;

/*
// 2011 par
static const double Y_sig_par_A = 0.058;
static const double Y_sig_par_B = 0.047;
static const double Y_sig_par_C = 0.22;
*/

// 2012 par
static const double Y_sig_par_A = 62.62;
static const double Y_sig_par_B = 56.3;
static const double Y_sig_par_C = -20.77;

chibRootupler::chibRootupler(const edm::ParameterSet & iConfig): 
chi_(consumes<pat::CompositeCandidateCollection>(iConfig.getParameter < edm::InputTag > ("chi_cand"))),
ups_(consumes<pat::CompositeCandidateCollection>(iConfig.getParameter < edm::InputTag > ("ups_cand"))),
refit1_(consumes<pat::CompositeCandidateCollection>(iConfig.getParameter < edm::InputTag > ("refit1S"))),
refit2_(consumes<pat::CompositeCandidateCollection>(iConfig.getParameter < edm::InputTag > ("refit2S"))),
refit3_(consumes<pat::CompositeCandidateCollection>(iConfig.getParameter < edm::InputTag > ("refit3S"))),
primaryVertices_(consumes<reco::VertexCollection>(iConfig.getParameter < edm::InputTag > ("primaryVertices"))),
triggerResults_(consumes<edm::TriggerResults>(iConfig.getParameter < edm::InputTag > ("TriggerResults"))), 
isMC_(iConfig.getParameter < bool > ("isMC"))
{
    edm::Service < TFileService > fs;
    chib_tree = fs->make < TTree > ("chiTree", "Tree of chib");

    chib_tree->Branch("run",      &run,      "run/i");
    chib_tree->Branch("event",    &event,    "event/l");
    chib_tree->Branch("lumiblock",&lumiblock,"lumiblock/i");

    chib_tree->Branch("chi_p4",    "TLorentzVector", &chi_p4);
    chib_tree->Branch("dimuon_p4", "TLorentzVector", &dimuon_p4);
    chib_tree->Branch("muonP_p4",  "TLorentzVector", &muonP_p4);
    chib_tree->Branch("muonM_p4",  "TLorentzVector", &muonM_p4);
    chib_tree->Branch("photon_p4", "TLorentzVector", &photon_p4);
    chib_tree->Branch("ele_trk0", "TLorentzVector", &ele_trk0);
    chib_tree->Branch("ele_trk1", "TLorentzVector", &ele_trk1);

    chib_tree->Branch("rf1S_chi_p4", "TLorentzVector", &rf1S_chi_p4);
    chib_tree->Branch("rf2S_chi_p4", "TLorentzVector", &rf2S_chi_p4);
    chib_tree->Branch("rf3S_chi_p4", "TLorentzVector", &rf3S_chi_p4);
    chib_tree->Branch("invm1S",      &invm1S,          "invm1S/D");
    chib_tree->Branch("invm2S",      &invm2S,          "invm2S/D");
    chib_tree->Branch("invm3S",      &invm3S,          "invm3S/D");
    chib_tree->Branch("probFit1S",   &probFit1S,       "probFit1S/D");
    chib_tree->Branch("probFit2S",   &probFit2S,       "probFit2S/D");
    chib_tree->Branch("probFit3S",   &probFit3S,       "probFit3S/D");
    chib_tree->Branch("y1S_nsigma",  &y1S_nsigma,      "y1S_nsigma/D");
    chib_tree->Branch("y2S_nsigma",  &y2S_nsigma,      "y2S_nsigma/D");
    chib_tree->Branch("y3S_nsigma",  &y3S_nsigma,      "y3S_nsigma/D");

    chib_tree->Branch("ele_lowerPt_pt",  &ele_lowerPt_pt,  "ele_lowerPt_pt/D");
    chib_tree->Branch("ele_higherPt_pt", &ele_higherPt_pt, "ele_higherPt_pt/D");

    chib_tree->Branch("ctpv",         &ctpv,         "ctpv/D");
    chib_tree->Branch("ctpv_error",   &ctpv_error,   "ctpv_error/D");
    chib_tree->Branch("conv_vertex",  &conv_vertex,  "conv_vertex/D");
    chib_tree->Branch("conv_vertex_x",  &conv_vertex_x,  "conv_vertex_x/D");
    chib_tree->Branch("conv_vertex_y",  &conv_vertex_y,  "conv_vertex_y/D");
    chib_tree->Branch("conv_vertex_z",  &conv_vertex_z,  "conv_vertex_z/D");
    chib_tree->Branch("conv_vertex_r",  &conv_vertex_r,  "conv_vertex_r/D");
    chib_tree->Branch("conv_vertex_eta",  &conv_vertex_eta,  "conv_vertex_eta/D");
    chib_tree->Branch("conv_vertex_theta",  &conv_vertex_theta,  "conv_vertex_theta/D");
    chib_tree->Branch("conv_vertex_phi",  &conv_vertex_phi,  "conv_vertex_phi/D");
    chib_tree->Branch("dz",           &dz,           "dz/D");

    chib_tree->Branch("photon_flags", &photon_flags, "photon_flags/i");

    chib_tree->Branch("numPrimaryVertices", &numPrimaryVertices, "numPrimaryVertices/i");
    chib_tree->Branch("trigger",            &trigger,            "trigger/i");
    chib_tree->Branch("rf1S_rank",          &rf1S_rank,          "rf1S_rank/i");

    if (isMC_) {
       chib_tree->Branch("chi_pdgId",     &chi_pdgId,        "chi_pdgId/I");
       chib_tree->Branch("yns_pdgId",     &yns_pdgId,        "yns_pdgId/I");
       chib_tree->Branch("gen_chi_p4",    "TLorentzVector",  &gen_chi_p4);
       chib_tree->Branch("gen_yns_p4",    "TLorentzVector",  &gen_yns_p4);
       chib_tree->Branch("gen_dimuon_p4", "TLorentzVector",  &gen_dimuon_p4);
       chib_tree->Branch("gen_photon_p4", "TLorentzVector",  &gen_photon_p4);
       chib_tree->Branch("gen_muonP_p4",  "TLorentzVector",  &gen_muonP_p4);
       chib_tree->Branch("gen_muonM_p4",  "TLorentzVector",  &gen_muonM_p4);
    }
    genCands_ = consumes<reco::GenParticleCollection>((edm::InputTag)"prunedGenParticles");

    upsilon_tree = fs->make<TTree>("upsTree","Tree of Upsilon");
    upsilon_tree->Branch("mumu_p4",  "TLorentzVector", &mumu_p4);
    upsilon_tree->Branch("muP_p4",   "TLorentzVector", &muP_p4);
    upsilon_tree->Branch("muM_p4",   "TLorentzVector", &muM_p4);
    upsilon_tree->Branch("trigger",  &trigger,         "trigger/i");
    upsilon_tree->Branch("numPrimaryVertices", &numPrimaryVertices, "numPrimaryVertices/i");
    upsilon_tree->Branch("mumu_rank",&mumu_rank,       "mumu_rank/i"); 
}

//Check recursively if any ancestor of particle is the given one
bool chibRootupler::isAncestor(const reco::Candidate* ancestor, const reco::Candidate * particle) {
   if (ancestor == particle ) return true;
   if (particle->numberOfMothers() && isAncestor(ancestor,particle->mother(0))) return true;
   return false;
}

// ------------ method called for each event  ------------
void chibRootupler::analyze(const edm::Event & iEvent, const edm::EventSetup & iSetup) {

  edm::Handle < pat::CompositeCandidateCollection >chi_cand_handle;
  iEvent.getByToken(chi_, chi_cand_handle);

  edm::Handle < pat::CompositeCandidateCollection >ups_hand;
  iEvent.getByToken(ups_, ups_hand);

  edm::Handle < pat::CompositeCandidateCollection >refit1S_handle;
  iEvent.getByToken(refit1_, refit1S_handle);

  edm::Handle < pat::CompositeCandidateCollection >refit2S_handle;
  iEvent.getByToken(refit2_, refit2S_handle);

  edm::Handle < pat::CompositeCandidateCollection >refit3S_handle;
  iEvent.getByToken(refit3_, refit3S_handle);

  edm::Handle < reco::VertexCollection  >primaryVertices_handle;
  iEvent.getByToken(primaryVertices_, primaryVertices_handle);

  edm::Handle < edm::TriggerResults > triggerResults_handle;
  iEvent.getByToken(triggerResults_, triggerResults_handle);

  numPrimaryVertices = primaryVertices_handle->size();
  run       = iEvent.id().run();
  event     = iEvent.id().event();
  lumiblock = iEvent.id().luminosityBlock();

  pat::CompositeCandidate chi_cand;
  pat::CompositeCandidate refit1S;
  pat::CompositeCandidate refit2S;
  pat::CompositeCandidate refit3S;

  edm::Handle<reco::GenParticleCollection> pruned;
  iEvent.getByToken(genCands_,pruned);

  if (isMC_ && pruned.isValid()) {
   gen_chi_p4.SetPtEtaPhiM(0, 0, 0, 0);
   gen_yns_p4.SetPtEtaPhiM(0, 0, 0, 0);
   gen_dimuon_p4.SetPtEtaPhiM(0, 0, 0, 0);
   chi_pdgId = 0;
   for (size_t i=0; i<pruned->size(); i++) {
      int p_id = abs((*pruned)[i].pdgId());
      int p_status = (*pruned)[i].status();
      yns_pdgId = 0;
      int foundit = 0;
      if ( (p_id == 20553 || p_id == 555 || p_id == 10551) && p_status == 2) yns_pdgId = 553;
      if (yns_pdgId > 0) {
         chi_pdgId = p_id;
         foundit++;
         const reco::Candidate * pwave = &(*pruned)[i];
         gen_chi_p4.SetPtEtaPhiM(pwave->pt(),pwave->eta(),pwave->phi(),pwave->mass());
         if (pwave->mass() > 10.5) { chi_pdgId += 200000; }
         else { if (pwave->mass() > 10.2) chi_pdgId += 100000; }
         for (size_t j=0; j<pwave->numberOfDaughters(); j++) {
            const reco::Candidate *dau = pwave->daughter(j);
            int d_id = dau->pdgId();
            if ((d_id == 553 || d_id == 100553 || d_id == 200553 ) && dau->status() == 2) {
               yns_pdgId = d_id;
               gen_yns_p4.SetPtEtaPhiM(dau->pt(),dau->eta(),dau->phi(),dau->mass());
               uint nmuons = 0;
               for (size_t k=0; k<dau->numberOfDaughters(); k++) {
                  const reco::Candidate *gdau = dau->daughter(k);
                  if (gdau->pdgId() == 13 && gdau->status()==1) {
                     nmuons++;
                     gen_muonM_p4.SetPtEtaPhiM(gdau->pt(),gdau->eta(),gdau->phi(),gdau->mass());
                  } else {
                     if (gdau->pdgId() == -13 && gdau->status()==1) {
                        nmuons++;
                        gen_muonP_p4.SetPtEtaPhiM(gdau->pt(),gdau->eta(),gdau->phi(),gdau->mass());
                     } 
                  }
               }
               if (nmuons == 2 ) {
                  foundit += 3;                                  // found complete dimuon decay
                  gen_dimuon_p4 = gen_muonM_p4 + gen_muonP_p4;   // will account fsr
               }
            } else {
               if (dau->pdgId() == 22 && dau->status() ==1) { 
                  foundit++;
                  gen_photon_p4.SetPtEtaPhiM(dau->pt(),dau->eta(),dau->phi(),dau->mass());
               }  else std::cout << "Rootupler: unexpected pdg_id " << dau->pdgId() << " (" << run << "," << event << ")" << std::endl; 
            }
            if (foundit == 5 ) break;                             // decay found !
         } 
      }
      if (chi_pdgId && yns_pdgId && foundit==5) break;        // just one decay of this kind is expected
      else chi_pdgId = 0;
   } 
   if (!chi_pdgId)  std::cout << "Rootupler does not found the given decay " << run << "," << event << std::endl;
  }

   //grab Trigger informations
   // save it in variable trigger, trigger is an int between 0 and 15, in binary it is:
   // (pass 11)(pass 8)(pass 7)(pass 5)
   // es. 11 = pass 5, 7 and 11
   // es. 4 = pass only 8

   trigger = 0;
   if (triggerResults_handle.isValid()) {
      const edm::TriggerNames & TheTriggerNames = iEvent.triggerNames(*triggerResults_handle);
      unsigned int NTRIGGERS = 8;
      std::string TriggersToTest[NTRIGGERS] = {
	      "HLT_Dimuon20_Jpsi_Barrel_Seagulls","HLT_Dimuon25_Jpsi",
	      "HLT_Dimuon10_PsiPrime_Barrel_Seagulls","HLT_Dimuon18_PsiPrime",
	      "HLT_Dimuon10_Upsilon_Barrel_Seagulls","HLT_Dimuon12_Upsilon_eta1p5","HLT_Dimuon14_Phi_Barrel_Seagulls","HLT_Dimuon24_Upsilon_noCorrL1"};

      for (unsigned int i = 0; i < NTRIGGERS; i++) {
         for (int version = 1; version < 19; version++) {
            std::stringstream ss;
            ss << TriggersToTest[i] << "_v" << version;
            unsigned int bit = TheTriggerNames.triggerIndex(edm::InputTag(ss.str()).label());
            if (bit < triggerResults_handle->size() && triggerResults_handle->accept(bit) && !triggerResults_handle->error(bit)) {
               trigger += (1<<i);
               break;
            }
         }
      }
    } else std::cout << "*** NO triggerResults found " << iEvent.id().run() << "," << iEvent.id().event() << std::endl;

    bool bestCandidateOnly_ = false;
    rf1S_rank = 0;
    photon_flags = 0;
    // grabbing chi inforamtion
    if (chi_cand_handle.isValid() && !chi_cand_handle->empty()) {

       unsigned int csize = chi_cand_handle->size();
       if (bestCandidateOnly_) csize = 1;

       for (unsigned int i = 0; i < csize; i++) {
	   chi_cand = chi_cand_handle->at(i);

	   chi_p4.SetPtEtaPhiM(chi_cand.pt(), chi_cand.eta(), chi_cand.phi(), chi_cand.mass());
	   dimuon_p4.SetPtEtaPhiM(chi_cand.daughter("dimuon")->pt(), chi_cand.daughter("dimuon")->eta(), 
                                  chi_cand.daughter("dimuon")->phi(), chi_cand.daughter("dimuon")->mass());

     photon_p4.SetPtEtaPhiM(chi_cand.daughter("photon")->pt(), chi_cand.daughter("photon")->eta(), 
                                  chi_cand.daughter("photon")->phi(), chi_cand.daughter("photon")->mass());

	   reco::Candidate::LorentzVector vP = chi_cand.daughter("dimuon")->daughter("muon1")->p4();
	   reco::Candidate::LorentzVector vM = chi_cand.daughter("dimuon")->daughter("muon2")->p4();

	   if (chi_cand.daughter("dimuon")->daughter("muon1")->charge() < 0) {
	      vP = chi_cand.daughter("dimuon")->daughter("muon2")->p4();
	      vM = chi_cand.daughter("dimuon")->daughter("muon1")->p4();
	   }

	   muonP_p4.SetPtEtaPhiM(vP.pt(), vP.eta(), vP.phi(), vP.mass());
	   muonM_p4.SetPtEtaPhiM(vM.pt(), vM.eta(), vM.phi(), vM.mass());

     	   Double_t ele1_pt = (dynamic_cast<const pat::CompositeCandidate *>(chi_cand.daughter("photon"))->
                                    userData<reco::Track>("track0"))->pt();
     	   Double_t ele2_pt = (dynamic_cast<const pat::CompositeCandidate *>(chi_cand.daughter("photon"))->
                                    userData<reco::Track>("track1"))->pt();
     	   Double_t ele1_eta = (dynamic_cast<const pat::CompositeCandidate *>(chi_cand.daughter("photon"))->
                                    userData<reco::Track>("track0"))->eta();
     	   Double_t ele2_eta = (dynamic_cast<const pat::CompositeCandidate *>(chi_cand.daughter("photon"))->
                                    userData<reco::Track>("track1"))->eta();
     	   Double_t ele1_phi = (dynamic_cast<const pat::CompositeCandidate *>(chi_cand.daughter("photon"))->
                                    userData<reco::Track>("track0"))->phi();
     	   Double_t ele2_phi = (dynamic_cast<const pat::CompositeCandidate *>(chi_cand.daughter("photon"))->
                                    userData<reco::Track>("track1"))->phi();

	   if (ele1_pt > ele2_pt) {
	      ele_higherPt_pt = ele1_pt;
	      ele_lowerPt_pt = ele2_pt;
	   } else {
	      ele_higherPt_pt = ele2_pt;
	      ele_lowerPt_pt = ele1_pt;
	   }

     	   ele_trk0.SetPtEtaPhiM(ele1_pt, ele1_eta, ele1_phi, ele_mass);
     	   ele_trk1.SetPtEtaPhiM(ele2_pt, ele2_eta, ele2_phi, ele_mass);

	   ctpv = (dynamic_cast < pat::CompositeCandidate * >(chi_cand.daughter("dimuon")))->userFloat("ppdlPV");
	   ctpv_error = (dynamic_cast < pat::CompositeCandidate * >(chi_cand.daughter("dimuon")))->userFloat("ppdlErrPV");
	   photon_flags = (UInt_t) dynamic_cast<const pat::CompositeCandidate *>(chi_cand.daughter("photon"))->userInt("flags");

     	   conv_vertex = chi_cand.daughter("photon")->vertex().rho();
     	   conv_vertex_x = chi_cand.daughter("photon")->vertex().x();
     	   conv_vertex_y = chi_cand.daughter("photon")->vertex().y();
     	   conv_vertex_z = chi_cand.daughter("photon")->vertex().z();
     	   conv_vertex_r = chi_cand.daughter("photon")->vertex().r();
     	   conv_vertex_eta = chi_cand.daughter("photon")->vertex().eta();
     	   conv_vertex_phi = chi_cand.daughter("photon")->vertex().phi();
     	   conv_vertex_theta = chi_cand.daughter("photon")->vertex().theta();

	   dz = chi_cand.userFloat("dz");

	   // 2012 parameterization
	   double sigma = Y_sig_par_A + Y_sig_par_B * pow(fabs(dimuon_p4.Rapidity()), 2) + 
                          Y_sig_par_C * pow(fabs(dimuon_p4.Rapidity()), 3);

	   y1S_nsigma = fabs(dimuon_p4.M() - y1SMass) / sigma;
	   y2S_nsigma = fabs(dimuon_p4.M() - y2SMass) / sigma;
	   y3S_nsigma = fabs(dimuon_p4.M() - y3SMass) / sigma;
           double QValue = chi_p4.M() - dimuon_p4.M();

           invm1S = QValue + y1SMass;
           invm2S = QValue + y2SMass;
           invm3S = QValue + y3SMass;

           int j = -1;
           if (refit1S_handle.isValid() && i < refit1S_handle->size()) { j = (refit1S_handle->at(i)).userInt("Index"); }
	   if ( j >= 0  && (unsigned int) j == i ) {
	      refit1S = refit1S_handle->at(i);
	      rf1S_chi_p4.SetPtEtaPhiM(refit1S.pt(), refit1S.eta(), refit1S.phi(), refit1S.mass());
	      probFit1S = refit1S.userFloat("vProb");
	   } else {
	      rf1S_chi_p4.SetPtEtaPhiM(chi_cand.pt(), chi_cand.eta(), chi_cand.phi(), invm1S);
	      probFit1S = 0;
	   }	// if rf1S is valid
           j = -1;
           if (refit2S_handle.isValid() && i < refit2S_handle->size()) { j = (refit2S_handle->at(i)).userInt("Index"); }
           if ( j >= 0  && (unsigned int) j == i ) {
	      refit2S = refit2S_handle->at(i);
	      rf2S_chi_p4.SetPtEtaPhiM(refit2S.pt(), refit2S.eta(), refit2S.phi(), refit2S.mass());
	      probFit2S = refit2S.userFloat("vProb");
           } else {
	      rf2S_chi_p4.SetPtEtaPhiM(chi_cand.pt(), chi_cand.eta(), chi_cand.phi(), invm2S);
	      probFit2S = 0;
	   }	// if rf2S valid
           j = -1;
           if (refit3S_handle.isValid() && i < refit3S_handle->size()) { j = (refit3S_handle->at(i)).userInt("Index"); }
           if ( j >= 0  && (unsigned int) j == i ) {
              refit3S = refit3S_handle->at(i);
              rf3S_chi_p4.SetPtEtaPhiM(refit3S.pt(), refit3S.eta(), refit3S.phi(), refit3S.mass());
              probFit3S = refit3S.userFloat("vProb");
           } else {
              rf3S_chi_p4.SetPtEtaPhiM(chi_cand.pt(), chi_cand.eta(), chi_cand.phi(), invm3S);
              probFit3S = 0;
           }    // if rf3S valid
	   chib_tree->Fill();
           rf1S_rank++;
	}		// for i on chi_cand_handle
    } //else std::cout << "no valid chi handle" << std::endl;
    
    mumu_rank = 0;
    if (ups_hand.isValid() && !ups_hand->empty()) {
      for (unsigned int i=0; i< ups_hand->size(); i++) {
        pat::CompositeCandidate ups_ = ups_hand->at(i);
        mumu_p4.SetPtEtaPhiM(ups_.pt(), ups_.eta(), ups_.phi(), ups_.mass());

        reco::Candidate::LorentzVector vP = ups_.daughter("muon1")->p4();
        reco::Candidate::LorentzVector vM = ups_.daughter("muon2")->p4();
        if (ups_.daughter("muon1")->charge() < 0) {
           vP = ups_.daughter("muon2")->p4();
           vM = ups_.daughter("muon1")->p4();
        }

        muP_p4.SetPtEtaPhiM(vP.pt(), vP.eta(), vP.phi(), vP.mass());
        muM_p4.SetPtEtaPhiM(vM.pt(), vM.eta(), vM.phi(), vM.mass());
        upsilon_tree->Fill();
        mumu_rank++;
      }
    } 
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void chibRootupler::fillDescriptions(edm::ConfigurationDescriptions & descriptions) {
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(chibRootupler);
