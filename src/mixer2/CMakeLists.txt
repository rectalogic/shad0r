find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(FREI0R REQUIRED frei0r)

set(ANGLE_DIR ../../angle)

include_directories(
    ${ANGLE_DIR}/include
    ${ANGLE_DIR}/src
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${FREI0R_INCLUDE_DIRS}
)
link_directories(${GLFW_LIBRARY_DIRS})

set(ANGLE_SOURCES
    ${ANGLE_DIR}/include/GLSLANG/ShaderLang.h

    ${ANGLE_DIR}/src/common/blocklayout.h
    ${ANGLE_DIR}/src/common/utilities.h

    ${ANGLE_DIR}/src/common/blocklayout.cpp
    ${ANGLE_DIR}/src/common/utilities.cpp

    ${ANGLE_DIR}/src/compiler/preprocessor/DiagnosticsBase.h
    ${ANGLE_DIR}/src/compiler/preprocessor/DirectiveHandlerBase.h
    ${ANGLE_DIR}/src/compiler/preprocessor/DirectiveParser.h
    ${ANGLE_DIR}/src/compiler/preprocessor/ExpressionParser.h
    ${ANGLE_DIR}/src/compiler/preprocessor/Input.h
    ${ANGLE_DIR}/src/compiler/preprocessor/Lexer.h
    ${ANGLE_DIR}/src/compiler/preprocessor/Macro.h
    ${ANGLE_DIR}/src/compiler/preprocessor/MacroExpander.h
    ${ANGLE_DIR}/src/compiler/preprocessor/Preprocessor.h
    ${ANGLE_DIR}/src/compiler/preprocessor/SourceLocation.h
    ${ANGLE_DIR}/src/compiler/preprocessor/Token.h
    ${ANGLE_DIR}/src/compiler/preprocessor/Tokenizer.h
    ${ANGLE_DIR}/src/compiler/preprocessor/numeric_lex.h
    ${ANGLE_DIR}/src/compiler/preprocessor/pp_utils.h

    ${ANGLE_DIR}/src/compiler/preprocessor/DiagnosticsBase.cpp
    ${ANGLE_DIR}/src/compiler/preprocessor/DirectiveHandlerBase.cpp
    ${ANGLE_DIR}/src/compiler/preprocessor/DirectiveParser.cpp
    ${ANGLE_DIR}/src/compiler/preprocessor/ExpressionParser.cpp
    ${ANGLE_DIR}/src/compiler/preprocessor/ExpressionParser.y
    ${ANGLE_DIR}/src/compiler/preprocessor/Input.cpp
    ${ANGLE_DIR}/src/compiler/preprocessor/Lexer.cpp
    ${ANGLE_DIR}/src/compiler/preprocessor/Macro.cpp
    ${ANGLE_DIR}/src/compiler/preprocessor/MacroExpander.cpp
    ${ANGLE_DIR}/src/compiler/preprocessor/Preprocessor.cpp
    ${ANGLE_DIR}/src/compiler/preprocessor/Token.cpp
    ${ANGLE_DIR}/src/compiler/preprocessor/Tokenizer.cpp

    ${ANGLE_DIR}/src/compiler/translator/BaseTypes.h
    ${ANGLE_DIR}/src/compiler/translator/BuiltInFunctionEmulator.h
    ${ANGLE_DIR}/src/compiler/translator/Common.h
    ${ANGLE_DIR}/src/compiler/translator/ConstantUnion.h
    ${ANGLE_DIR}/src/compiler/translator/DetectCallDepth.h
    ${ANGLE_DIR}/src/compiler/translator/DetectDiscontinuity.h
    ${ANGLE_DIR}/src/compiler/translator/Diagnostics.h
    ${ANGLE_DIR}/src/compiler/translator/DirectiveHandler.h
    ${ANGLE_DIR}/src/compiler/translator/ExtensionBehavior.h
    ${ANGLE_DIR}/src/compiler/translator/FlagStd140Structs.h
    ${ANGLE_DIR}/src/compiler/translator/ForLoopUnroll.h
    ${ANGLE_DIR}/src/compiler/translator/HashNames.h
    ${ANGLE_DIR}/src/compiler/translator/InfoSink.h
    ${ANGLE_DIR}/src/compiler/translator/Initialize.h
    ${ANGLE_DIR}/src/compiler/translator/InitializeDll.h
    ${ANGLE_DIR}/src/compiler/translator/InitializeGlobals.h
    ${ANGLE_DIR}/src/compiler/translator/InitializeParseContext.h
    ${ANGLE_DIR}/src/compiler/translator/InitializeVariables.h
    ${ANGLE_DIR}/src/compiler/translator/LoopInfo.h
    ${ANGLE_DIR}/src/compiler/translator/MMap.h
    ${ANGLE_DIR}/src/compiler/translator/NodeSearch.h
    ${ANGLE_DIR}/src/compiler/translator/OutputESSL.h
    ${ANGLE_DIR}/src/compiler/translator/OutputGLSL.h
    ${ANGLE_DIR}/src/compiler/translator/OutputGLSLBase.h
    ${ANGLE_DIR}/src/compiler/translator/OutputHLSL.h
    ${ANGLE_DIR}/src/compiler/translator/ParseContext.h
    ${ANGLE_DIR}/src/compiler/translator/PoolAlloc.h
    ${ANGLE_DIR}/src/compiler/translator/Pragma.h
    ${ANGLE_DIR}/src/compiler/translator/QualifierAlive.h
    ${ANGLE_DIR}/src/compiler/translator/RemoveTree.h
    ${ANGLE_DIR}/src/compiler/translator/RenameFunction.h
    ${ANGLE_DIR}/src/compiler/translator/RewriteElseBlocks.h
    ${ANGLE_DIR}/src/compiler/translator/SearchSymbol.h
    ${ANGLE_DIR}/src/compiler/translator/ShHandle.h
    ${ANGLE_DIR}/src/compiler/translator/SymbolTable.h
    ${ANGLE_DIR}/src/compiler/translator/TranslatorESSL.h
    ${ANGLE_DIR}/src/compiler/translator/TranslatorGLSL.h
    ${ANGLE_DIR}/src/compiler/translator/TranslatorHLSL.h
    ${ANGLE_DIR}/src/compiler/translator/Types.h
    ${ANGLE_DIR}/src/compiler/translator/UnfoldShortCircuit.h
    ${ANGLE_DIR}/src/compiler/translator/UnfoldShortCircuitAST.h
    ${ANGLE_DIR}/src/compiler/translator/ValidateLimitations.h
    ${ANGLE_DIR}/src/compiler/translator/ValidateOutputs.h
    ${ANGLE_DIR}/src/compiler/translator/VariableInfo.h
    ${ANGLE_DIR}/src/compiler/translator/VariablePacker.h
    ${ANGLE_DIR}/src/compiler/translator/VersionGLSL.h
    ${ANGLE_DIR}/src/compiler/translator/compilerdebug.h
    ${ANGLE_DIR}/src/compiler/translator/glslang.h
    ${ANGLE_DIR}/src/compiler/translator/glslang_tab.h
    ${ANGLE_DIR}/src/compiler/translator/intermediate.h
    ${ANGLE_DIR}/src/compiler/translator/length_limits.h
    ${ANGLE_DIR}/src/compiler/translator/localintermediate.h
    ${ANGLE_DIR}/src/compiler/translator/osinclude.h
    ${ANGLE_DIR}/src/compiler/translator/util.h

    ${ANGLE_DIR}/src/compiler/translator/BuiltInFunctionEmulator.cpp
    ${ANGLE_DIR}/src/compiler/translator/CodeGen.cpp
    ${ANGLE_DIR}/src/compiler/translator/Compiler.cpp
    ${ANGLE_DIR}/src/compiler/translator/DetectCallDepth.cpp
    ${ANGLE_DIR}/src/compiler/translator/DetectDiscontinuity.cpp
    ${ANGLE_DIR}/src/compiler/translator/Diagnostics.cpp
    ${ANGLE_DIR}/src/compiler/translator/DirectiveHandler.cpp
    ${ANGLE_DIR}/src/compiler/translator/FlagStd140Structs.cpp
    ${ANGLE_DIR}/src/compiler/translator/ForLoopUnroll.cpp
    ${ANGLE_DIR}/src/compiler/translator/InfoSink.cpp
    ${ANGLE_DIR}/src/compiler/translator/Initialize.cpp
    ${ANGLE_DIR}/src/compiler/translator/InitializeDll.cpp
    ${ANGLE_DIR}/src/compiler/translator/InitializeParseContext.cpp
    ${ANGLE_DIR}/src/compiler/translator/InitializeVariables.cpp
    ${ANGLE_DIR}/src/compiler/translator/IntermTraverse.cpp
    ${ANGLE_DIR}/src/compiler/translator/Intermediate.cpp
    ${ANGLE_DIR}/src/compiler/translator/LoopInfo.cpp
    ${ANGLE_DIR}/src/compiler/translator/OutputESSL.cpp
    ${ANGLE_DIR}/src/compiler/translator/OutputGLSL.cpp
    ${ANGLE_DIR}/src/compiler/translator/OutputGLSLBase.cpp
    ${ANGLE_DIR}/src/compiler/translator/OutputHLSL.cpp
    ${ANGLE_DIR}/src/compiler/translator/ParseContext.cpp
    ${ANGLE_DIR}/src/compiler/translator/PoolAlloc.cpp
    ${ANGLE_DIR}/src/compiler/translator/QualifierAlive.cpp
    ${ANGLE_DIR}/src/compiler/translator/RemoveTree.cpp
    ${ANGLE_DIR}/src/compiler/translator/RewriteElseBlocks.cpp
    ${ANGLE_DIR}/src/compiler/translator/SearchSymbol.cpp
    ${ANGLE_DIR}/src/compiler/translator/ShaderLang.cpp
    ${ANGLE_DIR}/src/compiler/translator/SymbolTable.cpp
    ${ANGLE_DIR}/src/compiler/translator/TranslatorESSL.cpp
    ${ANGLE_DIR}/src/compiler/translator/TranslatorGLSL.cpp
    ${ANGLE_DIR}/src/compiler/translator/TranslatorHLSL.cpp
    ${ANGLE_DIR}/src/compiler/translator/Types.cpp
    ${ANGLE_DIR}/src/compiler/translator/UnfoldShortCircuit.cpp
    ${ANGLE_DIR}/src/compiler/translator/UnfoldShortCircuitAST.cpp
    ${ANGLE_DIR}/src/compiler/translator/ValidateLimitations.cpp
    ${ANGLE_DIR}/src/compiler/translator/ValidateOutputs.cpp
    ${ANGLE_DIR}/src/compiler/translator/VariableInfo.cpp
    ${ANGLE_DIR}/src/compiler/translator/VariablePacker.cpp
    ${ANGLE_DIR}/src/compiler/translator/VersionGLSL.cpp
    ${ANGLE_DIR}/src/compiler/translator/compilerdebug.cpp
    ${ANGLE_DIR}/src/compiler/translator/glslang_lex.cpp
    ${ANGLE_DIR}/src/compiler/translator/glslang_tab.cpp
    ${ANGLE_DIR}/src/compiler/translator/intermOut.cpp
    ${ANGLE_DIR}/src/compiler/translator/ossource_posix.cpp
    # ${ANGLE_DIR}/src/compiler/translator/ossource_win.cpp
    ${ANGLE_DIR}/src/compiler/translator/parseConst.cpp
    ${ANGLE_DIR}/src/compiler/translator/util.cpp

    ${ANGLE_DIR}/src/compiler/translator/depgraph/DependencyGraph.h
    ${ANGLE_DIR}/src/compiler/translator/depgraph/DependencyGraphBuilder.h
    ${ANGLE_DIR}/src/compiler/translator/depgraph/DependencyGraphOutput.h

    ${ANGLE_DIR}/src/compiler/translator/depgraph/DependencyGraph.cpp
    ${ANGLE_DIR}/src/compiler/translator/depgraph/DependencyGraphBuilder.cpp
    ${ANGLE_DIR}/src/compiler/translator/depgraph/DependencyGraphOutput.cpp
    ${ANGLE_DIR}/src/compiler/translator/depgraph/DependencyGraphTraverse.cpp

    ${ANGLE_DIR}/src/compiler/translator/timing/RestrictFragmentShaderTiming.h
    ${ANGLE_DIR}/src/compiler/translator/timing/RestrictVertexShaderTiming.h

    ${ANGLE_DIR}/src/compiler/translator/timing/RestrictFragmentShaderTiming.cpp
    ${ANGLE_DIR}/src/compiler/translator/timing/RestrictVertexShaderTiming.cpp

    ${ANGLE_DIR}/src/third_party/compiler/ArrayBoundsClamper.h
    ${ANGLE_DIR}/src/third_party/compiler/ArrayBoundsClamper.cpp
)

add_library(shad0r MODULE
    shad0r.c
    ${ANGLE_SOURCES}
)

target_link_libraries(shad0r
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
#target_link_libraries(shad0r LINK_INTERFACE_LIBRARIES "")

set_target_properties(shad0r PROPERTIES PREFIX "")

install(TARGETS shad0r
  LIBRARY DESTINATION lib COMPONENT libraries
)
