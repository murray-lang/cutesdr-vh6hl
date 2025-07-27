cmake_minimum_required(VERSION 3.8)

project(cutesdr-vk6hl VERSION 0.1 LANGUAGES CXX)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --enable-threads")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --enable-threads")

#set(CMAKE_PREFIX_PATH "/home/murray/Qt/6.9.1/gcc_64/lib/cmake")
#set(CMAKE_PREFIX_PATH "/home/murray/Qt6.5/6.5.3/gcc_64/lib/cmake")
set(CMAKE_PREFIX_PATH "/home/murray/Qt/6.5.0/gcc_64/lib/cmake")


set(VOLK_MIN_VERSION "2.4.1")         ## first version with CPU features

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Multimedia Widgets Charts Multimedia)
find_library(HIDAPI_LIBRARY hidapi-libusb)

message(STATUS "")
message(STATUS "Configuring VOLK support...")
find_package(Volk)
if(Volk_FOUND)
  message(STATUS "  Found VOLK:")
  message(STATUS "  * Version: ${Volk_VERSION}")
  message(STATUS "  * Libraries: ${VOLK_LIBRARIES}")
  message(STATUS "  * Includes: ${VOLK_INCLUDE_DIRS}")
  if("${Volk_VERSION}" STREQUAL "")
    message(WARNING "Empty VOLK version string. Assuming compatibility. Good luck!")
  else()
    if("${Volk_VERSION}" VERSION_LESS ${VOLK_MIN_VERSION})
      message(FATAL_ERROR "VOLK version ${Volk_VERSION} < ${VOLK_MIN_VERSION}")
    endif()
  endif()
else()
  message(WARNING "VOLK not found.")
endif()

if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(USB)
    if(NOT LIBUSB_FOUND)
        message(FATAL_ERROR "libusb required to compile cutesdr-vk6hl")
    endif()
    if(HIDAPI_LIBRARY)
        MESSAGE(STATUS "System Hidapi Lib ${HIDAPI_LIBRARY} is used")
    else()
        MESSAGE(FATAL_ERROR "Hidapi required to compile cutesdr-vk6hl")
    endif()
endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.h mainwindow.cpp
        mainwindow.ui
        SampleTypes.h
        io/iq/audio/IqAudioDevice.h
        io/iq/audio/IqSampleCursor.h
        dsp/utils/FilterCoefficients.h
        dsp/utils/FftThread.h dsp/utils/FftThread.cpp
        dsp/utils/FftData.h
        dsp/blocks/Oscillator.h dsp/blocks/Oscillator.cpp
        dsp/utils/constants.h
        dsp/utils/PingPongBuffers.h
        dsp/utils/VectorAssembler.h
        dsp/utils/pocketfft/pocketfft_hdronly.h
        radio/iq/IqReceiver.h radio/iq/IqReceiver.cpp
        dsp/stages/IqStage.h
        dsp/stages/oscillators/OscillatorStage.h
        dsp/stages/oscillators/OscillatorMixer.h
        dsp/stages/oscillators/OscillatorInjector.h
        dsp/stages/filters/FastFIR.h
        dsp/stages/DcShift.h
        dsp/stages/demodulators/Demodulator.h
        dsp/stages/demodulators/AmDemodulator.h

        io/control/usb/UsbControl.h
        io/control/usb/UsbException.cpp
        io/control/usb/UsbException.h
        io/control/usb/LibUsbControl.cpp
        io/control/usb/LibUsbControl.h
        io/control/usb/HidUsbControl.cpp
        io/control/usb/HidUsbControl.h
        io/control/usb/UsbControl.cpp
        io/control/device/DeviceControl.cpp
        io/control/device/DeviceControl.h
        io/control/device/UsbDeviceControl.cpp
        io/control/device/UsbDeviceControl.h
        io/control/device/FunCubeDongle/FunCubeDongle.cpp
        io/control/device/FunCubeDongle/FunCubeDongle.h
        io/control/device/DeviceControlException.cpp
        io/control/device/DeviceControlException.h
        radio/settings/RfSettings.h
        radio/settings/IfSettings.h
        radio/settings/ReceiverSettings.h
        radio/settings/RadioSettings.h
        radio/settings/TransmitterSettings.h
        io/iq/IqIoException.h
        io/iq/IqIoException.cpp
        io/iq/IqSink.h
        dsp/stages/demodulators/DemodulatorSettings.h
        dsp/stages/demodulators/AmDemodulator.cpp
        dsp/stages/filters/kernels/FirKernel.h
        dsp/stages/filters/kernels/BandPassFirKernel.h
        dsp/stages/filters/kernels/BandPassFirKernel.cpp
        dsp/stages/filters/kernels/BandStopFirKernel.cpp
        dsp/stages/filters/kernels/BandStopFirKernel.h
        dsp/stages/filters/kernels/LowPassFirKernel.cpp
        dsp/stages/filters/kernels/LowPassFirKernel.h
        DiagnosticSignaller.h
        dsp/stages/filters/OverlapBuffers.h
        dsp/utils/window.h
        dsp/utils/OverlapBuffers.h
        dsp/stages/decimator/Decimator.h
        dsp/stages/decimator/MyFirCoefficients.h
        io/audio/AudioOutput.cpp
        io/audio/AudioOutput.h
        dsp/stages/metering/MeteringStage.h

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(cutesdr-vk6hl
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET cutesdr-vk6hl APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(cutesdr-vk6hl SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(cutesdr-vk6hl
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(cutesdr-vk6hl PUBLIC
    /usr/local/include
    /home/murray/Qt/Examples/Qt-6.5.0/multimedia/shared
    ${VOLK_INCLUDE_DIRS}
    /usr/include/hidapi/
)
target_link_directories(cutesdr-vk6hl PRIVATE ${DSPL_DIR})

target_link_libraries(cutesdr-vk6hl
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    #fftw3
    ${VOLK_LIBRARIES}
    usb-1.0
    hidapi-libusb
)

set_target_properties(cutesdr-vk6hl PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS cutesdr-vk6hl
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(cutesdr-vk6hl)
endif()
