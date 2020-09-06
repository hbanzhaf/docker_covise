FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive 
ENV OPENCRG_HOME=/opt/opencrg

# Install packages
RUN apt-get update && apt-get install -y \
    sudo \
    software-properties-common \
    vim \
    git gitk \
    cmake cmake-curses-gui \
    libxerces-c-dev \
    libqt4-dev \
    qttools5-dev qtscript5-dev libqt5scripttools5 libqt5svg5-dev libqt5opengl5-dev libqt5webkit5-dev pyqt5-dev pyqt5-dev-tools python-pyqt5 python3-pyqt5 \ 
    libboost-all-dev \ 
    libglew-dev \
    libopenscenegraph-dev \
    libjpeg-dev libpng-dev libtiff-dev \
    freeglut3-dev \
    libvtk6-dev

# Compile OpenCRG and move it to $OPENCRG_HOME
COPY OpenCRG.1.1.2 /tmp/OpenCRG.1.1.2
RUN make -C /tmp/OpenCRG.1.1.2/c-api
RUN mkdir -p $OPENCRG_HOME/include && mkdir $OPENCRG_HOME/lib && mkdir $OPENCRG_HOME/obj
RUN cp /tmp/OpenCRG.1.1.2/c-api/baselib/inc/* $OPENCRG_HOME/include/
RUN cp /tmp/OpenCRG.1.1.2/c-api/baselib/lib/* $OPENCRG_HOME/lib/
RUN cp /tmp/OpenCRG.1.1.2/c-api/baselib/obj/* $OPENCRG_HOME/obj/
RUN rm -r /tmp/OpenCRG.1.1.2

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
