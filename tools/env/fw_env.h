/*
 * (C) Copyright 2002-2008
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <aes.h>
#include <stdint.h>

/* Pull in the current config to define the default environment */
#include <linux/kconfig.h>

#ifndef __ASSEMBLY__
#define __ASSEMBLY__ /* get only #defines from config.h */
#include <config.h>
#undef	__ASSEMBLY__
#else
#include <config.h>
#endif

/*
 * To build the utility with the static configuration
 * comment out the next line.
 * See included "fw_env.config" sample file
 * for notes on configuration.
 */
#define CONFIG_FILE     "/etc/fw_env.config"

#ifndef CONFIG_FILE
#define HAVE_REDUND /* For systems with 2 env sectors */
#define DEVICE1_NAME      "/dev/mtd1"
#define DEVICE2_NAME      "/dev/mtd2"
#define DEVICE1_OFFSET    0x0000
#define ENV1_SIZE         0x4000
#define DEVICE1_ESIZE     0x4000
#define DEVICE1_ENVSECTORS     2
#define DEVICE2_OFFSET    0x0000
#define ENV2_SIZE         0x4000
#define DEVICE2_ESIZE     0x4000
#define DEVICE2_ENVSECTORS     2
#endif

#ifndef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE		115200
#endif

#ifndef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/
#endif

#ifndef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND							\
	"bootdelay=0\0" \
	"ethaddr=12:34:56:78:99:aa\0" \
	"ethaddr1=12:34:56:78:99:ab\0" \
	"ipaddr=192.168.6.55\0" \
	"serverip=192.168.6.120\0" \
	"gatewayip=192.168.6.1\0" \
	"watchdog=off\0" \
	"bootargs=console=ttyS0,115200n8 root=/dev/mtdblock5 rootfstype=jffs2 rw noinitrd init=/linuxrc mem=64MB mtdparts=nand0:512k(u-boot-spl),512k(env),512k(u-boot),512k(env-bak),4M(Kernel),16M(rootfs),64M(app),-(data)\0" \
	"bootcmd=nboot 0x7fc0 0 0x200000;bootm 0x7fc0\0" 
#endif

struct env_opts {
#ifdef CONFIG_FILE
	char *config_file;
#endif
	int aes_flag; /* Is AES encryption used? */
	uint8_t aes_key[AES_KEY_LENGTH];
	char *lockname;
};

int parse_aes_key(char *key, uint8_t *bin_key);

/**
 * fw_printenv() - print one or several environment variables
 *
 * @argc: number of variables names to be printed, prints all if 0
 * @argv: array of variable names to be printed, if argc != 0
 * @value_only: do not repeat the variable name, print the bare value,
 *          only one variable allowed with this option, argc must be 1
 * @opts: encryption key, configuration file, defaults are used if NULL
 *
 * Description:
 *  Uses fw_env_open, fw_getenv
 *
 * Return:
 *  0 on success, -1 on failure (modifies errno)
 */
int fw_printenv(int argc, char *argv[], int value_only, struct env_opts *opts);

/**
 * fw_setenv() - adds or removes one variable to the environment
 *
 * @argc: number of strings in argv, argv[0] is variable name,
 *          argc==1 means erase variable, argc > 1 means add a variable
 * @argv: argv[0] is variable name, argv[1..argc-1] are concatenated separated
 *           by single blank and set as the new value of the variable
 * @opts: how to retrieve environment from flash, defaults are used if NULL
 *
 * Description:
 *  Uses fw_env_open, fw_env_write, fw_env_close
 *
 * Return:
 *  0 on success, -1 on failure (modifies errno)
 *
 * ERRORS:
 *  EROFS - some variables ("ethaddr", "serial#") cannot be modified
 */
int fw_setenv(int argc, char *argv[], struct env_opts *opts);

/**
 * fw_parse_script() - adds or removes multiple variables with a batch script
 *
 * @fname: batch script file name
 * @opts: encryption key, configuration file, defaults are used if NULL
 *
 * Description:
 *  Uses fw_env_open, fw_env_write, fw_env_close
 *
 * Return:
 *  0 success, -1 on failure (modifies errno)
 *
 * Script Syntax:
 *
 *  key [ [space]+ value]
 *
 *  lines starting with '#' treated as comment
 *
 *  A variable without value will be deleted. Any number of spaces are allowed
 *  between key and value. The value starts with the first non-space character
 *  and ends with newline. No comments allowed on these lines.  Spaces inside
 *  the value are preserved verbatim.
 *
 * Script Example:
 *
 *  netdev         eth0
 *
 *  kernel_addr    400000
 *
 *  foo            spaces           are copied verbatim
 *
 *  # delete variable bar
 *
 *  bar
 */
int fw_parse_script(char *fname, struct env_opts *opts);


/**
 * fw_env_open() - read enviroment from flash into RAM cache
 *
 * @opts: encryption key, configuration file, defaults are used if NULL
 *
 * Return:
 *  0 on success, -1 on failure (modifies errno)
 */
int fw_env_open(struct env_opts *opts);

/**
 * fw_getenv() - lookup variable in the RAM cache
 *
 * @name: variable to be searched
 * Return:
 *  pointer to start of value, NULL if not found
 */
char *fw_getenv(char *name);

/**
 * fw_env_write() - modify a variable held in the RAM cache
 *
 * @name: variable
 * @value: delete variable if NULL, otherwise create or overwrite the variable
 *
 * This is called in sequence to update the environment in RAM without updating
 * the copy in flash after each set
 *
 * Return:
 *  0 on success, -1 on failure (modifies errno)
 *
 * ERRORS:
 *  EROFS - some variables ("ethaddr", "serial#") cannot be modified
 */
int fw_env_write(char *name, char *value);

/**
 * fw_env_close - write the environment from RAM cache back to flash
 *
 * @opts: encryption key, configuration file, defaults are used if NULL
 *
 * Return:
 *  0 on success, -1 on failure (modifies errno)
 */
int fw_env_close(struct env_opts *opts);

unsigned long crc32(unsigned long, const unsigned char *, unsigned);
