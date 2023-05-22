set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)

set (BOOST_ROOT "${IMTCOREDIR}/3rdParty/boost/1.75.0/")
set (Boost_USE_STATIC_LIBS ON)
find_package (Boost REQUIRED)# COMPONENTS program_options random REQUIRED)
# Если библиотека найдена, то
if (Boost_FOUND)
	include_directories (${Boost_INCLUDE_DIR})
	target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES}) # подключаем библиотеку
endif ()

include_directories(${IMTCOREDIR}/3rdParty/mongocxx/mongo-cxx-driver-3.7.0/include)
target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/mongocxx/mongo-cxx-driver-3.7.0/lib/Debug_VC17_x64/bsoncxx-static.lib")
target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/mongocxx/mongo-cxx-driver-3.7.0/lib/Debug_VC17_x64/mongocxx-static.lib")

include_directories(${IMTCOREDIR}/3rdParty/mongoc/mongo-c-driver-1.23.2/include/libbson-1.0)
target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/mongoc/mongo-c-driver-1.23.2/lib/Debug_VC17_x64/bson-static-1.0.lib")

include_directories(${IMTCOREDIR}/3rdParty/mongoc/mongo-c-driver-1.23.2/include/libmongoc-1.0)
target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/mongoc/mongo-c-driver-1.23.2/lib/Debug_VC17_x64/mongoc-static-1.0.lib")


include_directories(${IMTCOREDIR}/3rdParty/icu/icu-72.1/include)
target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/icu/icu-72.1/lib/Debug_VC17_x64/sicudtd.lib")
target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/icu/icu-72.1/lib/Debug_VC17_x64/sicuucd.lib")

include_directories(${IMTCOREDIR}/3rdParty/zlib/zlib-1.2.13/include)
target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/zlib/zlib-1.2.13/lib/Debug_VC17_x64/zlib.lib")

include_directories(${IMTCOREDIR}/3rdParty/zstd/zstd-1.5.4/include)
target_link_libraries(${PROJECT_NAME} "${IMTCOREDIR}/3rdParty/zstd/zstd-1.5.4/lib/Debug_VC17_x64/zstd_static.lib")

target_link_libraries(${PROJECT_NAME} Ws2_32 crypt32 secur32 dnsapi advapi32 bcrypt shlwapi)

