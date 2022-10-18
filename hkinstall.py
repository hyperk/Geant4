from hkpilot.utils.cmake import CMake
from hkpilot.utils import fancylogger

import inspect
import os

logger = fancylogger.getLogger(__name__)

class HKGeant4(CMake):

    def __init__(self, path):
        super().__init__(path)

        self._package_name = "hk-Geant4"
        self._package_version = "10.1.3"
        self._download_url = "http://cern.ch/geant4-data/releases/geant4_10_01_p03.zip"

        self._cmakelist_path = "src/geant4_10_01_p03"

        self._cmake_options = {
            "WITH_TLS": "OFF",
            "GEANT4_INSTALL_DATA": "ON"
        }

    def post_install(self):
        logger.info(f"Post-installation of {self._package_name} in progress...")

        src = f"{self._install_folder}/lib/{self._package_name}-{self._package_version}"
        dst = f"{self._install_folder}/lib/cmake"
        if not os.path.exists(src):
            if os.path.exists(f"{self._install_folder}/lib64"):
                logger.info("Switching to lib64 folder for cmake...")
                src = f"{self._install_folder}/lib64/{self._package_name}-{self._package_version}"
                dst = f"{self._install_folder}/lib64/cmake"
            else:
                logger.error(f"Cannot find lib or lib64 subfolder!")
                # return False
        logger.debug(f"Creating symlink between {src} and {dst}")
        if os.path.exists(dst):
            os.unlink(dst)
        os.symlink(src, dst, target_is_directory=True)
        logger.info(f"Post-installation of {self._package_name} done successfully")
        return True
