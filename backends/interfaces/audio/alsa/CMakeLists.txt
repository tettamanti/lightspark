INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("..")

pkg_check_modules(ALSA REQUIRED alsa)
# NB: do not add ALSA_INCLUDE_DIRS, it will cause conflicts with local
# files (e.g. timer.h)
SET(ALSAPLUGIN_SOURCES alsaplugin.cpp ../../IPlugin.cpp ../IAudioPlugin.cpp)

ADD_LIBRARY(alsaplugin SHARED ${ALSAPLUGIN_SOURCES})
#TARGET_LINK_LIBRARIES(alsaplugin spark)
TARGET_LINK_LIBRARIES(alsaplugin ${ALSA_LIBRARIES})
SET_TARGET_PROPERTIES(alsaplugin PROPERTIES OUTPUT_NAME lightsparkalsaplugin)

IF(UNIX)
	INSTALL(TARGETS alsaplugin LIBRARY DESTINATION ${PRIVATELIBDIR}/plugins)
ENDIF(UNIX)
