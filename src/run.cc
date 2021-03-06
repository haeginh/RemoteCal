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

#include "run.hh"

Run::Run()
:G4Run()//, dap(0)
{
    fCollID_skin
    = G4SDManager::GetSDMpointer()->GetCollectionID("meshSD/doseS");
    fCollID_lens
    = G4SDManager::GetSDMpointer()->GetCollectionID("meshSD/doseE");
//    fCollID_dap
//    = G4SDManager::GetSDMpointer()->GetCollectionID("dap/dose");
    doseMapS.resize(60*60*60,0);
    doseMapL.resize(60*60*60,0);
}

Run::~Run()
{}

void Run::RecordEvent(const G4Event* event)
{
    // Hits collections
    //
    G4HCofThisEvent* HCE = event->GetHCofThisEvent();
    if(!HCE) return;

    auto _doseMapS =
            *static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID_skin))->GetMap();
    auto _doseMapL =
            *static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID_lens))->GetMap();

    for(auto itr:_doseMapS){
        doseMapS[itr.first]+= *itr.second;
        doseMapL[itr.first]+= *_doseMapL[itr.first];
    }
//    auto dapMap =
//            *static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID_dap))->GetMap();
//    if(dapMap.find(1000)!=dapMap.end()) dap += *dapMap[1000];

}

void Run::Merge(const G4Run* run)
{
    const Run* localRun = static_cast<const Run*>(run);
    // merge the data from each thread
    auto localMapS = localRun->doseMapS;
    auto localMapL = localRun->doseMapL;

    for(G4int i=0;i<60*60*60;i++){
        doseMapS[i]  += localMapS[i];
        doseMapL[i]  += localMapL[i];
    }
//    dap += localRun->dap;
    G4Run::Merge(run);
}
