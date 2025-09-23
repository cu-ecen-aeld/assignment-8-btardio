# Notes:

docker run -it --network=host -u bitbake -v ./:/repo classimg /bin/bash

after moving .config to the buildroot use

make BR2_EXTERNAL=/repo/ext-tree/


### Generate root password for BR2_TARGET_GENERIC_ROOT_PASSWD

openssl passwd -6 -salt $(openssl rand -base64 12) root
