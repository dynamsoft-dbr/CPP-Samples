file(GLOB mains RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.c" "*.cpp")
#message(STATUS ${DBR_PATH})
include(./CMakeDefines.txt)

message(STATUS ${aa})
foreach(mainfile IN LISTS mains)
    # Get file name without directory
    get_filename_component(mainname ${mainfile} NAME_WE)
	get_filename_component(lastE ${mainfile} EXT)
	
	if(lastE STREQUAL ".cpp")
	Set(BuildName "BUILD_${mainname}_CPP")
	elseif(lastE STREQUAL ".c")
	Set(BuildName "BUILD_${mainname}_C")
	endif()	
	if(mainname STREQUAL DecodeVideoFrame)
	option(${BuildName} "complie_${mainname}_Sample" OFF)
	elseif(mainname STREQUAL MultiThreadBarcodeReader AND CMAKE_SYSTEM_NAME MATCHES "Linux")
	option(${BuildName} "complie_${mainname}_Sample" OFF)
	else()
	option(${BuildName} "complie_${mainname}_Sample" ON)
	endif()	
	if(${BuildName})
	# message(STATUS ${BuildName})
	# if(mainname STREQUAL DecodeDPM OR mainname STREQUAL DecodeVideoFrame)
	# set(filetype "Intermediate")
	# elseif(mainname STREQUAL SimpleBarcodeReader OR mainname STREQUAL MultiThreadBarcodeReader)
	# set(filetype "Primary")
	# endif()
	
	
	if(lastE STREQUAL ".cpp")
	set(LastName "(sample_cpp)${mainname}")
	set(the_target "${mainname}_CPP")
	elseif(lastE STREQUAL ".c")
	set(LastName "(sample_c)${mainname}")
	set(the_target "${mainname}_C")
	endif()
    add_executable(${the_target} ${mainfile})
	set_target_properties(${the_target} PROPERTIES PROJECT_LABEL ${LastName})
	set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	#message(STATUS ${LastName})

	if(mainname STREQUAL "DecodeVideoFrame")
	target_include_directories(${the_target} PRIVATE ${CMAKE_SOURCE_DIR}/thirdLib/Include)
	target_link_directories(${the_target} PRIVATE ${CMAKE_SOURCE_DIR}/thirdLib/lib/$(Configuration))
	
	
	# target_link_libraries(${the_target} debug opencv_core345d.lib)
	# target_link_libraries(${the_target} debug opencv_highgui345d.lib)
	# target_link_libraries(${the_target} debug opencv_videoio345d.lib)
	
	# target_link_libraries(${the_target} optimized opencv_core345.lib)
	# target_link_libraries(${the_target} optimized opencv_highgui345.lib)
	# target_link_libraries(${the_target} optimized opencv_videoio345.lib)
	
	endif()


	if(MSVC)
		IF(CMAKE_CL_64)
		add_custom_command(TARGET ${the_target}
		POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${DynamsoftBarcodeReader_PATH}/Components/C_C++/Redist/x64 ${PROJECT_SOURCE_DIR}/bin/$(Configuration))
		ELSE(CMAKE_CL_64)
		add_custom_command(TARGET ${the_target}
		POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${DynamsoftBarcodeReader_PATH}/Components/C_C++/Redist/x86 ${PROJECT_SOURCE_DIR}/bin/$(Configuration))
		ENDIF(CMAKE_CL_64)	
	endif()
	endif()
endforeach()