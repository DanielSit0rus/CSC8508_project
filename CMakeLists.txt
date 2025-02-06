cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(CSC8503 CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
#set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
if(USE_VULKAN)
	find_package(Vulkan)
endif() 
set(ASSET_ROOT "${CMAKE_SOURCE_DIR}/Assets/" CACHE STRING "" FORCE)
add_compile_definitions(ASSETROOTLOCATION="${ASSET_ROOT}") 

set(USE_VULKAN CACHE BOOL FORCE)
if(USE_VULKAN)
    add_compile_definitions("USEVULKAN") 
	add_compile_definitions("VK_USE_PLATFORM_WIN32_KHR") 
endif() 

if(MSVC)
    add_compile_definitions("NOMINMAX")
    add_compile_definitions("WIN32_LEAN_AND_MEAN")  
endif()

################################################################################
# FMOD Setup
################################################################################
# Set root path of FMOD
set(FMOD_CORE_ROOT "${CMAKE_SOURCE_DIR}/SDK/FMOD Studio API Windows/api/core")
set(FMOD_FSBANK_ROOT "${CMAKE_SOURCE_DIR}/SDK/FMOD Studio API Windows/api/fsbank")
set(FMOD_STUDIO_ROOT "${CMAKE_SOURCE_DIR}/SDK/FMOD Studio API Windows/api/studio")

# Set include path of FMOD
include_directories(
    ${FMOD_CORE_ROOT}/inc
    ${FMOD_FSBANK_ROOT}/inc
    ${FMOD_STUDIO_ROOT}/inc
)

# Set library path of FMOD
link_directories(
    ${FMOD_CORE_ROOT}/lib/x64
    ${FMOD_FSBANK_ROOT}/lib/x64
    ${FMOD_STUDIO_ROOT}/lib/x64
)

# set library file of FMOD
set(FMOD_CORE_LIB fmod)            	# FMOD CORE lib
set(FMOD_FSBANK_LIB fsbank)  		# FMOD Studio lib
set(FMOD_STUDIO_LIB fmodstudio)  	# FMOD Studio lib

################################################################################
# ReactPhysics3D Setup
################################################################################
# set ReactPhysics3D root path
set(ReactPhysics3D_ROOT "${CMAKE_SOURCE_DIR}/SDK/ReactPhysics3D")
include_directories(${ReactPhysics3D_ROOT}/include)

# set ReactPhysics3D lib path
link_directories(${ReactPhysics3D_ROOT}/lib)

# set ReactPhysics3D lib name
set(ReactPhysics3D_Lib reactphysics3d)

# Visual Studio
if(MSVC)
    # Debug mode path
    set(DEBUG_LIB_PATH "${ReactPhysics3D_ROOT}/lib/Debug/${ReactPhysics3D_Lib}.lib")
    
    # Release mode path
    set(RELEASE_LIB_PATH "${ReactPhysics3D_ROOT}/lib/Release/${ReactPhysics3D_Lib}.lib")
endif()

################################################################################
# Sub-projects
################################################################################
add_subdirectory(NCLCoreClasses)
add_subdirectory(CSC8503CoreClasses)
add_subdirectory(OpenGLRendering)
add_subdirectory(CSC8503)
if(USE_VULKAN)
    add_subdirectory(VulkanRendering)
endif()
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CSC8503)