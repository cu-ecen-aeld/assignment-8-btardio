qemu-system-aarch64 \
    -M virt  \
    -cpu cortex-a53 -nographic -smp 1 \
    -kernel buildroot/output/images/Image \
    -append "rootwait root=/dev/vda console=ttyAMA0" \
    -netdev user,id=eth0,hostfwd=tcp::10022-:22,hostfwd=tcp::9000-:9000 \
    -device virtio-net-device,netdev=eth0 \
    -drive file=buildroot/output/images/rootfs.ext4,if=none,format=raw,id=hd0 \
    -device virtio-blk-device,drive=hd0 -device virtio-rng-pci

#qemu-system-arm \
#    -M virt  \
#    -cpu cortex-a7 -nographic -smp 1 \
#    -kernel buildroot/output/images/Image \
#    -append "rootwait root=/dev/vda console=ttyAMA0" \
#    -netdev user,id=eth0,hostfwd=tcp::10022-:22,hostfwd=tcp::9000-:9000 \
#    -device virtio-net-device,netdev=eth0 \
#    -drive file=buildroot/output/images/rootfs.ext4,if=none,format=raw,id=hd0 \
#    -device virtio-blk-device,drive=hd0 -device virtio-rng-pci

# dont think this is going to work https://xenbits.xen.org/docs/unstable/man/xen-vtpmmgr.7.html
# swtpm socket --tpm2 -t -d --tpmstate dir=/tmp/tpm --ctrl type=unixio,path=swtpm-sock

#emu/build/qemu-system-aarch64 -kernel buildroot/output/images/Image -drive file=buildroot/output/images/rootfs.ext4,if=none,format=raw,id=hd0 -M virt -netdev user,id=eth0,hostfwd=tcp::10022-:22,hostfwd=tcp::9000-:9000 -device virtio-net-device,netdev=eth0 -append "rootwait root=/dev/vda console=ttyAMA0" -device virtio-blk-device,drive=hd0 -device virtio-rng-pci -cpu cortex-a53 -device gpu

#qemu/build/qemu-system-aarch64 -kernel buildroot/output/images/Image -drive file=buildroot/output/images/rootfs.ext4,if=none,format=raw,id=hd0 
#     -M virt  \
#     -cpu cortex-a53 
#     -nographic -smp 1 \
#     -kernel buildroot/output/images/Image \
#     -append "rootwait root=/dev/vda console=ttyAMA0" \
#     -netdev user,id=eth0,hostfwd=tcp::10022-:22,hostfwd=tcp::9000-:9000 \
#     -device virtio-net-device,netdev=eth0 \
#     -drive file=buildroot/output/images/rootfs.ext4,if=none,format=raw,id=hd0 \
#     -device virtio-blk-device,drive=hd0 -device virtio-rng-pci
# \
#     -chardev socket,id=chrtpm,path=swtpm-sock \
#     -tpmdev emulator,id=tpm0,chardev=chrtpm \
#     -device tpm-tis-device,tpmdev=tpm0
