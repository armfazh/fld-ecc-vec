find_package(Doxygen QUIET)

if (Doxygen_FOUND)
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html
            COMMAND doxygen gen_doc.doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen_doc.doxyfile
    )

    add_custom_target(doc
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html
            SOURCES gen_doc.doxyfile
    )
    set_target_properties(doc PROPERTIES EXCLUDE_FROM_ALL true)
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/html/index.html PROPERTIES GENERATED TRUE)
else ()
    message("Warning: Doxygen must be installed for generating documentation.")
endif ()