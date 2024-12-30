cmake_minimum_required(VERSION 3.16)

project(MusicPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)
find_package(QHotkey REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appMusicPlayer
    main.cpp
)

qt_add_resources(appMusicPlayer "app_images"
    PREFIX "/"
    FILES
        assets/disk.jpeg
)


qt_add_qml_module(appMusicPlayer
    URI MusicPlayer
    VERSION 1.0
    QML_FILES
        Main.qml
        QML_FILES MyWindow.qml
        RESOURCES
        assets/icon/clear.svg
        assets/icon/download.svg
        assets/icon/drag.svg
        assets/icon/folder_open.svg
        assets/icon/left.svg
        assets/icon/list.svg
        assets/icon/list_cycle.svg
        assets/icon/net_ease_cloud.svg
        assets/icon/pause.svg
        assets/icon/play.svg
        assets/icon/poweroff.svg
        assets/icon/pull_down.svg
        assets/icon/random.svg
        assets/icon/right.svg
        assets/icon/search.svg
        assets/icon/single_cycle.svg
        SOURCES songattachedpicprovider.h songattachedpicprovider.cpp
        RESOURCES
        QML_FILES MyImage.qml
        SOURCES songitem.h songitem.cpp
        SOURCES songhelper.h songhelper.cpp
        SOURCES localsongitem.h localsongitem.cpp
        SOURCES visualizer.h visualizer.cpp
        SOURCES barsvisualizer.h barsvisualizer.cpp
        SOURCES neteasecloudmusicsongitem.h neteasecloudmusicsongitem.cpp
        SOURCES asyncimageresponse.h asyncimageresponse.cpp

)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appMusicPlayer PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appMusicPlayer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appMusicPlayer
    PRIVATE Qt6::Quick
)
target_link_libraries(appMusicPlayer
    PRIVATE qhotkey
)
target_link_libraries(appMusicPlayer
    PRIVATE Qt6::Multimedia
)
target_link_libraries(appMusicPlayer
    PRIVATE Qt6::Widgets
)

include(GNUInstallDirs)
install(TARGETS appMusicPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 设置FFmpeg的安装路径
set(FFMPEG_HOME "C:/Program Files/ffmpeg-master-latest-win64-gpl-shared")

# 添加FFmpeg的头文件路径
include_directories(${FFMPEG_HOME}/include)

# 获取FFmpeg相关的链接库
file(GLOB FFMPEG_LIBRARIES "${FFMPEG_HOME}/lib/*.lib")

# 添加FFmpeg相关的链接库
target_link_libraries(appMusicPlayer PRIVATE ${FFMPEG_LIBRARIES})


target_link_libraries(appMusicPlayer PRIVATE "C:/Users/PC/Desktop/learning/Qt_learning/MyProjects/MusicPlayer/fftw-3.3.5-dll64/libfftw3f-3.lib")

set(LIBRARY_DIR "${PROJECT_SOURCE_DIR}/QCloudMusicApi")
# 添加头文件路径
include_directories(${LIBRARY_DIR}/include)
# 添加库文件路径
link_directories(${LIBRARY_DIR}/bin)

file(GLOB QCLOUDMUSICAPI_LIBRARIES "${LIBRARY_DIR}/lib/*.lib")
target_link_libraries(appMusicPlayer PRIVATE ${QCLOUDMUSICAPI_LIBRARIES})
