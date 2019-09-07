# Data-Analysis

Here is the code for data analysis.

## mergeSi_dat.sh

In the *ascii* directory we keep the raw data read by digitizer. For each run of data taking, several ascii files are produced, so it is useful to run the script mergeSi_dat.sh to create a unique ascii file for the run. To run the script it is necessary to change the path of the data inside the script. To run: source mergeSi_dat.sh [run number]

## Reader.C

The Reader.C ROOT macro transforms the information contained in the ascii file in a ROOT tuple, selecting only the important information.

## EventAnalysis.C

With the EventAnalysis.C one can perform many operations, with different functions.
### RunStatsPmt()
Allows to show the Run statisicts of the recorded PMT channels, and to select events on the base of a time window cut.
### xyrad_histo()
Shows plots of the hits in silicon detectors and their projections on the radiator's plane.
### ShowPmtSignal()
Shows the PMT signals of a particular events in a 2d histograms
