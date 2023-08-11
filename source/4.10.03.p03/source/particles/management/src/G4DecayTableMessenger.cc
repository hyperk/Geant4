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
// $Id: G4DecayTableMessenger.cc 81373 2014-05-27 13:06:32Z gcosmo $
//
//
//---------------------------------------------------------------
//
//  G4DecayTableMessenger.cc
//
//  Description:
//    This is a messenger class to interface to exchange information
//    between ParticleDefinition and UI.
//
//  History:
//    13 June 1997, H. Kurashige   : The 1st version created.
//    10 Nov. 1997  H. Kurashige   : fixed bugs 
//    08 jan. 1998  H. Kurashige   : new UIcommnds 
//
//---------------------------------------------------------------

#include "G4DecayTableMessenger.hh"
#include "G4UImanager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4VDecayChannel.hh"
#include "G4DecayTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ios.hh"                 // Include from 'system'
#include <iomanip>                  // Include from 'system'

G4DecayTableMessenger::G4DecayTableMessenger(G4ParticleTable* pTable)
                      :theParticleTable(pTable),
		       currentParticle(0), currentDecayTable(0),
		       idxCurrentChannel(-1), currentChannel(0),
		       thisDirectory(0),dumpCmd(0),selectCmd(0),brCmd(0) 
{
  if ( theParticleTable == 0) theParticleTable = G4ParticleTable::GetParticleTable();

  currentParticle = 0;

  //Commnad   /particle/property/decay/
  thisDirectory = new G4UIdirectory("/particle/property/decay/");
  thisDirectory->SetGuidance("Decay Table control commands.");

  //Commnad   /particle/property/decay/select
  selectCmd = new G4UIcmdWithAnInteger("/particle/property/decay/select",this);
  selectCmd->SetGuidance("Enter index of decay mode.");
  selectCmd->SetParameterName("mode", true);
  selectCmd->SetDefaultValue(0);
  selectCmd->SetRange("mode >=0");
  currentChannel = 0;

  //Commnad   /particle/property/decay/dump
  dumpCmd = new G4UIcmdWithoutParameter("/particle/property/decay/dump",this);
  dumpCmd->SetGuidance("Dump decay mode information.");

  //Command   /particle/property/decay/br
  brCmd = new G4UIcmdWithADouble("/particle/property/decay/br",this);
  brCmd->SetGuidance("Set branching ratio. [0< BR <1.0]");
  brCmd->SetParameterName("br",false);
  brCmd->SetRange("(br >=0.0) && (br <=1.0)");

}

G4DecayTableMessenger::~G4DecayTableMessenger()
{
  delete dumpCmd;
  delete selectCmd;
  delete brCmd;
  delete thisDirectory;
}

void G4DecayTableMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{
  if (SetCurrentParticle()==0) {
    G4cout << "Particle is not selected yet !! Command ignored." << G4endl;
    return;
  }
  if (currentDecayTable==0) {
    G4cout << "The particle has no decay table !! Command ignored." << G4endl;
    return;
  }

  if( command == dumpCmd ){
    //Commnad   /particle/property/decay/dump
    currentDecayTable->DumpInfo();

  } else if ( command == selectCmd ){
    //Commnad   /particle/property/decay/select
    G4int index = selectCmd->GetNewIntValue(newValue) ;
    currentChannel = currentDecayTable->GetDecayChannel(index);
    if ( currentChannel == 0 ) {
      G4cout << "Invalid index. Command ignored." << G4endl;
    } else {
      idxCurrentChannel = index;
    }

  } else {
    if ( currentChannel == 0 ) {
      G4cout << "Select a decay channel. Command ignored." << G4endl;
      return;
    }
    if (command == brCmd) {
      //Commnad   /particle/property/decay/br
      G4double  br = brCmd->GetNewDoubleValue(newValue);
      if( (br<0.0) || (br>1.0) ) { 
	G4cout << "Invalid brancing ratio. Command ignored." << G4endl;
      } else {
	currentChannel->SetBR(br);
      }
    }
  }
}


G4ParticleDefinition* G4DecayTableMessenger::SetCurrentParticle()
{
  // set currentParticle pointer
  
  // get particle name by asking G4ParticleMessenger via UImanager

  G4String particleName = G4UImanager::GetUIpointer()->GetCurrentStringValue("/particle/select");

  if (currentParticle != 0 ){
    // check whether selection is changed 
    if (currentParticle->GetParticleName() != particleName) {
      currentParticle = theParticleTable->FindParticle(particleName);
      idxCurrentChannel = -1;
      currentDecayTable = 0;
    } else {
      // no change 
      return currentParticle;
    }
  } else {
    currentParticle = theParticleTable->FindParticle(particleName);
    idxCurrentChannel = -1;
    currentDecayTable = 0;
  }

  if (currentParticle != 0 ){
    currentDecayTable = currentParticle->GetDecayTable();
    if ((currentDecayTable != 0 ) && (idxCurrentChannel >0) ) {
      currentChannel = currentDecayTable->GetDecayChannel(idxCurrentChannel);
    } else {
      idxCurrentChannel = -1;
      currentChannel = 0;
    }
  }
  return currentParticle;
}

G4String G4DecayTableMessenger::GetCurrentValue(G4UIcommand * command)
{
  G4String returnValue('\0');

  if (SetCurrentParticle()==0) {
    // no particle is selected. return null 
    return returnValue;
  }

  if( command == selectCmd ){
    //Commnad   /particle/property/decay/select
    returnValue = selectCmd->ConvertToString(idxCurrentChannel);

  } else if( command == brCmd ){
    if ( currentChannel != 0) {
      returnValue = brCmd->ConvertToString(currentChannel->GetBR());
    } 
  }
  return returnValue;
}








