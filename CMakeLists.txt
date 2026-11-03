cmake_minimum_required(VERSION 3.16)
project(Kursovaya_KG CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

if(WIN32)
    set(MINGW_ROOT "C:/msys64/mingw64" CACHE PATH "Папка mingw64 из MSYS2")
    set(wxWidgets_INCLUDE_DIRS
            ${MINGW_ROOT}/include/wx-3.2
            ${MINGW_ROOT}/lib/wx/include/msw-unicode-3.2)
    set(wxWidgets_DEFINITIONS __WXMSW__ WXUSINGDLL _FILE_OFFSET_BITS=64)
    set(wxWidgets_LIBRARIES
            ${MINGW_ROOT}/lib/libwx_mswu_gl-3.2.dll.a
            ${MINGW_ROOT}/lib/libwx_mswu_core-3.2.dll.a
            ${MINGW_ROOT}/lib/libwx_baseu-3.2.dll.a)
else()
    find_package(wxWidgets REQUIRED COMPONENTS gl core base)
    include(${wxWidgets_USE_FILE})
endif()

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/MainFrame.cpp
        src/ScenePanel.cpp
        src/ShaderProgram.cpp
        src/Mesh.cpp
        src/Scene.cpp
        src/Camera.cpp
        src/SceneObject.cpp
        src/FogModel.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${wxWidgets_DEFINITIONS})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} OpenGL::GL GLEW::GLEW)
