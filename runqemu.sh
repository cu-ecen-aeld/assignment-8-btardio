
PARENT_COMMAND=$(ps -o comm= $PPID)
echo "The calling process name is: $PARENT_COMMAND"

if [[ "assignment-test" == "$PARENT_COMMAND" ]]; then
    echo "Qemu starting, called from assignment-test"
    KERNEL=Image
    DRIVE=rootfs.ext4
else
    echo "Qemu starting, called from $PARENT_COMMAND"
    KERNEL=buildroot/output/images/Image
    DRIVE=buildroot/output/images/rootfs.ext4
fi

qemu-system-aarch64 \
    -M virt  \
    -cpu cortex-a53 -nographic -smp 1 \
    -kernel $KERNEL \
    -append "rootwait root=/dev/vda console=ttyAMA0" \
    -netdev user,id=eth0,hostfwd=tcp::10022-:22,hostfwd=tcp::9000-:9000 \
    -device virtio-net-device,netdev=eth0 \
    -drive file=$DRIVE,if=none,format=raw,id=hd0 \
    -device virtio-blk-device,drive=hd0 -device virtio-rng-pci

