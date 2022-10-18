FROM ghcr.io/hyperk/hk-pilot:main

COPY . /usr/local/hk/hk-Geant4

RUN git config --global url."https://github.com/".insteadOf git@github.com: &&\
    git config --global url."https://".insteadOf git://

WORKDIR /usr/local/hk
RUN . /usr/local/hk/hk-pilot/setup.sh &&\
    hkp install -r hk-Geant4
