NAME:=boc-build-engine

DEPENDENCIES:= \
	libs/utility \
	libs/io \
	libs/log \

SOURCES:= \
	adjacency_list.cpp \
	dirty_list.cpp \
	entity.cpp \
	entity_allocator.cpp \
	entity_closure.cpp \
	entity_registry.cpp \
	entity_scheduler.cpp \
	entity_serializer.cpp \
