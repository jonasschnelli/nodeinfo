# Copyright (c) 2013-2016 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

ACLOCAL_AMFLAGS = -I build-aux/m4
.PHONY: deploy FORCE

bin_PROGRAMS = nodeinfo

QT_QRC_CPP = src/qrc_nodeinfo.cpp
QT_QRC = src/nodeinfo.qrc
QT_QRC_LOCALE_CPP = qt/qrc_bitcoin_locale.cpp
QT_QRC_LOCALE = qt/bitcoin_locale.qrc

nodeinfo_SOURCES = $(QT_QRC_CPP) src/moc_stats.cpp src/stats.h src/stats.cpp src/main.cpp
nodeinfo_CPPFLAGS = $(AM_CPPFLAGS) $(BITCOIN_QT_INCLUDES) $(QT_INCLUDES) $(QT_DBUS_INCLUDES)
nodeinfo_CXXFLAGS = $(AM_CXXFLAGS) $(QT_PIE_FLAGS)
nodeinfo_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)
nodeinfo_LDADD = $(QT_LIBS) $(QT_DBUS_LIBS)

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
