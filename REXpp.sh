#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export REXppSrc=${DIR}/

export LD_LIBRARY_PATH=$REXppSrc/lib:$LD_LIBRARY_PATH
