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
	file(GLOB OPENSSL_ANDROID_CRYPTO ${OPENSSLDIR}/lib/android/libcrypto_*.so)
	file(GLOB OPENSSL_ANDROID_SSL ${OPENSSLDIR}/lib/android/libssl_*.so)

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
	target_link_libraries(${PROJECT_NAME} PUBLIC
		# no compiled lib for v1.1
		# ${OPENSSLDIR}/lib/linux/libssl.so
		${OPENSSLDIR}/lib/linux/libcrypto.so
	)
endif()
