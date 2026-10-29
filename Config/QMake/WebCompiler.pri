
defineTest(copyToWebDir) {
    files = $$1
    dir = $$2

    # replace slashes in destination path for Windows
    win32:dir ~= s,/,\\,g

    for(file, files) {
        # replace slashes in source path for Windows
        win32:file ~= s,/,\\,g

        isEmpty(WEB_COMMAND){
		}
		else {
		    WEB_COMMAND += &&
		}

        WEB_COMMAND +=  $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir)
		}

    export(WEB_COMMAND)
}

defineTest(copyFile) {
    file = $$1
	fileNew = $$2
	# replace slashes in destination path for Windows
	win32:fileNew ~= s,/,\\,g

    # replace slashes in source path for Windows
	win32:file ~= s,/,\\,g

    isEmpty(WEB_COMMAND) {
	}
	else {
	    WEB_COMMAND += &&
	}
	WEB_COMMAND += $$QMAKE_COPY_FILE $$shell_quote($$file) $$shell_quote($$fileNew)

	export(WEB_COMMAND)
}

defineTest(compyleWeb) {
    buildwebdir = $$1
	resname = $$2
	dir = $$1/src
	jqmldir = $(IMTCOREDIR)/Tools/JQML/v2
	npmexe = npm

    win32{
	    dir ~= s,/,\\,g
		jqmldir ~= s,/,\\,g

        PATH += $(IMTCOREDIR)/3rdParty/nodejs
		npmexe = $(IMTCOREDIR)/Tools/JQML/v2/jqml2compiler.bat
		npmexe ~= s,/,\\,g
	}

WEB_COMMAND += && cd $$shell_quote($$buildwebdir) && $$npmexe  $$shell_quote($$dir)

    copyFile($$buildwebdir/src/jqml.full.js, $$buildwebdir/Resources/jqml.$${resname}.js)

    QRC_WEB_FILE = $${buildwebdir}/Resources/$${TARGET}JsWeb.qrc
	QRC_CPP_WEB_FILE = $${buildwebdir}/Resources/qrc_$${TARGET}Web.cpp
	win32:QRC_WEB_FILE ~= s,/,\\,g
	win32:QRC_CPP_WEB_FILE ~= s,/,\\,g

    win32{
	    QMAKE_RCC = rcc.exe
	}
	else{
	    QMAKE_RCC = rcc
	}
	WEB_COMMAND += && $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name $${TARGET}Web $${QRC_WEB_FILE} -o $${QRC_CPP_WEB_FILE}

	export(WEB_COMMAND)
}

#! Compiles web application with the JQML v3 compiler (QMake analog of jq_compile_web in Config/CMake/WebCompiler.cmake)
#! QML sources are compiled directly from the directories listed in the JQML config file, so no copying of sources is needed.
#! Generated resource: $$buildwebdir/Resources/qrc_$${TARGET}Web.cpp (Qt resource name: $${TARGET}Web)
#! \param 1 buildwebdir		- web build directory
#! \param 2 inputjs			- absolute path to the JQML config (*.json) file
#! \param 3 startqml		- absolute path to the entry QML file
#! \param 4 dataroot		- root path of the web application
#! \param 5 appicon			- application icon reference used in the generated html
#! \param 6 translationdirs	- (optional) list of directories containing *.ts files to embed
defineTest(jqCompileWeb) {
	buildwebdir = $$1
	inputjs = $$2
	startqml = $$3
	dataroot = $$4
	appicon = $$5
	translationdirs = $$6

	imtcoredir = $$(IMTCOREDIR)
	jqmldir = $$imtcoredir/Tools/JQML/v3
	jqmldistdir = $$jqmldir/dist
	resourcesdir = $$buildwebdir/Resources
	translationsqrcfile = $$resourcesdir/qmlTranslationsWeb.qrc
	qrcwebfile = $$resourcesdir/qmlJsWeb.qrc
	qrccppwebfile = $$resourcesdir/qrc_$${TARGET}Web.cpp

	targetname = Qt$${QT_MAJOR_VERSION}_$$COMPILER_CODE
	imtcoredirbuild = $$(IMTCOREDIR_BUILD)
	isEmpty(imtcoredirbuild){
		imtcoredirbuild = $$imtcoredir
	}

	pythonexe = $$(PYTHONEXE)
	isEmpty(pythonexe){
		win32{
			pythonexe = python.exe
		}
		else{
			pythonexe = python3
		}
	}

	win32{
		nodeexe = $$imtcoredir/3rdParty/nodejs/node.exe
		rccexe = $$[QT_INSTALL_BINS]/rcc.exe
	}
	else{
		nodeexe = node
		greaterThan(QT_MAJOR_VERSION, 5){
			rccexe = $$[QT_HOST_LIBEXECS]/rcc
		}
		else{
			rccexe = $$[QT_INSTALL_BINS]/rcc
		}
	}

	win32{
		jqmldir ~= s,/,\\,g
		jqmldistdir ~= s,/,\\,g
		resourcesdir ~= s,/,\\,g
		translationsqrcfile ~= s,/,\\,g
		qrcwebfile ~= s,/,\\,g
		qrccppwebfile ~= s,/,\\,g
		inputjs ~= s,/,\\,g
		startqml ~= s,/,\\,g
		translationdirs ~= s,/,\\,g
		imtcoredirbuild ~= s,/,\\,g
		nodeexe ~= s,/,\\,g
		rccexe ~= s,/,\\,g
	}

	win32{
		mkdircommand = (if not exist $$shell_quote($$resourcesdir) mkdir $$shell_quote($$resourcesdir))
		cdcommand = cd /d $$shell_quote($$jqmldir)
		nodecommand = set \"TARGETNAME=$$targetname\" && set \"IMTCOREDIR_BUILD=$$imtcoredirbuild\" && $$shell_quote($$nodeexe)
	}
	else{
		mkdircommand = mkdir -p $$shell_quote($$resourcesdir)
		cdcommand = cd $$shell_quote($$jqmldir)
		nodecommand = TARGETNAME=$$shell_quote($$targetname) IMTCOREDIR_BUILD=$$shell_quote($$imtcoredirbuild) $$nodeexe
	}

	for(translationdir, translationdirs){
		quotedtranslationdirs += $$shell_quote($$translationdir)
	}

	jqcommand = $$mkdircommand
	jqcommand += && $$cdcommand
	jqcommand += && $$pythonexe preparesources.py $$shell_quote($$jqmldistdir) $$shell_quote($$resourcesdir)
	jqcommand += && $$pythonexe generate_translations_qrc.py $$shell_quote($$translationsqrcfile) $$quotedtranslationdirs
	jqcommand += && $$nodecommand compiler/compiler.js -n index -i $$shell_quote($$appicon) -o $$shell_quote($$resourcesdir) -m html
	jqcommand += && $$nodecommand compiler/compiler.js -c $$shell_quote($$inputjs) -n index -o $$shell_quote($$resourcesdir) -r $$shell_quote($$dataroot) -e $$shell_quote($$startqml) -m js
	jqcommand += && $$shell_quote($$rccexe) -name $${TARGET}Web $$shell_quote($$qrcwebfile) $$shell_quote($$translationsqrcfile) -o $$shell_quote($$qrccppwebfile)

	isEmpty(WEB_COMMAND){
		WEB_COMMAND = $$jqcommand
	}
	else{
		WEB_COMMAND += && $$jqcommand
	}

	export(WEB_COMMAND)
}

