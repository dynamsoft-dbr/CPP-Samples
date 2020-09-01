cmake_minimum_required (VERSION 2.8)

project (AllBarcodeReaderDemo)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")
 
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(platform Linux)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set(platform Windows)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	set(platform FreeBSD)
ELSE ()
	MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")

ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")



if(MSVC)
set(DynamsoftBarcodeReader_PATH "Your DBR Installation Directory" CACHE PATH "DynamsoftBarcodeReader Installation Directory")
include_directories (${DynamsoftBarcodeReader_PATH}/Components/C_C++/Include)
link_directories(${DynamsoftBarcodeReader_PATH}/Components/C_C++/Lib)
IF(CMAKE_CL_64)
    link_libraries(DBRx64)
ELSE(CMAKE_CL_64)
    link_libraries(DBRx86)
ENDIF(CMAKE_CL_64)
else()
set(DynamsoftBarcodeReader_PATH "/home" CACHE PATH "DynamsoftBarcodeReader Installation Directory")
include_directories (${DynamsoftBarcodeReader_PATH}/include)
link_directories(${DynamsoftBarcodeReader_PATH}/lib)
link_libraries(libDynamsoftBarcodeReader.so)
endif()

add_subdirectory(src)
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/resource DESTINATION ${PROJECT_SOURCE_DIR}/bin/)  
