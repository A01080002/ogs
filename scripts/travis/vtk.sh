#!/bin/sh
set -e
if [ ! -d "$HOME/VTK-Install-7.1/include" ]; then
    cd $HOME
    wget http://ogsstorage.blob.core.windows.net/jenkins/ogs6-lib-sources/vtk-7.1.0-binary.tar.gz
    tar -xzf vtk-7.1.0-binary.tar.gz
else
  echo 'Using cached vtk directory.';
fi
