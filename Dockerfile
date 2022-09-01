FROM ghcr.io/hyper-k/hk-pilot:latest

COPY . /usr/local/hk/Geant4

WORKDIR /usr/local/hk

RUN . /usr/local/hk/hkpilot/setup.sh &&\
    hkp install Geant4
