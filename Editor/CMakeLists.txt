set(EDITOR_HEADERS_DIR ${CMAKE_SOURCE_DIR}/Editor/_Headers)
set(EDITOR_SOURCES_DIR ${CMAKE_SOURCE_DIR}/Editor/_Source)

set(EDITOR_SOURCES
${EDITOR_SOURCES_DIR}/ZEditor.cpp
${EDITOR_SOURCES_DIR}/ZEditorTool.cpp
${EDITOR_SOURCES_DIR}/ZEditorReadyEvent.cpp
${EDITOR_HEADERS_DIR}/ZEditor.hpp
${EDITOR_HEADERS_DIR}/ZEditorTool.hpp
${EDITOR_HEADERS_DIR}/ZEditorReadyEvent.hpp
${EDITOR_SOURCES_DIR}/_ThirdParty/DearIMGUI/imgui_widgets.cpp
${EDITOR_SOURCES_DIR}/_ThirdParty/DearIMGUI/imgui.cpp
${EDITOR_SOURCES_DIR}/_ThirdParty/DearIMGUI/imgui_impl_opengl3.cpp
${EDITOR_SOURCES_DIR}/_ThirdParty/DearIMGUI/imgui_impl_glfw.cpp
${EDITOR_SOURCES_DIR}/_ThirdParty/DearIMGUI/imgui_demo.cpp
${EDITOR_SOURCES_DIR}/_ThirdParty/DearIMGUI/imgui_draw.cpp
${EDITOR_HEADERS_DIR}/_ThirdParty/IconsFontAwesome5.h
${EDITOR_HEADERS_DIR}/_ThirdParty/DearIMGUI/imgui.h
${EDITOR_HEADERS_DIR}/_ThirdParty/DearIMGUI/imstb_textedit.h
${EDITOR_HEADERS_DIR}/_ThirdParty/DearIMGUI/imconfig.h
${EDITOR_HEADERS_DIR}/_ThirdParty/DearIMGUI/imstb_truetype.h
${EDITOR_HEADERS_DIR}/_ThirdParty/DearIMGUI/imgui_impl_opengl3.h
${EDITOR_HEADERS_DIR}/_ThirdParty/DearIMGUI/imgui_internal.h
${EDITOR_HEADERS_DIR}/_ThirdParty/DearIMGUI/imgui_impl_glfw.h
${EDITOR_HEADERS_DIR}/_ThirdParty/DearIMGUI/imstb_rectpack.h
${EDITOR_HEADERS_DIR}/Tools/ZConsoleTool.hpp
${EDITOR_HEADERS_DIR}/Tools/ZHierarchyTool.hpp
${EDITOR_HEADERS_DIR}/Tools/ZInspectorTool.hpp
${EDITOR_HEADERS_DIR}/Tools/ZPerformanceTool.hpp
${EDITOR_HEADERS_DIR}/Tools/ZPreviewTool.hpp
${EDITOR_HEADERS_DIR}/Tools/ZProjectSettingsTool.hpp
${EDITOR_HEADERS_DIR}/Tools/ZProjectTool.hpp
${EDITOR_HEADERS_DIR}/Tools/ZSceneTool.hpp
${EDITOR_HEADERS_DIR}/Tools/ZMenuBar.hpp
${EDITOR_HEADERS_DIR}/Tools/ZActionBar.hpp
${EDITOR_SOURCES_DIR}/Tools/ZConsoleTool.cpp
${EDITOR_SOURCES_DIR}/Tools/ZHierarchyTool.cpp
${EDITOR_SOURCES_DIR}/Tools/ZInspectorTool.cpp
${EDITOR_SOURCES_DIR}/Tools/ZPerformanceTool.cpp
${EDITOR_SOURCES_DIR}/Tools/ZPreviewTool.cpp
${EDITOR_SOURCES_DIR}/Tools/ZProjectSettingsTool.cpp
${EDITOR_SOURCES_DIR}/Tools/ZProjectTool.cpp
${EDITOR_SOURCES_DIR}/Tools/ZSceneTool.cpp
${EDITOR_SOURCES_DIR}/Tools/ZMenuBar.cpp
${EDITOR_SOURCES_DIR}/Tools/ZActionBar.cpp
${EDITOR_SOURCES_DIR}/main.cpp
)

SET(EDITOR_INCLUDES
${EDITOR_HEADERS_DIR}
${EDITOR_HEADERS_DIR}/Tools
${EDITOR_HEADERS_DIR}/_ThirdParty
${EDITOR_HEADERS_DIR}/_ThirdParty/DearIMGUI/
)

file(GLOB_RECURSE EDITOR_ASSETS "${CMAKE_SOURCE_DIR}/Editor/_Assets/*")
list(APPEND EDITOR_SOURCES ${EDITOR_ASSETS})