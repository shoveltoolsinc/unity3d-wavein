SET(SRCS
	iulib.h
    wave_in_device.cc
    wave_in_device.h)

IF(WIN32)
    SET(PLATFORM_SRCS
        win/win_wave_in_device.cc
        win/win_wave_in_device.h)
ELSEIF(APPLE)
    SET(PLATFORM_SRCS
        mac/mac_wave_in_device.cc
        mac/mac_wave_in_device.h)
ENDIF(WIN32)

ADD_LIBRARY(iulib ${SRCS} ${PLATFORM_SRCS})
