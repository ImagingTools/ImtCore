cmake_minimum_required(VERSION 3.9)
project(imtpay LANGUAGES CXX DESCRIPTION "The basic library for working with payments/transactions")

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlAutoConfig.cmake)
add_custom_target(${PROJECT_NAME}-SDL-related-files SOURCES
			${CMAKE_CURRENT_LIST_DIR}/../Sdl/ImtPay.sdl)

ImtCoreAutoConfigureSdlCppQml(
			SDL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/../Sdl/ImtPay.sdl"
			CUSTOM_TARGETS_DEPS ON
			CUSTOM_HEADER_DEP "1.0/CPP/Receipt.h"
			CUSTOM_SOURCE_DEP "1.0/CPP/Receipt.cpp"
			CUSTOM_QML_RES_DEP "1.0/QML/imtpayReceiptSdl/imtpayReceiptSdl.qrc")
