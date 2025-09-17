# Notes:

docker run -it --network=host -u bitbake -v ./:/repo classimg /bin/bash

after moving .config to the buildroot use

make BR2_EXTERNAL=/repo/ext-tree/


