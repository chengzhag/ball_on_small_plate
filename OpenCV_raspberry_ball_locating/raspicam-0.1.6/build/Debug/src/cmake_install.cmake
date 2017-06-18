# Install script for directory: C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/raspicam")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "DEBUG")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "main" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES
    "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/build/Debug/src/CMakeFiles/CMakeRelink.dir/libraspicam.so.0.1.6"
    "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/build/Debug/src/CMakeFiles/CMakeRelink.dir/libraspicam.so.0.1"
    "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/build/Debug/src/CMakeFiles/CMakeRelink.dir/libraspicam.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libraspicam.so.0.1.6"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libraspicam.so.0.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libraspicam.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "C:/SysGCC/Raspberry/bin/arm-linux-gnueabihf-strip.exe" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "main" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/build/Debug/src/CMakeFiles/CMakeRelink.dir/libraspicam_cv.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libraspicam_cv.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libraspicam_cv.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/SysGCC/Raspberry/bin/arm-linux-gnueabihf-strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libraspicam_cv.so")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "main" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/raspicam" TYPE FILE FILES
    "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/src/raspicamtypes.h"
    "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/src/raspicam.h"
    "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/src/raspicam_cv.h"
    "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/src/raspicam_still_cv.h"
    )
endif()

