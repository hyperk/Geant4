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
// $Id$
//
/// \file ExG4EventAction01Messenger.hh
/// \brief Definition of the ExG4EventAction01Messenger class

#ifndef ExG4EventAction01Messenger_h
#define ExG4EventAction01Messenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class ExG4EventAction01;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;

/// Messenger class that defines commands for ExG4EventAction01
///
/// It implements commands:
/// - /ExG4/event/verboseLevel level
/// - /ExG4/event/printModulo modulo
/// - /ExG4/event/saveRndm true|false

class ExG4EventAction01Messenger: public G4UImessenger
{
  public:
    ExG4EventAction01Messenger(ExG4EventAction01* eventAction);
    ~ExG4EventAction01Messenger();
    
    virtual void SetNewValue(G4UIcommand* command, G4String newValue);
    
  private:
    // data members
    ExG4EventAction01*    fEventAction;   
    G4UIdirectory*        fTopDirectory;
    G4UIdirectory*        fDirectory;
    G4UIcmdWithAnInteger* fSetVerboseLevelCmd;
    G4UIcmdWithABool*     fSetSaveRndmCmd;
};

#endif //ExG4EventAction01Messenger_h
