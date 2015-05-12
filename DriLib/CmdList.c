#include <stdint.h>
#include <stdbool.h>
#include "CmdList.h"
#include "cmdline.h"
#include "driverlib/eeprom.h"
#include "driverlib/uart.h"
#include "ustdlib.h"
#include "uartstdio.h"


// argc la` so' cai' nha^.p vao`. VD:  "read 8" thi` argc = 2 . "write 8 33" thi` argc = 3
// argv[0] la` *(agrv) = "read"  hoac "write" . chua' ma~ le^.nh
// argv [1] la cai nap thu' 2. VD "read 8" thi` argv[1] = 8
//argv [2] la cai nap thu'  3. VD "write 8 55" thi` argv[2] = 55
//ustrtoul(const char * restrict nptr, const char ** restrict endptr, int base)
// base la` dang cua? gia tri dang chua'


//*****************************************************************************
//
// Command: help
//
// Print the help strings for all commands.
//
//*****************************************************************************

int
CMD_help(int argc, char **argv)
{
	int32_t i32Index;

	(void)argc;
	(void)argv;

	//
	// Start at the beginning of the command table
	//
	i32Index = 0;

	//
	// Get to the start of a clean line on the serial output.
	//
	UARTprintf("\nAvailable Commands\n------------------\n\n");

	//
	// Display strings until we run out of them.
	//
	while(g_psCmdTable[i32Index].pcCmd)
	{
		UARTprintf("%17s %s\n", g_psCmdTable[i32Index].pcCmd,
				g_psCmdTable[i32Index].pcHelp);
		i32Index++;
	}

	//
	// Leave a blank line after the help strings.
	//

	UARTprintf("\n");

	return (0);
}

int
CMD_read(int argc, char **argv) {
	uint32_t addr;
	uint32_t val;

	if (argc < 2)
		return (CMDLINE_TOO_FEW_ARGS);

	if (argc > 2)
		return (CMDLINE_TOO_MANY_ARGS);

	addr = ustrtoul(argv[1], 0, 10);

	EEPROMRead(&val, addr & ~3, 4);

	UARTprintf("EEPROM[%d] == %d\n", addr & ~3, val);
	return (0);

}

int
CMD_write(int argc, char **argv) {
	uint32_t addr;
	uint32_t val;

	if (argc < 3)
		return (CMDLINE_TOO_FEW_ARGS);

	if (argc > 3)
		return (CMDLINE_TOO_MANY_ARGS);

	addr = ustrtoul(argv[1],  0, 10);
	val  = ustrtoul(argv[2],  0, 10);

	EEPROMProgram(&val, addr & ~3, 4);
	UARTprintf("EEPROM[%d] <- %d\n", addr & ~3, val);


	return (0);

}

// Table of valid command strings, callback functions and help messages.

tCmdLineEntry g_psCmdTable[] =
{
		{"help",     CMD_help,      " : Display list of commands" },
		{"read",     CMD_read,      " : Read word from eeprom :  read addr "},
		{"write",    CMD_write,     " : Write word to eeprom :  write addr val"},
		{0,0,0}
};
