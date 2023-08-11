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
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:  G4PhotoElectricAngularGeneratorSimple
//
// Author:        Andreia Trindade (andreia@lip.pt)
// 
// Creation date: 10 May 2004
//
// Modifications: 
// 10 May 2003 A. Trindade    First implementation acording with new design
//
// Class Description: 
//
// This class is obsolete and will be removed

// -------------------------------------------------------------------
//

#ifndef G4PhotoElectricAngularGeneratorSimple_h
#define G4PhotoElectricAngularGeneratorSimple_h 1

#include "G4VEmAngularDistribution.hh"
#include "G4ios.hh"
#include "globals.hh"

class G4PhotoElectricAngularGeneratorSimple : public G4VEmAngularDistribution
{

public:

  G4PhotoElectricAngularGeneratorSimple();

  ~G4PhotoElectricAngularGeneratorSimple();

  virtual G4ThreeVector& SampleDirection(const G4DynamicParticle* dp,
                                         G4double e = 0.0,
                                         G4int shellId = 0,
                                         const G4Material* mat = 0);

  void PrintGeneratorInformation() const;

private:

  // hide assignment operator 
  G4PhotoElectricAngularGeneratorSimple & operator=(const  G4PhotoElectricAngularGeneratorSimple &right);
  G4PhotoElectricAngularGeneratorSimple(const  G4PhotoElectricAngularGeneratorSimple&);

};

#endif

