TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += $$PWD/Src/CHStyleDebugger.pro
SUBDIRS += $$PWD/Examples/Demo.pro

Demo.depends = CHStyleDebugger
