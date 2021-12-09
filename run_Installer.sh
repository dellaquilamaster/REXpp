#!/bin/bash

g++ -I./detectors/Oscar -I./detectors/Strip -I./detectors/PadHodoscope -I./detectors/DetectionSetup -fPIC -shared -O3 -Wall -o lib/libdetectors.so detectors/DetectionSetup/TDetectionSetup.cpp detectors/DetectionSetup/TDetectionUnit.cpp detectors/Oscar/TOscarHodoscope.cpp detectors/Strip/TStripDetector.cpp detectors/Strip/TStripCluster.cpp detectors/Strip/TVerticalStripDetector.cpp detectors/PadHodoscope/TPadHodoscope.cpp `root-config --libs --cflags` -lGeom -lEve -lGed -lRGL -lEG -lTreePlayer
g++ -I./LISETools/ -I./detectors/Strip -I./detectors/PadHodoscope -I./detectors/DetectionSetup -fPIC -shared -Wall -O3 -o lib/libLISE.so LISETools/RelativisticKinematics.cpp LISETools/EnergyLossModule.cpp LISETools/nuclear_masses.cpp `root-config --libs --cflags` -lGeom -lEve -lGed -lRGL -lEG -lTreePlayer
