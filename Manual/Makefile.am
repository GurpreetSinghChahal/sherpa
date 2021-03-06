MAKEFLAGS=-j1

## For documentation on texinfo, see
## http://www.gnu.org/software/texinfo/manual

## html is generated by 'make html'
## and installed by 'make install-html'

info_TEXINFOS = Sherpa.texi

Sherpa_TEXINFOS = \
	API.texi \
	Beam.texi \
        BFactory.texi \
	Bibliography.texi \
	Commands.texi \
	Customization.texi \
	CustomScale.texi \
	HEFT.texi \
	Events.texi \
	Examples.texi \
	UFO.texi \
	Fragmentation.texi \
	HardDecays.texi \
	HERA_DIS.texi \
	InclusiveCrossSections.texi \
	Input.texi \
	Integration.texi \
	Introduction.texi \
	ISR.texi \
	LHC_HInt.texi \
	LHC_HJets.texi \
	LHC_HJets_FiniteMt.texi \
	LHC_Jets.texi \
	LEP_Jets.texi \
	LHC_TTBar.texi \
	LHC_TTH.texi \
	LHC_TTW.texi \
	LHC_Vbb.texi \
	LHC_VHJets.texi \
	LHC_WJets.texi \
	LHC_ZJets.texi \
	LHC_VVJets.texi \
	NLO_NTuples.texi \
	Macros.texi \
	ME.texi \
	ME2.texi \
	MI.texi \
	MinBias.texi \
	MinimumBiasEvents.texi \
	Models.texi \
	MPIEvents.texi \
	MSSM.texi \
	NewModel.texi \
	NLOtips.texi \
	OLE.texi \
	Parameters.texi \
	PDF.texi \
	Processes.texi \
	RNG.texi \
	Run.texi \
	ScaleVars.texi \
	Selector.texi \
	Sherpa.texi \
	Shower.texi \
	SM.texi \
	SoftQED.texi \
	Starting.texi \
	Tips.texi \
	version.texi

man1_MANS = Sherpa.1

dist_pkgdata_DATA         = \
	sherpa-completion \
	completion.index

dist_html_DATA = \
        Sherpa.html \
        SherpaLogo.jpg

## this is needed to get the includes from Examples/.../ working.
## we can't use relative paths there because that breaks out-of-source builds
AM_MAKEINFOFLAGS = -I $(top_srcdir)

update-manpage:
	groff -Tascii -mandoc $(srcdir)/Sherpa.trf > Sherpa.1

completion.index: $(srcdir)/*.texi
	$(MAKE) $(AM_MAKEFLAGS) Sherpa.info
	@echo "Creating index for tab completion."
	@rm -f Sherpa.txt* Sherpa.index
	-@makeinfo $(AM_MAKEINFOFLAGS) --plaintext -o Sherpa.txt $(srcdir)/Sherpa.texi && \
        awk 'BEGIN{ m=0; }{ if (match($$0,"Appendix B Index")>0) { m=m+1; getline; getline; getline;} if (m==2) { print $$0 } }' Sherpa.txt | cut -d ":" -f 1 > completion.index || touch completion.index
	@rm -f Sherpa.txt*

Sherpa.html: $(srcdir)/*.texi $(srcdir)/*.css
	$(MAKE) $(AM_MAKEFLAGS) Sherpa.info
	if test -x "$$(which texi2html 2>/dev/null)"; then \
          texi2html --I $(top_srcdir) --nosec-nav --css-include=$(srcdir)/Sherpa.css $(srcdir)/Sherpa.texi; \
        elif test -x "$$(which makeinfo 2>/dev/null)"; then \
          makeinfo $(AM_MAKEINFOFLAGS) --html --no-split --css-include=$(srcdir)/Sherpa.css -o Sherpa.html $(srcdir)/Sherpa.texi; \
        else \
          touch Sherpa.html; \
        fi
dvi:

EXTRA_DIST    = Sherpa.1 Sherpa.css
