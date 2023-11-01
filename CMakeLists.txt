project (prgbfx)

# define library with all its sources
add_library(${PROJECT_NAME} INTERFACE)

target_compile_options(${PROJECT_NAME} INTERFACE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)

# include statements need to find the headers, but not the cpp files
target_include_directories(${PROJECT_NAME} 
	INTERFACE
	   include
        include/effects
        ../prgbld/include  # #ugly
)
