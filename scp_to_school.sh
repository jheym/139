#!/bin/bash

# This script will copy a directory to the school server.
# The script will be called with the following arguments:
# $1: The student's username for ecs login 
# $2: The folder path

# Usage: scp_to_school.sh username folderpath
if [ $# -ne 2 ]; then
    echo "Usage: $0 username folderpath"
    exit 1
fi

folderbasename=$(basename $2)

scp -r $2 $1@ecs-pa-coding1.ecs.csus.edu:~/