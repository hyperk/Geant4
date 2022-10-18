FROM ghcr.io/hyperk/hk-pilot:main

COPY . /usr/local/hk/hk-Geant4

WORKDIR /usr/local/hk

RUN . /usr/local/hk/hk-pilot/setup.sh &&\
    hkp install hk-Geant4
