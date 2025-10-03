QT += quick sql network quickcontrols2 webview networkauth webchannel webenginequick webenginecore webenginewidgets

LANGUAGES = en
# parameters: var, prepend, append
defineReplace(prependAll) {
for(a,$$1):result += $$2$${a}$$3
return($$result)
}

TRANSLATIONS = $$prependAll(LANGUAGES, $$PWD/translations/translations_, .ts)

TRANSLATIONS_FILES =

qtPrepareTool(LRELEASE, lrelease)
for(tsfile, TRANSLATIONS) {
 qmfile = $$tsfile
 qmfile ~= s,.ts$,.qm,
 !exists($$qmfile) {
 qmdir = $$dirname(qmfile)
 !exists($$qmdir) {
 mkpath($$qmdir)|error("Aborting.")
 }
 command = $$LRELEASE -removeidentical $$tsfile -qm $$qmfile
 message("Command: $$command")
 system($$command)|error("Failed to run: $$command")
 }
 TRANSLATIONS_FILES += $$qmfile
}

# Enable console so we can print to stdout
CONFIG += console

SOURCES += src/main.cpp \
    src/appmodule.cpp \
    src/customlogger.cpp \
    src/configmanager.cpp \
    src/db.cpp \
    src/openetworkaccessmanagerfactory.cpp \
    src/ex_canvas.cpp \
    cm/cm_classroom.cpp \
    cm/cm_database.cpp \
    cm/cm_httpserver.cpp \
    cm/cm_javascripthandler.cpp \
    cm/cm_machine.cpp \
    cm/cm_mimetypes.cpp \
    cm/cm_persistentobject.cpp \
    cm/cm_persistentobjectmodel.cpp \
    cm/cm_screengrab.cpp \
    cm/cm_sequentialguid.cpp \
    cm/cm_users.cpp \
    cm/cm_webrequest.cpp \
    cm/cm_websockettransport.cpp \
    cm/file/cm_fileinfo.cpp \
    cm/file/cm_syncfile.cpp \
    cm/file/cm_syncfilechunk.cpp \
    cm/file/cm_syncfileversion.cpp \
    cm/school/sc_classes.cpp \
    cm/school/sc_classmodel.cpp \
    cm/school/sc_lessonitem.cpp \
    cm/school/sc_lessonitemmodel.cpp \
    cm/school/sc_modulemodel.cpp \
    cm/school/sc_modules.cpp \
    cm/school/sc_programmodel.cpp \
    cm/school/sc_programs.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Hide duplicate include warnings - stupid ms c++...
#QMAKE_LFLAGS += /ignore:4042

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050F00
#QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/appmodule.h \
    src/customlogger.h \
    src/configmanager.h \
    src/db.h \
    src/openetworkaccessmanagerfactory.h \
    src/ex_canvas.h \
    cm/file/cm_fileinfo.h \
    cm/file/cm_syncfile.h \
    cm/file/cm_syncfilechunk.h \
    cm/file/cm_syncfileversion.h \
    cm/school/sc_classes.h \
    cm/school/sc_classmodel.h \
    cm/school/sc_lessonitem.h \
    cm/school/sc_lessonitemmodel.h \
    cm/school/sc_modulemodel.h \
    cm/school/sc_modules.h \
    cm/school/sc_programmodel.h \
    cm/school/sc_programs.h \
    cm/cm_classroom.h \
    cm/cm_database.h \
    cm/cm_httpserver.h \
    cm/cm_javascripthandler.h \
    cm/cm_machine.h \
    cm/cm_mimetypes.h \
    cm/cm_persistentobject.h \
    cm/cm_persistentobjectmodel.h \
    cm/cm_screengrab.h \
    cm/cm_sequentialguid.h \
    cm/cm_users.h \
    cm/cm_webrequest.h \
    cm/cm_websockettransport.h


RC_ICONS = resources/images/logo_icon.ico

DISTFILES += \
    resources/scripts/App.js \
    lms.qml \
    not_credentialed.qml \
    qt.conf \
    qtquickcontrols2.conf \
    config.json



# Rules to force qrc rebuild each time - deal with bug where qml files aren't updated on next run
update_qml.target = qml.qrc
update_qml.commands = echo>>$${update_qml.target} # same as touch
update_qml.depends = $$files($${PWD}/*.qml, true) # recurse into subdirs
QMAKE_EXTRA_TARGETS += update_qml
PRE_TARGETDEPS += $${update_qml.target}

# DEPLOYMENT STUFF - Copy Binary files to build folder
CONFIG (debug, debug|release) {
    VARIANT = debug
} else {
    VARIANT = release
}


OPEN_SSL_PATH = "C:/Qt/Tools/OpenSSLv3/Win_x64"
OPEN_SSL_DLL_PATH = $${OPEN_SSL_PATH}/bin
LIBS += -L"$${OPEN_SSL_PATH}/lib" -llibssl -llibcrypto #-llibcrypto -llibssl # -lcrypto -lssl
INCLUDEPATH += "$${OPEN_SSL_PATH}/include"

# Make sure we have files copied to the build folder
copy_files.commands = $(COPY_DIR) \"$$shell_path($$PWD\\www_content)\" \"$$shell_path($$OUT_PWD\\$$VARIANT\\web_content)\" && \
    $(COPY_FILE) \"$$shell_path($$PWD\\resources\\images\\logo_icon.ico)\" \"$$shell_path($$OUT_PWD\\$$VARIANT\\)\" && \
    $(COPY_FILE) \"$$shell_path($$PWD\\resources\\images\\favicon.ico)\" \"$$shell_path($$OUT_PWD\\$$VARIANT\\web_content\\)\" && \
    $(COPY_FILE) \"$$shell_path($$PWD\\resources\\scripts\\qwebchannel.js)\" \"$$shell_path($$OUT_PWD\\$$VARIANT\\web_content\\)\" && \
    $(COPY_FILE) \"$$shell_path($$PWD\\resources\\scripts\\opeWebViewClient.js)\" \"$$shell_path($$OUT_PWD\\$$VARIANT\\web_content\\)\" && \
    $(COPY_FILE) \"$$shell_path($$PWD\\mime_types.csv)\" \"$$shell_path($$OUT_PWD\\$$VARIANT\\)\" && \
    $(COPY_FILE) \"$$shell_path($$PWD\\qt.conf)\" \"$$shell_path($$OUT_PWD\\$$VARIANT\\)\" && \
    $(COPY_FILE) \"$$shell_path($$PWD\\qtquickcontrols2.conf)\" \"$$shell_path($$OUT_PWD\\$$VARIANT\\)\" && \
    $(COPY_FILE) \"$$shell_path($$PWD\\config.json)\" \"$$shell_path($$OUT_PWD\\$$VARIANT\\)\"


first.depends = $(first) copy_files
export(first.depends)
export(copy_files.commands)
QMAKE_EXTRA_TARGETS += first copy_files
