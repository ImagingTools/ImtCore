# Registration of the tests with CTest, on top of add_tests of ACF.
#
# The tests are registered by name, so `ctest -L <label>` runs the same tests on every platform and never starts a
# server program by accident. Labels in use: unit - in process, no services; database - needs PostgreSQL
# (TEST_PG_*); api - test suites against a running server, which need PostgreSQL and Node.js; qml - Qt Quick Tests
# of the QML screens, they need the built ImtCore Qt Quick Test program.
# A test that runs past its timeout is stopped and fails.

# add_tests: test programs
include(${ACFDIR}/Config/CMake/AcfTestRegistration.cmake)


# add_qml_tests(<name> DIRECTORY <dir> [IMPORT_DIRS <dir>...] [LABEL <label>] [TIMEOUT <seconds>])
#
# Registers the Qt Quick Tests (tst_*.qml) of DIRECTORY. They are run from their sources by the Qt Quick Test program of
# ImtCore (ImtCoreQmlTest), which provides the ImtCore QML modules and replaces the GraphQL server with
# imtqmltest.GqlTestTransport. IMPORT_DIRS are the folders with the QML modules that the tests import, such as the ones
# of the SDL. The program prints nothing to the console, its report is the file <name>.txt in the folder Testing of
# the build. LABEL defaults to qml. Without the program the tests are not registered.
function(add_qml_tests testName)
	cmake_parse_arguments(PARSE_ARGV 1 ARG "" "DIRECTORY;LABEL;TIMEOUT" "IMPORT_DIRS")
	if(NOT ARG_DIRECTORY)
		message(FATAL_ERROR "add_qml_tests: DIRECTORY is required")
	endif()

	if(NOT ARG_LABEL)
		set(ARG_LABEL qml)
	endif()

	if(NOT ARG_TIMEOUT)
		set(ARG_TIMEOUT 300)
	endif()

	find_program(QML_TEST_PROGRAM
		NAMES ImtCoreQmlTest
		PATHS "${IMTCOREDIR_BUILD}/Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME}"
		NO_DEFAULT_PATH
	)
	if(NOT QML_TEST_PROGRAM)
		message(STATUS "ImtCoreQmlTest is not built: the QML tests ${testName} are not registered with CTest")

		return()
	endif()

	set(programArguments -input "${ARG_DIRECTORY}")
	foreach(importDir IN LISTS ARG_IMPORT_DIRS)
		list(APPEND programArguments -import "${importDir}")
	endforeach()
	list(APPEND programArguments -o "${CMAKE_BINARY_DIR}/Testing/${testName}.txt,txt")

	add_test(NAME ${testName} COMMAND "${QML_TEST_PROGRAM}" ${programArguments})
	set_tests_properties(${testName} PROPERTIES LABELS ${ARG_LABEL} TIMEOUT ${ARG_TIMEOUT})
endfunction()


# add_node_test_suite(<name> DIRECTORY <dir> LABEL <label> [SERVER <target>] [SERVICES <name>=<path>...]
#		[TIMEOUT <seconds>])
#
# Registers a Node.js test suite: `npm ci` in DIRECTORY as a fixture, then `node run-tests.mjs`. SERVER passes the
# built program of that target to the runner (--bin); without it, the runner looks the server up by itself. SERVICES
# passes the programs of the services the suite starts before the server, such as the authorization server of the
# product, each as <name>=<path> (--service-bin); a service that is not listed is looked up by the runner as well.
# Without Node.js or npm the suite is not registered.
function(add_node_test_suite suiteName)
	cmake_parse_arguments(PARSE_ARGV 1 ARG "" "DIRECTORY;SERVER;LABEL;TIMEOUT" "SERVICES")
	if(NOT ARG_DIRECTORY OR NOT ARG_LABEL)
		message(FATAL_ERROR "add_node_test_suite: DIRECTORY and LABEL are required")
	endif()

	if(NOT ARG_TIMEOUT)
		set(ARG_TIMEOUT 1800)
	endif()

	find_program(NODE_EXECUTABLE node)
	find_program(NPM_EXECUTABLE NAMES npm.cmd npm)
	set(missingPrograms)
	if(NOT NODE_EXECUTABLE)
		list(APPEND missingPrograms "Node.js")
	endif()

	if(NOT NPM_EXECUTABLE)
		list(APPEND missingPrograms "npm")
	endif()

	if(missingPrograms)
		list(JOIN missingPrograms " and " missingProgramsText)
		message(STATUS "${missingProgramsText} not found: the test suite ${suiteName} is not registered with CTest")

		return()
	endif()

	# On Windows npm is npm.cmd, which Windows starts by itself; wrapping it in "cmd /c" breaks on the quotes CTest
	# puts around a path with spaces
	add_test(NAME ${suiteName}Install COMMAND "${NPM_EXECUTABLE}" ci --no-audit --no-fund WORKING_DIRECTORY "${ARG_DIRECTORY}")
	set_tests_properties(${suiteName}Install PROPERTIES LABELS ${ARG_LABEL} TIMEOUT 600 FIXTURES_SETUP ${suiteName}Modules)

	set(runnerArguments run-tests.mjs)
	if(ARG_SERVER)
		list(APPEND runnerArguments --bin "$<TARGET_FILE:${ARG_SERVER}>")
	endif()

	foreach(service IN LISTS ARG_SERVICES)
		list(APPEND runnerArguments --service-bin "${service}")
	endforeach()

	add_test(NAME ${suiteName} COMMAND "${NODE_EXECUTABLE}" ${runnerArguments} WORKING_DIRECTORY "${ARG_DIRECTORY}")
	# The runner loads the node test kit from the ImtCore checkout (Tests/NodeTestKit)
	set_tests_properties(${suiteName} PROPERTIES LABELS ${ARG_LABEL} TIMEOUT ${ARG_TIMEOUT} FIXTURES_REQUIRED ${suiteName}Modules ENVIRONMENT "IMTCOREDIR=${IMTCOREDIR}")
endfunction()

