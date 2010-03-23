#
# $Id$
#

i3_project(tableio
	DOCS_DIR resources/docs
	PYTHON_DIR python
)

i3_add_library(tableio
  private/tableio/*.cxx private/tableio/converter/*.cxx
  USE_TOOLS log4cplus boost
  USE_PROJECTS icetray dataclasses
)

add_subdirectory(private/pybindings)

i3_test_executable(test 
    private/test/*.cxx
    USE_PROJECTS dataclasses icetray phys-services tableio)

i3_test_scripts(resources/scripts/test_pybindings.py)