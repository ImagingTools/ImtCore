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
# The dependencies below are derived from the #include graph of each library and
# the historical explicit target_link_libraries() lists. Dependencies onto the
# underlying ACF / ACF-Solutions / IAcf foundations are expressed through the
# Acf::<lib> / AcfSln::<lib> / IAcf::<lib> imported targets; entries whose target
# does not exist in the current configuration (for example when the foundations
# are still consumed through the legacy environment-variable shim rather than
# find_package, or when a feature-gated library is not built) are silently
# ignored. A few libraries are mutually dependent (e.g. imtbase <-> imtstyle and
# imt3d <-> imt3dgui); CMake explicitly allows cyclic dependencies between static
# libraries.
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

imt_declare_library_dependencies(imt3d              	Acf::i3d Acf::icomp Acf::idoc Acf::ifile Acf::iimg Acf::ilog Acf::imath Acf::iser Acf::istd AcfSln::icalib)
imt_declare_library_dependencies(imt3dgui           	Acf::ibase Acf::imod Acf::iqtgui Acf::istd imt3d imt3dview)
imt_declare_library_dependencies(imt3dview          	Acf::icomp Acf::imod Acf::istd imt3d imt3dgui)
imt_declare_library_dependencies(imtapp             	Acf::icomp Acf::ifile Acf::iser Acf::istd imtbase)
imt_declare_library_dependencies(imtauth            	Acf::icomp Acf::idoc Acf::ifile Acf::iimg Acf::ilog Acf::imod Acf::iprm Acf::iqt Acf::iser Acf::istd AcfSln::iauth imtbase imtcrypt imtdoc imtlic imtmail)
imt_declare_library_dependencies(imtauthdb          	Acf::icomp Acf::idoc Acf::imod Acf::iprm Acf::iser Acf::istd imtauth imtbase imtdb imtgql imtlic)
imt_declare_library_dependencies(imtauthgql         	Acf::ibase Acf::icomp Acf::ilog Acf::imod Acf::iprm Acf::iqt Acf::iser Acf::istd AcfSln::iauth imtauth imtauthsdl imtbase imtbasesdl imtclientgql imtcrypt imtdb imtdoc imtgql imtlic imtqml imtrest imtsdl imtserverapp imtservergql)
imt_declare_library_dependencies(imtauthgui         	Acf::ibase Acf::ifile Acf::ifilegui Acf::iimg Acf::ilog Acf::imod Acf::iprm Acf::iqt Acf::iqtgui Acf::istd AcfSln::iauth imtauth imtbase imtcom imtgui imtqml)
imt_declare_library_dependencies(imtbase            	Acf::ibase Acf::icomp Acf::idoc Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::iqt Acf::iser Acf::istd AcfSln::iauth AcfSln::icomm AcfSln::iinsp AcfSln::imeas imtfile imtstyle)
imt_declare_library_dependencies(imtchat            	Acf::icomp Acf::iser Acf::istd imtbase imtdb imtrest)
imt_declare_library_dependencies(imtchatdb          	Acf::imod Acf::iprm imtbase imtchat imtdb)
imt_declare_library_dependencies(imtchatgql         	Acf::istd imtbase imtbasesdl imtchat imtchatsdl imtdoc imtgql imtservergql)
imt_declare_library_dependencies(imtclientgql       	Acf::ibase Acf::icomp Acf::idoc Acf::ilog Acf::imod Acf::iprm Acf::iser Acf::istd imtauth imtbase imtbasesdl imtcom imtgql imtrest imtservergql)
imt_declare_library_dependencies(imtcol             	Acf::icomp Acf::ilog Acf::iser Acf::istd imtbase)
imt_declare_library_dependencies(imtcom             	Acf::icomp Acf::ifile Acf::ilog Acf::iprm Acf::iser Acf::istd imtbase imtclientgql imtgql imtrest)
imt_declare_library_dependencies(imtcrypt           	Acf::icomp Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::istd imtbase)
imt_declare_library_dependencies(imtdb              	Acf::icomp Acf::idoc Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::iser Acf::istd imtapp imtauth imtbase imtcol imtrest)
imt_declare_library_dependencies(imtdbgql           	Acf::idoc Acf::iprm Acf::istd imtbase imtcom imtdb imtgql imtrest imtservergql)
imt_declare_library_dependencies(imtdbgui           	Acf::iprm Acf::iqtgui Acf::istd imtbase imtdb)
imt_declare_library_dependencies(imtddl             	Acf::ilog Acf::iprm Acf::istd AcfSln::iproc)
imt_declare_library_dependencies(imtdesign          	Acf::ifile Acf::ilog Acf::iprm Acf::istd imtbase imtstyle)
imt_declare_library_dependencies(imtdesk            	Acf::icomp Acf::iser Acf::istd imtauth imtbase imtdoc)
imt_declare_library_dependencies(imtdeskdb          	Acf::icomp Acf::iprm imtauth imtbase imtdb imtdesk imtgql)
imt_declare_library_dependencies(imtdeskgql         	Acf::ibase Acf::imod Acf::iprm Acf::istd imtauth imtbase imtchat imtdesk imtdesksdl imtdoc imtgql imtservergql)
imt_declare_library_dependencies(imtdev             	Acf::iattr Acf::icomp Acf::idoc Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::iser Acf::istd imtbase)
imt_declare_library_dependencies(imtdevgui          	Acf::iprm Acf::iqtgui Acf::iwidgets imtbase imtdev)
imt_declare_library_dependencies(imtdoc             	Acf::idoc Acf::ilog Acf::imod Acf::iser Acf::istd imtbase)
imt_declare_library_dependencies(imtej              	Acf::iprm Acf::iqt Acf::istd)
imt_declare_library_dependencies(imtfile            	Acf::ifile Acf::ilog Acf::istd AcfSln::ifileproc)
imt_declare_library_dependencies(imtgeo             	Acf::idoc Acf::imod Acf::iprm Acf::iser Acf::istd imtbase imtcol imtdb imtservergql)
imt_declare_library_dependencies(imtgql             	Acf::iqt Acf::iser Acf::istd imtappsdl imtauth imtbase imtcom imtrest)
imt_declare_library_dependencies(imtgqltest         	Acf::ipackage Acf::istd imtbase imtcom imtdb imtgql imtrest)
imt_declare_library_dependencies(imtgui             	Acf::iattr Acf::ibase Acf::icomp Acf::idoc Acf::ifile Acf::ifilegui Acf::ilog Acf::imod Acf::iprm Acf::iqtdoc Acf::iqtgui Acf::iser Acf::istd Acf::iwidgets AcfSln::iauth AcfSln::imeas imtbase imtcol imtstyle imtwidgets)
imt_declare_library_dependencies(imtguigql          	Acf::iprm imtservergql)
imt_declare_library_dependencies(imthttp            	imtcom)
imt_declare_library_dependencies(imthype            	Acf::ibase Acf::icomp Acf::idoc Acf::ilog Acf::imod Acf::iprm Acf::iser Acf::istd AcfSln::iinsp AcfSln::iproc imtbase)
imt_declare_library_dependencies(imthypedb          	Acf::iser imtbase imtdb imthype)
imt_declare_library_dependencies(imthypegui         	Acf::ibase Acf::ifile Acf::iimg Acf::imod Acf::iprm Acf::iqtgui Acf::istd imtbase imtgui imthype imtwidgets)
imt_declare_library_dependencies(imtimg             	Acf::idoc Acf::iimg Acf::imod imtbase)
imt_declare_library_dependencies(imtlic             	Acf::icomp Acf::idoc Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::iqt Acf::iser Acf::istd AcfSln::iauth imtbase imtcrypt imtrest)
imt_declare_library_dependencies(imtlicdb           	Acf::iprm imtbase imtdb imtlic)
imt_declare_library_dependencies(imtlicgql          	Acf::icomp Acf::ifile Acf::ilog Acf::iprm Acf::istd imtauth imtbase imtdb imtgui imtguigql imtlic imtlicsdl imtservergql imtwidgets)
imt_declare_library_dependencies(imtlicgui          	Acf::ifile Acf::ifilegui Acf::ilog Acf::imod Acf::iprm Acf::iqtgui Acf::iser Acf::istd imtauth imtbase imtcrypt imtgui imtlic)
imt_declare_library_dependencies(imtlog             	Acf::ibase Acf::icomp Acf::ifile Acf::ilog Acf::imod Acf::iser Acf::istd AcfSln::iauth imtbase imtfile)
imt_declare_library_dependencies(imtloggui          	Acf::ibase Acf::icomp Acf::idoc Acf::ilog Acf::imod Acf::iprm Acf::iqtgui Acf::iser Acf::istd Acf::iwidgets AcfSln::imeas imtbase imtlog)
imt_declare_library_dependencies(imtmail            	Acf::icomp Acf::ilog Acf::iprm Acf::iser Acf::istd)
imt_declare_library_dependencies(imtmdbx            	Acf::icomp Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::istd imtauth imtbase imtbasesdl imtdb imtserverapp)
imt_declare_library_dependencies(imtmongo           	Acf::idoc Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::istd imtbase imtdb)
imt_declare_library_dependencies(imtoas             	Acf::istd)
imt_declare_library_dependencies(imtpy              	Acf::iattr Acf::ibase Acf::icomp Acf::ifile Acf::ilog Acf::iprm AcfSln::iproc)
imt_declare_library_dependencies(imtqml             	Acf::ibase Acf::icomp Acf::idoc Acf::ilog Acf::imod Acf::iprm Acf::iqt Acf::iqtgui Acf::istd imtbase imtclientgql imtcom imtdoc imtgql)
imt_declare_library_dependencies(imtqml2d           	Acf::i2d Acf::iimg Acf::imod)
imt_declare_library_dependencies(imtrepo            	Acf::idoc Acf::ifile Acf::ilog Acf::imod Acf::iser Acf::istd AcfSln::iauth imtbase imtfile)
imt_declare_library_dependencies(imtreport          	Acf::i2d Acf::ibase Acf::icmm Acf::icomp Acf::idoc Acf::iimg Acf::imod Acf::iprm Acf::iser Acf::istd)
imt_declare_library_dependencies(imtreportgui       	Acf::i2d Acf::ibase Acf::icomp Acf::ifile Acf::ilog Acf::iqtgui imtreport)
imt_declare_library_dependencies(imtrest            	Acf::ibase Acf::icomp Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::iser Acf::istd imtbase imtclientgql imtcom imtservergql)
imt_declare_library_dependencies(imtsdl             	Acf::icomp Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::iser Acf::istd AcfSln::iproc imtbase)
imt_declare_library_dependencies(imtsdlgencpp       	Acf::icomp Acf::ifile Acf::ilog Acf::iprm Acf::istd AcfSln::iproc imtbase imtsdl imtservergql)
imt_declare_library_dependencies(imtsdlgenqml       	Acf::ifile Acf::ilog Acf::iprm Acf::iser Acf::istd AcfSln::iproc imtsdl)
imt_declare_library_dependencies(imtserverapp       	Acf::icmm Acf::icomp Acf::idoc Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::iqt Acf::iser Acf::istd imtapp imtauth imtbase imtclientgql imtcol imtcom imtdb imtgql imtlic imtqml imtrest imtsdl imtservice)
imt_declare_library_dependencies(imtservergql       	Acf::ibase Acf::icomp Acf::idoc Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::iqt Acf::iser Acf::istd imtauth imtauthgql imtbase imtbasesdl imtclientgql imtcol imtcom imtdoc imtgql imthype imtrest imtserverapp)
imt_declare_library_dependencies(imtservice         	Acf::ibase Acf::icomp Acf::ilog Acf::iser Acf::istd imtbase imtcom)
imt_declare_library_dependencies(imtstyle           	Acf::ibase Acf::icomp Acf::ilog Acf::imod Acf::iprm Acf::iqt Acf::iqtgui Acf::istd imtbase imtdesign)
imt_declare_library_dependencies(imttest            	Acf::idoc Acf::imod Acf::iser Acf::istd imtbase imtdb)
imt_declare_library_dependencies(imtupdate          	Acf::ibase Acf::ifile Acf::ilog Acf::imod Acf::iprm Acf::iser Acf::istd imtbase)
imt_declare_library_dependencies(imtwidgets         	Acf::iwidgets)
imt_declare_library_dependencies(imtzip             	Acf::icomp imtfile)

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
