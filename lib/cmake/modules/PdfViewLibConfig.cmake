# use absolute or relative paths for the directories below;
# relative paths will be relative to ${CMAKE_INSTALL_PREFIX}
#set(FIND_LIBRARY_USE_LIB64_PATHS TRUE)
set(INCLUDES_INSTALL_DIR include)
set(LIB_INSTALL_DIR lib${LIB_SUFFIX})
set(DATA_INSTALL_DIR share)
set(PDFVIEWLIB_DATA_INSTALL_DIR ${DATA_INSTALL_DIR}/pdfviewlib)
set(PDFVIEWLIB_TRANSLATIONS_INSTALL_DIR ${PDFVIEWLIB_DATA_INSTALL_DIR}/locale)

set(PDFVIEWLIB_PRINT_PROGRAM lp)

set(USE_SYNCTEX TRUE) # hide this to disable synctex support
