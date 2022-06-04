from hkpilot.utils.cmake import CMake

import inspect
import os


class Geant4(CMake):

    def __init__(self, path):
        super().__init__(path)

        self._package_name = "Geant4"
        self._package_version = "4.10.01.p03"

        self._cmakelist_path = "source/4.10.01.p03"

        self._cmake_options = {
            "WITH_TLS": "OFF"
        }

        # self._path = os.path.relpath(inspect.getfile(self.__class__))
