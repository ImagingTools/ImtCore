# Root of ImtCore project
TEMPLATE = subdirs

SUBDIRS += imtbase
imtbase.file = ../../Include/imtbase/QMake/imtbase.pro

SUBDIRS += imtdesign
imtdesign.file = ../../Include/imtdesign/QMake/imtdesign.pro

SUBDIRS += ImtDesignPck
ImtDesignPck.file = ../../Impl/ImtDesignPck/QMake/ImtDesignPck.pro
ImtDesignPck.depends = imtdesign

SUBDIRS += DesignTokenCreatorExe
DesignTokenCreatorExe.file = ../../Impl/DesignTokenCreatorExe/QMake/DesignTokenCreatorExe.pro
DesignTokenCreatorExe.depends = imtbase imtdesign ImtDesignPck

SUBDIRS += imtdev
imtdev.file = ../../Include/imtdev/QMake/imtdev.pro

SUBDIRS += ImtDevPck
ImtDevPck.file = ../../Impl/ImtDevPck/QMake/ImtDevPck.pro
ImtDevPck.depends = imtdev

SUBDIRS += imtstyle
imtstyle.file = ../../Include/imtstyle/QMake/imtstyle.pro
imtstyle.depends = DesignTokenCreatorExe

SUBDIRS += imtstyleqml
imtstyleqml.file = ../../Include/imtstyleqml/QMake/imtstyleqml.pro
imtstyleqml.depends = imtstyle

SUBDIRS += ImtStylePck
ImtStylePck.file = ../../Impl/ImtStylePck/QMake/ImtStylePck.pro
ImtStylePck.depends = imtstyle DesignTokenCreatorExe

SUBDIRS += imtapp
imtapp.file = ../../Include/imtapp/QMake/imtapp.pro

SUBDIRS += imtauth
imtauth.file = ../../Include/imtauth/QMake/imtauth.pro

SUBDIRS += imtauthdb
imtauthdb.file = ../../Include/imtauthdb/QMake/imtauthdb.pro

SUBDIRS += imtauthgql
imtauthgql.file = ../../Include/imtauthgql/QMake/imtauthgql.pro

SUBDIRS += imtauthgui
imtauthgui.file = ../../Include/imtauthgui/QMake/imtauthgui.pro
imtauthgui.depends = DesignTokenCreatorExe imtgui

SUBDIRS += imtcrypt
imtcrypt.file = ../../Include/imtcrypt/QMake/imtcrypt.pro

SUBDIRS += imtwidgets
imtwidgets.file = ../../Include/imtwidgets/QMake/imtwidgets.pro

SUBDIRS += imtgeo
imtgeo.file = ../../Include/imtgeo/QMake/imtgeo.pro

SUBDIRS += imtcom
imtcom.file = ../../Include/imtcom/QMake/imtcom.pro

SUBDIRS += imtrest
imtrest.file = ../../Include/imtrest/QMake/imtrest.pro

SUBDIRS += imtrepo
imtrepo.file = ../../Include/imtrepo/QMake/imtrepo.pro

SUBDIRS += imtreport
imtreport.file = ../../Include/imtreport/QMake/imtreport.pro

SUBDIRS += imtreportgui
imtreportgui.file = ../../Include/imtreportgui/QMake/imtreportgui.pro

SUBDIRS += imt3d
imt3d.file = ../../Include/imt3d/QMake/imt3d.pro

SUBDIRS += imtgui
imtgui.file = ../../Include/imtgui/QMake/imtgui.pro
imtgui.depends = imtwidgets DesignTokenCreatorExe

SUBDIRS += imtloggui
imtloggui.file = ../../Include/imtloggui/QMake/imtloggui.pro

SUBDIRS += imtlog
imtlog.file = ../../Include/imtlog/QMake/imtlog.pro

SUBDIRS += imt3dview
imt3dview.file = ../../Include/imt3dview/QMake/imt3dview.pro

SUBDIRS += imt3dgui
imt3dgui.file = ../../Include/imt3dgui/QMake/imt3dgui.pro
imt3dgui.depends = DesignTokenCreatorExe

!macx {
	SUBDIRS += imtfile
	imtfile.file = ../../Include/imtfile/QMake/imtfile.pro
}

SUBDIRS += imtzip
imtzip.file = ../../Include/imtzip/QMake/imtzip.pro

#!macx {
#	SUBDIRS += imtpy
#	imtpy.file = ../../Include/imtpy/QMake/imtpy.pro
#}

SUBDIRS += imtqml
imtqml.file = ../../Include/imtqml/QMake/imtqml.pro

SUBDIRS += imtcontrols
imtcontrols.file = ../../Include/imtcontrols/QMake/imtcontrols.pro

SUBDIRS += imtlic
imtlic.file = ../../Include/imtlic/QMake/imtlic.pro

SUBDIRS += imtlicgql
imtlicgql.file = ../../Include/imtlicgql/QMake/imtlicgql.pro

SUBDIRS += imtlicgui
imtlicgui.file = ../../Include/imtlicgui/QMake/imtlicgui.pro
imtlicgui.depends = DesignTokenCreatorExe

SUBDIRS += imtgql
imtgql.file = ../../Include/imtgql/QMake/imtgql.pro

SUBDIRS += imtguigql
imtguigql.file = ../../Include/imtguigql/QMake/imtguigql.pro

SUBDIRS += imtdb
imtdb.file = ../../Include/imtdb/QMake/imtdb.pro

SUBDIRS += imtlicdb
imtlicdb.file = ../../Include/imtlicdb/QMake/imtlicdb.pro

SUBDIRS += imtdbgui
imtdbgui.file = ../../Include/imtdbgui/QMake/imtdbgui.pro

SUBDIRS += imtimg
imtimg.file = ../../Include/imtimg/QMake/imtimg.pro

SUBDIRS += imtclientgql
imtclientgql.file = ../../Include/imtclientgql/QMake/imtclientgql.pro

######### Component packages
SUBDIRS += ImtCorePck
ImtCorePck.file = ../../Impl/ImtCorePck/QMake/ImtCorePck.pro
ImtCorePck.depends = imtbase

SUBDIRS += ImtAuthPck
ImtAuthPck.file = ../../Impl/ImtAuthPck/QMake/ImtAuthPck.pro
ImtAuthPck.depends = imtbase imtauth

SUBDIRS += ImtDatabasePck
ImtDatabasePck.file = ../../Impl/ImtDatabasePck/QMake/ImtDatabasePck.pro
ImtDatabasePck.depends = imtbase imtdb

SUBDIRS += ImtAuthGuiPck
ImtAuthGuiPck.file = ../../Impl/ImtAuthGuiPck/QMake/ImtAuthGuiPck.pro
ImtAuthGuiPck.depends = imtbase imtauth imtauthgui imtgui

SUBDIRS += ImtLicensePck
ImtLicensePck.file = ../../Impl/ImtLicensePck/QMake/ImtLicensePck.pro
ImtLicensePck.depends = imtbase

SUBDIRS += ImtLicenseGqlPck
ImtLicenseGqlPck.file = ../../Impl/ImtLicenseGqlPck/QMake/ImtLicenseGqlPck.pro
ImtLicenseGqlPck.depends = imtlicgql imtlicgui

SUBDIRS += ImtLicenseGuiPck
ImtLicenseGuiPck.file = ../../Impl/ImtLicenseGuiPck/QMake/ImtLicenseGuiPck.pro
ImtLicenseGuiPck.depends = imtbase imtlic imtlicgui

SUBDIRS += ImtCryptPck
ImtCryptPck.file = ../../Impl/ImtCryptPck/QMake/ImtCryptPck.pro
ImtCryptPck.depends = imtbase imtcrypt

SUBDIRS += ImtComPck
ImtComPck.file = ../../Impl/ImtComPck/QMake/ImtComPck.pro
ImtComPck.depends = imtbase imtcom

SUBDIRS += ImtRestPck
ImtRestPck.file = ../../Impl/ImtRestPck/QMake/ImtRestPck.pro
ImtRestPck.depends = imtbase imtrest

SUBDIRS += Imt3dPck
Imt3dPck.file = ../../Impl/Imt3dPck/QMake/Imt3dPck.pro
Imt3dPck.depends = imt3d

SUBDIRS += ImtReportGuiPck
ImtReportGuiPck.file = ../../Impl/ImtReportGuiPck/QMake/ImtReportGuiPck.pro
ImtReportGuiPck.depends = imtreport imtreportgui

SUBDIRS += ImtRepoPck
ImtRepoPck.file = ../../Impl/ImtRepoPck/QMake/ImtRepoPck.pro
ImtRepoPck.depends = imtbase

SUBDIRS += ImtReportPck
ImtReportPck.file = ../../Impl/ImtReportPck/QMake/ImtReportPck.pro
ImtReportPck.depends = imtreport imtbase

SUBDIRS += ImtCoreLoc
ImtCoreLoc.file = ../../Impl/ImtCoreLoc/QMake/ImtCoreLoc.pro

SUBDIRS += ImtGuiPck
ImtGuiPck.file = ../../Impl/ImtGuiPck/QMake/ImtGuiPck.pro
ImtGuiPck.depends = imt3d imtgui

SUBDIRS += ImtGuiGqlPck
ImtGuiGqlPck.file = ../../Impl/ImtGuiGqlPck/QMake/ImtGuiGqlPck.pro
ImtGuiGqlPck.depends = imtgui imtguigql imtgql

SUBDIRS += ImtQmlPck
ImtQmlPck.file = ../../Impl/ImtQmlPck/QMake/ImtQmlPck.pro
ImtQmlPck.depends = imtqml imtgui

SUBDIRS += ImtLogPck
ImtLogPck.file = ../../Impl/ImtLogPck/QMake/ImtLogPck.pro
ImtLogPck.depends = imtbase

SUBDIRS += ImtLogGuiPck
ImtLogGuiPck.file = ../../Impl/ImtLogGuiPck/QMake/ImtLogGuiPck.pro
ImtLogGuiPck.depends = imt3d imtgui

SUBDIRS += Imt3dViewPck
Imt3dViewPck.file = ../../Impl/Imt3dViewPck/QMake/Imt3dViewPck.pro
Imt3dViewPck.depends = imt3dview

SUBDIRS += Imt3dGuiPck
Imt3dGuiPck.file = ../../Impl/Imt3dGuiPck/QMake/Imt3dGuiPck.pro
Imt3dGuiPck.depends = imt3dview imt3dgui imtgui

SUBDIRS += ImtZipPck
ImtZipPck.file = ../../Impl/ImtZipPck/QMake/ImtZipPck.pro
ImtZipPck.depends = imtzip

!macx {
#	SUBDIRS += ImtPyPck
#	ImtPyPck.file = ../../Impl/ImtPyPck/QMake/ImtPyPck.pro
#	ImtPyPck.depends = imtpy
}

SUBDIRS += ImtGqlPck
ImtGqlPck.file = ../../Impl/ImtGqlPck/QMake/ImtGqlPck.pro
ImtGqlPck.depends = imtgql

SUBDIRS += ImtLicenseDbPck
ImtLicenseDbPck.file = ../../Impl/ImtLicenseDbPck/QMake/ImtLicenseDbPck.pro
ImtLicenseDbPck.depends = imtlic imtlicdb

SUBDIRS += ImtDatabaseGuiPck
ImtDatabaseGuiPck.file = ../../Impl/ImtDatabaseGuiPck/QMake/ImtDatabaseGuiPck.pro
ImtDatabaseGuiPck.depends = imtgui imtdbgui

SUBDIRS += ImtImagePck
ImtImagePck.file = ../../Impl/ImtImagePck/QMake/ImtImagePck.pro
ImtImagePck.depends = imtimg

SUBDIRS += ImtAppPck
ImtAppPck.file = ../../Impl/ImtAppPck/QMake/ImtAppPck.pro
ImtAppPck.depends = imtapp

SUBDIRS += ImtAuthDbPck
ImtAuthDbPck.file = ../../Impl/ImtAuthDbPck/QMake/ImtAuthDbPck.pro
ImtAuthDbPck.depends = imtauthdb imtdb

SUBDIRS += ImtAuthGqlPck
ImtAuthGqlPck.file = ../../Impl/ImtAuthGqlPck/QMake/ImtAuthGqlPck.pro
ImtAuthGqlPck.depends = imtauthgql

SUBDIRS += ImtClientGqlPck
ImtClientGqlPck.file = ../../Impl/ImtClientGqlPck/QMake/ImtClientGqlPck.pro
ImtClientGqlPck.depends = imtclientgql

#SUBDIRS += ImtControlsGallery
#ImtControlsGallery.file = ../../Impl/ImtControlsGallery/QMake/ImtControlsGallery.pro
#ImtControlsGallery.depends = imtqml imtstyle imtcontrols

