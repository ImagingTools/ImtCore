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

SUBDIRS += imtsdl
imtsdl.file = ../../Include/imtsdl/QMake/imtsdl.pro

SUBDIRS += imtsdlgenqml
imtsdlgenqml.file = ../../Include/imtsdlgenqml/QMake/imtsdlgenqml.pro

SUBDIRS += imtsdlgencpp
imtsdlgencpp.file = ../../Include/imtsdlgencpp/QMake/imtsdlgencpp.pro


SUBDIRS += ImtDesignPck
ImtDesignPck.file = ../../Impl/ImtDesignPck/QMake/ImtDesignPck.pro
ImtDesignPck.depends = imtdesign

SUBDIRS += DesignTokenCreator
DesignTokenCreator.file = ../../Impl/DesignTokenCreatorExe/QMake/DesignTokenCreatorExe.pro
DesignTokenCreator.depends = imtbase imtdesign ImtDesignPck

SUBDIRS += ImtDdlPck
ImtDdlPck.file = ../../Impl/ImtDdlPck/QMake/ImtDdlPck.pro
ImtDdlPck.depends = imtddl

SUBDIRS += DdlCodeCreator
DdlCodeCreator.file = ../../Impl/DdlCodeCreatorExe/QMake/DdlCodeCreatorExe.pro
DdlCodeCreator.depends = imtddl ImtDdlPck

SUBDIRS += ImtSdlPck
ImtSdlPck.file = ../../Impl/ImtSdlPck/QMake/ImtSdlPck.pro
ImtSdlPck.depends = imtsdl

SUBDIRS += ImtSdlGenQmlPck
ImtSdlGenQmlPck.file = ../../Impl/ImtSdlGenQmlPck/QMake/ImtSdlGenQmlPck.pro
ImtSdlGenQmlPck.depends = imtsdl imtsdlgenqml

SUBDIRS += ImtSdlGenCppPck
ImtSdlGenCppPck.file = ../../Impl/ImtSdlGenCppPck/QMake/ImtSdlGenCppPck.pro
ImtSdlGenCppPck.depends = imtsdl imtsdlgenqml imtsdlgencpp

SUBDIRS += ImtCorePck
ImtCorePck.file = ../../Impl/ImtCorePck/QMake/ImtCorePck.pro
ImtCorePck.depends = imtbase imtfile

SUBDIRS += SdlCodeGenerator
SdlCodeGenerator.file = ../../Impl/SdlCodeGeneratorExe/QMake/SdlCodeGeneratorExe.pro
SdlCodeGenerator.depends = ImtCorePck ImtSdlPck ImtSdlGenQmlPck ImtSdlGenCppPck

