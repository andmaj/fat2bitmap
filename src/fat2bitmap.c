/*
 * Creates a bitmap from FAT file system (free/used clusters)
 * 
 * Written by Andras Majdan
 * License: GNU General Public License Version 3
 * 
 * Report bugs to <majdan.andras@gmail.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>

#include "common.h"
#include "fsck.fat.h"
#include "io.h"
#include "boot.h"
#include "fat.h"
#include "file.h"
#include "check.h"
#include "charconv.h"

#ifndef NOSIGNAL
#include <signal.h>
#endif

#define VERSION "1.0.0"
 
/* Required for dosfstools */
int interactive = 0, rw = 0, list = 0, test = 0, verbose = 0, write_immed = 0;
int atari_format = 0, boot_only = 0;
unsigned n_files = 0;
void *mem_queue = NULL;

unsigned long long out_bytes = 0; 
int run;

time_t start_time;

void print_progress(FILE *out)
{
	time_t end_time;
	double s;
	
	time(&end_time);
	s =  difftime(end_time, start_time);
	
	fprintf(out, 
			"%llu bytes out\n"
			"%.0f s, %.1f MB/s\n",
			out_bytes, s, 
			(double)out_bytes/s/(double)1000000);
}

#ifndef NOSIGNAL
void sig_handler(int signum)
{
	if(signum==SIGUSR1) print_progress(stderr);
	if(signum==SIGINT) run = 0;
}
#endif

// Returns 0 on success
int print_fat_cluster_map(DOS_FS *fs, FILE *out, 
							unsigned long long *out_bytes)
{
	#ifndef NOSIGNAL
	signal(SIGUSR1, sig_handler);
	signal(SIGINT, sig_handler);
	#endif
	
	time(&start_time);
	
    unsigned long i;

    for (i = 2; i < fs->clusters + 2; i++) {
		if(!run) return 0;
		FAT_ENTRY curEntry;
		get_fat(&curEntry, fs->fat, i, fs);

		if (!get_owner(fs, i) && !FAT_IS_BAD(fs, curEntry.value))
		{
			if ( fprintf(out, "0") != 1 ) return 1;
		}
		else
		{
			if ( fprintf(out, "1") != 1 ) return 1;
		}
		(*out_bytes)++;
    }
    
    return 0;
}

int fat2bitmap(char *in, FILE *out)
{
	DOS_FS fs;
	memset(&fs, 0, sizeof(fs));
	
	set_dos_codepage(-1);
	fs_open(in, 0);
	read_boot(&fs);
	read_fat(&fs);
	scan_root(&fs);
	qfree(&mem_queue);
	run = 1;
	if( print_fat_cluster_map(&fs, out, &out_bytes) )
	{
		fprintf(stderr, "I/O error\n");
		print_progress(stderr);
		fs_close(0);
		fclose(out);
		return 1;
	}
	
	if(out != stdout) print_progress(stdout);
	fs_close(0);
	fclose(out);
	return 0;
}

int main(int argc, char *argv[])
{
	int c;
	
	char *in;
	FILE *out;
	
	/* Default values */
	in = NULL;
	out = stdout;
 
	while(1)
	{
		static struct option long_options[] =
		{
			{"if", 		required_argument, 0, 'i'},
			{"of",    	required_argument, 0, 'o'},	
			{"help",    no_argument, 	   0, 'h'},
			{"version", no_argument, 	   0, 'v'},
			{0, 0, 0, 0}
		};
		
		int option_index = 0;
     
		c = getopt_long(argc, argv, "i:o:hv",
										long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;
     
		switch (c)
		{
			case 'i':
				in = optarg;
				break;
				
			case 'o':
				out = fopen(optarg, "w+b");
				if(out==NULL)
				{
					fprintf(stderr,
							"Cannot open output file: %s\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			
			case 'h':
				printf(
					"Usage: fat2bitmap [options]\n\n"
					
					"Creates a bitmap from FAT file system (free/used clusters).\n\n"
					
					"Options:\n"
					"  --if FILE     Input file\n"
					"  --of FILE     Output file (default: stdout)\n"
					"  --help        Usage information\n"
					"  --version     Print version\n\n"
 
					"Example: fat2bitmap --if filesys.iso --of map.txt\n\n"

					"Written by Andras Majdan\n"
					"License: GNU General Public License Version 3\n"
					"Report bugs to <majdan.andras@gmail.com>\n");
				exit (EXIT_SUCCESS);
				break;
			case 'v':
				printf("Version: %s (Compiled on %s)\n", VERSION, __DATE__);
				exit (EXIT_SUCCESS);
				break;
			case '?':
				/* getopt_long already printed an error message. */
				exit(EXIT_FAILURE);
				break;
		}
	}
     
	/* Print any remaining command line arguments (not options). */
	if (optind < argc)
	{
		fprintf (stderr, "Invalid option(s): ");
		while (optind < argc)
			fprintf (stderr, "%s ", argv[optind++]);
		fprintf(stderr, "\n");
			
		exit(EXIT_FAILURE);
	}
	
	if( in == NULL )
	{
		fprintf(stderr, "Required option: --if\n");
		exit(EXIT_FAILURE);
	}
	
	if( fat2bitmap(in, out) )
		return EXIT_FAILURE;
	else 
		return EXIT_SUCCESS;	
}

