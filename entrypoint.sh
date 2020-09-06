#!/bin/bash

# user
groupadd -g $HOST_GID $DOCKER_USER
useradd -M -s /bin/bash -g $HOST_GID -d $DOCKER_HOME -u $HOST_UID $DOCKER_USER
bash -c "grep -q $DOCKER_USER /etc/sudoers || echo '$DOCKER_USER ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers"

# home
if [ -d $DOCKER_HOME ]; then
    chown -R $HOST_UID:$HOST_GID $DOCKER_HOME
fi

exec su $DOCKER_USER
