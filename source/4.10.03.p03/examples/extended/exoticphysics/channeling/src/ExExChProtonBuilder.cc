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
/// \file channeling/src/ExExChProtonBuilder.cc
/// \brief Implementation of the ExExChProtonBuilder class
//

#include "ExExChProtonBuilder.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"

// Wrapper
#include "XWrapperDiscreteProcess.hh"

void ExExChProtonBuilder::Build()
{
    wasActivated = true;
    std::vector<G4VProtonBuilder *>::iterator i;
    for(i=theModelCollections.begin(); i!=theModelCollections.end(); i++)
    {
        (*i)->Build(theProtonInelastic);
    }
    G4ProcessManager * theProcMan = G4Proton::Proton()->GetProcessManager();
    XWrapperDiscreteProcess* theProtonInelastic_wrapper =
        new XWrapperDiscreteProcess();
    theProtonInelastic_wrapper->RegisterProcess(theProtonInelastic,1);
    theProcMan->AddDiscreteProcess(theProtonInelastic_wrapper);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExExChProtonBuilder::
ExExChProtonBuilder(): wasActivated(false)
{
    theProtonInelastic=new G4ProtonInelasticProcess;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ExExChProtonBuilder::
~ExExChProtonBuilder()
{
    delete theProtonInelastic;
}
