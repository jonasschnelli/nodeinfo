## NodeInfo

Simple QT fullscreen app compatible with linuxfb (direct framebuffer) that displays information about a Bitcoin Node


#### Compile

Debian / Ubuntu

    sudo apt-get install build-essential libtool autotools-dev automake pkg-config
    sudo apt-get install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools
    
    ./autogen
    ./configure
    make

OSX

    brew install automake libtool pkg-config qt
    
    ./autogen
    ./configure
    make


#### Run

X11/OSX

    BITCOIN_CLI=/path/to/bin/bitcoin-cli ./nodeinfo

Regtest Linux Without X11 direct framebuffer

    BITCOIN_CLI=/btc/apps/bitcoin-0.18.0rc3/bin/bitcoin-cli ./nodeinfo -platform linuxfb

Mainnet custom Datadir Linux Without X11 direct framebuffer

    BITCOIN_ARGS="-datadir=/btc/data/bitcoin" BITCOIN_CLI=/btc/apps/bitcoin-0.18.0rc3/bin/bitcoin-cli ./nodeinfo -platform linuxfb
