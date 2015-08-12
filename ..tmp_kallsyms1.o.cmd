cmd_.tmp_kallsyms1.o := gcc -Wp,-MD,./..tmp_kallsyms1.o.d -D__ASSEMBLY__   -m64   -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h    -c -o .tmp_kallsyms1.o .tmp_kallsyms1.S

deps_.tmp_kallsyms1.o := \
  .tmp_kallsyms1.S \
  include/asm/types.h \

.tmp_kallsyms1.o: $(deps_.tmp_kallsyms1.o)

$(deps_.tmp_kallsyms1.o):
