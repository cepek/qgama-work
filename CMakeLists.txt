cmake_minimum_required(VERSION 3.18)

project(qgama LANGUAGES CXX VERSION "2.06")

set(QGAMA_CONFIGURE_NAME      ${PROJECT_NAME})
set(QGAMA_CONFIGURE_VERSION   ${PROJECT_VERSION})
set(QGAMA_CONFIGURE_COPYRIGHT "2023")

configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/configure.h.in"
                "${CMAKE_CURRENT_SOURCE_DIR}/qgama/configure.h")

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/gama)
    message("Qgama cannot be build without GNU Gama project")
    message("You can get the source codes from GIT or FTP server")
    message("   * git clone git://git.savannah.gnu.org/gama.git")
    message("   * ftp://ftp.gnu.org/gnu/gama")

    message(FATAL_ERROR
            "Directory ${CMAKE_CURRENT_SOURCE_DIR}/gama is missing")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# CMAKE_PREFIX_PATH  By default this is empty. It is intended to be set by
# the project. You can use CMakeLists.txt as it is or set up the environment
# variable CMAKE_PREFIX_PATH.
#
# example:
#
#   cmake .. -GNinja -DCMAKE_PREFIX_PATH=C:/Qt/6.2.2/msvc2019_64/
#

if (NOT DEFINED CMAKE_PREFIX_PATH)
#
#   Linux
#
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_PREFIX_PATH "~/Qt/6.5.3/gcc_64/")
endif()
#
#   $ cmake .. -G Ninja
#   $ cmake --build .
#
#
#   Windows
#
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.2.2/msvc2019_64/")
endif()
#
# On Windows you can build qgama either with QtCreator, Visual Studio
# or simply using cmake (On Windows cmake uses Visual Studio as the
# implicit generator). Anyway you would have to set the
# CMAKE_PREFIX_PATH properly. Also you would probably need to specify
# if you want Release build or Debug build (implicit).
#
# From the PowerShell prompt run
#
#   > cmake ..
#   > cmake --build . --config Release
#
# and
#
#   > cpack -C Release -G ZIP
#
# to create the distribution archive qgama-${VERSION}-win64.zip
#

# QtCreator supports the following variables for Android, which are
# identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt6 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

endif()  # end of CMAKE_PREFIX_PATH setting
#
message("   -- Qgama cmake build CMAKE_PREFIX_PATH: " ${CMAKE_PREFIX_PATH})


add_definitions(-DGNU_gama_expat_1_1)  # if we use local copy of expat sources
include_directories(gama/lib qgama)

find_package(Qt6 COMPONENTS
  Core Gui Sql Widgets PrintSupport SvgWidgets REQUIRED)

set(QGSRC
    qgama/main.cpp
    qgama/adjustment.cpp
    qgama/adjustment.h
    qgama/adjustmentresults.cpp
    qgama/adjustmentresults.h
    qgama/constants.cpp
    qgama/constants.h
    qgama/dbconnection.cpp
    qgama/dbconnection.h
    qgama/dbfunctions.cpp
    qgama/dbfunctions.h
    qgama/drawsettings.cpp
    qgama/drawsettings.h
    qgama/importconfigurationfile.cpp
    qgama/importconfigurationfile.h
    qgama/importkrummnetworkfile.cpp
    qgama/importkrummnetworkfile.h
    qgama/insertclusterdialog.cpp
    qgama/insertclusterdialog.h
    qgama/insertobservationdialog.cpp
    qgama/insertobservationdialog.h
    qgama/lineeditdelegate.cpp
    qgama/lineeditdelegate.h
    qgama/networkadjustmentpanel.cpp
    qgama/networkadjustmentpanel.h
    qgama/networksvg.cpp
    qgama/networksvg.h
    qgama/observationeditor.cpp
    qgama/observationeditor.h
    qgama/observationtablemodel.cpp
    qgama/observationtablemodel.h
    qgama/parametereditor.cpp
    qgama/parametereditor.h
    qgama/pointeditor.cpp
    qgama/pointeditor.h
    qgama/pointtablemodel.cpp
    qgama/pointtablemodel.h
    qgama/pointtypecombobox.cpp
    qgama/pointtypecombobox.h
    qgama/qgamacontrolpanel.cpp
    qgama/qgamacontrolpanel.h
    qgama/qgamahelp.cpp
    qgama/qgamahelp.h
    qgama/qgamainterfaces.h
    qgama/selectadjresultslanguage.cpp
    qgama/selectadjresultslanguage.h
    qgama/selectconfiguration.cpp
    qgama/selectconfiguration.h
    qgama/showmessage.h
    qgama/shrinkbandwidth.cpp
    qgama/shrinkbandwidth.h
    qgama/tableviewstyle.cpp
    qgama/tableviewstyle.h
    qgama/xml2txt.cpp
    qgama/xml2txt.h
    qgama/qgama.qrc
    )

if(ANDROID)
  add_library(qgama SHARED
    ${QGSRC}
    $<TARGET_OBJECTS:libgama>
  )
else()
  add_executable(qgama
    ${QGSRC}
    $<TARGET_OBJECTS:libgama>
  )
endif()

target_link_libraries(qgama PRIVATE
  Qt6::Widgets Qt6::Sql Qt6::PrintSupport Qt6::SvgWidgets)

# Building with plugins by default. If you don't want to build plugins,
# use the following option to disable them

OPTION(BUILD_PLUGINS "Build with plugins." OFF)
if(BUILD_PLUGINS)
    add_subdirectory(qgama-plugins/adjustmentdemoplugin)
    add_subdirectory(qgama-plugins/dbdemoplugin)
    target_link_libraries(adjustmentdemoplugin)
    target_link_libraries(dbdemoplugin)
endif(BUILD_PLUGINS)

set (GAMA_INSTDIR bin)
add_subdirectory(gama)  # git clone https://git.savannah.gnu.org/git/gama.git

install(TARGETS qgama DESTINATION .)
install(FILES README.md DESTINATION .)

# Run windeployqt.exe to copy dll libraries
#
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   find_program(TOOL_WINDEPLOYQT NAMES windeployqt
       PATH "${CMAKE_PREFIX_PATH}")

   add_custom_command(TARGET qgama POST_BUILD
       COMMAND ${TOOL_WINDEPLOYQT} $<TARGET_FILE_DIR:qgama>)

   install(FILES $<TARGET_FILE_DIR:qgama>/ DESTINATION .)
endif()

include(CPack)
