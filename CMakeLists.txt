cmake_minimum_required (VERSION 3.0)
project (EchoRam)

# Dependencies
find_package(OpenSSL REQUIRED)

# Setup a target
macro(setup_executable TARGET FILES)

	# Create executable
	add_executable(${TARGET} ${FILES})
	set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD 11)
	set_property(TARGET ${TARGET} PROPERTY CXX_STANDARD_REQUIRED ON)
	
	# C++11 trick for older CMake
	if (NOT WIN32)
		set(CMAKE_CXX_FLAGS "-std=c++11")
	endif()

	# Add includes
	target_include_directories(${TARGET} PUBLIC sources)
	target_include_directories(${TARGET} PUBLIC external/jsoncpp)
	target_include_directories(${TARGET} PUBLIC ${OPENSSL_INCLUDE_DIR})
	
	# Link with sockets, threads, OpenSSL...
	if(WIN32)
  		target_link_libraries(${TARGET} wsock32 ws2_32)
	else()
  		target_link_libraries(${TARGET} pthread)
	endif()
	target_link_libraries(${TARGET} ${OPENSSL_LIBRARIES})

	# Make it a console application for easier debugging
	if(WIN32)
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
		set_target_properties(${TARGET} PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
		set_target_properties(${TARGET} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
	endif(WIN32)

endmacro()

# System files
set (NETWORK_FILES
	sources/network/tcpsocket.h
	sources/network/tcpsocket.cpp
	sources/network/tcpserver.h
	sources/network/tcpserver.cpp
)

# Data files
set (DATA_FILES
	sources/data/handler.h
	sources/data/handler.cpp
	sources/data/database.h
	sources/data/database.cpp
)

# JSON files
set (JSON_FILES
	external/jsoncpp/json/json.h
	external/jsoncpp/json/json-forwards.h
	external/jsoncpp/jsoncpp.cpp
)

# Server files
set (SERVER_FILES
	sources/service.cpp
)

# Client files
set (CLIENT_FILES
	sources/testclient.cpp
)

# Organize solution
source_group("JSON" FILES ${JSON_FILES})
source_group("Data" FILES ${DATA_FILES})
source_group("Network" FILES ${NETWORK_FILES})
source_group("Service" FILES ${SERVER_FILES})
source_group("Client" FILES ${CLIENT_FILES})

# Server
set (SERVER_PROJECT_FILES
	${SERVER_FILES}
	${NETWORK_FILES}
	${DATA_FILES}
	${JSON_FILES}
)
setup_executable(Server "${SERVER_PROJECT_FILES}")

# Client files
set (CLIENT_PROJECT_FILES
	${CLIENT_FILES}
	${NETWORK_FILES}
	${DATA_FILES}
	${JSON_FILES}
)
setup_executable(TestClient "${CLIENT_PROJECT_FILES}")
