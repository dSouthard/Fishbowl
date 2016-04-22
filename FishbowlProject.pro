SOURCES = sources/project.cpp sources/projectViewer.cpp sources/projectOpengl.cpp sources/Object.cpp sources/Fish.cpp sources/Cube.cpp
HEADERS = includes/Fish.h includes/Object.h includes/projectOpengl.h includes/projectViewer.h includes/Cube.h includes/Type.h
QT += opengl
RESOURCES = project.qrc
DESTDIR = bin #Target file directory
OBJECTS_DIR = generatedFiles
MOC_DIR = generatedFiles
