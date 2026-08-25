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
# Link scopes are explicit in this file (PUBLIC/PRIVATE/INTERFACE) and applied
# by the shared declare_target_dependencies() helper from
# ACF/Acf/Config/CMake/ProjectRoot.cmake.
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
declare_target_dependencies(imtbase			AcfSln::iauth AcfSln::icomm AcfSln::imeas imtfile)
declare_target_dependencies(imtdesign			imtbase Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
declare_target_dependencies(imtfile			AcfSln::ifileproc)

#------ DDLCodeCreator ------
declare_target_dependencies(imtddl				AcfSln::iproc)

#------ SDL toolset ------
declare_target_dependencies(imtsdl				imtbase)
declare_target_dependencies(imtsdlgencpp		imtsdl)
declare_target_dependencies(imtsdlgenqml		imtsdl)

#------ OpenAPI ------
declare_target_dependencies(imtoas				Acf::istd)

#-------- SDL generated libraries --------
declare_target_dependencies(imtbasesdl			imtsdl)
declare_target_dependencies(imt2dsdl			imtbasesdl imtcolorsdl)
declare_target_dependencies(imtauthsdl			imtbasesdl imtauth)
declare_target_dependencies(imtappsdl			imtbasesdl imtapp)
declare_target_dependencies(imtchatsdl			imtbasesdl imtchat)
declare_target_dependencies(imtcolorsdl		imtbasesdl imtcol)
declare_target_dependencies(imtdesksdl			imtbasesdl imtdesk)
declare_target_dependencies(imtlicsdl			imtbasesdl imtlic)
declare_target_dependencies(imtpaysdl			imtbasesdl)
declare_target_dependencies(controlsgallerysdl	imtbasesdl imtservergql)

#-------- Libraries --------
declare_target_dependencies(imt2d				imt2dsdl imtcolorsdl Acf::istd Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui)
declare_target_dependencies(imt3d				Acf::i3d AcfSln::icalib Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
declare_target_dependencies(imt3dgui			Acf::iqtgui imt3dview Qt${QT_VERSION_MAJOR}::OpenGL Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
declare_target_dependencies(imt3dview			imt3d imt3dgui)
declare_target_dependencies(imtapp				Acf::ifile)
declare_target_dependencies(imtauth			imtdoc imtlic imtmail Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
declare_target_dependencies(imtauthdb			imtdb imtgql Qt${QT_VERSION_MAJOR}::Sql)
declare_target_dependencies(imtauthgql			imtservergql Qt${QT_VERSION_MAJOR}::Sql)
declare_target_dependencies(imtauthgui			imtgui imtqml)
declare_target_dependencies(imtchat			imtdb Qt${QT_VERSION_MAJOR}::Sql)
declare_target_dependencies(imtchatdb			imtchat Qt${QT_VERSION_MAJOR}::Sql)
declare_target_dependencies(imtchatgql			imtchatsdl imtservergql)
declare_target_dependencies(imtclientgql		imtbasesdl imtservergql Qt${QT_VERSION_MAJOR}::WebSockets)
declare_target_dependencies(imtcol				imtbasesdl)
declare_target_dependencies(imtcom				imtrest)
declare_target_dependencies(imtcrypt			Acf::ifile)
declare_target_dependencies(imtdb				imtapp imtauth imtcol Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg Qt${QT_VERSION_MAJOR}::Sql)
declare_target_dependencies(imtdbgql			imtservergql Qt${QT_VERSION_MAJOR}::WebSockets)
declare_target_dependencies(imtdbgui			Acf::iqtgui imtdb)
declare_target_dependencies(imtdesk			imtauth)
declare_target_dependencies(imtdeskdb			imtdb imtdesk imtgql Qt${QT_VERSION_MAJOR}::Sql)
declare_target_dependencies(imtdeskgql			imtchat imtdesksdl imtservergql)
declare_target_dependencies(controlsgallerygql	controlsgallerysdl imtservergql imtbase imtcrypt imtauth imtdoc imtdb imtbasesdl)
declare_target_dependencies(imtdev				imtbase)
declare_target_dependencies(imtdevgui			Acf::iqtgui imtdev)
declare_target_dependencies(imtdoc				imtbase)
declare_target_dependencies(imtej				Acf::iqt Qt${QT_VERSION_MAJOR}::PrintSupport)
declare_target_dependencies(imtgeo				imtdb imtservergql Qt${QT_VERSION_MAJOR}::Positioning Qt${QT_VERSION_MAJOR}::Core)
if(QT_VERSION_MAJOR EQUAL 5)
	declare_target_dependencies(imtgeo			Qt${QT_VERSION_MAJOR}::Location)
endif()
declare_target_dependencies(imtgql				imtauth Qt${QT_VERSION_MAJOR}::WebSockets)
declare_target_dependencies(imtgqltest			Acf::ipackage imtcom imtdb imtgql)
declare_target_dependencies(imtgui				Acf::iqtdoc imtcol imtwidgets)
declare_target_dependencies(imtguigql			imtservergql)
declare_target_dependencies(imthttp			imtcom)
declare_target_dependencies(imthype			imtbase)
declare_target_dependencies(imthypedb			imtdb imthype Qt${QT_VERSION_MAJOR}::Sql)
declare_target_dependencies(imthypegui			imtgui imthype Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
declare_target_dependencies(imtimg				imtbase)
declare_target_dependencies(imtlic				imtbase imtcrypt imtrest)
declare_target_dependencies(imtlicdb			imtdb Qt${QT_VERSION_MAJOR}::Sql)
declare_target_dependencies(imtlicgql			imtguigql imtlicsdl imtwidgets)
declare_target_dependencies(imtlicgui			imtauth imtgui imtlic)
declare_target_dependencies(imtlog				imtbase)
declare_target_dependencies(imtloggui			Acf::iqtgui imtlog Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
declare_target_dependencies(imtmail			Acf::iprm)
declare_target_dependencies(imtmdbx			imtbasesdl imtdb imtserverapp)
declare_target_dependencies(imtmongo			imtdb)
declare_target_dependencies(imtpy				AcfSln::iproc)
declare_target_dependencies(imtqml				Acf::iqtgui imtclientgql imtcom imtdoc imtgql Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::Concurrent)
declare_target_dependencies(imtqml2d			Acf::iimg)
declare_target_dependencies(imtrepo			imtbase)
declare_target_dependencies(imtreport			Acf::iimg)
declare_target_dependencies(imtreportgui		Acf::iqtgui imtreport Qt${QT_VERSION_MAJOR}::PrintSupport)
declare_target_dependencies(imtrest			imtcom imtservergql Qt${QT_VERSION_MAJOR}::WebSockets)
declare_target_dependencies(imtserverapp		imtdb imtqml imtsdl imtservice imtcolorsdl)
declare_target_dependencies(imtservergql		imtauthsdl imtappsdl imthype imtserverapp Qt${QT_VERSION_MAJOR}::WebSockets)
declare_target_dependencies(imtservice			imtbase imtcom)
declare_target_dependencies(imtstyle			imtdesign Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
declare_target_dependencies(imttest			imtdb Qt${QT_VERSION_MAJOR}::Sql)
declare_target_dependencies(imtupdate			imtbase Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
declare_target_dependencies(imtwidgets			Acf::iwidgets)
declare_target_dependencies(imtzip				imtfile)
if(QT_VERSION_MAJOR EQUAL 6)
	declare_target_dependencies(imtzip			Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

#-------- QML registration libraries --------
declare_target_dependencies(imt3dguiqml			imtqml imtgui imt3dgui)
declare_target_dependencies(imtauthguiqml			imtauthgui)
declare_target_dependencies(imtchatguiqml			imtqml imtgui)
declare_target_dependencies(imtcolguiqml			imtqml imtgui imtcol)
declare_target_dependencies(imtcontrolsqml			imtqml imtgui)
declare_target_dependencies(imtdeskguiqml			imtqml imtgui)
declare_target_dependencies(imtdocguiqml			imtqml imtgui imtdoc)
declare_target_dependencies(imtgeoguiqml			imtqml imtgui imtgeo)
declare_target_dependencies(imtguigqlqml			imtqml imtgui imtguigql)
declare_target_dependencies(imtguiqml				imtqml imtgui)
declare_target_dependencies(imtlicguiqml			imtqml imtlicgui)
declare_target_dependencies(imtstylecontrolsqml	imtqml imtgui imtstyle)

# --- Arxc-generated static libraries ----------------------------------------
declare_target_dependencies(ImtCoreLoc	Acf::icomp AcfSln::AcfSlnLoc Acf::AcfLoc)



