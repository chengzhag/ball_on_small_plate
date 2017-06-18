# Install script for directory: C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/utils

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

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/build/Debug/utils/CMakeFiles/CMakeRelink.dir/raspicam_test")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_test" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_test")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/SysGCC/Raspberry/bin/arm-linux-gnueabihf-strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_test")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/build/Debug/utils/CMakeFiles/CMakeRelink.dir/raspicam_still_test")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_still_test" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_still_test")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/SysGCC/Raspberry/bin/arm-linux-gnueabihf-strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_still_test")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/build/Debug/utils/CMakeFiles/CMakeRelink.dir/raspicam_cv_test")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_cv_test" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_cv_test")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/SysGCC/Raspberry/bin/arm-linux-gnueabihf-strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_cv_test")
    endif()
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "C:/Users/Sky/Desktop/OpenCV_raspberry_remote_camera/raspicam-0.1.6/build/Debug/utils/CMakeFiles/CMakeRelink.dir/raspicam_cv_still_test")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_cv_still_test" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_cv_still_test")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/SysGCC/Raspberry/bin/arm-linux-gnueabihf-strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/raspicam_cv_still_test")
    endif()
  endif()
endif()

