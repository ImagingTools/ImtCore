if (USE_FIND_PACKAGE)
	find_package(OpenSSL)
	target_link_libraries(${PROJECT_NAME} PUBLIC OpenSSL::Crypto OpenSSL::SSL)

	return()
endif()

if (OPENSSLDIR)
	file(TO_CMAKE_PATH "${OPENSSLDIR}" OPENSSLDIR)
endif()

include_directories(${OPENSSLDIR})

if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(OPENSSL_WIN_ARCH x64)
	else()
		set(OPENSSL_WIN_ARCH x86)
	endif()

	target_link_libraries(${PROJECT_NAME} PUBLIC
		${OPENSSLDIR}/lib/${OPENSSL_WIN_ARCH}/libcrypto.lib
		${OPENSSLDIR}/lib/${OPENSSL_WIN_ARCH}/libssl.lib
	)
elseif(ANDROID)
	if(CMAKE_ANDROID_ARCH_ABI)
		set(OPENSSL_ANDROID_ABI ${CMAKE_ANDROID_ARCH_ABI})
	else()
		set(OPENSSL_ANDROID_ABI ${ANDROID_ABI})
	endif()

	set(OPENSSL_ANDROID_LIB_DIR ${OPENSSLDIR}/lib/android/${OPENSSL_ANDROID_ABI})
	if(NOT EXISTS ${OPENSSL_ANDROID_LIB_DIR})
		message(FATAL_ERROR "OpenSSL libraries for Android ABI '${OPENSSL_ANDROID_ABI}' not found in ${OPENSSL_ANDROID_LIB_DIR}")
	endif()

	file(GLOB OPENSSL_ANDROID_CRYPTO ${OPENSSL_ANDROID_LIB_DIR}/libcrypto_*.so)
	file(GLOB OPENSSL_ANDROID_SSL ${OPENSSL_ANDROID_LIB_DIR}/libssl_*.so)

	target_link_libraries(${PROJECT_NAME} PUBLIC 
		${OPENSSL_ANDROID_CRYPTO} 
		${OPENSSL_ANDROID_SSL}
	)

	set(ANDROID_EXTRA_LIBS
		${OPENSSL_ANDROID_CRYPTO}
		${OPENSSL_ANDROID_SSL}
		CACHE INTERNAL ""
	)
	message("ANDROID_EXTRA_LIBS ${ANDROID_EXTRA_LIBS}")
elseif(APPLE)
	if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
		set(OPENSSL_OSX_ARCH arm64)
	else()
		set(OPENSSL_OSX_ARCH x64)
	endif()

	target_link_libraries(${PROJECT_NAME} PUBLIC
		${OPENSSLDIR}/lib/ClangOSX_${OPENSSL_OSX_ARCH}/libssl.a
		${OPENSSLDIR}/lib/ClangOSX_${OPENSSL_OSX_ARCH}/libcrypto.a
	)
elseif(UNIX)
	file(GLOB OPENSSL_CRYPTO ${OPENSSLDIR}/lib/linux/libcrypto.so*)
	file(GLOB OPENSSL_SSL ${OPENSSLDIR}/lib/linux/libssl.so*)
	
	target_link_libraries(${PROJECT_NAME} PUBLIC 
		${OPENSSL_SSL} 
		${OPENSSL_CRYPTO}
	)
endif()
