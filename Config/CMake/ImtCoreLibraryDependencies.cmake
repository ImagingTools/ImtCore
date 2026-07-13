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
# The target_link_libraries() signature is controlled by ACF_LIBRARY_LINK_SCOPE:
#  * when empty, the plain signature is used (matching the legacy ImtCore CMake),
#  * when set to PUBLIC/PRIVATE/INTERFACE, the keyword signature is used.
# CMake forbids mixing the plain and keyword signatures on the same target. For
# static libraries the dependency still propagates transitively to consumers.
#
# Dependencies are declared *minimally*: each library lists only its direct
# dependencies; transitive dependencies propagate automatically through the
# graph. Do not add a dependency that is already reachable through another
# listed target.
#
# Included once, centrally, from Build/CMake/CMakeLists.txt after all library
# targets have been created.
# ---------------------------------------------------------------------------

# Declare the dependencies of an ImtCore library, ignoring any entry whose target
# does not exist in the current configuration (for example feature-gated
# libraries, or Acf::/AcfSln::/IAcf:: targets that are not available because the
# legacy shim is used instead of find_package).
function(imt_declare_library_dependencies target)
	if(NOT TARGET ${target})
		return()
	endif()

	foreach(dependency IN LISTS ARGN)
		if(TARGET ${dependency})
			target_link_libraries(${target} ${ACF_LIBRARY_LINK_SCOPE} ${dependency})
		endif()
	endforeach()
endfunction()

# Declare Qt module dependencies for an ImtCore library. Uses the
# ACF_QT_MODULE_LINK_SCOPE variable (typically PRIVATE) rather than
# ACF_LIBRARY_LINK_SCOPE. Entries whose target does not exist are silently
# ignored (for example optional Qt modules on specific platforms).
function(imt_declare_qt_dependencies target)
	if(NOT TARGET ${target})
		return()
	endif()

	foreach(dependency IN LISTS ARGN)
		if(TARGET ${dependency})
			target_link_libraries(${target} ${ACF_QT_MODULE_LINK_SCOPE} ${dependency})
		endif()
	endforeach()
endfunction()

# ---------------------------------------------------------------------------
# Inter-library dependencies (minimal direct deps only; transitive propagation
# is relied upon for indirect deps).
#
# Key transitive chains (for reference when adding new entries):
#   imtbase → Acf::{ibase,icomp,idoc,ifile,ilog,imod,iprm,iqt,iser,istd}
#             AcfSln::{iauth,icomm,iinsp,imeas} imtfile imtstyle
#   imtfile → Acf::{ifile,ilog,istd} AcfSln::ifileproc
#   imtauth → imtbase imtcrypt imtdoc imtlic imtmail + Acf::iimg
#   imtdb   → imtapp imtauth imtcol imtrest
#   imtrest → imtclientgql imtcom imtservergql
# ---------------------------------------------------------------------------

imt_declare_library_dependencies(imt3d              	Acf::i3d Acf::iimg Acf::imath AcfSln::icalib)
imt_declare_library_dependencies(imt3dgui           	Acf::iqtgui imt3d imt3dview)
imt_declare_library_dependencies(imt3dview          	imt3d imt3dgui)
imt_declare_library_dependencies(imtapp             	imtbase)
imt_declare_library_dependencies(imtauth            	Acf::iimg imtbase imtcrypt imtdoc imtlic imtmail)
imt_declare_library_dependencies(imtauthdb          	imtauth imtdb imtgql)
imt_declare_library_dependencies(imtauthgql         	imtauthsdl imtbasesdl imtclientgql imtdb imtqml imtrest imtserverapp imtservergql)
imt_declare_library_dependencies(imtauthgui         	Acf::ifilegui Acf::iqtgui imtauth imtcom imtgui imtqml)
imt_declare_library_dependencies(imtbase            	Acf::ibase Acf::icomp Acf::idoc Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::iqt Acf::iser Acf::istd AcfSln::iauth AcfSln::icomm AcfSln::iinsp AcfSln::imeas imtfile imtstyle)
imt_declare_library_dependencies(imtchat            	imtbase imtdb imtrest)
imt_declare_library_dependencies(imtchatdb          	imtchat imtdb)
imt_declare_library_dependencies(imtchatgql         	imtbasesdl imtchat imtchatsdl imtdoc imtgql imtservergql)
imt_declare_library_dependencies(imtclientgql       	imtauth imtbasesdl imtcom imtgql imtservergql)
imt_declare_library_dependencies(imtcol             	imtbase)
imt_declare_library_dependencies(imtcom             	imtbase imtclientgql imtgql imtrest)
imt_declare_library_dependencies(imtcrypt           	imtbase)
imt_declare_library_dependencies(imtdb              	imtapp imtauth imtcol imtrest)
imt_declare_library_dependencies(imtdbgql           	imtcom imtdb imtgql imtrest imtservergql)
imt_declare_library_dependencies(imtdbgui           	Acf::iqtgui imtdb)
imt_declare_library_dependencies(imtddl             	Acf::ilog Acf::iprm Acf::istd AcfSln::iproc)
imt_declare_library_dependencies(imtdesign          	imtbase)
imt_declare_library_dependencies(imtdesk            	imtauth imtdoc)
imt_declare_library_dependencies(imtdeskdb          	imtdb imtdesk imtgql)
imt_declare_library_dependencies(imtdeskgql         	imtchat imtdesk imtdesksdl imtdoc imtgql imtservergql)
imt_declare_library_dependencies(imtdev             	Acf::iattr imtbase)
imt_declare_library_dependencies(imtdevgui          	Acf::iqtgui Acf::iwidgets imtdev)
imt_declare_library_dependencies(imtdoc             	imtbase)
imt_declare_library_dependencies(imtej              	Acf::iprm Acf::iqt Acf::istd)
imt_declare_library_dependencies(imtfile            	Acf::ifile Acf::ilog Acf::istd AcfSln::ifileproc)
imt_declare_library_dependencies(imtgeo             	imtcol imtdb imtservergql)
imt_declare_library_dependencies(imtgql             	imtappsdl imtauth imtcom imtrest)
imt_declare_library_dependencies(imtgqltest         	Acf::ipackage imtcom imtdb imtgql imtrest)
imt_declare_library_dependencies(imtgui             	Acf::iattr Acf::ifilegui Acf::iqtdoc Acf::iqtgui Acf::iwidgets imtbase imtcol imtwidgets)
imt_declare_library_dependencies(imtguigql          	imtservergql)
imt_declare_library_dependencies(imthttp            	imtcom)
imt_declare_library_dependencies(imthype            	Acf::iimg AcfSln::iinsp AcfSln::iproc imtbase)
imt_declare_library_dependencies(imthypedb          	imtdb imthype)
imt_declare_library_dependencies(imthypegui         	Acf::iqtgui imtgui imthype imtwidgets)
imt_declare_library_dependencies(imtimg             	Acf::iimg imtbase)
imt_declare_library_dependencies(imtlic             	imtbase imtcrypt imtrest)
imt_declare_library_dependencies(imtlicdb           	imtdb imtlic)
imt_declare_library_dependencies(imtlicgql          	imtguigql imtlic imtlicsdl imtservergql imtwidgets)
imt_declare_library_dependencies(imtlicgui          	Acf::ifilegui Acf::iqtgui imtauth imtcrypt imtgui imtlic)
imt_declare_library_dependencies(imtlog             	imtbase imtfile)
imt_declare_library_dependencies(imtloggui          	Acf::iqtgui Acf::iwidgets imtlog)
imt_declare_library_dependencies(imtmail            	Acf::icomp Acf::ilog Acf::iprm Acf::iser Acf::istd)
imt_declare_library_dependencies(imtmdbx            	imtauth imtbasesdl imtdb imtserverapp)
imt_declare_library_dependencies(imtmongo           	imtdb)
imt_declare_library_dependencies(imtoas             	Acf::istd)
imt_declare_library_dependencies(imtpy              	Acf::iattr Acf::ibase Acf::icomp Acf::ifile Acf::ilog Acf::iprm AcfSln::iproc)
imt_declare_library_dependencies(imtqml             	Acf::iqtgui imtbase imtclientgql imtcom imtdoc imtgql)
imt_declare_library_dependencies(imtqml2d           	Acf::i2d Acf::iimg Acf::imod)
imt_declare_library_dependencies(imtrepo            	imtbase imtfile)
imt_declare_library_dependencies(imtreport          	Acf::i2d Acf::icmm Acf::icomp Acf::idoc Acf::iimg Acf::imod Acf::iprm Acf::iser Acf::istd)
imt_declare_library_dependencies(imtreportgui       	Acf::iqtgui imtreport)
imt_declare_library_dependencies(imtrest            	imtbase imtclientgql imtcom imtservergql)
imt_declare_library_dependencies(imtsdl             	AcfSln::iproc imtbase)
imt_declare_library_dependencies(imtsdlgencpp       	AcfSln::iproc imtsdl imtservergql)
imt_declare_library_dependencies(imtsdlgenqml       	AcfSln::iproc imtsdl)
imt_declare_library_dependencies(imtserverapp       	Acf::icmm imtapp imtclientgql imtcol imtcom imtdb imtgql imtlic imtqml imtrest imtsdl imtservice)
imt_declare_library_dependencies(imtservergql       	imtauthgql imtbasesdl imtclientgql imtcol imtcom imtdoc imtgql imthype imtrest imtserverapp)
imt_declare_library_dependencies(imtservice         	imtbase imtcom)
imt_declare_library_dependencies(imtstyle           	Acf::iqtgui imtbase imtdesign)
imt_declare_library_dependencies(imttest            	imtdb)
imt_declare_library_dependencies(imtupdate          	imtbase)
imt_declare_library_dependencies(imtwidgets         	Acf::iwidgets)
imt_declare_library_dependencies(imtzip             	imtfile)

# ---------------------------------------------------------------------------
# SDL (schema-generated) libraries. Their generated C++ pulls in the Acf base
# serialization/component headers and the imt domain SDL types; those flow in
# transitively through the imtsdl runtime and imtbasesdl base-type libraries.
# ---------------------------------------------------------------------------
imt_declare_library_dependencies(imtbasesdl         	imtsdl imtbase)
imt_declare_library_dependencies(imt2dsdl           	imtsdl imtbasesdl)
imt_declare_library_dependencies(imtcolorsdl        	imtsdl imtbasesdl imtcol)
imt_declare_library_dependencies(imtauthsdl         	imtsdl imtbasesdl imtauth)
imt_declare_library_dependencies(imtlicsdl          	imtsdl imtbasesdl imtlic)
imt_declare_library_dependencies(imtappsdl          	imtsdl imtbasesdl imtapp)
imt_declare_library_dependencies(imtpaysdl          	imtsdl imtbasesdl)
imt_declare_library_dependencies(imtchatsdl         	imtsdl imtbasesdl imtchat)
imt_declare_library_dependencies(imtdesksdl         	imtsdl imtbasesdl imtdesk)

# ---------------------------------------------------------------------------
# QML registration libraries. Their generated C++ pulls in the Acf/imt GUI and
# QML headers; those flow in transitively through the imtqml runtime and the
# corresponding imt GUI library.
# ---------------------------------------------------------------------------
imt_declare_library_dependencies(imtstylecontrolsqml	imtqml imtgui imtstyle)
imt_declare_library_dependencies(imt3dguiqml        	imtqml imtgui imt3dgui)
imt_declare_library_dependencies(imtauthguiqml      	imtqml imtgui imtauthgui)
imt_declare_library_dependencies(imtchatguiqml      	imtqml imtgui)
imt_declare_library_dependencies(imtcolguiqml       	imtqml imtgui imtcol)
imt_declare_library_dependencies(imtcontrolsqml     	imtqml imtgui)
imt_declare_library_dependencies(imtdeskguiqml      	imtqml imtgui)
imt_declare_library_dependencies(imtdocguiqml       	imtqml imtgui imtdoc)
imt_declare_library_dependencies(imtgeoguiqml       	imtqml imtgui imtgeo)
imt_declare_library_dependencies(imtguiqml          	imtqml imtgui)
imt_declare_library_dependencies(imtguigqlqml       	imtqml imtgui imtguigql)
imt_declare_library_dependencies(imtlicguiqml       	imtqml imtgui imtlicgui)

# ---------------------------------------------------------------------------
# Qt module dependencies. These are the Qt modules that each static library
# requires directly (headers and/or symbols). The scope is controlled by
# ACF_QT_MODULE_LINK_SCOPE (typically PRIVATE for in-tree builds).
# ---------------------------------------------------------------------------
imt_declare_qt_dependencies(imt3d           	Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
imt_declare_qt_dependencies(imt3dgui        	Qt${QT_VERSION_MAJOR}::OpenGL Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
imt_declare_qt_dependencies(imtauth         	Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
imt_declare_qt_dependencies(imtauthdb       	Qt${QT_VERSION_MAJOR}::Sql)
imt_declare_qt_dependencies(imtauthgql      	Qt${QT_VERSION_MAJOR}::Sql)
imt_declare_qt_dependencies(imtchat         	Qt${QT_VERSION_MAJOR}::Sql)
imt_declare_qt_dependencies(imtchatdb       	Qt${QT_VERSION_MAJOR}::Sql)
imt_declare_qt_dependencies(imtclientgql    	Qt${QT_VERSION_MAJOR}::WebSockets)
imt_declare_qt_dependencies(imtdb           	Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg Qt${QT_VERSION_MAJOR}::Sql)
imt_declare_qt_dependencies(imtdbgql        	Qt${QT_VERSION_MAJOR}::WebSockets)
imt_declare_qt_dependencies(imtdesign       	Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
imt_declare_qt_dependencies(imtej           	Qt${QT_VERSION_MAJOR}::PrintSupport)
imt_declare_qt_dependencies(imtgeo          	Qt${QT_VERSION_MAJOR}::Positioning Qt${QT_VERSION_MAJOR}::Core)
if(QT_VERSION_MAJOR EQUAL 5)
	imt_declare_qt_dependencies(imtgeo      	Qt${QT_VERSION_MAJOR}::Location)
endif()
imt_declare_qt_dependencies(imtgql          	Qt${QT_VERSION_MAJOR}::WebSockets)
imt_declare_qt_dependencies(imthypegui      	Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
imt_declare_qt_dependencies(imtlicdb        	Qt${QT_VERSION_MAJOR}::Sql)
imt_declare_qt_dependencies(imtloggui       	Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
imt_declare_qt_dependencies(imtdeskdb       	Qt${QT_VERSION_MAJOR}::Sql)
imt_declare_qt_dependencies(imthypedb       	Qt${QT_VERSION_MAJOR}::Sql)
imt_declare_qt_dependencies(imtqml          	Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::Concurrent)
imt_declare_qt_dependencies(imtreportgui    	Qt${QT_VERSION_MAJOR}::PrintSupport)
imt_declare_qt_dependencies(imtrest         	Qt${QT_VERSION_MAJOR}::WebSockets)
imt_declare_qt_dependencies(imtservergql    	Qt${QT_VERSION_MAJOR}::WebSockets)
imt_declare_qt_dependencies(imtstyle        	Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
imt_declare_qt_dependencies(imttest         	Qt${QT_VERSION_MAJOR}::Sql)
imt_declare_qt_dependencies(imtupdate       	Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)
if(QT_VERSION_MAJOR EQUAL 6)
	imt_declare_qt_dependencies(imtzip      	Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()
