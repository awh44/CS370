cmd_kdb/modules/built-in.o :=  ld -m elf_x86_64  -r -o kdb/modules/built-in.o kdb/modules/kdbm_pg.o kdb/modules/kdbm_task.o kdb/modules/kdbm_vm.o kdb/modules/kdbm_sched.o kdb/modules/kdbm_x86.o
