dwcash_packages := libsnark libgmp libsodium
packages := boost openssl $(dwcash_packages) googletest googlemock
darwin_packages:=zeromq
linux_packages:=zeromq
native_packages := native_ccache

wallet_packages=bdb

upnp_packages=miniupnpc
