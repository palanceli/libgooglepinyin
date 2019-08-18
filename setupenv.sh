#!/bin/bash


# Helper for adding a directory to the stack and echoing the result
enterDir() {
  echo "Entering $1"
  pushd $1 > /dev/null
}

# Helper for popping a directory off the stack and echoing the result
leaveDir() {
  echo "Leaving `pwd`"
  popd > /dev/null
}


function clone_proj {
    local proj_url=$1
    local local_dir=$2
    local branch=$3
    git clone $proj_url $local_dir
    enterDir $local_dir
    git checkout $branch
    leaveDir
}

function build_install_module {
	local module_dir=$1
	local module_build_dir=$module_dir/_build
	mkdir $module_build_dir
	enterDir $module_dir/_build
	cmake .. && make && make install
	leaveDir
}

if [ ! -d "modules" ]; then
	mkdir modules
fi

if [ ! -d "modules/gflags" ]; then
	clone_proj "https://github.com/gflags/gflags.git" "modules/gflags" "v2.2.2"
fi
build_install_module "modules/gflags"

if [ ! -d "modules/gtest" ]; then
	clone_proj "https://github.com/google/glog.git" "modules/glog" "v0.4.0"
fi
build_install_module "modules/glog"

if [ ! -d "modules/googletest" ]; then
	clone_proj "https://github.com/google/googletest.git" "modules/googletest" "release-1.8.1"
fi
build_install_module "modules/googletest"



