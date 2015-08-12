#include <linux/stddef.h>
#include <linux/init.h>
static __initdata char kdb_cmd0[] = "defcmd archkdb \"\" \"First line arch debugging\"\n";
static __initdata char kdb_cmd1[] = "  set BTSYMARG 1\n";
static __initdata char kdb_cmd2[] = "  set BTARGS 5\n";
static __initdata char kdb_cmd3[] = "  pid R\n";
static __initdata char kdb_cmd4[] = "  -archkdbcommon\n";
static __initdata char kdb_cmd5[] = "  -bta\n";
static __initdata char kdb_cmd6[] = "endefcmd\n";
static __initdata char kdb_cmd7[] = "defcmd archkdbcpu \"\" \"archkdb with only tasks on cpus\"\n";
static __initdata char kdb_cmd8[] = "  set BTSYMARG 1\n";
static __initdata char kdb_cmd9[] = "  set BTARGS 5\n";
static __initdata char kdb_cmd10[] = "  pid R\n";
static __initdata char kdb_cmd11[] = "  -archkdbcommon\n";
static __initdata char kdb_cmd12[] = "  -btc\n";
static __initdata char kdb_cmd13[] = "endefcmd\n";
static __initdata char kdb_cmd14[] = "defcmd archkdbshort \"\" \"archkdb with less detailed backtrace\"\n";
static __initdata char kdb_cmd15[] = "  set BTSYMARG 0\n";
static __initdata char kdb_cmd16[] = "  set BTARGS 0\n";
static __initdata char kdb_cmd17[] = "  pid R\n";
static __initdata char kdb_cmd18[] = "  -archkdbcommon\n";
static __initdata char kdb_cmd19[] = "  -bta\n";
static __initdata char kdb_cmd20[] = "endefcmd\n";
static __initdata char kdb_cmd21[] = "defcmd archkdbcommon \"\" \"Common arch debugging\"\n";
static __initdata char kdb_cmd22[] = "  set LINES 2000000\n";
static __initdata char kdb_cmd23[] = "  set BTAPROMPT 0\n";
static __initdata char kdb_cmd24[] = "  -summary\n";
static __initdata char kdb_cmd25[] = "  -id %rip-24\n";
static __initdata char kdb_cmd26[] = "  -cpu\n";
static __initdata char kdb_cmd27[] = "  -ps\n";
static __initdata char kdb_cmd28[] = "  -dmesg 600\n";
static __initdata char kdb_cmd29[] = "  -bt\n";
static __initdata char kdb_cmd30[] = "  -cpu_pda *\n";
static __initdata char kdb_cmd31[] = "endefcmd\n";
extern char *kdb_cmds[]; char __initdata *kdb_cmds[] = {
  kdb_cmd0,
  kdb_cmd1,
  kdb_cmd2,
  kdb_cmd3,
  kdb_cmd4,
  kdb_cmd5,
  kdb_cmd6,
  kdb_cmd7,
  kdb_cmd8,
  kdb_cmd9,
  kdb_cmd10,
  kdb_cmd11,
  kdb_cmd12,
  kdb_cmd13,
  kdb_cmd14,
  kdb_cmd15,
  kdb_cmd16,
  kdb_cmd17,
  kdb_cmd18,
  kdb_cmd19,
  kdb_cmd20,
  kdb_cmd21,
  kdb_cmd22,
  kdb_cmd23,
  kdb_cmd24,
  kdb_cmd25,
  kdb_cmd26,
  kdb_cmd27,
  kdb_cmd28,
  kdb_cmd29,
  kdb_cmd30,
  kdb_cmd31,
  NULL
};
