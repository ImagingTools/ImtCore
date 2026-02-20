# Root of ImtCore project
TEMPLATE = subdirs

SUBDIRS += imtbase
imtbase.file = ../../Include/imtbase/QMake/imtbase.pro

SUBDIRS += imtdesign
imtdesign.file = ../../Include/imtdesign/QMake/imtdesign.pro
imtdesign.depends = imtbase

SUBDIRS += imtfile
imtfile.file = ../../Include/imtfile/QMake/imtfile.pro

SUBDIRS += imtddl
imtddl.file = ../../Include/imtddl/QMake/imtddl.pro

SUBDIRS += imtdoc
imtdoc.file = ../../Include/imtdoc/QMake/imtdoc.pro

SUBDIRS += imtsdl
imtsdl.file = ../../Include/imtsdl/QMake/imtsdl.pro

SUBDIRS += imtsdlgenqml
imtsdlgenqml.file = ../../Include/imtsdlgenqml/QMake/imtsdlgenqml.pro

SUBDIRS += imtsdlgencpp
imtsdlgencpp.file = ../../Include/imtsdlgencpp/QMake/imtsdlgencpp.pro

SUBDIRS += imtoas
imtoas.file = ../../Include/imtoas/QMake/imtoas.pro

SUBDIRS += ImtOasPck
ImtOasPck.file = ../../Impl/ImtOasPck/QMake/ImtOasPck.pro
ImtOasPck.depends = imtoas

SUBDIRS += ImtDesignPck
ImtDesignPck.file = ../../Impl/ImtDesignPck/QMake/ImtDesignPck.pro
ImtDesignPck.depends = imtdesign

SUBDIRS += DesignTokenCreator
DesignTokenCreator.file = ../../Impl/DesignTokenCreatorExe/QMake/DesignTokenCreatorExe.pro
DesignTokenCreator.depends = imtbase imtdesign ImtDesignPck

SUBDIRS += ImtDdlPck
ImtDdlPck.file = ../../Impl/ImtDdlPck/QMake/ImtDdlPck.pro
ImtDdlPck.depends = imtddl

SUBDIRS += ImtDocPck
ImtDocPck.file = ../../Impl/ImtDocPck/QMake/ImtDocPck.pro
ImtDocPck.depends = imtdoc

SUBDIRS += ImtFilePck
ImtFilePck.file = ../../Impl/ImtFilePck/QMake/ImtFilePck.pro
ImtFilePck.depends = imtfile

SUBDIRS += DdlCodeCreator
DdlCodeCreator.file = ../../Impl/DdlCodeCreatorExe/QMake/DdlCodeCreatorExe.pro
DdlCodeCreator.depends = imtddl ImtDdlPck

SUBDIRS += ImtSdlPck
ImtSdlPck.file = ../../Impl/ImtSdlPck/QMake/ImtSdlPck.pro
ImtSdlPck.depends = imtsdl

SUBDIRS += ImtSdlGenQmlPck
ImtSdlGenQmlPck.file = ../../Impl/ImtSdlGenQmlPck/QMake/ImtSdlGenQmlPck.pro
ImtSdlGenQmlPck.depends = imtsdlgenqml

SUBDIRS += ImtSdlGenCppPck
ImtSdlGenCppPck.file = ../../Impl/ImtSdlGenCppPck/QMake/ImtSdlGenCppPck.pro
ImtSdlGenCppPck.depends = imtsdl imtsdlgenqml imtsdlgencpp

SUBDIRS += SdlCodeGenerator
SdlCodeGenerator.file = ../../Impl/SdlCodeGeneratorExe/QMake/SdlCodeGeneratorExe.pro
SdlCodeGenerator.depends = ImtCorePck ImtSdlPck ImtSdlGenQmlPck ImtSdlGenCppPck

# SDL modules
SUBDIRS += imtbasesdl
imtbasesdl.file = ../../Sdl/imtbase/QMake/imtbasesdl.pro
imtbasesdl.depends = SdlCodeGenerator

SUBDIRS += imtserverapp
imtserverapp.file = ../../Include/imtserverapp/QMake/imtserverapp.pro
imtserverapp.depends = imtbasesdl

SUBDIRS += imtauthsdl
imtauthsdl.file = ../../Sdl/imtauth/QMake/imtauthsdl.pro
imtauthsdl.depends = imtbasesdl

SUBDIRS += imtcolorsdl
imtcolorsdl.file = ../../Sdl/imtcolor/QMake/imtcolorsdl.pro
imtcolorsdl.depends = imtbasesdl

SUBDIRS += imtlicsdl
imtlicsdl.file = ../../Sdl/imtlic/QMake/imtlicsdl.pro
imtlicsdl.depends = imtbasesdl

SUBDIRS += imtappsdl
imtappsdl.file = ../../Sdl/imtapp/QMake/imtappsdl.pro
imtappsdl.depends = imtbasesdl

SUBDIRS += imt2dsdl
imt2dsdl.file = ../../Sdl/imt2d/QMake/imt2dsdl.pro
imt2dsdl.depends = imtcolorsdl

SUBDIRS += imtcol
imtcol.file = ../../Include/imtcol/QMake/imtcol.pro
imtcol.depends = imtbase imtbasesdl

SUBDIRS += imtdev
imtdev.file = ../../Include/imtdev/QMake/imtdev.pro

SUBDIRS += ImtDevPck
ImtDevPck.file = ../../Impl/ImtDevPck/QMake/ImtDevPck.pro
ImtDevPck.depends = imtdev

SUBDIRS += imtdevgui
imtdevgui.file = ../../Include/imtdevgui/QMake/imtdevgui.pro

SUBDIRS += ImtDevGuiPck
ImtDevGuiPck.file = ../../Impl/ImtDevGuiPck/QMake/ImtDevGuiPck.pro
ImtDevGuiPck.depends = imtdevgui

SUBDIRS += imtstyle
imtstyle.file = ../../Include/imtstyle/QMake/imtstyle.pro
imtstyle.depends = DesignTokenCreator

SUBDIRS += ImtStylePck
ImtStylePck.file = ../../Impl/ImtStylePck/QMake/ImtStylePck.pro
ImtStylePck.depends = imtstyle DesignTokenCreator

SUBDIRS += imtstylecontrolsqml
imtstylecontrolsqml.file = ../../Include/imtstylecontrolsqml/QMake/imtstylecontrolsqml.pro

SUBDIRS += imtapp
imtapp.file = ../../Include/imtapp/QMake/imtapp.pro
imtapp.depends = imtappsdl

SUBDIRS += imtmail
imtmail.file = ../../Include/imtmail/QMake/imtmail.pro

SUBDIRS += imtauth
imtauth.file = ../../Include/imtauth/QMake/imtauth.pro

SUBDIRS += imtauthdb
imtauthdb.file = ../../Include/imtauthdb/QMake/imtauthdb.pro

SUBDIRS += imtauthgql
imtauthgql.file = ../../Include/imtauthgql/QMake/imtauthgql.pro
imtauthgql.depends = imtauthsdl

SUBDIRS += imtcrypt
imtcrypt.file = ../../Include/imtcrypt/QMake/imtcrypt.pro

SUBDIRS += imtwidgets
imtwidgets.file = ../../Include/imtwidgets/QMake/imtwidgets.pro

SUBDIRS += imtgeo
imtgeo.file = ../../Include/imtgeo/QMake/imtgeo.pro
imtgeo.depends = imtbasesdl

SUBDIRS += imtcom
imtcom.file = ../../Include/imtcom/QMake/imtcom.pro

SUBDIRS += imtrest
imtrest.file = ../../Include/imtrest/QMake/imtrest.pro

SUBDIRS += imtrepo
imtrepo.file = ../../Include/imtrepo/QMake/imtrepo.pro

SUBDIRS += imtreport
imtreport.file = ../../Include/imtreport/QMake/imtreport.pro

SUBDIRS += imt3d
imt3d.file = ../../Include/imt3d/QMake/imt3d.pro

SUBDIRS += imtlog
imtlog.file = ../../Include/imtlog/QMake/imtlog.pro

SUBDIRS += imtzip
imtzip.file = ../../Include/imtzip/QMake/imtzip.pro

SUBDIRS += imthttp
imthttp.file = ../../Include/imthttp/QMake/imthttp.pro

SUBDIRS += imtqml
imtqml.file = ../../Include/imtqml/QMake/imtqml.pro
imtqml.depends = imtbasesdl

SUBDIRS += imtlic
imtlic.file = ../../Include/imtlic/QMake/imtlic.pro

SUBDIRS += imtlicgql
imtlicgql.file = ../../Include/imtlicgql/QMake/imtlicgql.pro
imtlicgql.depends = imtlic imtlicsdl

SUBDIRS += imtgql
imtgql.file = ../../Include/imtgql/QMake/imtgql.pro

SUBDIRS += imtservergql
imtservergql.file = ../../Include/imtservergql/QMake/imtservergql.pro
imtservergql.depends = imtgql imtcol imtappsdl

SUBDIRS += imtdb
imtdb.file = ../../Include/imtdb/QMake/imtdb.pro

SUBDIRS += imtdbgql
imtdbgql.file = ../../Include/imtdbgql/QMake/imtdbgql.pro

SUBDIRS += imtlicdb
imtlicdb.file = ../../Include/imtlicdb/QMake/imtlicdb.pro

SUBDIRS += imtimg
imtimg.file = ../../Include/imtimg/QMake/imtimg.pro

SUBDIRS += imtclientgql
imtclientgql.file = ../../Include/imtclientgql/QMake/imtclientgql.pro
imtclientgql.depends = imtbasesdl imtappsdl

SUBDIRS += imtservice
imtservice.file = ../../Include/imtservice/QMake/imtservice.pro

SUBDIRS += imthype
imthype.file = ../../Include/imthype/QMake/imthype.pro

SUBDIRS += imthypedb
imthypedb.file = ../../Include/imthypedb/QMake/imthypedb.pro

SUBDIRS += imthypegui
imthypegui.file = ../../Include/imthypegui/QMake/imthypegui.pro

# GUI libraries
SUBDIRS += imtgui
imtgui.file = ../../Include/imtgui/QMake/imtgui.pro
imtgui.depends = imtstyle imtwidgets DesignTokenCreator DdlCodeCreator imtcol

SUBDIRS += imtauthgui
imtauthgui.file = ../../Include/imtauthgui/QMake/imtauthgui.pro
imtauthgui.depends = DesignTokenCreator imtgui

SUBDIRS += imtreportgui
imtreportgui.file = ../../Include/imtreportgui/QMake/imtreportgui.pro
imtreportgui.depends = imtgui

SUBDIRS += imtloggui
imtloggui.file = ../../Include/imtloggui/QMake/imtloggui.pro

SUBDIRS += imt3dgui
imt3dgui.file = ../../Include/imt3dgui/QMake/imt3dgui.pro
imt3dgui.depends = DesignTokenCreator

SUBDIRS += imt3dview
imt3dview.file = ../../Include/imt3dview/QMake/imt3dview.pro

SUBDIRS += imtlicgui
imtlicgui.file = ../../Include/imtlicgui/QMake/imtlicgui.pro
imtlicgui.depends = DesignTokenCreator

SUBDIRS += imtguigql
imtguigql.file = ../../Include/imtguigql/QMake/imtguigql.pro
imtguigql.depends = DesignTokenCreator

SUBDIRS += imtdbgui
imtdbgui.file = ../../Include/imtdbgui/QMake/imtdbgui.pro
imtdbgui.depends = DesignTokenCreator


# QML modules
SUBDIRS += imtguiqml
imtguiqml.file = ../../Qml/imtgui/QMake/imtguiqml.pro

SUBDIRS += imtdocguiqml
imtdocguiqml.file = ../../Qml/imtdocgui/QMake/imtdocguiqml.pro

SUBDIRS += imtcolguiqml
imtcolguiqml.file = ../../Qml/imtcolgui/QMake/imtcolguiqml.pro

SUBDIRS += imtauthguiqml
imtauthguiqml.file = ../../Qml/imtauthgui/QMake/imtauthguiqml.pro
imtauthguiqml.depends = imtauthsdl

SUBDIRS += imtguigqlqml
imtguigqlqml.file = ../../Qml/imtguigql/QMake/imtguigqlqml.pro

SUBDIRS += imtcontrolsqml
imtcontrolsqml.file = ../../Qml/imtcontrols/QMake/imtcontrolsqml.pro

SUBDIRS += imtlicguiqml
imtlicguiqml.file = ../../Qml/imtlicgui/QMake/imtlicguiqml.pro
imtlicguiqml.depends = imtlicsdl


######### Component packages
SUBDIRS += ImtCorePck
ImtCorePck.file = ../../Impl/ImtCorePck/QMake/ImtCorePck.pro
ImtCorePck.depends = imtbase imtfile

SUBDIRS += ImtAuthPck
ImtAuthPck.file = ../../Impl/ImtAuthPck/QMake/ImtAuthPck.pro
ImtAuthPck.depends = imtbase imtauth

SUBDIRS += ImtDatabasePck
ImtDatabasePck.file = ../../Impl/ImtDatabasePck/QMake/ImtDatabasePck.pro
ImtDatabasePck.depends = imtbase imtdb

SUBDIRS += ImtDatabaseGqlPck
ImtDatabaseGqlPck.file = ../../Impl/ImtDatabaseGqlPck/QMake/ImtDatabaseGqlPck.pro
ImtDatabaseGqlPck.depends = imtbase imtdb imtdbgql

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
ImtGuiGqlPck.depends = imtgui imtguigql imtgql imtservergql

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

SUBDIRS += ImtGqlPck
ImtGqlPck.file = ../../Impl/ImtGqlPck/QMake/ImtGqlPck.pro
ImtGqlPck.depends = imtgql

SUBDIRS += ImtServerGqlPck
ImtServerGqlPck.file = ../../Impl/ImtServerGqlPck/QMake/ImtServerGqlPck.pro
ImtServerGqlPck.depends = imtservergql imtbasesdl

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

SUBDIRS += ImtMailPck
ImtMailPck.file = ../../Impl/ImtMailPck/QMake/ImtMailPck.pro
ImtMailPck.depends = imtmail

SUBDIRS += ImtAuthDbPck
ImtAuthDbPck.file = ../../Impl/ImtAuthDbPck/QMake/ImtAuthDbPck.pro
ImtAuthDbPck.depends = imtauthdb imtdb

SUBDIRS += ImtAuthGqlPck
ImtAuthGqlPck.file = ../../Impl/ImtAuthGqlPck/QMake/ImtAuthGqlPck.pro
ImtAuthGqlPck.depends = imtauthgql

SUBDIRS += ImtClientGqlPck
ImtClientGqlPck.file = ../../Impl/ImtClientGqlPck/QMake/ImtClientGqlPck.pro
ImtClientGqlPck.depends = imtclientgql imtbasesdl

SUBDIRS += ImtHttpPck
ImtHttpPck.file = ../../Impl/ImtHttpPck/QMake/ImtHttpPck.pro
ImtHttpPck.depends = imthttp

SUBDIRS += ImtServicePck
ImtServicePck.file = ../../Impl/ImtServicePck/QMake/ImtServicePck.pro
ImtServicePck.depends = imtservice

SUBDIRS += ImtColPck
ImtColPck.file = ../../Impl/ImtColPck/QMake/ImtColPck.pro
ImtColPck.depends = imtcol imtbase

SUBDIRS += ImtServerAppPck
ImtServerAppPck.file = ../../Impl/ImtServerAppPck/QMake/ImtServerAppPck.pro
ImtServerAppPck.depends = imtserverapp

SUBDIRS += EagleView
EagleView.file = ../../Impl/EagleView/QMake/EagleView.pro
EagleView.depends = imt3dgui

SUBDIRS += ImtHypePck
ImtHypePck.file = ../../Impl/ImtHypePck/QMake/ImtHypePck.pro
ImtHypePck.depends = imthype

SUBDIRS += ImtHypeDbPck
ImtHypeDbPck.file = ../../Impl/ImtHypeDbPck/QMake/ImtHypeDbPck.pro
ImtHypeDbPck.depends = imthypedb

SUBDIRS += ImtHypeGuiPck
ImtHypeGuiPck.file = ../../Impl/ImtHypeGuiPck/QMake/ImtHypeGuiPck.pro
ImtHypeGuiPck.depends = imthypegui imthype

# SUBDIRS += ImtControlsGallery
# ImtControlsGallery.file = ../../Impl/ImtControlsGallery/QMake/ImtControlsGallery.pro
# ImtControlsGallery.depends = imtgui imtqml imtstyle imtstylecontrolsqml


######### Test projects
SUBDIRS += imtbasetest
imtbasetest.file = ../../Include/imtbase/Test/QMake/imtbasetest.pro
imtbasetest.depends = imtbase

SUBDIRS += imtgqltest
imtgqltest.file = ../../Include/imtgql/Test/QMake/imtgqltest.pro
imtgqltest.depends = imtgql imtbase

SUBDIRS += imtmdbxtest
imtmdbxtest.file = ../../Include/imtmdbx/Test/QMake/imtmdbxtest.pro
imtmdbxtest.depends = imtbase

SUBDIRS += imtsdlgencpptest
imtsdlgencpptest.file = ../../Include/imtsdlgencpp/Test/QMake/imtsdlgencpptest.pro
imtsdlgencpptest.depends = imtsdlgencpp imtsdl imtbase

SUBDIRS += imtsdlgenqmltest
imtsdlgenqmltest.file = ../../Include/imtsdlgenqml/Test/QMake/imtsdlgenqmltest.pro
imtsdlgenqmltest.depends = imtsdlgenqml imtsdl imtbase

SUBDIRS += imt3dtest
imt3dtest.file = ../../Include/imt3d/Test/QMake/imt3dTest.pro
imt3dtest.depends = imt3d
