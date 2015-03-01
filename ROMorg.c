/**
 * ROMorg
 * by Brandon, 2015
 */

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#define MAX_TITLE_LENGTH 128
#define TMP_DIR "romorg_tmp"
#define ROM_DIR "./"

int is_rom(char *filename)
{
	char *ext  = strrchr(filename, '.');
	int is_rom = 0;

	if (ext)
	{
		// lazy extension check
		if (!strcasecmp(ext, ".3ds") || !strcasecmp(ext, ".3dz"))
			is_rom = 1;
	}

	return is_rom;
}

int count_roms(struct dirent *ds, DIR *rom_dir)
{
	int rom_count = 0;

	while (ds = readdir(rom_dir))
	{
		if (!is_rom(ds->d_name)) 
			continue;
			
		rom_count++;
	}

	rewinddir(rom_dir);

	return rom_count;
}

void scan_roms(char *roms_list_buffer, struct dirent *ds, DIR *rom_dir)
{
	int i = 0;

	while (ds = readdir(rom_dir))
	{
		if (!is_rom(ds->d_name))
			continue;

		strcpy(&roms_list_buffer[i], ds->d_name);

		// fixme
		// strncpy(&roms_list_buffer[i], ds->d_name, sizeof(ds->d_name));

		i += MAX_TITLE_LENGTH;
	}
}

void main ()
{
	puts("-----------------------------");
	puts("---------   ROMorg   --------");
	puts("--------- by Brandon --------");
	puts("-----------------------------\n");

	struct dirent *ds;
	DIR *dirp     = opendir(ROM_DIR);
	int rom_count = count_roms(ds, dirp);
	char roms_list_buffer[(rom_count) * MAX_TITLE_LENGTH];

	if (!rom_count)
	{
		puts("No ROMs found.");
		return;
	}
	
	if (!(mkdir(TMP_DIR, 0777) == 0))
	{
		puts("Unable to create tmp directory.");
		return;
	}
	
	scan_roms(roms_list_buffer, ds, dirp);
	closedir(dirp);

	printf("Found %d ROMs:\n", rom_count);
	for (int i = 0; i < rom_count; i++)
	{
		printf("%d. %s\n", i+1, &roms_list_buffer[i * MAX_TITLE_LENGTH]);
	}


	printf("\nWhich ROM do you want to appear first?\nEnter ID: ");

	int rom_order_buffer[rom_count];
	int tmp_input;

	// todo: instead of for(), use a do/while until sizeof(rom_order_buffer) = rom_count
	for (int i = 0; i < rom_count; i++)
	{
		if (i >= 1)
			printf("Next ID: ");

		// check if valid int, and if valid rom id
		// todo: check if input already exists in rom_order_buffer
		if (scanf("%d", &tmp_input) && tmp_input <= rom_count && tmp_input > 0)
		{
			rom_order_buffer[i] = tmp_input;
		}
		else
		{
			// fixme: this will skip any remaining iterations and print invalid rom id for each, only if input != int
			// if input is an invalid rom id but still an int, it continues as expected
			puts("invalid rom id");
		}
	}

	printf("\nSorting...");

	// move 'em in
	char rom_in_tmp[(rom_count) * (MAX_TITLE_LENGTH + sizeof(TMP_DIR))];
	int rom_in_tmp_index;
	for (int i = 0; i < rom_count; i++)
	{
		rom_in_tmp_index = (i * (MAX_TITLE_LENGTH + sizeof(TMP_DIR)));
		strcpy(&rom_in_tmp[rom_in_tmp_index], TMP_DIR "/");
		strcat(&rom_in_tmp[rom_in_tmp_index], &roms_list_buffer[i * MAX_TITLE_LENGTH]);

		// printf("\nmoving %s into %s...", &roms_list_buffer[i * MAX_TITLE_LENGTH], &rom_in_tmp[rom_in_tmp_index]);
		rename(&roms_list_buffer[i * MAX_TITLE_LENGTH], &rom_in_tmp[rom_in_tmp_index]);
		printf(".");
	}

	// take 'em out, in order
	char rom_in_root;
	int user_ordered_index;
	for (int i = 0; i < rom_count; i++)
	{
		user_ordered_index = (rom_order_buffer[i] - 1);
		rom_in_tmp_index   = (user_ordered_index * (MAX_TITLE_LENGTH + sizeof(TMP_DIR)));
		strcpy(&rom_in_root, ROM_DIR);
		strcat(&rom_in_root, &roms_list_buffer[user_ordered_index * MAX_TITLE_LENGTH]);

		// printf("\nmoving %s into %s...", &rom_in_tmp[rom_in_tmp_index], &rom_in_root);
		rename(&rom_in_tmp[rom_in_tmp_index], &rom_in_root);
		printf(".");
	}

	// bye bye
	remove(TMP_DIR);
	
	printf("\nDone!");
	return;
}
