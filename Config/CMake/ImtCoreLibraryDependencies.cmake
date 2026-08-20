# ---------------------------------------------------------------------------
# Clean, target-based inter-library dependency graph for ImtCore.
#
# This mirrors the approach introduced for the ACF foundation (Acf) in
# Config/CMake/AcfLibraryDependencies.cmake, for AcfSln in
# Config/CMake/AcfSlnLibraryDependencies.cmake and for IAcf in
# Config/CMake/IAcfLibraryDependencies.cmake: instead of relying on the final
# executable/package link to resolve symbols and on a hand-tuned build order, the
# dependencies between the ImtCore libraries - and their dependencies onto the
# underlying Acf::, AcfSln:: and IAcf:: libraries - are declared here as target
# usage requirements. Include paths and link order then propagate transitively
# and automatically, both for the in-tree build and for downstream consumers that
# use find_package(ImtCore) and link a single ImtCore::<lib> target.
#
# Link scope is controlled by ACF_LIBRARY_LINK_SCOPE and applied by the shared
# acf_declare_target_dependencies() helper from ACF/Acf/Config/CMake/ProjectRoot.cmake.
# The helper uses target properties and therefore avoids plain-vs-keyword
# target_link_libraries() signature conflicts in mixed environments.
#
# Dependencies are declared *minimally*: each library lists only its direct
# dependencies; transitive dependencies propagate automatically through the
# graph. Do not add a dependency that is already reachable through another
# listed target.
#
# Included once, centrally, from Build/CMake/CMakeLists.txt after all library
# targets have been created.
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Inter-library dependencies (minimal direct deps only; transitive propagation
# is relied upon for indirect deps).
#
# Key transitive chains (for reference when adding new entries):
#	imtbase → Acf::{ibase,icomp,idoc,ifile,ilog,imod,iprm,iqt,iser,istd}
#			  AcfSln::{iauth,icomm,iinsp,imeas} imtfile
#	imtfile → Acf::{ifile,ilog,istd} AcfSln::ifileproc
#	imtauth → imtdoc imtlic imtmail → imtbase imtcrypt
#	imtdb	→ imtapp imtauth imtcol → imtrest
#
# NOTE: imtrest → imtservergql → imtserverapp → imtdb → imtauth → imtlic → imtrest
#		imtqml → imtclientgql → imtservergql → imtserverapp → imtqml
#		imtcom ↔ imtrest
# ---------------------------------------------------------------------------

#------ Required libs ------
acf_declare_target_dependencies(imtbase			AcfSln::iauth AcfSln::icomm AcfSln::imeas imtfile)
acf_declare_target_dependencies(imtdesign			imtbase Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
acf_declare_target_dependencies(imtfile			AcfSln::ifileproc)

#------ DDLCodeCreator ------
acf_declare_target_dependencies(imtddl				AcfSln::iproc)

#------ SDL toolset ------
acf_declare_target_dependencies(imtsdl				imtbase)
acf_declare_target_dependencies(imtsdlgencpp		imtsdl)
acf_declare_target_dependencies(imtsdlgenqml		imtsdl)

#------ OpenAPI ------
acf_declare_target_dependencies(imtoas				Acf::istd)

#-------- SDL generated libraries --------
acf_declare_target_dependencies(imtbasesdl			imtsdl)
acf_declare_target_dependencies(imt2dsdl			imtbasesdl imtcolorsdl)
acf_declare_target_dependencies(imtauthsdl			imtbasesdl imtauth)
acf_declare_target_dependencies(imtappsdl			imtbasesdl imtapp)
acf_declare_target_dependencies(imtchatsdl			imtbasesdl imtchat)
acf_declare_target_dependencies(imtcolorsdl		imtbasesdl imtcol)
acf_declare_target_dependencies(imtdesksdl			imtbasesdl imtdesk)
acf_declare_target_dependencies(imtlicsdl			imtbasesdl imtlic)
acf_declare_target_dependencies(imtpaysdl			imtbasesdl)

#-------- Libraries --------
acf_declare_target_dependencies(imt2d				imt2dsdl imtcolorsdl Acf::istd Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui)
acf_declare_target_dependencies(imt3d				Acf::i3d AcfSln::icalib Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
acf_declare_target_dependencies(imt3dgui			Acf::iqtgui imt3dview Qt${QT_VERSION_MAJOR}::OpenGL Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
acf_declare_target_dependencies(imt3dview			imt3d imt3dgui)
acf_declare_target_dependencies(imtapp				Acf::ifile)
acf_declare_target_dependencies(imtauth			imtdoc imtlic imtmail Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
acf_declare_target_dependencies(imtauthdb			imtdb imtgql Qt${QT_VERSION_MAJOR}::Sql)
acf_declare_target_dependencies(imtauthgql			imtservergql Qt${QT_VERSION_MAJOR}::Sql)
acf_declare_target_dependencies(imtauthgui			imtgui imtqml)
acf_declare_target_dependencies(imtchat			imtdb Qt${QT_VERSION_MAJOR}::Sql)
acf_declare_target_dependencies(imtchatdb			imtchat Qt${QT_VERSION_MAJOR}::Sql)
acf_declare_target_dependencies(imtchatgql			imtchatsdl imtservergql)
acf_declare_target_dependencies(imtclientgql		imtbasesdl imtservergql Qt${QT_VERSION_MAJOR}::WebSockets)
acf_declare_target_dependencies(imtcol				imtbasesdl)
acf_declare_target_dependencies(imtcom				imtrest)
acf_declare_target_dependencies(imtcrypt			Acf::ifile)
acf_declare_target_dependencies(imtdb				imtapp imtauth imtcol Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg Qt${QT_VERSION_MAJOR}::Sql)
acf_declare_target_dependencies(imtdbgql			imtservergql Qt${QT_VERSION_MAJOR}::WebSockets)
acf_declare_target_dependencies(imtdbgui			Acf::iqtgui imtdb)
acf_declare_target_dependencies(imtdesk			imtauth)
acf_declare_target_dependencies(imtdeskdb			imtdb imtdesk imtgql Qt${QT_VERSION_MAJOR}::Sql)
acf_declare_target_dependencies(imtdeskgql			imtchat imtdesksdl imtservergql)
acf_declare_target_dependencies(imtdev				imtbase)
acf_declare_target_dependencies(imtdevgui			Acf::iqtgui imtdev)
acf_declare_target_dependencies(imtdoc				imtbase)
acf_declare_target_dependencies(imtej				Acf::iqt Qt${QT_VERSION_MAJOR}::PrintSupport)
acf_declare_target_dependencies(imtgeo				imtdb imtservergql Qt${QT_VERSION_MAJOR}::Positioning Qt${QT_VERSION_MAJOR}::Core)
if(QT_VERSION_MAJOR EQUAL 5)
	acf_declare_target_dependencies(imtgeo			Qt${QT_VERSION_MAJOR}::Location)
endif()
acf_declare_target_dependencies(imtgql				imtauth Qt${QT_VERSION_MAJOR}::WebSockets)
acf_declare_target_dependencies(imtgqltest			Acf::ipackage imtcom imtdb imtgql)
acf_declare_target_dependencies(imtgui				Acf::iqtdoc imtcol imtwidgets)
acf_declare_target_dependencies(imtguigql			imtservergql)
acf_declare_target_dependencies(imthttp			imtcom)
acf_declare_target_dependencies(imthype			imtbase)
acf_declare_target_dependencies(imthypedb			imtdb imthype Qt${QT_VERSION_MAJOR}::Sql)
acf_declare_target_dependencies(imthypegui			imtgui imthype Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
acf_declare_target_dependencies(imtimg				imtbase)
acf_declare_target_dependencies(imtlic				imtbase imtcrypt imtrest)
acf_declare_target_dependencies(imtlicdb			imtdb Qt${QT_VERSION_MAJOR}::Sql)
acf_declare_target_dependencies(imtlicgql			imtguigql imtlicsdl imtwidgets)
acf_declare_target_dependencies(imtlicgui			imtauth imtgui imtlic)
acf_declare_target_dependencies(imtlog				imtbase)
acf_declare_target_dependencies(imtloggui			Acf::iqtgui imtlog Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
acf_declare_target_dependencies(imtmail			Acf::iprm)
acf_declare_target_dependencies(imtmdbx			imtbasesdl imtdb imtserverapp)
acf_declare_target_dependencies(imtmongo			imtdb)
acf_declare_target_dependencies(imtpy				AcfSln::iproc)
acf_declare_target_dependencies(imtqml				Acf::iqtgui imtclientgql imtcom imtdoc imtgql Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::Concurrent)
acf_declare_target_dependencies(imtqml2d			Acf::iimg)
acf_declare_target_dependencies(imtrepo			imtbase)
acf_declare_target_dependencies(imtreport			Acf::iimg)
acf_declare_target_dependencies(imtreportgui		Acf::iqtgui imtreport Qt${QT_VERSION_MAJOR}::PrintSupport)
acf_declare_target_dependencies(imtrest			imtcom imtservergql Qt${QT_VERSION_MAJOR}::WebSockets)
acf_declare_target_dependencies(imtserverapp		imtdb imtqml imtsdl imtservice imtcolorsdl)
acf_declare_target_dependencies(imtservergql		imtauthsdl imtappsdl imthype imtserverapp Qt${QT_VERSION_MAJOR}::WebSockets)
acf_declare_target_dependencies(imtservice			imtbase imtcom)
acf_declare_target_dependencies(imtstyle			imtdesign Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
acf_declare_target_dependencies(imttest			imtdb Qt${QT_VERSION_MAJOR}::Sql)
acf_declare_target_dependencies(imtupdate			imtbase Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
acf_declare_target_dependencies(imtwidgets			Acf::iwidgets)
acf_declare_target_dependencies(imtzip				imtfile)
if(QT_VERSION_MAJOR EQUAL 6)
	acf_declare_target_dependencies(imtzip			Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

#-------- QML registration libraries --------
acf_declare_target_dependencies(imt3dguiqml			imtqml imtgui imt3dgui)
acf_declare_target_dependencies(imtauthguiqml			imtauthgui)
acf_declare_target_dependencies(imtchatguiqml			imtqml imtgui)
acf_declare_target_dependencies(imtcolguiqml			imtqml imtgui imtcol)
acf_declare_target_dependencies(imtcontrolsqml			imtqml imtgui)
acf_declare_target_dependencies(imtdeskguiqml			imtqml imtgui)
acf_declare_target_dependencies(imtdocguiqml			imtqml imtgui imtdoc)
acf_declare_target_dependencies(imtgeoguiqml			imtqml imtgui imtgeo)
acf_declare_target_dependencies(imtguigqlqml			imtqml imtgui imtguigql)
acf_declare_target_dependencies(imtguiqml				imtqml imtgui)
acf_declare_target_dependencies(imtlicguiqml			imtqml imtlicgui)
acf_declare_target_dependencies(imtstylecontrolsqml	imtqml imtgui imtstyle)

# --- Arxc-generated static libraries ----------------------------------------
acf_declare_target_dependencies(ImtCoreLoc	Acf::icomp AcfSln::AcfSlnLoc Acf::AcfLoc)



