TARGET = qhelpgui


include($(ACFDIR)/Config/QMake/StaticConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)

lessThan(QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 4){
	QT += webkitwidgets
}
else{
	QT += webenginewidgets
}


