# Install script for directory: D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master")
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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xtensor" TYPE FILE FILES
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xaccessible.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xaccumulator.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xadapt.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xarray.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xassign.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xaxis_iterator.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xaxis_slice_iterator.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xblockwise_reducer.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xblockwise_reducer_functors.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xbroadcast.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xbuffer_adaptor.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xbuilder.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xchunked_array.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xchunked_assign.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xchunked_view.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xcomplex.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xcontainer.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xcsv.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xdynamic_view.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xeval.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xexception.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xexpression.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xexpression_holder.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xexpression_traits.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xfixed.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xfunction.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xfunctor_view.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xgenerator.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xhistogram.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xindex_view.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xinfo.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xio.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xiterable.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xiterator.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xjson.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xlayout.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xmanipulation.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xmasked_view.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xmath.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xmime.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xmultiindex_iterator.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xnoalias.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xnorm.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xnpy.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xoffset_view.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xoperation.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xoptional.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xoptional_assembly.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xoptional_assembly_base.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xoptional_assembly_storage.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xpad.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xrandom.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xreducer.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xrepeat.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xscalar.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xsemantic.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xset_operation.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xshape.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xslice.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xsort.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xstorage.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xstrided_view.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xstrided_view_base.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xstrides.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xtensor.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xtensor_config.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xtensor_forward.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xtensor_simd.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xutils.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xvectorize.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xview.hpp"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/include/xtensor/xview_utils.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/xtensor" TYPE FILE FILES
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/xtensorConfig.cmake"
    "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/xtensorConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/xtensor/xtensorTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/xtensor/xtensorTargets.cmake"
         "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/CMakeFiles/Export/0f2a327e949144b0b747c96acb1eb12f/xtensorTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/xtensor/xtensorTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/xtensor/xtensorTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/xtensor" TYPE FILE FILES "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/CMakeFiles/Export/0f2a327e949144b0b747c96acb1eb12f/xtensorTargets.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" TYPE FILE FILES "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/xtensor.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/xtensor.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/Vaishnavi/Master Thesis/Main Project/C++code/uRoboForm/xtensor-master/xtensor-master/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
