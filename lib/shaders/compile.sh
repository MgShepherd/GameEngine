#!/bin/bash

function help() {
	cat <<-END
OVERVIEW: A script to help with compiling GLSL vertex and fragment shaders into their SPV representation

USAGE: ./compile.sh [GLSLC_LOCATION] [SHADER_PATH]

GLSLC_LOCATION:  
	This should be the path to the directory containing the GLSLC executable
	This comes as part of the Vulkan SDK and is usually the bin folder

SHADER_PATH:  	 
	The directory containing your shader file
	This scripts assumes both shaders are in the same directory and are named shader.vert and shader.frag

END
}

if [ -z "$1" ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
	help
	exit
fi

GLSLC_LOCATION=$1
SHADER_PATH=$2

$GLSLC_LOCATION/glslc $SHADER_PATH/shader.vert -o $SHADER_PATH/vert.spv
$GLSLC_LOCATION/glslc $SHADER_PATH/shader.frag -o $SHADER_PATH/frag.spv
