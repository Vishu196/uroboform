# Install script for directory: D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/xtl")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xtl" TYPE FILE FILES
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xany.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xbasic_fixed_string.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xbase64.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xclosure.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xcompare.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xcomplex.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xcomplex_sequence.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xspan.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xspan_impl.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xdynamic_bitset.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xfunctional.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xhalf_float.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xhalf_float_impl.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xhash.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xhierarchy_generator.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xiterator_base.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xjson.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xmasked_value_meta.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xmasked_value.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xmeta_utils.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xmultimethods.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xoptional_meta.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xoptional.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xoptional_sequence.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xplatform.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xproxy_wrapper.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xsequence.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xsystem.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xtl_config.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xtype_traits.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xvariant.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xvariant_impl.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/include/xtl/xvisitor.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/xtl" TYPE FILE FILES
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/xtlConfig.cmake"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/xtlConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/xtl/xtlTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/xtl/xtlTargets.cmake"
         "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/CMakeFiles/Export/2fc63ec57839ed115fc15a5438bb5aec/xtlTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/xtl/xtlTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/xtl/xtlTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/xtl" TYPE FILE FILES "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/CMakeFiles/Export/2fc63ec57839ed115fc15a5438bb5aec/xtlTargets.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" TYPE FILE FILES "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/xtl.pc")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtl-master/xtl-master/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
