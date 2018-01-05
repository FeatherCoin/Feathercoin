#
# ---- Base Node ----
FROM ubuntu:16.04 AS base
RUN apt-get update && \
    apt-get install -y build-essential libtool autotools-dev autoconf libssl-dev git-core libboost-all-dev libdb4.8-dev libdb4.8++-dev libminiupnpc-dev pkg-config nano curl cmake && \
    apt-get upgrade -y && \
    apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

#
# ---- Build Source ----
FROM base as build
# Checkout latest Sourcecode
RUN git clone https://github.com/feathercoin/Feathercoin.git /opt/feathercoin
# Build All the Dependencies from the build folder
RUN cd /opt/feathercoin && \
    git checkout 0.9.6.2 && \
    make -j3 -C depends HOST=x86_64-unknown-linux-gnu NO_QT=1 NO_UPNP=1

RUN cd /opt/feathercoin && \
	./autogen.sh && \
    ./configure --prefix=/opt/feathercoin/depends/x86_64-unknown-linux-gnu --bindir=/opt/feathercoin/bin --libdir=/opt/feathercoin/lib --disable-tests --without-gui --without-qrcode

RUN cd /opt/feathercoin && \
    make -j2 install

#
# ---- Release ----
FROM ubuntu:16.04 as release
RUN mkdir /data
COPY feathercoin.conf /data/feathercoin.conf
COPY --from=build /opt/feathercoin/bin/feathercoind /usr/local/bin/feathercoind
COPY --from=build /opt/feathercoin/bin/feathercoin-cli /usr/local/bin/feathercoin-cli
VOLUME /data
EXPOSE 9336 9337
CMD ["/usr/local/bin/feathercoind", "-datadir=/data", "-conf=/data/feathercoin.conf", "-server", "-txindex", "-printtoconsole" ]
