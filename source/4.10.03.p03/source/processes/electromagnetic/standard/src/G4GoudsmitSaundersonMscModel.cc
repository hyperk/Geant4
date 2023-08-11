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
// $Id: G4GoudsmitSaundersonMscModel.cc 100399 2016-10-20 07:38:12Z gcosmo $
//
// ----------------------------------------------------------------------------
//
// GEANT4 Class implementation file
//
// File name:     G4GoudsmitSaundersonMscModel
//
// Author:        Mihaly Novak / (Omrane Kadri)
//
// Creation date: 20.02.2009
//
// Modifications:
// 04.03.2009 V.Ivanchenko cleanup and format according to Geant4 EM style
// 12.05.2010 O.Kadri: adding Qn1 and Qn12 as private doubles
// 18.05.2015 M. Novak provide PRELIMINARY verison of the revised class
//            This class has been revised and updated, new methods added.
//            A new version of Kawrakow-Bielajew Goudsmit-Saunderson MSC model
//            based on the screened Rutherford DCS for elastic scattering of
//            electrons/positrons has been introduced[1,2]. The corresponding MSC
//            angular distributions over a 2D parameter grid have been recomputed
//            and the CDFs are now stored in a variable transformed (smooth) form[2,3]
//            together with the corresponding rational interpolation parameters.
//            These angular distributions are handled by the new
//            G4GoudsmitSaundersonTable class that is responsible to sample if
//            it was no, single, few or multiple scattering case and delivers the
//            angular deflection (i.e. cos(theta) and sin(theta)).
//            Two screening options are provided:
//             - if fIsUsePWATotalXsecData=TRUE i.e. SetOptionPWAScreening(TRUE)
//               was called before initialisation: screening parameter value A is
//               determined such that the first transport coefficient G1(A)
//               computed according to the screened Rutherford DCS for elastic
//               scattering will reproduce the one computed from the PWA elastic
//               and first transport mean free paths[4].
//             - if fIsUsePWATotalXsecData=FALSE i.e. default value or
//               SetOptionPWAScreening(FALSE) was called before initialisation:
//               screening parameter value A is computed according to Moliere's
//               formula (by using material dependent parameters \chi_cc2 and b_c
//               precomputed for each material used at initialization in
//               G4GoudsmitSaundersonTable) [3]
//            Elastic and first trasport mean free paths are used consistently.
//            The new version is self-consistent, several times faster, more
//            robust and accurate compared to the earlier version.
//            Spin effects as well as a more accurate energy loss correction and
//            computations of Lewis moments will be implemented later on.
// 02.09.2015 M. Novak: first version of new step limit is provided.
//            fUseSafetyPlus corresponds to Urban fUseSafety (default)
//            fUseDistanceToBoundary corresponds to Urban fUseDistanceToBoundary
//            fUseSafety  corresponds to EGSnrc error-free stepping algorithm
//            Range factor can be significantly higher at each case than in Urban.
//
// Class description:
//   Kawrakow-Bielajew Goudsmit-Saunderson MSC model based on the screened
//   Rutherford DCS for elastic scattering of electrons/positrons. Step limitation
//   algorithm as well as true to geomerty and geometry to true step length
//   computations are adopted from Urban model[5].
//
// References:
//   [1] A.F.Bielajew, NIMB 111 (1996) 195-208
//   [2] I.Kawrakow, A.F.Bielajew, NIMB 134(1998) 325-336
//   [3] I.Kawrakow, E.Mainegra-Hing, D.W.O.Rogers, F.Tessier,B.R.B.Walters, NRCC
//       Report PIRS-701 (2013)
//   [4] F.Salvat, A.Jablonski, C.J. Powell, CPC 165(2005) 157-190
//   [5] L.Urban, Preprint CERN-OPEN-2006-077 (2006)
//
// -----------------------------------------------------------------------------


#include "G4GoudsmitSaundersonMscModel.hh"

#include "G4GoudsmitSaundersonTable.hh"
#include "G4PWATotalXsecTable.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4ParticleChangeForMSC.hh"
#include "G4DynamicParticle.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4LossTableManager.hh"
#include "G4Track.hh"
#include "G4PhysicsTable.hh"
#include "Randomize.hh"
#include "G4Log.hh"
#include "G4Exp.hh"
#include "G4Pow.hh"
#include <fstream>

G4GoudsmitSaundersonTable* G4GoudsmitSaundersonMscModel::fgGSTable      = 0;
G4PWATotalXsecTable*       G4GoudsmitSaundersonMscModel::fgPWAXsecTable = 0;

// set accurate energy loss and dispalcement sampling to be always on now
G4bool G4GoudsmitSaundersonMscModel::fgIsUseAccurate = true;
// set the usual optimization to be always active now
G4bool G4GoudsmitSaundersonMscModel::fgIsOptimizationOn = true;

////////////////////////////////////////////////////////////////////////////////
G4GoudsmitSaundersonMscModel::G4GoudsmitSaundersonMscModel(const G4String& nam)
  : G4VMscModel(nam),lowKEnergy(0.1*keV),highKEnergy(1.*GeV) {
  charge               = 0;
  currentMaterialIndex = -1;

  lambdalimit   = 1.*mm ;
  fr            = 0.02     ;
  rangeinit     = 0.;
  geombig       = 1.e50*mm;
  geomlimit     = geombig;
  tgeom         = geombig;
  tlimit        = 1.e+10*mm;
  presafety     = 0.*mm;

  particle         = 0;
  theManager       = G4LossTableManager::Instance();
  firstStep        = true;
  currentKinEnergy = 0.0;
  currentRange     = 0.0;

  tlimitminfix2 = 1.*nm;
  par1=par2=par3= 0.0;
  tausmall      = 1.e-16;
  mass          = electron_mass_c2;
  taulim        = 1.e-6;

  currentCouple = 0;
  fParticleChange = 0;

  // by default Moliere screeing parameter will be used but it can be set to the
  // PWA screening before calling the Initialise method
  fIsUsePWATotalXsecData = false;

  //*****************
  fLambda0 = 0.0; // elastic mean free path
  fLambda1 = 0.0; // first transport mean free path
  fScrA    = 0.0; // screening parameter
  fG1      = 0.0; // first transport coef.
  //******************
  fTheTrueStepLenght    = 0.;
  fTheTransportDistance = 0.;
  fTheZPathLenght       = 0.;
  fTheDisplacementVector.set(0.,0.,0.);
  fTheNewDirection.set(0.,0.,1.);

  fIsEverythingWasDone  = false;
  fIsMultipleSacettring = false;
  fIsSingleScattering   = false;
  fIsEndedUpOnBoundary  = false;
  fIsNoScatteringInMSC  = false;
  fIsNoDisplace         = false;
  fIsInsideSkin         = false;
  fIsWasOnBoundary      = false;
  fIsFirstRealStep      = false;

  fZeff = 1.;
  //+++++++++++++

  rndmEngineMod = G4Random::getTheEngine();

  //===================  base
  facsafety     = 0.6;
}

////////////////////////////////////////////////////////////////////////////////
G4GoudsmitSaundersonMscModel::~G4GoudsmitSaundersonMscModel(){
  if(IsMaster()){
    if(fgGSTable){
      delete fgGSTable;
      fgGSTable = 0;
    }
    if(fgPWAXsecTable){
      delete fgPWAXsecTable;
      fgPWAXsecTable = 0;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
void G4GoudsmitSaundersonMscModel::Initialise(const G4ParticleDefinition* p,
					                                    const G4DataVector&){
  SetParticle(p);
  fParticleChange = GetParticleChangeForMSC(p);
  //
  // -create static GoudsmitSaundersonTable and PWATotalXsecTable is necessary
  //
  if(IsMaster()) {

   // Could delete as well if they are exist but then the same data are reloaded
   // in case of e+ for example.
/*    if(fgGSTable) {
      delete fgGSTable;
      fgGSTable = 0;
    }
    if(fgPWAXsecTable) {
      delete fgPWAXsecTable;
      fgPWAXsecTable = 0;
    }
*/
    if(!fgGSTable)
      fgGSTable      = new G4GoudsmitSaundersonTable();

    // G4GSTable will be initialised (data are loaded) only if they are not there yet.
    fgGSTable->Initialise();

    if(fIsUsePWATotalXsecData) {
      if(!fgPWAXsecTable)
         fgPWAXsecTable = new G4PWATotalXsecTable();

      // Makes sure that all (and only those) atomic PWA data are in memory that
      // are in the current MaterilaTable.
      fgPWAXsecTable->Initialise();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// gives back the first transport mean free path in internal G4 units
G4double
G4GoudsmitSaundersonMscModel::GetTransportMeanFreePath(const G4ParticleDefinition* /*partdef*/,
                                                       G4double kineticEnergy) {
  // kinetic energy is assumed to be in Geant4 internal energy unit which is MeV
  G4double efEnergy = kineticEnergy;
  if(efEnergy<lowKEnergy) efEnergy=lowKEnergy;
  if(efEnergy>highKEnergy)efEnergy=highKEnergy;

  ///////////////////////////////////////////
  const G4Material*  mat = currentCouple->GetMaterial();

  fLambda0 = 0.0; // elastic mean free path
  fLambda1 = 0.0; // first transport mean free path
  fScrA    = 0.0; // screening parameter
  fG1      = 0.0; // first transport coef.

  if(fIsUsePWATotalXsecData) {
    // use PWA total xsec data to determine screening and lambda0 lambda1
    const G4ElementVector* theElementVector = mat->GetElementVector();
    const G4double* theAtomNumDensityVector = mat->GetVecNbOfAtomsPerVolume();
    G4int nelm = mat->GetNumberOfElements();

    int elowindx = fgPWAXsecTable->GetPWATotalXsecForZet( G4lrint((*theElementVector)[0]->GetZ()) )
                   ->GetPWATotalXsecEnergyBinIndex(efEnergy);
    for(G4int i=0;i<nelm;i++){
      int zet    = G4lrint((*theElementVector)[i]->GetZ());
      // total elastic scattering cross section in Geant4 internal length2 units
      int indx   = (G4int)(1.5 + charge*1.5); // specify that want to get the total elastic scattering xsection
      double elxsec = (fgPWAXsecTable->GetPWATotalXsecForZet(zet))->GetInterpXsec(efEnergy,elowindx,indx);
      // first transport cross section in Geant4 internal length2 units
      indx   = (G4int)(2.5 + charge*1.5); // specify that want to get the first tarnsport xsection
      double t1xsec = (fgPWAXsecTable->GetPWATotalXsecForZet(zet))->GetInterpXsec(efEnergy,elowindx,indx);
      fLambda0 += (theAtomNumDensityVector[i]*elxsec);
      fLambda1 += (theAtomNumDensityVector[i]*t1xsec);
    }
    if(fLambda0>0.0) { fLambda0 =1./fLambda0; }
    if(fLambda1>0.0) { fLambda1 =1./fLambda1; }

    // determine screening parameter such that it gives back PWA G1
    fG1=0.0;
    if(fLambda1>0.0) { fG1 = fLambda0/fLambda1; }

    fScrA = fgGSTable->GetScreeningParam(fG1);
  } else {
    // Get SCREENING FROM MOLIER

    // below 1 keV it can give bananas so prevet (1 keV)
    if(efEnergy<0.001) efEnergy = 0.001;

    // total mometum square in Geant4 internal energy2 units which is MeV2
    G4double pt2   = efEnergy*(efEnergy+2.0*electron_mass_c2);
    G4double beta2 = pt2/(pt2+electron_mass_c2*electron_mass_c2);
    G4int    matindx = mat->GetIndex();
    G4double bc = fgGSTable->GetMoliereBc(matindx);
    fScrA    = fgGSTable->GetMoliereXc2(matindx)/(4.0*pt2*bc);
    // total elastic mean free path in Geant4 internal lenght units
    fLambda0 = beta2/bc;//*(1.+fScrA);
    fG1      = 2.0*fScrA*((1.0+fScrA)*G4Log(1.0/fScrA+1.0)-1.0);
    fLambda1 = fLambda0/fG1;
  }

  return fLambda1;
}

////////////////////////////////////////////////////////////////////////////////
G4double
G4GoudsmitSaundersonMscModel::GetTransportMeanFreePathOnly(const G4ParticleDefinition* /*partdef*/,
                                                       G4double kineticEnergy) {
  // kinetic energy is assumed to be in Geant4 internal energy unit which is MeV
  G4double efEnergy = kineticEnergy;
  if(efEnergy<lowKEnergy) efEnergy=lowKEnergy;
  if(efEnergy>highKEnergy)efEnergy=highKEnergy;

  ///////////////////////////////////////////
  const G4Material*  mat = currentCouple->GetMaterial();

  G4double lambda0 = 0.0; // elastc mean free path
  G4double lambda1 = 0.0; // first transport mean free path
  G4double scrA    = 0.0; // screening parametr
  G4double g1      = 0.0; // first transport mean free path

  if(fIsUsePWATotalXsecData) {
    // use PWA total xsec data to determine screening and lambda0 lambda1
    const G4ElementVector* theElementVector = mat->GetElementVector();
    const G4double* theAtomNumDensityVector = mat->GetVecNbOfAtomsPerVolume();
    G4int nelm = mat->GetNumberOfElements();

    int elowindx = fgPWAXsecTable->GetPWATotalXsecForZet( G4lrint((*theElementVector)[0]->GetZ()) )
                   ->GetPWATotalXsecEnergyBinIndex(efEnergy);
    for(G4int i=0;i<nelm;i++){
      int zet    = G4lrint((*theElementVector)[i]->GetZ());
      // first transport cross section in Geant4 internal length2 units
      int indx   = (G4int)(2.5 + charge*1.5); // specify that want to get the first tarnsport xsection
      double t1xsec = (fgPWAXsecTable->GetPWATotalXsecForZet(zet))->GetInterpXsec(efEnergy,elowindx,indx);
      lambda1 += (theAtomNumDensityVector[i]*t1xsec);
    }
    if(lambda1>0.0) { lambda1 =1./lambda1; }

  } else {
    // Get SCREENING FROM MOLIER

    // below 1 keV it can give bananas so prevet (1 keV)
    if(efEnergy<0.001) efEnergy = 0.001;

    // total mometum square in Geant4 internal energy2 units which is MeV2
    G4double pt2   = efEnergy*(efEnergy+2.0*electron_mass_c2);
    G4double beta2 = pt2/(pt2+electron_mass_c2*electron_mass_c2);
    G4int    matindx = mat->GetIndex();
    G4double bc = fgGSTable->GetMoliereBc(matindx);
    scrA    = fgGSTable->GetMoliereXc2(matindx)/(4.0*pt2*bc);
    // total elastic mean free path in Geant4 internal lenght units
    lambda0 = beta2/bc;//*(1.+scrA);
    g1      = 2.0*scrA*((1.0+scrA)*G4Log(1.0/scrA+1.0)-1.0);
    lambda1 = lambda0/g1;
  }

  return lambda1;
}

////////////////////////////////////////////////////////////////////////////////
void G4GoudsmitSaundersonMscModel::StartTracking(G4Track* track)
{
  SetParticle(track->GetDynamicParticle()->GetDefinition());
  firstStep = true;
  tlimit = tgeom = rangeinit = geombig;
}


////////////////////////////////////////////////////////////////////////////////
G4double G4GoudsmitSaundersonMscModel::ComputeTruePathLengthLimit(
                                                  const G4Track& track,
			                                            G4double& currentMinimalStep){

  G4double skindepth = 0.;

  const G4DynamicParticle* dp = track.GetDynamicParticle();

  G4StepPoint* sp = track.GetStep()->GetPreStepPoint();
  G4StepStatus stepStatus = sp->GetStepStatus();
  currentCouple = track.GetMaterialCutsCouple();
  SetCurrentCouple(currentCouple);
  currentMaterialIndex = currentCouple->GetIndex();
  currentKinEnergy = dp->GetKineticEnergy();
  currentRange = GetRange(particle,currentKinEnergy,currentCouple);



  // *** Elastic and first transport mfp, fScrA and fG1 are also set in this !!!
  fLambda1 = GetTransportMeanFreePath(particle,currentKinEnergy);

  //
  // Set initial values:
  //  : lengths are initialised to currentMinimalStep  which is the true, minimum
  //    step length from all other physics
  fTheTrueStepLenght    = currentMinimalStep;
  fTheTransportDistance = currentMinimalStep;
  fTheZPathLenght       = currentMinimalStep;  // will need to be converted
  fTheDisplacementVector.set(0.,0.,0.);
  fTheNewDirection.set(0.,0.,1.);

  // Can everything be done in the step limit phase ?
  fIsEverythingWasDone  = false;
  // Multiple scattering needs to be sample ?
  fIsMultipleSacettring = false;
  // Single scattering needs to be sample ?
  fIsSingleScattering   = false;
  // Was zero deflection in multiple scattering sampling ?
  fIsNoScatteringInMSC  = false;
  // Do not care about displacement in MSC sampling
  // ( used only in the case of fgIsOptimizationOn = true)
  fIsNoDisplace = false;

  // get pre-step point safety
  presafety = sp->GetSafety();

  // Zeff  = TotNbOfElectPerVolume/TotNbOfAtomsPerVolume
  fZeff = currentCouple->GetMaterial()->GetIonisation()->GetZeffective();

  // distance will take into account max-fluct.
  G4double distance = currentRange;
  distance *= (1.20-fZeff*(1.62e-2-9.22e-5*fZeff));



  //
  // Possible optimization : if the distance is samller than the safety -> the
  // particle will never leave this volume -> dispalcement
  // as the effect of multiple elastic scattering can be skipped
  // Important : this optimization can cause problems if one does scoring
  // in a bigger volume since MSC won't be done deep inside the volume when
  // distance < safety so don't use optimized-mode in such case.
  if( fgIsOptimizationOn && (distance < presafety) ) {
     // Indicate that we need to do MSC after transportation and no dispalcement.
     fIsMultipleSacettring = true;
     fIsNoDisplace = true;
  } else if( steppingAlgorithm == fUseDistanceToBoundary ){
       //Compute geomlimit (and presafety) :
       // - geomlimit will be:
       //    == the straight line distance to the boundary if currentRange is
       //       longer than that
       //    == a big value [geombig = 1.e50*mm] if currentRange is shorter than
       //       the straight line distance to the boundary
       // - presafety will be updated as well
       // So the particle can travell 'gemlimit' distance (along a straight
       // line!) in its current direction:
       //  (1) before reaching a boundary (geomlimit < geombig) OR
       //  (2) before reaching its current range (geomlimit == geombig)
       geomlimit = ComputeGeomLimit(track, presafety, currentRange);

       // Record that the particle is on a boundary
       if( (stepStatus==fGeomBoundary) || (stepStatus==fUndefined && presafety==0.0))
          fIsWasOnBoundary = true;

       // Set skin depth = skin x elastic_mean_free_path
       skindepth = skin*fLambda0;
       // Init the flag that indicates that the particle are within a skindepth
       // distance from a boundary
       fIsInsideSkin = false;
       // Check if we can try Single Scattering because we are within skindepth
       // distance from/to a boundary OR the current minimum true-step-length is
       // shorter than skindepth. NOTICE: the latest has only efficieny reasons
       // because the MSC angular sampling is fine for any short steps but much
       // faster to try single scattering in case of short steps.
       if( (stepStatus == fGeomBoundary) || (presafety < skindepth) || (fTheTrueStepLenght < skindepth)){
         // check if we are within skindepth distance from a boundary
         if( (stepStatus == fGeomBoundary) || (presafety < skindepth))
           fIsInsideSkin = true;
         //Try single scattering:
         // - sample distance to next single scattering interaction (sslimit)
         // - compare to current minimum length
         //      == if sslimit is the shorter:
         //          - set the step length to sslimit
         //          - indicate that single scattering needs to be done
         //      == else : nothing to do
         //- in both cases, the step length was very short so geometrical and
         //  true path length are the same
         G4double sslimit = -1.*fLambda0*G4Log(rndmEngineMod->flat());
         // compare to current minimum step length
         if(sslimit < fTheTrueStepLenght) {
           fTheTrueStepLenght  = sslimit;
           fIsSingleScattering = true;
         }

          // short step -> true step length equal to geometrical path length
          fTheZPathLenght     = fTheTrueStepLenght;
          // Set taht everything is done in step-limit phase so no MSC call
          // We will check if we need to perform the single-scattering angular
          // sampling i.e. if single elastic scattering was the winer!
          fIsEverythingWasDone = true;
       } else {
       // After checking we know that we cannot try single scattering so we will
       // need to make an MSC step
         // Indicate that we need to make and MSC step. We do not check if we can
         // do it now i.e. if presafety>final_true_step_length so we let the
         // fIsEverythingWasDone = false which indicates that we will perform
         // MSC after transportation.
         fIsMultipleSacettring = true;

         // Init the first-real-step falg: it will indicate if we do the first
         // non-single scattering step in this volume with this particle
         fIsFirstRealStep = false;
         // If previously the partcile was on boundary it was within skin as
         // well. When it is not within skin anymore it has just left the skin
         // so we make the first real MSC step with the particle.
         if(fIsWasOnBoundary && !fIsInsideSkin){
             // reset the 'was on boundary' indicator flag
             fIsWasOnBoundary = false;
             fIsFirstRealStep = true;
         }

         // If this is the first-real msc step (the partcile has just left the
         // skin) or this is the first step with the particle (was born or
         // primary):
         //   - set the initial range that will be used later to limit its step
         //     (only in this volume, because after boundary crossing at the
         //     first-real MSC step we will reset)
         //  - don't let the partcile to cross the volume just in one step
         if(firstStep || fIsFirstRealStep){
           rangeinit = currentRange;
           // If geomlimit < geombig than the particle might reach the boundary
           // along its initial direction before losing its energy (in this step)
           // Otherwise we can be sure that the particle will lose it energy
           // before reaching the boundary along a starigth line so there is no
           // geometrical limit appalied. [However, tgeom is set only in the
           // first or the first-real MSC step. After the first or first real
           // MSC step the direction will change tgeom won't guaranty anything!
           // But we will try to end up within skindepth from the boundary using
           // the actual value of geomlimit(See later at step reduction close to
           // boundary).]
           if(geomlimit < geombig){
               // transfrom straight line distance to the boundary to real step
               // length based on the mean values (using the prestep point
               // first-transport mean free path i.e. no energy loss correction)
               if((1.-geomlimit/fLambda1) > 0.)
                 geomlimit = -fLambda1*G4Log(1.-geomlimit/fLambda1);
              // the 2-different case that could lead us here
              if(firstStep)
                tgeom = 2.*geomlimit/facgeom;
               else
                tgeom = geomlimit/facgeom;
           } else {
             tgeom = geombig;
           }
         }

         // True step length limit from range factor. Noteice, that the initial
         // range is used that was set at the first step or first-real MSC step
         // in this volume with this particle.
         tlimit = facrange*rangeinit;
         // Take the minimum of the true step length limits coming from
         // geometrical constraint or range-factor limitation
         tlimit = std::min(tlimit,tgeom);

         // Step reduction close to boundary: we try to end up within skindepth
         // from the boundary ( Notice: in case of mag. field it might not work
         // because geomlimit is the straigth line distance to the boundary in
         // the currect direction (if geomlimit<geombig) and mag. field can
         // change the initial direction. So te particle might hit some boundary
         // before in a different direction. However, here we restrict the true
         // path length to this (straight line) lenght so the corresponding
         // transport distance (straight line) will be even shorter than
         // geomlimit-0.999*skindepth after the change of true->geom.
         if(geomlimit < geombig)
           tlimit = std::min(tlimit, geomlimit-0.999*skindepth);

         // randomize 1st step or 1st 'normal' step in volume
         if(firstStep || fIsFirstRealStep) {
            fTheTrueStepLenght = std::min(fTheTrueStepLenght, Randomizetlimit());
         } else {
           fTheTrueStepLenght = std::min(fTheTrueStepLenght, tlimit);
         }
    }
  } else if (steppingAlgorithm == fUseSafety) { // THE ERROR_FREE stepping alg.
    presafety =  ComputeSafety(sp->GetPosition(),fTheTrueStepLenght);
    geomlimit = presafety;

    // Set skin depth = skin x elastic_mean_free_path
    skindepth = skin*fLambda0;
    // Check if we can try Single Scattering because we are within skindepth
    // distance from/to a boundary OR the current minimum true-step-length is
    // shorter than skindepth. NOTICE: the latest has only efficieny reasons
    // because the MSC angular sampling is fine for any short steps but much
    // faster to try single scattering in case of short steps.
    if( (stepStatus == fGeomBoundary) || (presafety < skindepth) || (fTheTrueStepLenght < skindepth)){
      //Try single scattering:
      // - sample distance to next single scattering interaction (sslimit)
      // - compare to current minimum length
      //      == if sslimit is the shorter:
      //          - set the step length to sslimit
      //          - indicate that single scattering needs to be done
      //      == else : nothing to do
      //- in both cases, the step length was very short so geometrical and
      //  true path length are the same
      G4double sslimit = -1.*fLambda0*G4Log(rndmEngineMod->flat());
      // compare to current minimum step length
      if(sslimit < fTheTrueStepLenght) {
        fTheTrueStepLenght  = sslimit;
        fIsSingleScattering = true;
      }

       // short step -> true step length equal to geometrical path length
       fTheZPathLenght     = fTheTrueStepLenght;
       // Set taht everything is done in step-limit phase so no MSC call
       // We will check if we need to perform the single-scattering angular
       // sampling i.e. if single elastic scattering was the winer!
       fIsEverythingWasDone = true;
    } else {
    // After checking we know that we cannot try single scattering so we will
    // need to make an MSC step
      // Indicate that we need to make and MSC step.
      fIsMultipleSacettring = true;
      fIsEverythingWasDone  = true;

      // limit from range factor
      fTheTrueStepLenght = std::min(fTheTrueStepLenght, facrange*currentRange);

      // never let the particle go further than the safety if we are out of the skin
      // if we are here we are out of the skin, presafety > 0.
      if(fTheTrueStepLenght > presafety)
        fTheTrueStepLenght = std::min(fTheTrueStepLenght, presafety);

      // make sure that we are still within the aplicability of condensed histry model
      // i.e. true step length is not longer than first transport mean free path.
      // We schould take into account energy loss along 0.5x lambda_transport1
      // step length as well. So let it 0.4 x lambda_transport1
      fTheTrueStepLenght = std::min(fTheTrueStepLenght, fLambda1*0.4);
    }
  } else {

    // This is the default stepping algorithm: the fastest but the least
    // accurate that corresponds to fUseSafety in Urban model. Note, that GS
    // model can handle any short steps so we do not need the minimum limits

    // NO single scattering in case of skin or short steps (by defult the MSC
    // model will be single or even no scattering in case of short steps
    // compared to the elastic mean free path.)

    // indicate that MSC needs to be done (always and always after transportation)
    fIsMultipleSacettring = true;

      if( stepStatus != fGeomBoundary ) {
        presafety = ComputeSafety(sp->GetPosition(),fTheTrueStepLenght);
      }

      // Far from boundary-> in optimized mode do not sample dispalcement.
      if( (distance < presafety) && (fgIsOptimizationOn)) {
        fIsNoDisplace = true;
      } else {
        // Urban like
        if(firstStep || (stepStatus == fGeomBoundary)) {
          rangeinit = currentRange;
          fr = facrange;
// We don't use this: we won't converge to the single scattering results with
//                    decreasing range-factor.          
//              rangeinit = std::max(rangeinit, fLambda1);
//              if(fLambda1 > lambdalimit) {
//                fr *= (0.75+0.25*fLambda1/lambdalimit);
//              }

        }

        //step limit
        tlimit = std::max(fr*rangeinit, facsafety*presafety);

        // first step randomization
        if(firstStep || stepStatus == fGeomBoundary) {
          fTheTrueStepLenght = std::min(fTheTrueStepLenght, Randomizetlimit());
        } else {
          fTheTrueStepLenght = std::min(fTheTrueStepLenght, tlimit);
        }
      }
  }

  // unset first-step
  firstStep =false;

  // performe single scattering, multiple scattering if this later can be done safely here
  if(fIsEverythingWasDone){
    if(fIsSingleScattering){
      // sample single scattering
      G4double cost,sint,cosPhi,sinPhi;
      SingleScattering(cost, sint);
      G4double phi = CLHEP::twopi*rndmEngineMod->flat();
      sinPhi = std::sin(phi);
      cosPhi = std::cos(phi);
      fTheNewDirection.set(sint*cosPhi,sint*sinPhi,cost);
    } else if(fIsMultipleSacettring){
      // sample multiple scattering
      SampleMSC(); // fTheZPathLenght, fTheDisplacementVector and fTheNewDirection will be set
    } // and if single scattering but it was longer => nothing to do
  } //else { do nothing here but after transportation

  return ConvertTrueToGeom(fTheTrueStepLenght,currentMinimalStep);
}


////////////////////////////////////////////////////////////////////////////////
G4double G4GoudsmitSaundersonMscModel::ComputeGeomPathLength(G4double)
{
  //
  // convert true ->geom
  // It is called from the step limitation ComputeTruePathLengthLimit if
  // !fIsEverythingWasDone but protect:
  //
  par1 = -1. ;
  par2 = par3 = 0. ;

  // if fIsEverythingWasDone = TRUE  => fTheZPathLenght is already set
  // so return with the already known value
  // Otherwise:
  if(!fIsEverythingWasDone) {
     // this correction needed to run MSC with eIoni and eBrem inactivated
     // and makes no harm for a normal run
     fTheTrueStepLenght = std::min(fTheTrueStepLenght,currentRange);

     //  do the true -> geom transformation
     fTheZPathLenght = fTheTrueStepLenght;

     // z = t for very small true-path-length
     if(fTheTrueStepLenght < tlimitminfix2) return fTheZPathLenght;

     G4double tau = fTheTrueStepLenght/fLambda1;

     if (tau <= tausmall) {
       fTheZPathLenght = std::min(fTheTrueStepLenght, fLambda1);
     } else  if (fTheTrueStepLenght < currentRange*dtrl) {
       if(tau < taulim) fTheZPathLenght = fTheTrueStepLenght*(1.-0.5*tau) ;
       else             fTheZPathLenght = fLambda1*(1.-G4Exp(-tau));
     } else if(currentKinEnergy < mass || fTheTrueStepLenght == currentRange)  {
       par1 = 1./currentRange ;     // alpha =1/range_init for Ekin<mass
       par2 = 1./(par1*fLambda1) ;  // 1/(alphaxlambda01)
       par3 = 1.+par2 ;             // 1+1/
       if(fTheTrueStepLenght < currentRange) {
           fTheZPathLenght = 1./(par1*par3) * (1.-std::pow(1.-par1*fTheTrueStepLenght,par3));
       } else {
         fTheZPathLenght = 1./(par1*par3);
       }

    } else {
      G4double rfin = std::max(currentRange-fTheTrueStepLenght, 0.01*currentRange);
      G4double T1 = GetEnergy(particle,rfin,currentCouple);
      G4double lambda1 = GetTransportMeanFreePathOnly(particle,T1);

      par1 = (fLambda1-lambda1)/(fLambda1*fTheTrueStepLenght);  // alpha
      par2 = 1./(par1*fLambda1);
      par3 = 1.+par2 ;
      G4Pow *g4calc = G4Pow::GetInstance();
      fTheZPathLenght = 1./(par1*par3) * (1.-g4calc->powA(1.-par1*fTheTrueStepLenght,par3));
    }
  }
  fTheZPathLenght = std::min(fTheZPathLenght, fLambda1);

 return fTheZPathLenght;
}

////////////////////////////////////////////////////////////////////////////////
G4double G4GoudsmitSaundersonMscModel::ComputeTrueStepLength(G4double geomStepLength)
{
  // init
  fIsEndedUpOnBoundary = false;

  // step defined other than transportation
  if(geomStepLength == fTheZPathLenght) {
    return fTheTrueStepLenght;
  }

  // else ::
  // - set the flag that transportation was the winer so DoNothin in DOIT !!
  // - convert geom -> true by using the mean value
  fIsEndedUpOnBoundary = true; // OR LAST STEP

  fTheZPathLenght = geomStepLength;

  // was a short single scattering step
  if(fIsEverythingWasDone && !fIsMultipleSacettring) {
    fTheTrueStepLenght = geomStepLength;
    return fTheTrueStepLenght;
  }

  // t = z for very small step
  if(geomStepLength < tlimitminfix2) {
    fTheTrueStepLenght = geomStepLength;
  // recalculation
  } else {
    G4double tlength = geomStepLength;
    if(geomStepLength > fLambda1*tausmall ) {
      if(par1 <  0.) {
        tlength = -fLambda1*G4Log(1.-geomStepLength/fLambda1) ;
      } else {
       if(par1*par3*geomStepLength < 1.) {
           G4Pow *g4calc = G4Pow::GetInstance();
           tlength = (1.-g4calc->powA( 1.-par1*par3*geomStepLength,1./par3))/par1;
        } else {
          tlength = currentRange;
        }
      }
     if(tlength < geomStepLength)   { tlength = geomStepLength; }
     else if(tlength > fTheTrueStepLenght) { tlength = geomStepLength; }
    }
    fTheTrueStepLenght = tlength;
  }
  return fTheTrueStepLenght;
}

////////////////////////////////////////////////////////////////////////////////
G4ThreeVector&
G4GoudsmitSaundersonMscModel::SampleScattering(const G4ThreeVector& oldDirection, G4double) {
  if(steppingAlgorithm == fUseDistanceToBoundary && fIsEverythingWasDone && fIsSingleScattering){ // ONLY single scattering is done in advance
      // single scattering was and scattering happend
      fTheNewDirection.rotateUz(oldDirection);
//      fTheDisplacementVector.set(0.,0.,0.);
      fParticleChange->ProposeMomentumDirection(fTheNewDirection);
      return fTheDisplacementVector;
  } else if(steppingAlgorithm == fUseSafety) {
      if(fIsEndedUpOnBoundary) {// do nothing
//        fTheDisplacementVector.set(0.,0.,0.);
        return fTheDisplacementVector;
      } else if(fIsEverythingWasDone) {// evrything is done if not optimizations case !!!
        // check single scattering and see if it happened
        if(fIsSingleScattering) {
          fTheNewDirection.rotateUz(oldDirection);
  //        fTheDisplacementVector.set(0.,0.,0.);
          fParticleChange->ProposeMomentumDirection(fTheNewDirection);
          return fTheDisplacementVector;
        }

        // check if multiple scattering happened and do things only if scattering was really happening
        if(fIsMultipleSacettring && !fIsNoScatteringInMSC){
             fTheNewDirection.rotateUz(oldDirection);
             fTheDisplacementVector.rotateUz(oldDirection);
             fParticleChange->ProposeMomentumDirection(fTheNewDirection);
        }
        // The only thing that could happen if we are here (fUseSafety and fIsEverythingWasDone)
        // is that  single scattering was tried but did not win so scattering did not happen.
        // So no displacement and no scattering
        return fTheDisplacementVector;
     }
     //
     // The only thing that could still happen with fUseSafety is that we are in the
     // optimization branch: so sample MSC angle here (no displacement)
  }

 //else
   // MSC needs to be done here
   SampleMSC();
   if(!fIsNoScatteringInMSC) {
      fTheNewDirection.rotateUz(oldDirection);
      fParticleChange->ProposeMomentumDirection(fTheNewDirection);
      if(!fIsNoDisplace)
        fTheDisplacementVector.rotateUz(oldDirection);
   }
   return fTheDisplacementVector;
}


void G4GoudsmitSaundersonMscModel::SingleScattering(G4double &cost, G4double &sint){
    G4double rand1 = rndmEngineMod->flat();
    // sampling 1-cos(theta)
    G4double dum0  = 2.0*fScrA*rand1/(1.0 - rand1 + fScrA);
    // add protections
    if(dum0 < 0.0)       dum0 = 0.0;
    else if(dum0 > 2.0 ) dum0 = 2.0;
    // compute cos(theta) and sin(theta)
    cost = 1.0 - dum0;
    sint = std::sqrt(dum0*(2.0 - dum0));
}


////////////////////////////////////////////////////////////////////////////////
void G4GoudsmitSaundersonMscModel::SampleMSC(){
  fIsNoScatteringInMSC = false;
  // kinetic energy is assumed to be in Geant4 internal energy unit which is MeV
  G4double kineticEnergy = currentKinEnergy;

  ///////////////////////////////////////////
  // Energy loss correction: 2 version
  G4double eloss  = 0.0;
//  if (fTheTrueStepLenght > currentRange*dtrl) {
    eloss = kineticEnergy -
      GetEnergy(particle,currentRange-fTheTrueStepLenght,currentCouple);
//  } else {
//    eloss = fTheTrueStepLenght*GetDEDX(particle,kineticEnergy,currentCouple);
//  }


  G4double tau  = 0.;//    = kineticEnergy/electron_mass_c2; // where kinEnergy is the mean kinetic energy
  G4double tau2 = 0.;//   = tau*tau;
  G4double eps0 = 0.;//   = eloss/kineticEnergy0; // energy loss fraction to the begin step energy
  G4double epsm = 0.;//   = eloss/kineticEnergy;  // energy loss fraction to the mean step energy


  // - init.
  G4double efEnergy = kineticEnergy;
  G4double efStep   = fTheTrueStepLenght;

  G4double kineticEnergy0 = kineticEnergy;
  if(fgIsUseAccurate) {    // - use accurate energy loss correction
     kineticEnergy -= 0.5*eloss;  // mean energy along the full step

     // other parameters for energy loss corrections
     tau    = kineticEnergy/electron_mass_c2; // where kinEnergy is the mean kinetic energy
     tau2   = tau*tau;
     eps0   = eloss/kineticEnergy0; // energy loss fraction to the begin step energy
     epsm   = eloss/kineticEnergy;  // energy loss fraction to the mean step energy

      efEnergy       = kineticEnergy * (1. - epsm*epsm*(6.+10.*tau+5.*tau2)/(24.*tau2+48.*tau+72.));
      G4double dum  = 0.166666*(4.+tau*(6.+tau*(7.+tau*(4.+tau))))*
                     (epsm/((tau+1.)*(tau+2.)))*(epsm/((tau+1.)*(tau+2.)));
      efStep =fTheTrueStepLenght*(1.-dum);
  } else {                              // - take only mean energy
      kineticEnergy -= 0.5*eloss;  // mean energy along the full step
      efEnergy       = kineticEnergy;
      G4double factor = 1./(1. + 0.9784671*kineticEnergy); //0.9784671 = 1/(2*rm)
      eps0= eloss/kineticEnergy0;
      epsm= eps0/(1.-0.5*eps0);
      G4double temp   = 0.3*(1. - factor*(1. - 0.333333*factor))*eps0*eps0;
      efStep = fTheTrueStepLenght*(1. + temp);
  }

  ///////////////////////////////////////////
  // Compute elastic mfp, first transport mfp, screening parameter, and G1
  fLambda1 = GetTransportMeanFreePath(particle,efEnergy);

  G4double lambdan=0.;

  if(fLambda0>0.0) { lambdan=efStep/fLambda0; }
  if(lambdan<=1.0e-12) {
      if(fIsEverythingWasDone)
        fTheZPathLenght = fTheTrueStepLenght;
    fIsNoScatteringInMSC = true;
    return;
  }

  // correction from neglected term 1+A (only for Moliere parameter)
  if(!fIsUsePWATotalXsecData)  
    lambdan = lambdan/(1.+fScrA);

  G4double Qn1 = lambdan *fG1;//2.* lambdan *scrA*((1.+scrA)*log(1.+1./scrA)-1.);

  /////////////////////////////////////////////////////////////////////////////
  // Sample scattering angles
  // new direction, relative to the orriginal one is in {uss,vss,wss}
  G4double cosTheta1 =1.0,sinTheta1 =0.0, cosTheta2 =1.0, sinTheta2 =0.0;
  G4double cosPhi1=1.0,sinPhi1=0.0, cosPhi2 =1.0, sinPhi2 =0.0;
  G4double uss=0.0,vss=0.0,wss=1.0,x_coord=0.0,y_coord=0.0,z_coord=1.0;
  G4double u2=0.0,v2=0.0;

  // if we are above the upper grid limit with lambdaxG1=true-length/first-trans-mfp
  // => izotropic distribution: lambG1_max =7.992 but set it to 7
  if(0.5*Qn1 > 7.0){
    G4double vrand[2];
    rndmEngineMod->flatArray(2,vrand); //get 2 random number
    cosTheta1 = 1.-2.*vrand[0];
    sinTheta1 = std::sqrt((1.-cosTheta1)*(1.+cosTheta1));
    cosTheta2 = 1.-2.*vrand[1];
    sinTheta2 = std::sqrt((1.-cosTheta2)*(1.+cosTheta2));
  } else {
     // sample 2 scattering cost1, sint1, cost2 and sint2 for half path
     fgGSTable->Sampling(0.5*lambdan, 0.5*Qn1, fScrA, cosTheta1, sinTheta1);
     fgGSTable->Sampling(0.5*lambdan, 0.5*Qn1, fScrA, cosTheta2, sinTheta2);
     if(cosTheta1 + cosTheta2 == 2.) { // no scattering happened
        if(fIsEverythingWasDone)
           fTheZPathLenght = fTheTrueStepLenght;
        fIsNoScatteringInMSC = true;
        return;
     }
  }
  // sample 2 azimuthal angles
  G4double vrand[2];
  rndmEngineMod->flatArray(2,vrand); //get 2 random number
  G4double phi1 = CLHEP::twopi*vrand[0];
  sinPhi1 = std::sin(phi1);
  cosPhi1 = std::cos(phi1);
  G4double phi2 = CLHEP::twopi*vrand[1];
  sinPhi2 = std::sin(phi2);
  cosPhi2 = std::cos(phi2);

  // compute final direction realtive to z-dir
  u2  = sinTheta2*cosPhi2;
  v2  = sinTheta2*sinPhi2;
  G4double u2p = cosTheta1*u2 + sinTheta1*cosTheta2;
  uss  = u2p*cosPhi1 - v2*sinPhi1;
  vss  = u2p*sinPhi1 + v2*cosPhi1;
  wss  = cosTheta1*cosTheta2 - sinTheta1*u2;

  ////////////////////////////////////////////////////////////////////
  fTheNewDirection.set(uss,vss,wss);

  // set the fTheZPathLenght if we don't sample displacement and
  // we should do everything at the step-limit-phase before we return
  if(fIsNoDisplace && fIsEverythingWasDone)
    fTheZPathLenght = fTheTrueStepLenght;

  // in optimized-mode if the current-safety > current-range we do not use dispalcement
  if(fIsNoDisplace)
    return;

  //////////////////////////////////////////////////////////////////////
  // Compute final position
  if(fgIsUseAccurate) {
     // correction parameter
     G4double par =1.;
     if(Qn1<0.7) par = 1.;
     else if (Qn1<7.0) par = -0.031376*Qn1+1.01356;
     else par = 0.79;

     // Moments with energy loss correction
     // --first the uncorrected (for energy loss) values of gamma, eta, a1=a2=0.5*(1-eta), delta
     // gamma = G_2/G_1 based on G2 computed from A by using the Wentzel DCS form of G2
     G4double loga   = G4Log(1.0+1.0/fScrA);
     G4double gamma  = 6.0*fScrA*(1.0 + fScrA)*(loga*(1.0 + 2.0*fScrA) - 2.0)/fG1;
     // sample eta from p(eta)=2*eta i.e. P(eta) = eta_square ;-> P(eta) = rand --> eta = sqrt(rand)
     G4double eta    = std::sqrt(rndmEngineMod->flat());
     G4double eta1   = 0.5*(1 - eta);  // used  more than once
     // 0.5 +sqrt(6)/6 = 0.9082483;
     // 1/(4*sqrt(6))  = 0.1020621;
     // (4-sqrt(6)/(24*sqrt(6))) = 0.026374715
     // delta = 0.9082483-(0.1020621-0.0263747*gamma)*Qn1 without energy loss cor.
     G4double delta  = 0.9082483-(0.1020621-0.0263747*gamma)*Qn1;

     // compute alpha1 and alpha2 for energy loss correction
     G4double temp1 = 2.0 + tau;
     G4double temp  = (2.0+tau*temp1)/((tau+1.0)*temp1);
     //Take logarithmic dependence
     temp = temp - (tau+1.0)/((tau+2.0)*(loga*(1.0+fScrA)-1.0));
     temp = temp * epsm;
     temp1 = 1.0 - temp;
     delta = delta + 0.40824829*(eps0*(tau+1.0)/((tau+2.0)*
             (loga*(1.0+fScrA)-1.0)*(loga*(1.0+2.0*fScrA)-2.0)) - 0.25*temp*temp);
     G4double b      = eta*delta;
     G4double c      = eta*(1.0-delta);

     //Calculate transport direction cosines:
     // ut,vt,wt is the final position divided by the true step length
     G4double w1v2 = cosTheta1*v2;
     G4double ut   = b*sinTheta1*cosPhi1 + c*(cosPhi1*u2 - sinPhi1*w1v2) + eta1*uss*temp1;
     G4double vt   = b*sinTheta1*sinPhi1 + c*(sinPhi1*u2 + cosPhi1*w1v2) + eta1*vss*temp1;
     G4double wt   = eta1*(1+temp) +       b*cosTheta1 +  c*cosTheta2    + eta1*wss*temp1;

     // long step correction
     ut *=par;
     vt *=par;
     wt *=par;

     // final position relative to the pre-step point in the scattering frame
     // ut = x_f/s so needs to multiply by s
     x_coord = ut*fTheTrueStepLenght;
     y_coord = vt*fTheTrueStepLenght;
     z_coord = wt*fTheTrueStepLenght;

     if(fIsEverythingWasDone){
       // We sample in the step limit so set fTheZPathLenght = transportDistance
       // and lateral displacement (x_coord,y_coord,z_coord-transportDistance)
       //Calculate transport distance
       G4double transportDistance  = std::sqrt(x_coord*x_coord+y_coord*y_coord+z_coord*z_coord);
       // protection
       if(transportDistance>fTheTrueStepLenght)
          transportDistance = fTheTrueStepLenght;
       fTheZPathLenght = transportDistance;
     }
     // else:: we sample in the DoIt so
     //       the fTheZPathLenght was already set and was taken as transport along zet
     fTheDisplacementVector.set(x_coord,y_coord,z_coord-fTheZPathLenght);
  } else {
     // compute zz = <z>/tPathLength
     // s -> true-path-length
     // z -> geom-path-length:: when PRESTA is used z =(def.) <z>
     // r -> lateral displacement = s/2 sin(theta)  => x_f = r cos(phi); y_f = r sin(phi)
     G4double zz = 0.0;
     if(fIsEverythingWasDone){
        // We sample in the step limit so set fTheZPathLenght = transportDistance
        // and lateral displacement (x_coord,y_coord,z_coord-transportDistance)
        if(Qn1<0.1) { // use 3-order Taylor approximation of (1-exp(-x))/x around x=0
          zz = 1.0 - Qn1*(0.5 - Qn1*(0.166666667 - 0.041666667*Qn1)); // 1/6 =0.166..7 ; 1/24=0.041..
        } else {
          zz = (1.-G4Exp(-Qn1))/Qn1;
        }
     } else {
        // we sample in the DoIt so
        // the fTheZPathLenght was already set and was taken as transport along zet
        zz = fTheZPathLenght/fTheTrueStepLenght;
     }

     G4double rr = (1.-zz*zz)/(1.-wss*wss); // s^2 >= <z>^2+r^2  :: where r^2 = s^2/4 sin^2(theta)
     if(rr >= 0.25) rr = 0.25;            // (1-<z>^2/s^2)/sin^2(theta) >= r^2/(s^2 sin^2(theta)) = 1/4 must hold
     G4double rperp = fTheTrueStepLenght*std::sqrt(rr);  // this is r/sint
     x_coord  = rperp*uss;
     y_coord  = rperp*vss;
     z_coord  = zz*fTheTrueStepLenght;

     if(fIsEverythingWasDone){
       G4double transportDistance = std::sqrt(x_coord*x_coord + y_coord*y_coord + z_coord*z_coord);
       fTheZPathLenght = transportDistance;
     }

     fTheDisplacementVector.set(x_coord,y_coord,z_coord- fTheZPathLenght);
   }
}

/*
////////////////////////////////////////////////////////////////////////////////
void G4GoudsmitSaundersonMscModel::SampleMSC(){
  fIsNoScatteringInMSC = false;
  // kinetic energy is assumed to be in Geant4 internal energy unit which is MeV
  G4double kineticEnergy = currentKinEnergy;

  ///////////////////////////////////////////
  // Energy loss correction: 2 versions
  G4double eloss  = 0.0;
//  if (fTheTrueStepLenght > currentRange*dtrl) {
    eloss = kineticEnergy -
      GetEnergy(particle,currentRange-fTheTrueStepLenght,currentCouple);
//  } else {
//    eloss = fTheTrueStepLenght*GetDEDX(particle,kineticEnergy,currentCouple);
//  }
//eloss*=2.;

  G4double tau  = 0.;//    = kineticEnergy/electron_mass_c2; // where kinEnergy is the mean kinetic energy
  G4double tau2 = 0.;//   = tau*tau;
  G4double eps0 = 0.;//   = eloss/kineticEnergy0; // energy loss fraction to the begin step energy
  G4double epsm = 0.;//   = eloss/kineticEnergy;  // energy loss fraction to the mean step energy


  // - init.
  G4double efEnergy = kineticEnergy;
  G4double efStep   = fTheTrueStepLenght;

  G4double kineticEnergy0 = kineticEnergy;
  if(fgIsUseAccurate) {    // - use accurate energy loss correction
     kineticEnergy -= 0.5*eloss;  // mean energy along the full step

     // other parameters for energy loss corrections
     tau    = kineticEnergy/electron_mass_c2; // where kinEnergy is the mean kinetic energy
     tau2   = tau*tau;
     eps0   = eloss/kineticEnergy0; // energy loss fraction to the begin step energy
     epsm   = eloss/kineticEnergy;  // energy loss fraction to the mean step energy


//     efEnergy = kineticEnergy0 * ( 1.0 - 0.5*eps0 - eps0*eps0/(12.0*(2.0-eps0))* ( (5.*tau2 +10.*tau +6.)/( (tau+1.)*(tau+2.) )  )  );
//EGSnrc
efEnergy       = kineticEnergy * (1. - epsm*epsm*(6.+10.*tau+5.*tau2)/(24.*tau2+48.*tau+72.));

     // the real efStep will be s*(1-efStep)
  //  G4double efStep    = 0.166666*epsilon*epsilonp*(tau2*tau2+4.0*tau2*tau+7.0*tau2+6.0*tau+4.0)/((tau2+3.0*tau+2)2);

//     efStep    = 0.166666*eps0*epsm*(4.0+tau*(6.0+tau*(7.0+tau*(4.0+tau))));
//     efStep            /= ((tau2+3.0*tau+2)*(tau2+3.0*tau+2));
//     efStep             = fTheTrueStepLenght*(1.0-efStep);


//      efStep = (1.-eps0*eps0/(3.*(2.-eps0))*  (tau2*tau2 + 4.*tau2*tau + 7.*tau2 +6.*tau +4.)/( ((tau+1.)*(tau+2.))*((tau+1.)*(tau+2.)) ) );
//      efStep *=fTheTrueStepLenght;
// EGSnrc
G4double dum  = 0.166666*(4.+tau*(6.+tau*(7.+tau*(4.+tau))))*
                (epsm/((tau+1.)*(tau+2.)))*(epsm/((tau+1.)*(tau+2.)));
         efStep =fTheTrueStepLenght*(1.-dum);
  } else {                              // - take only mean energy
      kineticEnergy -= 0.5*eloss;  // mean energy along the full step
      efEnergy       = kineticEnergy;
      //Account for energy loss in the MS distribution
      G4double factor = 1./(1. + 0.9784671*kineticEnergy); //0.9784671 = 1/(2*rm)
      eps0= eloss/kineticEnergy0;
      epsm= eps0/(1.-0.5*eps0);
      G4double temp   = 0.3*(1. - factor*(1. - 0.333333*factor))*eps0*eps0;
      efStep = fTheTrueStepLenght*(1. + temp);
  }
  /////////////////////////////////////////////////////////////////////////
//efEnergy = kineticEnergy0;
//efStep   = fTheTrueStepLenght;
//eps0 =0.;
//epsm =0.;
  ///////////////////////////////////////////
  // Compute elastic mfp, first transport mfp, screening parameter, and G1
  fLambda1 = GetTransportMeanFreePath(particle,efEnergy);

  G4double lambdan=0.;

  if(fLambda0>0.0) { lambdan=efStep/fLambda0; }
  if(lambdan<=1.0e-12) {
      if(fIsEverythingWasDone)
        fTheZPathLenght = fTheTrueStepLenght;
    fIsNoScatteringInMSC = true;
    return;
  }

// correction from neglected term 1+A
  lambdan = lambdan/(1.+fScrA);

  G4double Qn1 = lambdan *fG1;//2.* lambdan *scrA*((1.+scrA)*log(1.+1./scrA)-1.);

//if(Qn1>0.5 && !fIsNoDisplace){
  //G4cout<<"  Qn1 = "<<Qn1<<G4endl;
//}

  /////////////////////////////////////////////////////////////////////////////
  // Sample scattering angles
  // new direction, relative to the orriginal one is in {us,vs,ws}
  G4double cosTheta1 =1.0,sinTheta1 =0.0, cosTheta2 =1.0, sinTheta2 =0.0;
  G4double cosPhi1=1.0,sinPhi1=0.0, cosPhi2 =1.0, sinPhi2 =0.0;
  G4double us=0.0,vs=0.0,ws=1.0,x_coord=0.0,y_coord=0.0,z_coord=1.0;
  G4double u2=0.0,v2=0.0;

  // if we are above the upper grid limit with lambdaxG1=true-length/first-trans-mfp
  // => izotropic distribution: lambG1_max =7.992 but set it to 7
  if(0.5*Qn1 > 7.0){
    G4double vrand[2];
    rndmEngineMod->flatArray(2,vrand); //get 2 random number
    cosTheta1 = 1.-2.*vrand[0];
    sinTheta1 = std::sqrt((1.-cosTheta1)*(1.+cosTheta1));
    cosTheta2 = 1.-2.*vrand[1];
    sinTheta2 = std::sqrt((1.-cosTheta2)*(1.+cosTheta2));
  } else {
     // sample 2 scattering cost1, sint1, cost2 and sint2 for half path
     fgGSTable->Sampling(0.5*lambdan, 0.5*Qn1, fScrA, cosTheta1, sinTheta1);
     fgGSTable->Sampling(0.5*lambdan, 0.5*Qn1, fScrA, cosTheta2, sinTheta2);
     if(cosTheta1 + cosTheta2 == 2.) { // no scattering happened
        if(fIsEverythingWasDone)
           fTheZPathLenght = fTheTrueStepLenght;
        fIsNoScatteringInMSC = true;
        return;
     }
  }
  // sample 2 azimuthal angles
  G4double vrand[2];
  rndmEngineMod->flatArray(2,vrand); //get 2 random number
  G4double phi1 = CLHEP::twopi*vrand[0];
  sinPhi1 = std::sin(phi1);
  cosPhi1 = std::cos(phi1);
  G4double phi2 = CLHEP::twopi*vrand[1];
  sinPhi2 = std::sin(phi2);
  cosPhi2 = std::cos(phi2);

  // compute final direction realtive to z-dir
  u2  = sinTheta2*cosPhi2;
  v2  = sinTheta2*sinPhi2;
  G4double u2p = cosTheta1*u2 + sinTheta1*cosTheta2;
  us  = u2p*cosPhi1 - v2*sinPhi1;
  vs  = u2p*sinPhi1 + v2*cosPhi1;
  ws  = cosTheta1*cosTheta2 - sinTheta1*u2;

  ////////////////////////////////////////////////////////////////////
  fTheNewDirection.set(us,vs,ws);

  // set the fTheZPathLenght if we don't sample displacement and
  // we should do everything at the step-limit-phase before we return
  if(fIsNoDisplace && fIsEverythingWasDone)
    fTheZPathLenght = fTheTrueStepLenght;

  // in optimized-mode if the current-safety > current-range we do not use dispalcement
  if(fIsNoDisplace)
    return;

  //////////////////////////////////////////////////////////////////////
  // Compute final position
  if(fgIsUseAccurate) {
    // correction parameters

    G4double par =1.;
    if(Qn1<0.7) par = 1.;
    else if (Qn1<7.0) par = -0.031376*Qn1+1.01356;
    else par = 0.79;

    //
    // Compute elastic mfp, first transport mfp, screening parameter, and G1
    // for the initial energy
//!!!
//1    fLambda1 = GetTransportMeanFreePath(particle,currentKinEnergy);

     // Moments with energy loss correction
     // --first the uncorrected (for energy loss) values of gamma, eta, a1=a2=0.5*(1-eta), delta
     // gamma = G_2/G_1 based on G2 computed from A by using the Wentzel DCS form of G2
     G4double loga   = G4Log(1.0+1.0/fScrA);
     G4double gamma  = 6.0*fScrA*(1.0 + fScrA)*(loga*(1.0 + 2.0*fScrA) - 2.0)/fG1;
     // sample eta from p(eta)=2*eta i.e. P(eta) = eta_square ;-> P(eta) = rand --> eta = sqrt(rand)
     G4double eta    = std::sqrt(rndmEngineMod->flat());
     G4double eta1   = 0.5*(1 - eta);  // used  more than once
     // 0.5 +sqrt(6)/6 = 0.9082483;
     // 1/(4*sqrt(6))  = 0.1020621;
     // (4-sqrt(6)/(24*sqrt(6))) = 0.026374715
     // delta = 0.9082483-(0.1020621-0.0263747*gamma)*Qn1 without energy loss cor.
//!!!
//     Qn1=fTheTrueStepLenght/fLambda0*fG1; // without energy loss
     G4double delta  = 0.9082483-(0.1020621-0.0263747*gamma)*Qn1;

     // compute alpha1 and alpha2 for energy loss correction
     G4double temp1 = 2.0 + tau;
     G4double temp  = (2.0+tau*temp1)/((tau+1.0)*temp1);
     //Take logarithmic dependence
     temp = temp - (tau+1.0)/((tau+2.0)*(loga*(1.0+fScrA)-1.0));
     temp = temp * epsm;
     temp1 = 1.0 - temp;
     delta = delta + 0.40824829*(eps0*(tau+1.0)/((tau+2.0)*
             (loga*(1.0+fScrA)-1.0)*(loga*(1.0+2.0*fScrA)-2.0)) - 0.25*temp*temp);
     G4double b      = eta*delta;
     G4double c      = eta*(1.0-delta);

     //Calculate transport direction cosines:
     // ut,vt,wt is the final position divided by the true step length
     G4double w1v2 = cosTheta1*v2;
     G4double ut   = b*sinTheta1*cosPhi1 + c*(cosPhi1*u2 - sinPhi1*w1v2) + eta1*us*temp1;
     G4double vt   = b*sinTheta1*sinPhi1 + c*(sinPhi1*u2 + cosPhi1*w1v2) + eta1*vs*temp1;
     G4double wt   = eta1*(1+temp) +       b*cosTheta1 +  c*cosTheta2    + eta1*ws*temp1;

     // long step correction
     ut *=par;
     vt *=par;
     wt *=par;

     // final position relative to the pre-step point in the scattering frame
     // ut = x_f/s so needs to multiply by s
     x_coord = ut*fTheTrueStepLenght;
     y_coord = vt*fTheTrueStepLenght;
     z_coord = wt*fTheTrueStepLenght;

     if(fIsEverythingWasDone){
       // We sample in the step limit so set fTheZPathLenght = transportDistance
       // and lateral displacement (x_coord,y_coord,z_coord-transportDistance)
       //Calculate transport distance
       G4double transportDistance  = std::sqrt(x_coord*x_coord+y_coord*y_coord+z_coord*z_coord);
       // protection
       if(transportDistance>fTheTrueStepLenght)
          transportDistance = fTheTrueStepLenght;
       fTheZPathLenght = transportDistance;
     }
     // else:: we sample in the DoIt so
     //       the fTheZPathLenght was already set and was taken as transport along zet
     fTheDisplacementVector.set(x_coord,y_coord,z_coord-fTheZPathLenght);
  } else {
     // compute zz = <z>/tPathLength
     // s -> true-path-length
     // z -> geom-path-length:: when PRESTA is used z =(def.) <z>
     // r -> lateral displacement = s/2 sin(theta)  => x_f = r cos(phi); y_f = r sin(phi)
     G4double zz = 0.0;
     if(fIsEverythingWasDone){
        // We sample in the step limit so set fTheZPathLenght = transportDistance
        // and lateral displacement (x_coord,y_coord,z_coord-transportDistance)
        if(Qn1<0.1) { // use 3-order Taylor approximation of (1-exp(-x))/x around x=0
          zz = 1.0 - Qn1*(0.5 - Qn1*(0.166666667 - 0.041666667*Qn1)); // 1/6 =0.166..7 ; 1/24=0.041..
        } else {
          zz = (1.-G4Exp(-Qn1))/Qn1;
        }
     } else {
        // we sample in the DoIt so
        // the fTheZPathLenght was already set and was taken as transport along zet
        zz = fTheZPathLenght/fTheTrueStepLenght;
     }

     G4double rr = (1.-zz*zz)/(1.-ws*ws); // s^2 >= <z>^2+r^2  :: where r^2 = s^2/4 sin^2(theta)
     if(rr >= 0.25) rr = 0.25;            // (1-<z>^2/s^2)/sin^2(theta) >= r^2/(s^2 sin^2(theta)) = 1/4 must hold
     G4double rperp = fTheTrueStepLenght*std::sqrt(rr);  // this is r/sint
     x_coord  = rperp*us;
     y_coord  = rperp*vs;
     z_coord  = zz*fTheTrueStepLenght;

     if(fIsEverythingWasDone){
       G4double transportDistance = std::sqrt(x_coord*x_coord + y_coord*y_coord + z_coord*z_coord);
       fTheZPathLenght = transportDistance;
     }

     fTheDisplacementVector.set(x_coord,y_coord,z_coord- fTheZPathLenght);
   }
}
*/
