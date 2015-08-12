cmd_kdb/gen-kdb_cmds.o := gcc -Wp,-MD,kdb/.gen-kdb_cmds.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -O2 -fno-optimize-sibling-calls -mtune=generic -m64 -mno-red-zone -mcmodel=kernel -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -funit-at-a-time -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -maccumulate-outgoing-args -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -fno-stack-protector -Wdeclaration-after-statement -Wno-pointer-sign     -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(gen_kdb_cmds)"  -D"KBUILD_MODNAME=KBUILD_STR(gen_kdb_cmds)" -c -o kdb/gen-kdb_cmds.o kdb/gen-kdb_cmds.c

deps_kdb/gen-kdb_cmds.o := \
  kdb/gen-kdb_cmds.c \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/enable/must/check.h) \
  include/linux/compiler-gcc4.h \
    $(wildcard include/config/forced/inlining.h) \
  include/linux/compiler-gcc.h \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/acpi/hotplug/memory.h) \

kdb/gen-kdb_cmds.o: $(deps_kdb/gen-kdb_cmds.o)

$(deps_kdb/gen-kdb_cmds.o):
