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
// INCL++ intra-nuclear cascade model
// Alain Boudard, CEA-Saclay, France
// Joseph Cugnon, University of Liege, Belgium
// Jean-Christophe David, CEA-Saclay, France
// Pekka Kaitaniemi, CEA-Saclay, France, and Helsinki Institute of Physics, Finland
// Sylvie Leray, CEA-Saclay, France
// Davide Mancusi, CEA-Saclay, France
//
#define INCLXX_IN_GEANT4_MODE 1

#include "globals.hh"

/*
 * G4INCLParticleSpecies.cc
 *
 *  \date Nov 25, 2011
 * \author Davide Mancusi
 */

#include "G4INCLParticleSpecies.hh"
#include "G4INCLParticleTable.hh"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <algorithm>

namespace G4INCL {

  ParticleSpecies::ParticleSpecies(std::string const &pS) {
    // Normalise the string to lower case
    if(pS=="p" || pS=="proton") {
      theA = 1;
      theZ = 1;
      theType = G4INCL::Proton;
    } else if(pS=="n" || pS=="neutron") {
      theA = 1;
      theZ = 0;
      theType = G4INCL::Neutron;
    } else if(pS=="delta++" || pS=="deltaplusplus") {
      theA = 1;
      theZ = 2;
      theType = G4INCL::DeltaPlusPlus;
    } else if(pS=="delta+" || pS=="deltaplus") {
      theA = 1;
      theZ = 1;
      theType = G4INCL::DeltaPlus;
    } else if(pS=="delta0" || pS=="deltazero") {
      theA = 1;
      theZ = 0;
      theType = G4INCL::DeltaZero;
    } else if(pS=="delta-" || pS=="deltaminus") {
      theA = 1;
      theZ = -1;
      theType = G4INCL::DeltaMinus;
    } else if(pS=="pi+" || pS=="pion+" || pS=="piplus" || pS=="pionplus") {
      theA = 0;
      theZ = 1;
      theType = G4INCL::PiPlus;
    } else if(pS=="pi0" || pS=="pion0" || pS=="pizero" || pS=="pionzero") {
      theA = 0;
      theZ = 0;
      theType = G4INCL::PiZero;
    } else if(pS=="pi-" || pS=="pion-" || pS=="piminus" || pS=="pionminus") {
      theA = 0;
      theZ = -1;
      theType = G4INCL::PiMinus;
    } else if(pS=="d" || pS=="deuteron") {
      theA = 2;
      theZ = 1;
      theType = G4INCL::Composite;
    } else if(pS=="t" || pS=="triton") {
      theA = 3;
      theZ = 1;
      theType = G4INCL::Composite;
    } else if(pS=="a" || pS=="alpha") {
      theA = 4;
      theZ = 2;
      theType = G4INCL::Composite;
    } else if(pS=="eta") {
      theA = 0;
      theZ = 0;
      theType = G4INCL::Eta;
    } else if(pS=="omega") {
      theA = 0;
      theZ = 0;
      theType = G4INCL::Omega;
    } else if(pS=="etaprime" || pS=="etap") {
      theA = 0;
      theZ = 0;
      theType = G4INCL::EtaPrime;
    } else if(pS=="photon") {
      theA = 0;
      theZ = 0;
      theType = G4INCL::Photon;
    } else
      parseNuclide(pS);
  }

  ParticleSpecies::ParticleSpecies(ParticleType const t) :
    theType(t),
    theA(ParticleTable::getMassNumber(theType)),
    theZ(ParticleTable::getChargeNumber(theType))
  {}

  ParticleSpecies::ParticleSpecies(const G4int A, const G4int Z) :
    theType(Composite),
    theA(A),
    theZ(Z)
  {}

  void ParticleSpecies::parseNuclide(std::string const &pS) {
    theType = Composite;

    // Allowed characters
    const std::string separators("-_");
    std::string allowed("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    allowed += separators;

    // There must be at least one character
    if(pS.find_first_not_of(allowed)!=std::string::npos) {
      // Malformed input string
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }
    if(pS.size()<1) {
      // Malformed input string
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    std::size_t firstSeparator = pS.find_first_of(separators);
    std::size_t lastSeparator = pS.find_last_of(separators);
    if(firstSeparator!=std::string::npos && firstSeparator!=lastSeparator) {
      // Several separators in malformed input string
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    // Identify the type of the first character
    G4int (*predicate)(G4int);
    G4bool startsWithAlpha = std::isalpha(pS.at(0));
    if(startsWithAlpha) {
      predicate=std::isdigit;
    } else if(std::isdigit(pS.at(0))) {
      predicate=std::isalpha;
    } else {
      // Non-alphanumeric character in string
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    G4bool hasIsotope = true;
    size_t endFirstSection, beginSecondSection;
    if(firstSeparator==std::string::npos) {
      // No separator, Fe56 or 56Fe style
      // Identify the end of the first section

      // Find the first character that is not of the same type as the first one
      beginSecondSection = std::find_if(pS.begin()+1, pS.end(), predicate) - pS.begin();

      if(beginSecondSection>=pS.size()) {
        if(startsWithAlpha) {
          // Only alphabetic characters are present -- must be an element name
          hasIsotope = false;
        } else {
          // Only numeric characters in the string
          // Setting unknown particle species
          (*this) = ParticleSpecies(UnknownParticle);
          return;
        }
      }

      endFirstSection = beginSecondSection;

    } else {
      // One separator, Fe-56 or 56-Fe style
      endFirstSection = firstSeparator;
      beginSecondSection = firstSeparator+1;
    }

    std::string firstSection(pS.substr(0,endFirstSection));
    std::string secondSection(pS.substr(beginSecondSection,std::string::npos));
    std::stringstream parsingStream;

    // Parse the sections
    G4bool success;
    if(startsWithAlpha) {
      parsingStream.str(secondSection);
      success = parseElement(firstSection);
    } else {
      parsingStream.str(firstSection);
      success = parseElement(secondSection);
    }
    if(!success) {
      // Couldn't parse the element section
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    if(hasIsotope) {
      parsingStream >> theA;
      if(parsingStream.fail()) {
        // Couldn't parse the mass section
        // Setting unknown particle species
        (*this) = ParticleSpecies(UnknownParticle);
        return;
      }
    } else
      theA = 0;

    // Check that Z<=A
    if(theZ>theA && hasIsotope) {
      // Setting unknown particle species
      (*this) = ParticleSpecies(UnknownParticle);
      return;
    }

    // Special particle type for protons
    if(theZ==1 && theA==1)
      theType = Proton;
  }

  G4bool ParticleSpecies::parseElement(std::string const &s) {
    theZ = ParticleTable::parseElement(s);

    if(theZ<0)
      theZ = ParticleTable::parseIUPACElement(s);

    if(theZ<0)
      return false;
    else
      return true;
  }

  G4bool ParticleSpecies::parseIUPACElement(std::string const &s) {
    theZ = ParticleTable::parseIUPACElement(s);
    if(theZ==0)
      return false;
    else
      return true;
  }
	
  G4int ParticleSpecies::getPDGCode() const {
    switch (theType) {
		case Proton:
		    return 2212;
			break;
		case Neutron:
		    return 2112;
			break;
		case DeltaPlusPlus:
		    return 2224;
			break;
		case DeltaPlus:
		    return 2214;
			break;
		case DeltaZero:
		    return 2114;
			break;
		case DeltaMinus:
		    return 1114;
			break;
		case PiPlus:
		    return 211;
			break;
		case PiZero:
		    return 111;
			break;
		case PiMinus:
		    return -211;
			break;
		case Eta:
		    return 221;
			break;
		case Omega:
		    return 223;
			break;
		case EtaPrime:
		    return 331;
			break;
		case Photon:
		    return 22;
			break;
		case Composite:
		    return theA+theZ*1000;
			break;
		default:
			INCL_ERROR("ParticleSpecies::getPDGCode: Unknown particle type." << '\n');
			return 0;
			break;
	}	
  }
}

