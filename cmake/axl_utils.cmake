#..............................................................................

# print all variables (for debugging)

macro (
axl_print_all_vars)

	get_cmake_property (_VARIABLE_LIST VARIABLES)
	foreach (_VARIABLE ${_VARIABLE_LIST})
		message(STATUS "${_VARIABLE} = ${${_VARIABLE}}")
endmacro ()

#..............................................................................

# detect host CPU (currently we only care about x86 vs amd64)

macro (
axl_detect_cpu
	_CPU
	)

	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set (${_CPU} "amd64")
	else ()
		set (${_CPU} "x86")
	endif ()
endmacro ()

#..............................................................................

# push and pop variables on stack

set (__AXL_STACK)

macro (
axl_push
	_VALUE
	)

	list (INSERT __AXL_STACK 0 ${_VALUE})
endmacro ()

macro (
axl_pop
	_RESULT
	)

	list (GET __AXL_STACK 0 ${_RESULT})
	list (REMOVE_AT __AXL_STACK 0)
endmacro ()

macro (
axl_push_and_set
	_VARIABLE
	_VALUE
	)

	axl_push (${${_VARIABLE}})
	set (${_VARIABLE} ${_VALUE})
endmacro ()

#..............................................................................

# cache setting with list of options

macro (
axl_create_setting
	_SETTING
	_DEFAULT_VALUE
	_TYPE
	_DESCRIPTION
	# ...
	)

	if (NOT ${_SETTING})
		set (_FORCE FORCE)
	else ()
		set (_FORCE)
	endif ()

	set (
		${_SETTING} ${_DEFAULT_VALUE}
		CACHE ${_TYPE} ${_DESCRIPTION} ${_FORCE}
		)

	if (${ARGC} GREATER 4)
		set (_OPTION_LIST ${ARGN})

		set_property	(
			CACHE
			${_SETTING}
			PROPERTY STRINGS
			${_OPTION_LIST}
			)
	endif ()
endmacro ()

macro (
axl_create_flag_list_regex
	_REGEX
	# ...
	)

	set (_FLAG_LIST ${ARGN})

	set (${_REGEX} "")

	foreach (_FLAG ${_FLAG_LIST})
		set (_FLAG_REGEX "(^| +)${_FLAG}($| +)")

		if (${_REGEX} STREQUAL "")
			set (${_REGEX} ${_FLAG_REGEX})
		else ()
			set (${_REGEX} "${${_REGEX}}|${_FLAG_REGEX}")
		endif ()
	endforeach ()
endmacro ()

macro (
axl_create_c_cxx_flag_setting
	_SETTING
	_DESCRIPTION
	_DEFAULT_VALUE
	# ...
	)

	set (_FLAG_LIST ${ARGN})
	axl_create_flag_list_regex (_REGEX ${_FLAG_LIST})

	set (_CONFIGURATION_SUFFIX)

	# check if this is per-configuration setting

	string (TOUPPER "${_SETTING}" _SETTING_UC)

	foreach (_CONFIGURATION ${CMAKE_CONFIGURATION_TYPES})
		string (TOUPPER "${_CONFIGURATION}" _CONFIGURATION_UC)
		string (REGEX MATCH "${_CONFIGURATION_UC}$" _MATCH "${_SETTING}")

		if (NOT ${_MATCH} STREQUAL "")
			set (_CONFIGURATION_SUFFIX "_${_CONFIGURATION_UC}")
			break ()
		endif ()
	endforeach ()

	# get current value of the setting in C/C++ flags

	string (
		REGEX MATCH
		"${_REGEX}"
		_CURRENT_VALUE_C
		"${CMAKE_C_FLAGS${_CONFIGURATION_SUFFIX}}"
		)

	string (
		REGEX MATCH
		"${_REGEX}"
		_CURRENT_VALUE_CXX
		"${CMAKE_CXX_FLAGS${_CONFIGURATION_SUFFIX}}"
		)

	string (
		STRIP
		"${_CURRENT_VALUE_C}"
		_CURRENT_VALUE_C
		)

	string (
		STRIP
		"${_CURRENT_VALUE_CXX}"
		_CURRENT_VALUE_CXX
		)

	# create cached setting

	if (NOT ${_SETTING})
		set (_FORCE FORCE)
	else ()
		set (_FORCE)
	endif ()

	if ("${_DEFAULT_VALUE}" STREQUAL "")
		set (
			${_SETTING} ${_CURRENT_VALUE_CXX}
			CACHE STRING ${_DESCRIPTION} ${_FORCE}
			)
	else ()
		set (
			${_SETTING} ${_DEFAULT_VALUE}
			CACHE STRING ${_DESCRIPTION} ${_FORCE}
			)
	endif ()

	set_property (
		CACHE
		${_SETTING}
		PROPERTY STRINGS
		" " ${_FLAG_LIST}
		)

	# if this setting is per-configuration then clear configuration-common flags

	if (_CMAKE_CONFIGURATION_SUFFIX)
		string (
			REGEX REPLACE
			"${_REGEX}"
			" "
			CMAKE_C_FLAGS
			"${CMAKE_C_FLAGS}"
			)

		string (
			REGEX REPLACE
			"${_REGEX}"
			" "
			CMAKE_CXX_FLAGS
			"${CMAKE_CXX_FLAGS}"
			)
	endif ()

	# apply setting to C/C++ flags

	if ("${_CURRENT_VALUE_C}" STREQUAL "")
		set (CMAKE_C_FLAGS${_CONFIGURATION_SUFFIX} "${CMAKE_C_FLAGS${_CONFIGURATION_SUFFIX}} ${${_SETTING}}")
	else  ()
		string (
			REGEX REPLACE
			"${_REGEX}"
			" ${${_SETTING}} "
			CMAKE_C_FLAGS${_CONFIGURATION_SUFFIX}
			"${CMAKE_C_FLAGS${_CONFIGURATION_SUFFIX}}"
			)
	endif ()

	if ("${_CURRENT_VALUE_CXX}" STREQUAL "")
		set (CMAKE_CXX_FLAGS${_CONFIGURATION_SUFFIX} "${CMAKE_CXX_FLAGS${_CONFIGURATION_SUFFIX}} ${${_SETTING}}")
	else  ()
		string (
			REGEX REPLACE
			"${_REGEX}"
			" ${${_SETTING}} "
			CMAKE_CXX_FLAGS${_CONFIGURATION_SUFFIX}
			"${CMAKE_CXX_FLAGS${_CONFIGURATION_SUFFIX}}"
			)
	endif ()

	string (
		STRIP
		"${CMAKE_C_FLAGS${_CONFIGURATION_SUFFIX}}"
		CMAKE_C_FLAGS${_CONFIGURATION_SUFFIX}
		)

	string (
		STRIP
		"${CMAKE_CXX_FLAGS${_CONFIGURATION_SUFFIX}}"
		CMAKE_CXX_FLAGS${_CONFIGURATION_SUFFIX}
		)
endmacro ()

#..............................................................................

# precompiled headers

macro (
axl_set_pch_msvc
	_TARGET
	_PCH_H
	_PCH_CPP
	)

	get_filename_component (_PCH_NAME ${_PCH_H} NAME_WE)
	set (_PCH_BIN "${CMAKE_CURRENT_BINARY_DIR}/${_TARGET}.dir/$(Configuration)/${_PCH_NAME}.pch")

	set_target_properties (
		${_TARGET}
		PROPERTIES
		COMPILE_FLAGS "/Yu\"${_PCH_H}\" /Fp\"${_PCH_BIN}\""
		)

	set_source_files_properties (
		${_PCH_CPP}
		PROPERTIES
		COMPILE_FLAGS "/Yc\"${_PCH_H}\" /Fp\"${_PCH_BIN}\""
		OBJECT_OUTPUTS "${_PCH_BIN}"
		)
endmacro ()

macro (
axl_set_pch_gcc
	_TARGET
	_PCH_H
	)

	set (_EXTRA_FLAG_LIST ${ARGN})

	set (_PCH_GCC "${CMAKE_CURRENT_BINARY_DIR}/${_PCH_H}.gcc")
	set (_PCH_BIN "${CMAKE_CURRENT_BINARY_DIR}/${_PCH_H}.gcc.gch")

	# get directory flags, defines, includes
	
	get_property (
		_DIR_CXX_FLAGS
		DIRECTORY
		PROPERTY COMPILE_FLAGS
		)

	get_property (
		_DIR_CXX_DEFINES
		DIRECTORY
		PROPERTY COMPILE_DEFINITIONS
		)

	get_property (
		_DIR_CXX_INCLUDES
		DIRECTORY
		PROPERTY INCLUDE_DIRECTORIES
		)

	# get target flags, defines, includes
		
	get_target_property (
		_TARGET_CXX_DEFINES
		${_TARGET}
		COMPILE_DEFINITIONS
		)

	get_target_property (
		_TARGET_CXX_FLAGS
		${_TARGET}
		COMPILE_FLAGS
		)

	get_target_property (
		_TARGET_CXX_INCLUDES
		${_TARGET}
		INCLUDE_DIRECTORIES
		)

	# get global flags for current build type

	string (TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _CXX_FLAGS)
	set (_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${_CXX_FLAGS}}")

	if (_CXX_FLAGS)
		string (REPLACE " " ";" _CXX_FLAGS ${_CXX_FLAGS})
	endif ()

	# append directory flags, defines, includes
	
	if (_DIR_CXX_FLAGS)
		string (REPLACE " " ";" _DIR_CXX_FLAGS ${_DIR_CXX_FLAGS})
		list (APPEND _CXX_FLAGS ${_DIR_CXX_FLAGS})
	endif ()

	if (_DIR_CXX_DEFINES)
		foreach (_DEF ${_DIR_CXX_DEFINES})
			list (APPEND _CXX_FLAGS "-D${_DEF}")
		endforeach ()
	endif ()

	if (_DIR_CXX_INCLUDES)
		foreach (_DIR ${_DIR_CXX_INCLUDES})
			list (APPEND _CXX_FLAGS "-I${_DIR}")
		endforeach ()
	endif ()

	# append target flags, defines, includes

	if (_TARGET_CXX_FLAGS)
		string (REPLACE " " ";" _TARGET_CXX_FLAGS ${_TARGET_CXX_FLAGS})
		list (APPEND _CXX_FLAGS ${_TARGET_CXX_FLAGS})
	endif ()

	if (_TARGET_CXX_DEFINES)
		foreach (_DEF ${_TARGET_CXX_DEFINES})
			list (APPEND _CXX_FLAGS "-D${_DEF}")
		endforeach ()
	endif ()

	if (_TARGET_CXX_INCLUDES)
		foreach (_DIR ${_TARGET_CXX_INCLUDES})
			list (APPEND _CXX_FLAGS "-I${_DIR}")
		endforeach ()
	endif ()

	# append extra flags passed in vararg
	
	if (_EXTRA_FLAG_LIST)
		list (APPEND _CXX_FLAGS "${_EXTRA_FLAG_LIST}")
	endif ()

	# create pch command

	file (
		WRITE ${_PCH_GCC}
		"\#include \"${CMAKE_CURRENT_SOURCE_DIR}/${_PCH_H}\"\n"
		)

	add_custom_command (
		OUTPUT ${_PCH_BIN}
		MAIN_DEPENDENCY ${_PCH_H}
		COMMAND ${CMAKE_CXX_COMPILER} ${_CXX_FLAGS} -x c++-header -o${_PCH_BIN} ${_PCH_GCC}
		)

	include_directories (${CMAKE_CURRENT_BINARY_DIR})

	set_target_properties (
		${_TARGET}
		PROPERTIES COMPILE_FLAGS
		"-include \"${_PCH_GCC}\""
		)
endmacro ()

macro (
axl_set_pch
	_TARGET
	_PCH_H
	_PCH_CPP
	)
	if (MSVC AND MSVC_USE_PCH)
		axl_set_pch_msvc (${_TARGET} ${_PCH_H} ${_PCH_CPP} ${ARGN})
	elseif (GCC AND GCC_USE_PCH)
		axl_set_pch_gcc (${_TARGET} ${_PCH_H} ${ARGN})
	endif ()
endmacro ()

#..............................................................................

# exclude files from build (but keep it in project tree)

macro (
axl_set_header_file_only
	_IS_HEADER_FILE_ONLY
	# ...
	)

	set_source_files_properties (
		${ARGN}
		PROPERTIES
		HEADER_FILE_ONLY ${_IS_HEADER_FILE_ONLY}
		)
endmacro ()

macro (
axl_exclude_from_build
	# ...
	)

	axl_set_header_file_only (
		TRUE
		${ARGN}
		)
endmacro ()

macro (
axl_undo_exclude_from_build
	# ...
	)

	axl_set_header_file_only (
		FALSE
		${ARGN}
		)
endmacro ()

#..............................................................................

# file helpers

macro (
axl_make_path
	_FILE_PATH
	_FILE_NAME
	_BASE_DIR
	)

	string (REGEX MATCH "/" _SLASH ${_FILE_NAME})

	if (_SLASH)
		set (${_FILE_PATH} ${_FILE_NAME})
	else ()
		set (${_FILE_PATH} "${_BASE_DIR}/${_FILE_NAME}")
	endif ()

endmacro ()

macro (
axl_ensure_file_exists
	_FILE_PATH
	_TEMPLATE_FILE_PATH
	)
	if (NOT EXISTS ${_FILE_PATH})
		file (READ ${_TEMPLATE_FILE_PATH} _BUFFER)
		file (WRITE ${_FILE_PATH} ${_BUFFER})
	endif ()
endmacro ()

macro (
axl_copy_file_if_different
	_TARGET
	_SRC_DIR
	_DST_DIR
	_FILE_NAME
	)
	
	add_custom_command (
		TARGET ${_TARGET} 
		POST_BUILD        
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${_SRC_DIR}/${_FILE_NAME}"
			"${_DST_DIR}/${_FILE_NAME}"
		)
endmacro ()

macro (
axl_configure_file_w_permissions
	_SRC_FILE
	_DST_FILE
	# ...
	)
	
	set (_PERMISSION_LIST ${ARGN})
	set (_TMP_FILE ${CMAKE_CURRENT_BINARY_DIR}/tmp/${_DST_FILE})

	configure_file (${_SRC_FILE} ${_TMP_FILE})

	file (
		COPY ${_TMP_FILE}
		DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
		FILE_PERMISSIONS ${_PERMISSION_LIST}
		)

	file (REMOVE ${_TMP_FILE})
endmacro ()

#..............................................................................

# target property helpers

macro (
axl_add_target_string_property 
	_TARGET
	_PROPERTY
	_VALUE
	)

	get_target_property (
		_OLD_VALUE
		${_TARGET}
		${_PROPERTY}
		)
		
	if (${_OLD_VALUE})
		set (_VALUE, "${_OLD_VALUE} ${_VALUE}")
	endif ()

	set_target_properties (
		${_TARGET}
		PROPERTIES
		${_PROPERTY} ${_VALUE}
		)	
endmacro ()

macro (
axl_add_target_list_property 
	_TARGET
	_PROPERTY
	#...
	)

	set (_VALUE_LIST ${ARGN})
	
	get_target_property (
		_OLD_VALUE_LIST
		${_TARGET}
		${_PROPERTY}
		)
	
	if (${_OLD_VALUE_LIST})
		set (_NEW_VALUE_LIST ${_OLD_VALUE_LIST})
	else ()
		set (_NEW_VALUE_LIST)
	endif ()

	foreach (_VALUE ${_VALUE_LIST})
		list (APPEND ${_NEW_VALUE_LIST} ${_VALUE})
	endforeach ()

	set_target_properties (
		${_TARGET}
		PROPERTIES
		${_PROPERTY} ${_VALUE_LIST}
		)	
endmacro ()

#..............................................................................

# imports -- CMake' find_package replacement with support for manual override 

macro (
axl_include_import_file
	_IMPORT
	_IS_OPTIONAL
	)

	set (${_IMPORT}_FOUND FALSE)

	foreach (_IMPORT_DIR ${AXL_IMPORT_DIR_LIST})
		unset (_IMPORT_FILE_PATH)

		include (
			${_IMPORT_DIR}/import_${_IMPORT}.cmake 
			OPTIONAL 
			RESULT_VARIABLE _IMPORT_FILE_PATH
			)
			
		if (_IMPORT_FILE_PATH)
			break ()
		endif ()
	endforeach ()
	
	string (TOUPPER ${_IMPORT} _IMPORT_UC)
	if (NOT ${_IMPORT_UC}_FOUND AND NOT _IS_OPTIONAL)
		message (FATAL_ERROR "${_IMPORT} is required but not found, check paths.cmake")
	endif ()
endmacro ()

macro (
axl_import
	#...
	)

	set (_IMPORT_LIST ${ARGN})
	set (_IS_OPTIONAL FALSE)
	
	foreach (_IMPORT ${_IMPORT_LIST})
		if ("${_IMPORT}" STREQUAL "REQUIRED")
			set (_IS_OPTIONAL FALSE)
		elseif ("${_IMPORT}" STREQUAL "OPTIONAL")
			set (_IS_OPTIONAL TRUE)
		else ()
			axl_include_import_file (
				${_IMPORT}
				_IS_OPTIONAL
				)	
		endif ()
	endforeach ()
endmacro ()

#..............................................................................