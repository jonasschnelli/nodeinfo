# Copyright (c) 2013-2016 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

ACLOCAL_AMFLAGS = -I build-aux/m4
.PHONY: deploy FORCE

AM_LDFLAGS = $(LIBTOOL_LDFLAGS) $(HARDENED_LDFLAGS)
AM_CXXFLAGS = $(DEBUG_CXXFLAGS) $(HARDENED_CXXFLAGS) $(WARN_CXXFLAGS) $(NOWARN_CXXFLAGS) $(ERROR_CXXFLAGS)
AM_CPPFLAGS = $(DEBUG_CPPFLAGS) $(HARDENED_CPPFLAGS)

bin_PROGRAMS = nodeinfo

QT_QRC_CPP = src/qrc_nodeinfo.cpp
QT_QRC = src/nodeinfo.qrc
QT_QRC_LOCALE_CPP = qt/qrc_bitcoin_locale.cpp
QT_QRC_LOCALE = qt/bitcoin_locale.qrc

QT_H = \
  src/updater.h \
  src/datadrawer.h \
  src/utils.h

QT_CPP = \
  src/updater.cpp \
  src/datadrawer.cpp \
  src/main.cpp \
  src/mainwindow.cpp \
  src/utils.cpp

QT_MOC_CPP = \
  src/moc_mainwindow.cpp \
  src/moc_datadrawer.cpp \
  src/moc_updater.cpp
 
nodeinfo_SOURCES = $(QT_QRC_CPP) $(QT_MOC_CPP) $(QT_H) $(QT_CPP)
nodeinfo_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_QT_INCLUDES) $(QT_INCLUDES) $(QT_DBUS_INCLUDES)
nodeinfo_CXXFLAGS = $(AM_CXXFLAGS) $(QT_PIE_FLAGS)
nodeinfo_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)
nodeinfo_LDADD = $(QT_LIBS) $(QT_DBUS_LIBS)

CLEANFILES = $(QT_MOC_CPP) $(QT_MOC)

$(QT_QRC_CPP): $(QT_QRC)
	@test -f $(RCC)
	$(AM_V_GEN) QT_SELECT=$(QT_SELECT) $(RCC) -name bitcoin $< | \
	  $(SED) -e '/^\*\*.*Created:/d' -e '/^\*\*.*by:/d' > $@

%.moc: %.cpp
	$(AM_V_GEN) QT_SELECT=$(QT_SELECT) $(MOC) $(DEFAULT_INCLUDES) $(QT_INCLUDES) $(MOC_DEFS) $< | \
	  $(SED) -e '/^\*\*.*Created:/d' -e '/^\*\*.*by:/d' > $@

moc_%.cpp: %.h
	$(AM_V_GEN) QT_SELECT=$(QT_SELECT) $(MOC) $(DEFAULT_INCLUDES) $(QT_INCLUDES) $(MOC_DEFS) $< | \
	  $(SED) -e '/^\*\*.*Created:/d' -e '/^\*\*.*by:/d' > $@
