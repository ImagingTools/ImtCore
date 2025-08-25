cmake_minimum_required(VERSION 3.26)
project(imtpaysdl LANGUAGES CXX DESCRIPTION "The basic library for working with payments/transactions")

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlCustomConfig.cmake)

add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
	${CMAKE_CURRENT_LIST_DIR}/../Sdl/ImtPay.sdl)


ImtCoreCustomConfigureSdlCppQml(CXX QML
	SCHEMA_PATH	"${CMAKE_CURRENT_LIST_DIR}/../Sdl/ImtPay.sdl"
	VERSION		"1.0")
