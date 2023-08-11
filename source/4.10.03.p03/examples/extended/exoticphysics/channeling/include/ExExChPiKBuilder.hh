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
/// \file channeling/include/ExExChPiKBuilder.hh
/// \brief Definition of the ExExChPiKBuilder class
//

#ifndef ExExChPiKBuilder_h
#define ExExChPiKBuilder_h 1

#include "globals.hh"

#include "G4ProtonInelasticProcess.hh"
#include "G4VPiKBuilder.hh"

#include <vector>

class ExExChPiKBuilder
{
  public: 
    ExExChPiKBuilder();
    virtual ~ExExChPiKBuilder();

  public: 
    void Build();
    void RegisterMe(G4VPiKBuilder * aB) {theModelCollections.push_back(aB);}

  private:
    G4PionPlusInelasticProcess*  thePionPlusInelastic;
    G4PionMinusInelasticProcess* thePionMinusInelastic;
    G4KaonPlusInelasticProcess*  theKaonPlusInelastic;
    G4KaonMinusInelasticProcess* theKaonMinusInelastic;
    G4KaonZeroLInelasticProcess* theKaonZeroLInelastic;
    G4KaonZeroSInelasticProcess* theKaonZeroSInelastic;
     
    std::vector<G4VPiKBuilder *> theModelCollections;

    G4bool wasActivated;
};

#endif

