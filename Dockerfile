FROM ubuntu:22.04
RUN apt-get update
RUN apt-get install -y build-essential ruby cmake
# ADD docker_install.sh /docker_install.sh
# ADD runner_install.sh /runner_install.sh
# RUN sh /docker_install.sh
# RUN sh /runner_install.sh
# ENV RUNNER_ALLOW_RUNASROOT=1
# RUN /actions-runner/config.sh remove --url https://github.com/cu-ecen-aeld/assignment-1-btardio --token AFAF6AORHSIAUUEWISU3VGDHRMP2C
RUN apt-get install -y vim
RUN apt-get install -y wget
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-linux-gnu.tar.xz
RUN tar -xvf arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-linux-gnu.tar.xz
RUN apt-get install -y python3
# RUN apt-get install -y python3-dev
RUN apt-get install -y gdb
RUN apt-get install -y file
RUN apt-get install -y syslog-ng
RUN apt-get install -y libncurses-dev
RUN apt-get install -y flex

RUN apt-get install -y autotools-dev
RUN apt-get install -y autoconf
RUN apt-get install -y autopoint
RUN apt-get install -y bison
# RUN git clone https://github.com/akimd/bison.git
# RUN cd bison && git submodule update --init
#

# RUN git config --global --add safe.directory /repo/bison
# RUN git config --global --add safe.directory /repo/bison/gnulib
# RUN cd bison && ./bootstrap



RUN apt-get install -y libssl-dev
RUN apt-get install -y bc
RUN apt-get install -y libelf-dev

RUN apt-get install -y cpio
# RUN apt-get install -y qemu-system-aarch64

RUN apt-get install -y rsync
RUN apt-get install -y git

ADD entrypoint.sh /entrypoint.sh
RUN echo "set number" >> /root/.vimrc
RUN echo "set laststatus=2" >> /root/.vimrc
RUN echo "filetype plugin indent on" >> /root/.vimrc
RUN echo "set tabstop=4" >> /root/.vimrc
RUN echo "set shiftwidth=4" >> /root/.vimrc
RUN echo "set expandtab" >> /root/.vimrc
RUN echo "export PATH=$PATH:/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-linux-gnu/bin/" >> /root/.bashrc
RUN mkdir -p /root/.ssh


COPY id_ed25519 /root/.ssh/id_ed25519

RUN chmod +x /entrypoint.sh


RUN apt-get install -y sshpass
RUN apt-get install -y ncat
RUN apt-get install -y valgrind

ADD docker_install.sh /docker_install.sh
ADD runner_install.sh /runner_install.sh
RUN sh /docker_install.sh
RUN sh /runner_install.sh
ENV RUNNER_ALLOW_RUNASROOT=1
RUN /actions-runner/config.sh remove --url https://github.com/cu-ecen-aeld/assignment-1-btardio --token AFAF6AORHSIAUUEWISU3VGDHRMP2C

RUN apt-get install -y build-essential chrpath cpio debianutils diffstat file gawk gcc git iputils-ping libacl1 liblz4-tool locales 
RUN apt-get install -y python3 python3-git 
#RUN apt-get install -y python3-jinja2 
RUN apt-get install -y python3-pexpect 
RUN apt-get install -y python3-pip 
RUN apt-get install -y python3-subunit socat texinfo unzip wget xz-utils zstd

RUN echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen
RUN locale-gen
RUN adduser --disabled-password --gecos "" bitbake
RUN usermod -aG root bitbake


COPY id_ed25519 /home/bitbake/.ssh/id_ed25519


RUN echo "set number" >> /home/bitbake/.vimrc
RUN echo "set laststatus=2" >> /home/bitbake/.vimrc
RUN echo "filetype plugin indent on" >> /home/bitbake/.vimrc                                                                                            
RUN echo "set tabstop=4" >> /home/bitbake/.vimrc                                     
RUN echo "set shiftwidth=4" >> /home/bitbake/.vimrc
RUN echo "set expandtab" >> /home/bitbake/.vimrc
RUN echo "export PATH=$PATH:/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-linux-gnu/bin/" >> /home/bitbake/.bashrc
RUN mkdir -p /home/bitbake/.ssh  


RUN chown -R bitbake:bitbake /home/bitbake

RUN apt-get install -y strace

RUN apt-get install -y swtpm swtpm-tools

COPY known_hosts /known_hosts

RUN mkdir -p /home/bitbake/.ssh
RUN cat /known_hosts >> /home/bitbake/.ssh/known_hosts

RUN mkdir -p /root/.ssh
RUN cat /known_hosts >> /root/.ssh/known_hosts

RUN pip3 install tomli sphinx sphinx_rtd_theme ninja
RUN apt-get install -y glib-2.0

RUN apt-get install -y libslirp-dev

RUN apt-get install -y qemu-system-aarch64


ENTRYPOINT ["/entrypoint.sh"]




#, "/bin/bash"]

#CMD ["/bin/bash"]
