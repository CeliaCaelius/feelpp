add_custom_target(pyfeelpp)

feelpp_add_pymodule(feelpp SRCS feelpp.cpp DESTINATION feelpp )
feelpp_add_pymodule(core SRCS core.cpp remotedata.cpp  DESTINATION feelpp )
feelpp_add_pymodule(alg SRCS alg.cpp DESTINATION feelpp )
feelpp_add_pymodule(mesh SRCS mesh.cpp DESTINATION feelpp )
feelpp_add_pymodule(discr SRCS discr.cpp DESTINATION feelpp )
feelpp_add_pymodule(exporter SRCS exporter.cpp hbf.cpp  DESTINATION feelpp )
feelpp_add_pymodule(ts SRCS ts.cpp DESTINATION feelpp )
feelpp_add_pymodule(vf SRCS vf.cpp DESTINATION feelpp )
feelpp_add_pymodule(models SRCS models.cpp DESTINATION feelpp )

foreach(MODULE IN ITEMS core feelpp alg mesh discr exporter ts vf models )
  add_dependencies(pyfeelpp _${MODULE})
endforeach()

set(PYFILES __init__.py )
install(FILES ${PYFILES} DESTINATION ${FEELPP_PYTHON_MODULE_PATH}/feelpp)