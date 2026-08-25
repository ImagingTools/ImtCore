project(ControlsGalleryGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	controlsgallerygql
	controlsgallerysdl
	imtservergql
	imtserverapp
	imtgql
	imtcol
	imtbase
	imtauth
	imtbasesdl
)


