include(../CHComponents.pri)

QT       += core gui widgets printsupport

TARGET = CHStyleDebugger
TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
#enabled catch
CONFIG += exceptions

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# 预处理器定义
DEFINES += CH_CONFIG_HELPER_LIBRARY
DEFINES += _MBCS
DEFINES -= _UNICODE
DEFINES -= UNICODE
DEFINES += DEBUGGER_VERSION=$$VERSION

SOURCES += \
    Components/CHBaseDialog.cpp \
    Components/CHMessageToast.cpp \
    CHStyleDebugger.cpp \
    Components/CHPushButton.cpp \
    Convertqttype.cpp \
    GlobalSelector.cpp \
    ObjectInfoMenu.cpp \
    PropertyPanel.cpp \
    QssEditor.cpp \
    AppContext.cpp \
    StyleDebugger.cpp \
    qscintilla/lexers/LexCSS.cpp \
    qscintilla/lexlib/Accessor.cpp \
    qscintilla/lexlib/CharacterCategory.cpp \
    qscintilla/lexlib/CharacterSet.cpp \
    qscintilla/lexlib/LexerBase.cpp \
    qscintilla/lexlib/LexerModule.cpp \
    qscintilla/lexlib/LexerNoExceptions.cpp \
    qscintilla/lexlib/LexerSimple.cpp \
    qscintilla/lexlib/PropSetSimple.cpp \
    qscintilla/lexlib/StyleContext.cpp \
    qscintilla/lexlib/WordList.cpp \
    qscintilla/Qt4Qt5/InputMethod.cpp \
    qscintilla/Qt4Qt5/ListBoxQt.cpp \
    qscintilla/Qt4Qt5/MacPasteboardMime.cpp \
    qscintilla/Qt4Qt5/PlatQt.cpp \
    qscintilla/Qt4Qt5/qsciabstractapis.cpp \
    qscintilla/Qt4Qt5/qsciapis.cpp \
    qscintilla/Qt4Qt5/qscicommand.cpp \
    qscintilla/Qt4Qt5/qscicommandset.cpp \
    qscintilla/Qt4Qt5/qscidocument.cpp \
    qscintilla/Qt4Qt5/qscilexer.cpp \
    qscintilla/Qt4Qt5/qscilexercss.cpp \
    qscintilla/Qt4Qt5/qscilexercustom.cpp \
    qscintilla/Qt4Qt5/qscimacro.cpp \
    qscintilla/Qt4Qt5/qsciprinter.cpp \
    qscintilla/Qt4Qt5/qsciscintilla.cpp \
    qscintilla/Qt4Qt5/qsciscintillabase.cpp \
    qscintilla/Qt4Qt5/qscistyle.cpp \
    qscintilla/Qt4Qt5/qscistyledtext.cpp \
    qscintilla/Qt4Qt5/SciClasses.cpp \
    qscintilla/Qt4Qt5/ScintillaQt.cpp \
    qscintilla/src/AutoComplete.cpp \
    qscintilla/src/CallTip.cpp \
    qscintilla/src/CaseConvert.cpp \
    qscintilla/src/CaseFolder.cpp \
    qscintilla/src/Catalogue.cpp \
    qscintilla/src/CellBuffer.cpp \
    qscintilla/src/CharClassify.cpp \
    qscintilla/src/ContractionState.cpp \
    qscintilla/src/Decoration.cpp \
    qscintilla/src/Document.cpp \
    qscintilla/src/Editor.cpp \
    qscintilla/src/ExternalLexer.cpp \
    qscintilla/src/Indicator.cpp \
    qscintilla/src/KeyMap.cpp \
    qscintilla/src/LineMarker.cpp \
    qscintilla/src/PerLine.cpp \
    qscintilla/src/PositionCache.cpp \
    qscintilla/src/RESearch.cpp \
    qscintilla/src/RunStyles.cpp \
    qscintilla/src/ScintillaBase.cpp \
    qscintilla/src/Selection.cpp \
    qscintilla/src/Style.cpp \
    qscintilla/src/UniConversion.cpp \
    qscintilla/src/ViewStyle.cpp \
    qscintilla/src/XPM.cpp \
    qscilexerqss.cpp \
    Settings/SoftwareSettings.cpp \
    Settings/QssEditorSettings.cpp \
    Settings/About.cpp

HEADERS += \
    Components/CHBaseDialog.h \
    Components/CHMessageToast.h \
    CHStyleDebugger.h \
    Components/CHPushButton.h \
    ConvertQtTypee.h \
    GlobalSelector.h \
    ObjectInfoMenu.h \
    PropertyPanel.h \
    QssEditor.h \
    AppContext.h \
    Resources.h \
    StyleDebugger.h \
    qscintilla/include/ILexer.h \
    qscintilla/include/Platform.h \
    qscintilla/include/SciLexer.h \
    qscintilla/include/Scintilla.h \
    qscintilla/include/ScintillaWidget.h \
    qscintilla/lexlib/Accessor.h \
    qscintilla/lexlib/CharacterCategory.h \
    qscintilla/lexlib/CharacterSet.h \
    qscintilla/lexlib/LexAccessor.h \
    qscintilla/lexlib/LexerBase.h \
    qscintilla/lexlib/LexerModule.h \
    qscintilla/lexlib/LexerNoExceptions.h \
    qscintilla/lexlib/LexerSimple.h \
    qscintilla/lexlib/OptionSet.h \
    qscintilla/lexlib/PropSetSimple.h \
    qscintilla/lexlib/SparseState.h \
    qscintilla/lexlib/StyleContext.h \
    qscintilla/lexlib/SubStyles.h \
    qscintilla/lexlib/WordList.h \
    qscintilla/Qt4Qt5/ListBoxQt.h \
    qscintilla/Qt4Qt5/SciClasses.h \
    qscintilla/Qt4Qt5/SciNamespace.h \
    qscintilla/Qt4Qt5/ScintillaQt.h \
    qscintilla/Qt4Qt5/Qsci/qsciabstractapis.h \
    qscintilla/Qt4Qt5/Qsci/qsciapis.h \
    qscintilla/Qt4Qt5/Qsci/qscicommand.h \
    qscintilla/Qt4Qt5/Qsci/qscicommandset.h \
    qscintilla/Qt4Qt5/Qsci/qscidocument.h \
    qscintilla/Qt4Qt5/Qsci/qsciglobal.h \
    qscintilla/Qt4Qt5/Qsci/qscilexer.h \
    qscintilla/Qt4Qt5/Qsci/qscilexercss.h \
    qscintilla/Qt4Qt5/Qsci/qscilexercustom.h \
    qscintilla/Qt4Qt5/Qsci/qscimacro.h \
    qscintilla/Qt4Qt5/Qsci/qsciprinter.h \
    qscintilla/Qt4Qt5/Qsci/qsciscintilla.h \
    qscintilla/Qt4Qt5/Qsci/qsciscintillabase.h \
    qscintilla/Qt4Qt5/Qsci/qscistyle.h \
    qscintilla/Qt4Qt5/Qsci/qscistyledtext.h \
    qscintilla/src/AutoComplete.h \
    qscintilla/src/CallTip.h \
    qscintilla/src/CaseConvert.h \
    qscintilla/src/CaseFolder.h \
    qscintilla/src/Catalogue.h \
    qscintilla/src/CellBuffer.h \
    qscintilla/src/CharClassify.h \
    qscintilla/src/ContractionState.h \
    qscintilla/src/Decoration.h \
    qscintilla/src/Document.h \
    qscintilla/src/Editor.h \
    qscintilla/src/ExternalLexer.h \
    qscintilla/src/FontQuality.h \
    qscintilla/src/Indicator.h \
    qscintilla/src/KeyMap.h \
    qscintilla/src/LineMarker.h \
    qscintilla/src/Partitioning.h \
    qscintilla/src/PerLine.h \
    qscintilla/src/PositionCache.h \
    qscintilla/src/RESearch.h \
    qscintilla/src/RunStyles.h \
    qscintilla/src/ScintillaBase.h \
    qscintilla/src/Selection.h \
    qscintilla/src/SplitVector.h \
    qscintilla/src/Style.h \
    qscintilla/src/UnicodeFromUTF8.h \
    qscintilla/src/UniConversion.h \
    qscintilla/src/ViewStyle.h \
    qscintilla/src/XPM.h \
    qscilexerqss.h \
    Settings/SoftwareSettings.h \
    Settings/QssEditorSettings.h \
    Settings/About.h
DISTFILES += CHStyleDebugger.json \
    qscintilla/include/Scintilla.iface \
    qscintilla/lexlib/License.txt \
    qscintilla/Qt4Qt5/qscintilla_cs.qm \
    qscintilla/Qt4Qt5/qscintilla_de.qm \
    qscintilla/Qt4Qt5/qscintilla_es.qm \
    qscintilla/Qt4Qt5/qscintilla_fr.qm \
    qscintilla/Qt4Qt5/qscintilla_pt_br.qm \
    qscintilla/Qt4Qt5/qscintilla_ru.qm \
    qscintilla/Qt4Qt5/qscintilla_cs.ts \
    qscintilla/Qt4Qt5/qscintilla_de.ts \
    qscintilla/Qt4Qt5/qscintilla_es.ts \
    qscintilla/Qt4Qt5/qscintilla_fr.ts \
    qscintilla/Qt4Qt5/qscintilla_pt_br.ts \
    qscintilla/Qt4Qt5/qscintilla_ru.ts \
    qscintilla/src/License.txt \
    qscintilla/src/SciTE.properties

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    Components/CHBaseDialog.ui \
    Components/CHMessageToast.ui \
    ObjectInfoMenu.ui \
    PropertyPanel.ui \
    QssEditor.ui \
    StyleDebugger.ui \
    Settings/SoftwareSettings.ui \
    Settings/QssEditorSettings.ui \
    Settings/About.ui

#插件放在MyPlugins目录中
DESTDIR = $$sprintf($$DESTDIR/ChPlugins)
win32: DLLDESTDIR = $$sprintf($$DLLDESTDIR/ChPlugins)

RESOURCES += \
    Resources/Image.qrc \
    Resources/Qss.qrc

INCLUDEPATH += $PWD/Include
INCLUDEPATH += $$_PRO_FILE_PWD_/qscintilla $$_PRO_FILE_PWD_/qscintilla/include $$_PRO_FILE_PWD_/qscintilla/lexlib $$_PRO_FILE_PWD_/qscintilla/Qt4Qt5 $$_PRO_FILE_PWD_/qscintilla/src
DEFINES += SCINTILLA_QT SCI_LEXER

#引入私有头文件
INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtGui/$$[QT_VERSION]/ \
    $$[QT_INSTALL_HEADERS]/QtCore/$$[QT_VERSION]/
