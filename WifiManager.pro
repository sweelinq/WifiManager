QT += quick concurrent

SOURCES += \
        main.cpp \
        nmcli.cpp
        
HEADERS += \
        WifiManager.h \
        WifiScanner.h \
        nmcli.h

resources.files = main.qml 
resources.prefix = /$${TARGET}
RESOURCES += resources \
    Icons.qrc \
    qml.qrc
