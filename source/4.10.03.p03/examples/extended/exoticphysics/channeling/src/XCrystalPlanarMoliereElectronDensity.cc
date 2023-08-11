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
/// \file channeling/src/XCrystalPlanarMoliereElectronDensity.cc
/// \brief Implementation of the XCrystalPlanarMoliereElectronDensity class
//

#include "XCrystalPlanarMoliereElectronDensity.hh"

XCrystalPlanarMoliereElectronDensity::XCrystalPlanarMoliereElectronDensity(){
    fAlfa[0] = 0.1;
    fAlfa[1] = 0.55;
    fAlfa[2] = 0.35;
    
    fBeta[0] = 6.0;
    fBeta[1] = 1.2;
    fBeta[2] = 0.3;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

XCrystalPlanarMoliereElectronDensity::~XCrystalPlanarMoliereElectronDensity(){
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double XCrystalPlanarMoliereElectronDensity::
ComputeECForSinglePlane(G4double vXposition,
                        XPhysicalLattice* vLattice){

    G4double aTF = ComputeTFScreeningRadius(vLattice);

    G4double vValueForSinglePlane = 0.;
    for(unsigned int i=0;i<3;i++){
        vValueForSinglePlane += ( fAlfa[i] * fBeta[i] *
                                 std::exp( - std::fabs(vXposition) * fBeta[i] / aTF ) *
                                 ( 1. + fBeta[i] * std::fabs(vXposition) / aTF) );
    }
    
    vValueForSinglePlane /= aTF;
    
    vValueForSinglePlane *= 0.25;

    return vValueForSinglePlane;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
