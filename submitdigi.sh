#!/bin/tcsh

# expects a command line argument
if ($#argv == 1 ) then
    set RunList = $1
endif

#get the no of runs from the list
set Runs=`cat $RunList`

# set environment variable for directory to run from
setenv BATCH_DIR /home/rachel/geant4/TDIS_digi/CheckChargeShare/mTPC-DigiChargeShare
cd ${BATCH_DIR}

set tempdir = "/scratch/rachel/tdis"
setenv TEMPDIR $tempdir

# execute FarmDigi each run in run list
foreach RunStart ($Runs)
    setenv RUNNUMBER $RunStart

    echo "Submitting run # $RunStart"
    qsub -V FarmDigi
    #FarmDigi
    echo "Waiting 20 seconds for next submit"
    sleep 5

 end

echo "ALL JOBS LOADED"

