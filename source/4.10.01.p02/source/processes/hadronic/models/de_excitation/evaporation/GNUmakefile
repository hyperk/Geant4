# $Id: GNUmakefile 66892 2013-01-17 10:57:59Z gunter $
# -----------------------------------------------------------
# GNUmakefile for hadronic library.  Gabriele Cosmo, 18/9/96.
# -----------------------------------------------------------

name := G4hadronic_deex_evaporation

ifndef G4INSTALL
  G4INSTALL = ../../../../../..
endif

include $(G4INSTALL)/config/architecture.gmk

CPPFLAGS += -DG4HADRONIC_ALLOC_EXPORT
CPPFLAGS += -I$(G4BASE)/global/management/include \
            -I$(G4BASE)/global/HEPRandom/include \
            -I$(G4BASE)/global/HEPNumerics/include \
            -I$(G4BASE)/global/HEPGeometry/include \
            -I$(G4BASE)/track/include \
            -I$(G4BASE)/processes/management/include \
            -I$(G4BASE)/processes/electromagnetic/lowenergy/include \
            -I$(G4BASE)/processes/hadronic/management/include/ \
            -I$(G4BASE)/processes/hadronic/util/include \
            -I$(G4BASE)/processes/hadronic/cross_sections/include \
	    -I$(G4BASE)/processes/hadronic/models/util/include \
            -I$(G4BASE)/processes/hadronic/models/de_excitation/management/include \
            -I$(G4BASE)/processes/hadronic/models/de_excitation/gem_evaporation/include \
            -I$(G4BASE)/processes/hadronic/models/de_excitation/fission/include \
            -I$(G4BASE)/processes/hadronic/models/de_excitation/fermi_breakup/include \
            -I$(G4BASE)/processes/hadronic/models/de_excitation/photon_evaporation/include \
            -I$(G4BASE)/processes/hadronic/models/de_excitation/util/include \
            -I$(G4BASE)/particles/management/include \
            -I$(G4BASE)/particles/leptons/include \
            -I$(G4BASE)/particles/bosons/include \
            -I$(G4BASE)/particles/hadrons/mesons/include \
            -I$(G4BASE)/particles/hadrons/barions/include \
            -I$(G4BASE)/particles/hadrons/ions/include \
            -I$(G4BASE)/particles/shortlived/include \
            -I$(G4BASE)/materials/include


include $(G4INSTALL)/config/common.gmk

