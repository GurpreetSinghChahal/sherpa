
RIVET_SOURCES       =	Rivet_Interface.C

if RIVET_SUPPORT
pkglib_LTLIBRARIES       =	libSherpaRivetAnalysis.la

libSherpaRivetAnalysis_la_SOURCES  =	$(RIVET_SOURCES)

libSherpaRivetAnalysis_la_LIBADD   =	@CONDITIONAL_RIVETLDADD@

libSherpaRivetAnalysis_la_CPPFLAGS = $(AM_CPPFLAGS) @CONDITIONAL_RIVETCPPFLAGS@ @CONDITIONAL_HEPMC2INCS@

else
RIVET_EXTRA_DIST = $(RIVET_SOURCES)
endif

EXTRA_DIST = $(RIVET_EXTRA_DIST)
