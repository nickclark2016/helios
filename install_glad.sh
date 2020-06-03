#!/bin/sh

pypip=pip
command -v pip3 >/dev/null 2>&1 && pypip=pip3
exec $pypip install --upgrade git+https://github.com/dav1dde/glad.git@glad2#egg=glad