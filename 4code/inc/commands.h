
typedef enum USB_COMMANDS
{
    COMMAND_BAD = -1,
    COMMAND_PING = 0,
	COMMAND_BIG_DATA = 1,
	COMMAND_SET_FREQ = 2,
	COMMAND_START_SAMPLING = 3,
	COMMAND_SAMPLING_COMPLETE = 4,
	COMMAND_SAMPLING_BUFFER_SIZE = 5,
	COMMAND_GET_SAMPLES = 6,
	COMMAND_SET_TX = 7,
	COMMAND_GET_CALCULATED = 8,
	COMMAND_START_SAMPLING_AND_CALCULATE = 9,
	COMMAND_CS4272_READ_REG = 10,
} USB_COMMANDS;
