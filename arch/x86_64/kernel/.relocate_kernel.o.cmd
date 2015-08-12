cmd_arch/x86_64/kernel/relocate_kernel.o := gcc -Wp,-MD,arch/x86_64/kernel/.relocate_kernel.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__   -m64 -traditional   -c -o arch/x86_64/kernel/relocate_kernel.o arch/x86_64/kernel/relocate_kernel.S

deps_arch/x86_64/kernel/relocate_kernel.o := \
  arch/x86_64/kernel/relocate_kernel.S \
  include/linux/linkage.h \
  include/asm/linkage.h \
  include/asm/page.h \
    $(wildcard include/config/physical/start.h) \
    $(wildcard include/config/flatmem.h) \
  include/linux/const.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/out/of/line/pfn/to/page.h) \
  include/asm-generic/page.h \
  include/asm/kexec.h \

arch/x86_64/kernel/relocate_kernel.o: $(deps_arch/x86_64/kernel/relocate_kernel.o)

$(deps_arch/x86_64/kernel/relocate_kernel.o):
