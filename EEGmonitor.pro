QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DataInfo.cpp \
    ParameterInfo.cpp \
    SerialPortInfo.cpp \
    main.cpp \
    WidgetMain.cpp \
    third_party/FFTpsd/FFTpsd.cpp \
    third_party/FFTpsd/fft.cpp \
    third_party/FFTpsd/fft_driver.cpp \
    third_party/FFTpsd/fft_param.cpp \
    third_party/FFTpsd/general_tools.cpp \
    third_party/FFTpsd/string_to_type.cpp \
    third_party/QCustomPlot/qcustomplot.cpp

HEADERS += \
    DataInfo.h \
    Headers.h \
    ParameterInfo.h \
    SerialPortInfo.h \
    WidgetMain.h \
    third_party/FFTpsd/error.h \
    third_party/FFTpsd/fft.h \
    third_party/FFTpsd/fft_driver.h \
    third_party/FFTpsd/fft_param.h \
    third_party/FFTpsd/fft_type.h \
    third_party/FFTpsd/general_tools.h \
    third_party/FFTpsd/getpot.h \
    third_party/FFTpsd/string_to_type.h \
    third_party/QCustomPlot/qcustomplot.h

FORMS += \
    WidgetMain.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
