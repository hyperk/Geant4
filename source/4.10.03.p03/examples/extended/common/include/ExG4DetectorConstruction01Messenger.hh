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
// $Id: ExG4DetectorConstruction01Messenger.hh 85714 2014-11-04 14:30:40Z ihrivnac $
// 
/// \file ExG4DetectorConstruction01Messenger.hh
/// \brief Definition of the ExG4DetectorConstruction01Messenger class

#ifndef ExG4DetectorConstruction01Messenger_h
#define ExG4DetectorConstruction01Messenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class ExG4DetectorConstruction01;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithADouble;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// Messenger class that defines commands for ExG4DetectorConstruction01.
///
/// It implements commands:
/// - /ExG4/det/setMaterialName name
/// - /ExG4/det/setDimensions hx hy hz unit

class ExG4DetectorConstruction01Messenger: public G4UImessenger
{
  public:
    ExG4DetectorConstruction01Messenger(ExG4DetectorConstruction01* );
    virtual ~ExG4DetectorConstruction01Messenger();
    
    virtual void SetNewValue(G4UIcommand* command, G4String newValue);
    
  private:
    ExG4DetectorConstruction01* fDetectorConstruction;
    G4UIdirectory*             fTopDirectory;
    G4UIdirectory*             fDirectory;
    G4UIcmdWithAString*        fSetMaterialCmd;
    G4UIcmdWith3VectorAndUnit* fSetDimensionsCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

