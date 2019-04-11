## NodeInfo

Simple QT fullscreen app compatible with linuxfb (direct framebuffer) that displays information about a Bitcoin Node


### Compile

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


### Run

X11/OSX

    BITCOIN_CLI=/path/to/bin/bitcoin-cli ./nodeinfo

Regtest Linux Without X11 direct framebuffer

    BITCOIN_CLI=/btc/apps/bitcoin-0.18.0rc3/bin/bitcoin-cli ./nodeinfo -platform linuxfb

Mainnet custom Datadir Linux Without X11 direct framebuffer

    BITCOIN_ARGS="-datadir=/btc/data/bitcoin" BITCOIN_CLI=/btc/apps/bitcoin-0.18.0rc3/bin/bitcoin-cli ./nodeinfo -platform linuxfb

#### Configuration Options

Show BTC exchange rate:

* NodeInfo will look for a fill called `exchangerate`
* The file should contain a float without "," or "'" (example: `5000.00`,... but NOT `5'000.00` and NOT `5000,00`)
* It's possible to set the currency code by adding the text after a comma "," (example: `5000.00,CHF`   results in `BTC/CHF 5000.00`)
* It's possible to set the complete text adding a third element (example: `5000.00,CHF,BLA`    results in `BLA 5000.00 [second element is ignored])

Environment Variables

* `BITCOIN_CLI` path to the bitcoin-cli binary
* `BITCOIN_ARGS` arguments to padd to the bitcoin-cli (example: `-regtest -datadir = /tmp/dummy`)
* `BITCOIN_RPC_TIMEOUT` the shell-pipe call timeout
* `NODE_INFO_EXCHANGE_RATE_FILE` path to the exchangerate file
* `WINDOWED` if set to `1`, nodeinfo will run in a window (not compatible with the `linuxfd` platform)


### Screenshots

![screenshot](https://raw.githubusercontent.com/jonasschnelli/nodeinfo/master/docs/screenshot.png)
