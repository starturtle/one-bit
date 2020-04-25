find_package( Qt5 REQUIRED COMPONENTS Core Gui Qml Quick Network )
find_package(Qt5QuickCompiler)
add_library( gui
	UiApplication.cpp
	UiApplication.h
)
target_include_directories(gui PRIVATE  ${Qt5_DIR})
target_link_libraries( gui PUBLIC Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Network )
target_include_directories( gui PUBLIC ${CMAKE_SOURCE_DIR}/utilities )
target_link_libraries( gui PUBLIC utilities )
qtquick_compiler_add_resources(RESOURCES resources/one-bit-resources.qrc)