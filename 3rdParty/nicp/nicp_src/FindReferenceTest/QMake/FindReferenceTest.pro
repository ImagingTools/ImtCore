TARGET = FindReferenceTest

include($(ACFDIR)/Config/QMake/ApplicationConfig.pri)
include($(ACFDIR)/Config/QMake/QtBaseConfig.pri)
include($(ACFSLNDIR)/Config/QMake/AcfSln.pri)
include($(IACFDIR)/Config/QMake/Iacf.pri)
include($(IMTCOREDIR)/Config/QMake/Eigen3.pri)
include($(IMTCOREDIR)/Config/QMake/NICP.pri)
include($(IMTCOREDIR)/Config/QMake/Flann.pri)
include($(IACFDIR)/Config/QMake/OpenCV_3_3_1.pri)

QT -= gui
CONFIG += console

HEADERS =
SOURCES = ../test.cpp