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
// Author: Alfonso Mantero
//         
//
// History:
// -----------
//  05 Sep 2012   ALF 1st implementation based on G4VecpssrLiModel.hh
//
// -------------------------------------------------------------------

// Class description:
// Low Energy Electromagnetic Physics, Cross section, p and alpha ionisation, L shell
// Further documentation available from http://www.ge.infn.it/geant4/lowE

// -------------------------------------------------------------------


#ifndef G4VECPSSRMIMODEL_HH
#define G4VECPSSRMIMODEL_HH 1

#include "globals.hh"

class G4VecpssrMiModel 
{

public:

  G4VecpssrMiModel();

  virtual ~G4VecpssrMiModel();
			     
  virtual G4double CalculateM1CrossSection(G4int zTarget,G4double massIncident, G4double energyIncident) = 0;

  virtual G4double CalculateM2CrossSection(G4int zTarget,G4double massIncident, G4double energyIncident) = 0;

  virtual G4double CalculateM3CrossSection(G4int zTarget,G4double massIncident, G4double energyIncident) = 0;

  virtual G4double CalculateM4CrossSection(G4int zTarget,G4double massIncident, G4double energyIncident) = 0;

  virtual G4double CalculateM5CrossSection(G4int zTarget,G4double massIncident, G4double energyIncident) = 0;
				    

   

private:


  G4VecpssrMiModel(const G4VecpssrMiModel&);
  G4VecpssrMiModel & operator = (const G4VecpssrMiModel &right);

};

#endif
