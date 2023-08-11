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
// Code developed by:
//  S.Larsson
//
//    ******************************
//    *                            *
//    *    PurgMagRunAction.cc     *
//    *                            *
//    ******************************
//
// $Id: PurgMagRunAction.cc 84393 2014-10-15 07:11:25Z gcosmo $
//

#include "PurgMagRunAction.hh"

#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"

#include "PurgMagAnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

PurgMagRunAction::PurgMagRunAction()
{   
  saveRndm = 1;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

PurgMagRunAction::~PurgMagRunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void PurgMagRunAction::BeginOfRunAction(const G4Run* aRun)
{  
  //Analysis must be handled by master and workers
  PurgMagAnalysisManager* analysis = PurgMagAnalysisManager::getInstance();
  analysis->book();
  
  
  if (IsMaster())    
    G4cout << "---> Run " << aRun->GetRunID() << " (master) start." 
	   << G4endl;
  else
    G4cout << "---> Run " << aRun->GetRunID() << " (worker) start." 
	   << G4endl;

  
  // save Rndm status
  if (saveRndm > 0 && IsMaster())
    { 
      G4Random::showEngineStatus();
      G4Random::saveEngineStatus("beginOfRun.rndm");
    }
      
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void PurgMagRunAction::EndOfRunAction(const G4Run* aRun)
{     
  
  PurgMagAnalysisManager* analysis = PurgMagAnalysisManager::getInstance();
  analysis->finish();
  
  if (IsMaster())    
    G4cout << "Total number of event = " << aRun->GetNumberOfEvent() << G4endl;
  else
    G4cout << "Partial number of event in this worker = " 
	   << aRun->GetNumberOfEvent() << G4endl;
 
       
  if (IsMaster())
    {
      // save Rndm status
      if (saveRndm == 1)
	{ 
	  G4Random::showEngineStatus();
	  G4Random::saveEngineStatus("endOfRun.rndm");
	}             
    }            
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....







