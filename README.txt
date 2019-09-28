This softaware woeks with the input/const.root tree, that contains the tests results of VMM3a chips (of CERN ATLAS NSW project).
It generates files for the loacal database:
- Datatabase, Certificates of best measurements for every chip
- Statuses of all measurements of all chips
- Histograms of testing results
- intermediate files, that can be used to upload results to the Global CERN database.

This code was written and run on
LINUX
LSB Version:	:core-4.1-amd64:core-4.1-noarch
Distributor ID:	CentOS
Description:	CentOS Linux release 7.6.1810 (Core) 
Release:	7.6.1810
Codename:	Core

The ROOT should also be installed (I used ROOT 6.18/02)

How to launch: ./with_root_compile.sh Analyse.cxx
