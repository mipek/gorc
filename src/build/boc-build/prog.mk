NAME:=boc-build
BIN_DIR:=$(BUILD_BIN)

DEPENDENCIES:= \
	libs/system \
	libs/program \
	libs/text \
	libs/ast \
	libs/log \
	build/boc-build/engine \
	build/boc-build/entities \
	build/common \

SOURCES:= \
	create_graph_nodes.cpp \
	list_targets.cpp \
	main.cpp \
	print_status.cpp \
	project_file.cpp \
	project_graph.cpp \
	register_entities.cpp \
	run_build.cpp \

LIBRARIES:=$(SYSTEM_LIBRARIES)
