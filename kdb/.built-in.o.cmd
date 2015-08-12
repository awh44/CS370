cmd_kdb/built-in.o :=  ld -m elf_x86_64  -r -o kdb/built-in.o kdb/kdb_bt.o kdb/kdb_bp.o kdb/kdb_id.o kdb/kdbsupport.o kdb/gen-kdb_cmds.o kdb/kdbmain.o kdb/kdb_io.o kdb/modules/built-in.o
