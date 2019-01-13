TARGET = nicp

include($(ACFDIR)/Config/QMake/SharedLibraryConfig.pri)
include($(IACFDIR)/Config/QMake/Iacf.pri)
include($(IMTCOREDIR)/Config/QMake/Eigen3.pri)
include($(IMTCOREDIR)/Config/QMake/Flann.pri)
include($(IACFDIR)/Config/QMake/OpenCV_3_3_1.pri)

DEFINES += _USE_MATH_DEFINES nicp_EXPORTS

