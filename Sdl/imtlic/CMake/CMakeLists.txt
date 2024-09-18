project(imtlicsdl LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCoreSdlModuleConfig.cmake)

ImtCoreModuleConfigureSdlCppQml("1.0/Features" "CPP/Features" "sdl::imtlic::Features" "QML" "imtlicFeaturesSdl")
ImtCoreModuleConfigureSdlCppQml("1.0/Licenses" "CPP/Licenses" "sdl::imtlic::Licenses" "QML" "imtlicLicensesSdl")
ImtCoreModuleConfigureSdlCppQml("1.0/Products" "CPP/Products" "sdl::imtlic::Products" "QML" "imtlicProductsSdl")

