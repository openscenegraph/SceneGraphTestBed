PROJECT(stressVAOpolicy)
 
set(DYNAMIC_OPENSCENEGRAPH True)
if(DYNAMIC_OPENSCENEGRAPH)
    add_definitions(-DOSG_DYNAMIC_LIBRARY)
else()
    add_definitions(-DOSG_LIBRARY_STATIC)
endif()

find_package(OpenSceneGraph 3.6.0 REQUIRED
    osg
    osgDB
    osgGA
    osgUtil
    osgViewer
    osgText
)

INCLUDE_directories(${OSG_INCLUDE_DIR})
SET(PROJECT_SOURCES
    stressVAOpolicy.cpp
)

ADD_EXECUTABLE(stressVAOpolicy ${PROJECT_SOURCES} )

TARGET_LINK_LIBRARIES(stressVAOpolicy  ${OPENTHREADS_LIBRARIES}  ${OPENSCENEGRAPH_LIBRARIES}  )

SET_TARGET_PROPERTIES(stressVAOpolicy PROPERTIES PROJECT_LABEL "BenchTest stressVAOpolicy" DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

