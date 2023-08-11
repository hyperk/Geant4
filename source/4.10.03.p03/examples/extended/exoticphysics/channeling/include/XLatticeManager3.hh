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
/// \file channeling/include/XLatticeManager3.hh
/// \brief Definition of the XLatticeManager3 class
//

#ifndef XLatticeManager3_h
#define XLatticeManager3_h 1


#define MAXLAT 10 //maximum number of different crystal lattices supported

#include "G4ThreeVector.hh"
#include "XPhysicalLattice.hh"

class XLatticeManager3
{
private:
  static XLatticeManager3* LM;

protected:
  XLatticeManager3();
  ~XLatticeManager3();

  XPhysicalLattice* fLatticeList[MAXLAT]; 
  int fTotalLattices;

public:

  static XLatticeManager3* GetXLatticeManager(); 

  XPhysicalLattice* GetXPhysicalLattice(G4VPhysicalVolume*);
  bool RegisterLattice(XPhysicalLattice*);
  bool HasLattice(G4VPhysicalVolume*);
  double MapKtoV(G4VPhysicalVolume*,int,const G4ThreeVector &);
  G4ThreeVector MapKtoVDir(G4VPhysicalVolume*,int,const G4ThreeVector &);

};



#endif
