set(TRANSLATIONS fr)

foreach(_translation ${TRANSLATIONS})
	list(APPEND pdfviewer_TS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/pdfviewer_${_translation}.ts")
	if(BUILD_STANDALONE)
		list(APPEND pdfviewlib_TS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/translations/pdfviewlib_${_translation}.ts")
	endif(BUILD_STANDALONE)
endforeach(_translation ${TRANSLATIONS})

### Install qm files

if(BUILD_STANDALONE)
	file(GLOB pdfviewer_TS_FILES pdfviewer_*.ts ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/translations/pdfviewlib_*.ts)
else(BUILD_STANDALONE)
	file(GLOB pdfviewer_TS_FILES pdfviewer_*.ts)
endif(BUILD_STANDALONE)
qt4_add_translation(pdfviewer_QM_FILES ${pdfviewer_TS_FILES})
add_custom_target(translations ALL DEPENDS ${pdfviewer_QM_FILES})
install(FILES ${pdfviewer_QM_FILES} DESTINATION ${PDFVIEWER_TRANSLATIONS_INSTALL_DIR})

### Generate ts files (and rerun cmake so that the above knows when a new ts file is added)

add_custom_target(ts)
if(BUILD_STANDALONE)
	add_custom_command(TARGET ts
		COMMAND lupdate ${CMAKE_CURRENT_SOURCE_DIR}/../src -ts ${pdfviewer_TS_LIST}
		COMMAND lupdate ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/src -ts ${pdfviewlib_TS_LIST}
		COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/..
		VERBATIM
	)
else(BUILD_STANDALONE)
	add_custom_command(TARGET ts
		COMMAND lupdate ${CMAKE_CURRENT_SOURCE_DIR}/../src -ts ${pdfviewer_TS_LIST}
		COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/..
		VERBATIM
	)
endif(BUILD_STANDALONE)
