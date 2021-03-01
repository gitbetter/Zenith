add_definitions(-DEDITOR_ROOT="${ENGINE_DIRECTORY}/_Editor")

set(EDITOR_HEADERS_DIR ${ENGINE_DIRECTORY}/_Editor/_Headers)
set(EDITOR_SOURCES_DIR ${ENGINE_DIRECTORY}/_Editor/_Source)

set(EDITOR_SOURCES
${EDITOR_SOURCES_DIR}/ZEditor.cpp
${EDITOR_SOURCES_DIR}/ZEditorScene.cpp
${EDITOR_SOURCES_DIR}/ZEditorEntity.cpp
${EDITOR_SOURCES_DIR}/ZEditorReadyEvent.cpp
${EDITOR_HEADERS_DIR}/ZEditor.hpp
${EDITOR_HEADERS_DIR}/ZEditorScene.hpp
${EDITOR_HEADERS_DIR}/ZEditorEntity.hpp
${EDITOR_HEADERS_DIR}/ZEditorReadyEvent.hpp
${EDITOR_HEADERS_DIR}/Tools/ZEditorTool.hpp
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
${EDITOR_HEADERS_DIR}/Tools/ZFrameStatsDisplay.hpp
${EDITOR_HEADERS_DIR}/Controls/ZEditorControl.hpp
${EDITOR_HEADERS_DIR}/Controls/ZTextField.hpp
${EDITOR_HEADERS_DIR}/Controls/ZIntField.hpp
${EDITOR_HEADERS_DIR}/Controls/ZFloatField.hpp
${EDITOR_HEADERS_DIR}/Controls/ZVec3Field.hpp
${EDITOR_HEADERS_DIR}/Controls/ZVec4Field.hpp
${EDITOR_HEADERS_DIR}/Controls/ZRectField.hpp
${EDITOR_HEADERS_DIR}/Controls/ZBoolField.hpp
${EDITOR_HEADERS_DIR}/Gizmos/ZTransformGizmo.hpp
${EDITOR_HEADERS_DIR}/Gizmos/ZEditorGizmo.hpp
${EDITOR_SOURCES_DIR}/Tools/ZEditorTool.cpp
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
${EDITOR_SOURCES_DIR}/Tools/ZFrameStatsDisplay.cpp
${EDITOR_SOURCES_DIR}/Controls/ZEditorControl.cpp
${EDITOR_SOURCES_DIR}/Controls/ZTextField.cpp
${EDITOR_SOURCES_DIR}/Controls/ZIntField.cpp
${EDITOR_SOURCES_DIR}/Controls/ZFloatField.cpp
${EDITOR_SOURCES_DIR}/Controls/ZVec3Field.cpp
${EDITOR_SOURCES_DIR}/Controls/ZVec4Field.cpp
${EDITOR_SOURCES_DIR}/Controls/ZRectField.cpp
${EDITOR_SOURCES_DIR}/Controls/ZBoolField.cpp
${EDITOR_SOURCES_DIR}/Gizmos/ZTransformGizmo.cpp
${EDITOR_SOURCES_DIR}/Gizmos/ZEditorGizmo.cpp
${EDITOR_SOURCES_DIR}/main.cpp
)

SET(EDITOR_INCLUDES
${EDITOR_HEADERS_DIR}
${EDITOR_HEADERS_DIR}/Tools
${EDITOR_HEADERS_DIR}/Controls
${EDITOR_HEADERS_DIR}/Gizmos
)

file(GLOB_RECURSE EDITOR_ASSETS "${ENGINE_DIRECTORY}/_Editor/_Assets/*")
list(APPEND EDITOR_SOURCES ${EDITOR_ASSETS})