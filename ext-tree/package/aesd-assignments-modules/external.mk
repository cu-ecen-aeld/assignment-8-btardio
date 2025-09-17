##############################################################
#
# AESD-ASSIGNMENTS
#
##############################################################

AESD_ASSIGNMENTS_MODULES_VERSION = '5c3cae6ddc96b8645dfa6f6bc4ddbba08aae8789'

# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:

# Note: Unable to accomplish the git integration.

AESD_ASSIGNMENTS_MODULES_SITE = 'git@github.com:btardio/ldd3.git'
AESD_ASSIGNMENTS_MODULES_SITE_METHOD = git
AESD_ASSIGNMENTS_MODULES_GIT_SUBMODULES = YES

CROSS_COMPILE=aarch64-none-linux-gnu-
export CROSS_COMPILE

KERNELDIR=$(LINUX_DIR)
export KERNELDIR

KERNELRELEASE=$(LINUX_VERSION)
export KERNELRELEASE

$(info $(@D) is set)

#define AESD_ASSIGNMENTS_MODULES_BUILD_CMDS
        #$(info $(@D) is set!!!)
        #$(info $(KERNELRELEASE))
	#$(info $(KERNELDIR))
	#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/misc-modules/ modules

	#$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(KERNELDIR) M=$(@D)/misc-modules/ modules

#endef

AESD_ASSIGNMENTS_MODULES_MODULE_SUBDIRS = misc-modules/

$(eval $(kernel-module))
$(eval $(generic-package))

