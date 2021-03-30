//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//

#include "runaction.hh"
#include "primarygeneratoraction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"

RunAction::RunAction(ModelImport* _tetmodel)
 : G4UserRunAction(),
   tetmodel(_tetmodel), fGoodEvents(0)
{
  //add new units for dose
  //
  const G4double milligray = 1.e-3*gray;
  const G4double microgray = 1.e-6*gray;
  const G4double nanogray  = 1.e-9*gray;
  const G4double picogray  = 1.e-12*gray;

  new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
  new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
  new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
  new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);

  // Register accumulable to the accumulable manager
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fGoodEvents);
  for(G4int i=0;i<tetmodel->GetNumOfFace();i++){
      fSumDose.push_back(new G4Accumulable<double>(0));
      accumulableManager->RegisterAccumulable(*fSumDose[i]);
  }

  if(!isMaster) return;
  eyeFaces = tetmodel->GetEyeFaces();
}

RunAction::~RunAction()
{
    for(auto iter:fSumDose) delete iter;
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;

  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();
  G4RunManager::GetRunManager()->SetPrintProgress(run->GetNumberOfEventToBeProcessed()*0.1);

  if (!IsMaster()) return;
  massMap.clear(); totalMass=0;
  G4double divide3 = 1./3.;
  G4Material* tissue = G4NistManager::Instance()->FindOrBuildMaterial("G4_ADIPOSE_TISSUE_ICRP");
  for(G4int i=tetmodel->GetNonTargetNum();i<tetmodel->GetNumOfTet();i++){
     G4double mass = tetmodel->GetTetrahedron(i)->GetCubicVolume() * tissue->GetDensity();
     massMap[floor((i-tetmodel->GetNonTargetNum())*divide3)]
             += mass;
     totalMass+=mass;
  }
  eyeMass=0;
  for(G4double i:eyeFaces){
        eyeMass+=massMap[i];
  }
//  //inform the runManager to save random number seed
//  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  // Merge accumulables
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  // Print results
  //
  if (!IsMaster()) return;
    G4cout
     << G4endl
     << "--------------------End of Run#"<<run->GetRunID()<<"-----------------------"
     << G4endl
     << "  The run was " << nofEvents << " events "
     << "/ Nb of 'good' events: " << fGoodEvents.GetValue()  << G4endl;
    G4double sumDose(0), lensDose(0);
    for(G4int i:eyeFaces) lensDose+=fSumDose[i]->GetValue();
    for(size_t i=0;i<fSumDose.size();i++){
        sumDose += fSumDose[i]->GetValue();
        (*fSumDose[i]) *= 1./massMap[i];
    }

    G4cout
       << "  Total skin dose : " << G4BestUnit(sumDose/totalMass/nofEvents,"Dose")
       << "  Lens dose : " << G4BestUnit(lensDose/eyeMass/nofEvents,"Dose")
       << G4endl
       << "---------------------------------------------------------------" << G4endl
       << G4endl;
}
