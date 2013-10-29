set(TRANSLATIONS fr)

foreach(_translation ${TRANSLATIONS})
	list(APPEND pdfviewlib_TS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/pdfviewlib_${_translation}.ts")
endforeach(_translation ${TRANSLATIONS})

### Install qm files

file(GLOB pdfviewlib_TS_FILES pdfviewlib_*.ts)
qt4_add_translation(pdfviewlib_QM_FILES ${pdfviewlib_TS_FILES})
add_custom_target(translations ALL DEPENDS ${pdfviewlib_QM_FILES})
install(FILES ${pdfviewlib_QM_FILES} DESTINATION ${PDFVIEWLIB_TRANSLATIONS_INSTALL_DIR})

### Generate ts files (and rerun cmake so that the above knows when a new ts file is added)

add_custom_target(ts)
add_custom_command(TARGET ts
	COMMAND lupdate ${CMAKE_CURRENT_SOURCE_DIR}/../src -ts ${pdfviewlib_TS_LIST}
	COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/..
	VERBATIM
)
