cmd_.tmp_kallsyms2.o := gcc -Wp,-MD,./..tmp_kallsyms2.o.d -D__ASSEMBLY__   -m64   -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h    -c -o .tmp_kallsyms2.o .tmp_kallsyms2.S

deps_.tmp_kallsyms2.o := \
  .tmp_kallsyms2.S \
  include/asm/types.h \

.tmp_kallsyms2.o: $(deps_.tmp_kallsyms2.o)

$(deps_.tmp_kallsyms2.o):
