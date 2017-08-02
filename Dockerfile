FROM debian:jessie
RUN apt-get update
RUN apt-get -y install scons \
	    libboost-all-dev \
	    build-essential \
	    libgcrypt11-dev \
	    libgmp-dev

COPY . /app
WORKDIR /app
RUN scons
