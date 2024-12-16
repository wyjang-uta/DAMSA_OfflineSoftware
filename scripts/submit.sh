#!/bin/bash

DATADIR=/cluster/home/wyjang/public/DAMSA
/usr/bin/qsub -b y -q normal -o output_run15.log -e error_run15.log ./../dmssoft -b $DATADIR/run15.root run15.root
/usr/bin/qsub -b y -q normal -o output_run16.log -e error_run16.log ./../dmssoft -b $DATADIR/run16.root run16.root
/usr/bin/qsub -b y -q normal -o output_run17.log -e error_run17.log ./../dmssoft -b $DATADIR/run17.root run17.root
/usr/bin/qsub -b y -q normal -o output_run18.log -e error_run18.log ./../dmssoft -b $DATADIR/run18.root run18.root
/usr/bin/qsub -b y -q normal -o output_run19.log -e error_run19.log ./../dmssoft -b $DATADIR/run19.root run19.root
/usr/bin/qsub -b y -q normal -o output_run20.log -e error_run20.log ./../dmssoft -b $DATADIR/run20.root run20.root
/usr/bin/qsub -b y -q normal -o output_run21.log -e error_run21.log ./../dmssoft -b $DATADIR/run21.root run21.root
