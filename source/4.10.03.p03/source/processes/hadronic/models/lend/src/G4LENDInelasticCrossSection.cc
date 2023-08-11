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

#include "G4LENDInelasticCrossSection.hh"
#include "G4SystemOfUnits.hh"

G4double G4LENDInelasticCrossSection::getLENDCrossSection( G4GIDI_target* target , G4double ke , G4double temperature ) 
{
   if ( target == NULL ) return 0.0;
// 090407
//   return target->getElasticCrossSection( ke/MeV , temperature )*barn;
   //return target->getOthersCrossSectionAtE( ke/MeV , temperature )*barn;
   G4double result = target->getOthersCrossSectionAtE( ke/MeV , temperature )*barn;
   if ( result == 0.0 && ke/eV < 1.0e-4) 
   {
      G4double el = 1.0e-4*eV;
      G4double eh = 2.0e-4*eV;
      G4double xs_el = target->getOthersCrossSectionAtE( el/MeV , temperature )*barn;
      G4double xs_eh = target->getOthersCrossSectionAtE( eh/MeV , temperature )*barn;
      result = GetUltraLowEnergyExtrapolatedXS( el , eh , xs_el , xs_eh , ke );
   }
   return result;
}

