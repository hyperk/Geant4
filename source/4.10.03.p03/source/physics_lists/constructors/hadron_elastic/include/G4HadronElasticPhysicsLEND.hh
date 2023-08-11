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
// $Id: G4HadronElasticPhysicsLEND.hh 71037 2013-06-10 09:20:54Z gcosmo $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4HadronElasticPhysicsLEND
//
// Author: 02 June 2011 Tatsumi Koi
//
// Modified:
//
//----------------------------------------------------------------------------
//

#ifndef G4HadronElasticPhysicsLEND_h
#define G4HadronElasticPhysicsLEND_h 1

#include "globals.hh"
#include "G4VPhysicsConstructor.hh"

class G4HadronElasticPhysics;

class G4HadronElasticPhysicsLEND : public G4VPhysicsConstructor
{
public: 

  G4HadronElasticPhysicsLEND(G4int ver = 1,G4String evaluation=""); 

  virtual ~G4HadronElasticPhysicsLEND();

  // This method will be invoked in the Construct() method. 
  // each particle type will be instantiated
  virtual void ConstructParticle();
 
  // This method will be invoked in the Construct() method.
  // each physics process will be instantiated and
  // registered to the process manager of each particle type 
  virtual void ConstructProcess();

private:

  G4HadronElasticPhysicsLEND(G4HadronElasticPhysicsLEND &);
  G4HadronElasticPhysicsLEND & operator=(const G4HadronElasticPhysicsLEND &right);

  G4int    verbose;
  static G4ThreadLocal G4bool   wasActivated;
  static G4ThreadLocal G4HadronElasticPhysics* mainElasticBuilder;
  G4String evaluation;
};


#endif








