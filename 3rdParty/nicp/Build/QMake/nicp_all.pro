include ($(ACFDIR)/Config/QMake/Solution.pri)

SUBDIRS += nicp
nicp.file = ../../nicp_src/nicp/QMake/nicp.pro

#todo nicp_viewer

SUBDIRS += FindReferenceTest
FindReferenceTest.file = ../../nicp_src/FindReferenceTest/QMake/FindReferenceTest.pro
