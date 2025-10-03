project(imtbasesdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtBaseTypes.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ComplexCollectionFilter.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtCollection.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/DocumentRevision.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/MimeType.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/DocumentCollectionFilter.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Search.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Commands.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/Settings.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/CollectionImport.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/ProgressManager.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/DocumentManager.sdl
			${CMAKE_CURRENT_LIST_DIR}/../1.0/UndoManager.sdl
		)

ImtCoreCustomConfigureSdlCppQml(CXX QML
			SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtBaseTypes.sdl"
			VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ComplexCollectionFilter.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ImtCollection.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/DocumentRevision.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/MimeType.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/DocumentCollectionFilter.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Search.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Commands.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/Settings.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/CollectionImport.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/ProgressManager.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/DocumentManager.sdl"
	VERSION		"1.0")

ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../1.0/UndoManager.sdl"
	VERSION		"1.0")


