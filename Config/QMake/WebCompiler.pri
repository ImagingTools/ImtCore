
defineTest(copyToWebDir) {
    files = $$1
    dir = $$2
    # replace slashes in destination path for Windows
    win32:dir ~= s,/,\\,g

    for(file, files) {
        # replace slashes in source path for Windows
        win32:file ~= s,/,\\,g

        QMAKE_PRE_LINK += $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    export(QMAKE_PRE_LINK)
}

defineTest(copyFile) {
    file = $$1
	fileNew = $$2
	# replace slashes in destination path for Windows
	win32:fileNew ~= s,/,\\,g

    # replace slashes in source path for Windows
	win32:file ~= s,/,\\,g

    QMAKE_PRE_LINK += $$QMAKE_COPY_FILE $$shell_quote($$file) $$shell_quote($$fileNew) $$escape_expand(\\n\\t)


    export(QMAKE_PRE_LINK)
}

defineTest(compyleWeb) {
    buildwebdir = $$1
	resname = $$2
	dir = $$1/src
	jqmldir = $(IMTCOREDIR)/3rdParty/JQML
	npmexe = npm

    win32{
	    dir ~= s,/,\\,g
		jqmldir ~= s,/,\\,g

        PATH += $(IMTCOREDIR)/3rdParty/nodejs
		npmexe = $(IMTCOREDIR)/3rdParty/nodejs/npm_acf.cmd
		npmexe ~= s,/,\\,g
	}
	QMAKE_PRE_LINK += cd $$shell_quote($$jqmldir) && $$npmexe run compile  $$shell_quote($$dir) $$escape_expand(\\n\\t)
	export(QMAKE_PRE_LINK)

    copyFile($$buildwebdir/src/jqml.full.js, $$buildwebdir/Resources/jqml.$${resname}.js)
	copyToWebDir($$buildwebdir/src/imtqml/GraphQLRequest.js, $$buildwebdir/Resources)

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

    QMAKE_PRE_LINK += $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name $${TARGET}Web $${QRC_WEB_FILE} -o $${QRC_CPP_WEB_FILE}
	export(QMAKE_PRE_LINK)
}

