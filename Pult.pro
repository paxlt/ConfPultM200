#-------------------------------------------------
#
# Project created by QtCreator 2015-02-06T11:02:43
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pult
TEMPLATE = app
RC_FILE = icon.rc 

SOURCES += main.cpp \
    CConfParty.cpp \
    CConfPult.cpp \
    CFlowDownLayout.cpp \
    CInputDialog.cpp \
    CConfCore.cpp \
    CConfSMPClient.cpp

HEADERS  += \
    CConfParty.h \
    CConfPult.h \
    CFlowDownLayout.h \
    CInputDialog.h \
    CConfCore.h \
    CConfSMPClient.h

RESOURCES += \
    resources.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
