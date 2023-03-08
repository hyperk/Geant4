FROM ghcr.io/hyperk/hk-pilot:main

COPY . /usr/local/hk/Geant4

WORKDIR /usr/local/hk

RUN ls /usr/local/hk/Geant4
RUN ls /usr/local/hk/Geant4/source/4.10.01.p03

RUN . /usr/local/hk/hk-pilot/setup.sh &&\
    hkp install Geant4 && \
    hkp clean --src --build Geant4
