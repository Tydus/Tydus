/*
* Open Chinese Convert with judge function
*
* Copyright 2010 BYVoid <byvoid.kcp@gmail.com>
* Copyright 2012 Tydus <Tydus@Tydus.org>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <opencc/opencc.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#define BUFFER_SIZE 65536

#ifndef _
  #define _(x) (x)
#endif

int cri(const char * s, const char * const * lst)
{
    for(;*lst;lst++){
        if (strstr(s,*lst))
            return 0;
    }

    return 1;
}

void convert(
        const char * input_file,
        const char * output_file,
        const char * config_file,
        const char * const * lst
        )
{
	opencc_t od = opencc_open(config_file);
	if (od == (opencc_t) -1)
	{
		opencc_perror(_("OpenCC initialization error"));
		exit(1);
	}

	FILE * fp = stdin;
	FILE * fpo = stdout;
	
	if (input_file)
	{
		fp = fopen(input_file, "r");
		if (!fp)
		{
			fprintf(stderr, _("Can not read file: %s\n"), input_file);
			exit(1);
		}
	}
	
	if (output_file)
	{
		fpo = fopen(output_file, "w");
		if (!fpo)
		{
			fprintf(stderr, _("Can not write file: %s\n"), output_file);
			exit(1);
		}
	}
	
	size_t size = BUFFER_SIZE;
	char * buffer_in = NULL, * buffer_out = NULL;
	buffer_in = (char *) malloc(size * sizeof(char));
	
	while (fgets(buffer_in, size, fp) != NULL)
    {
        size_t freesize = size;

        char * buffer_in_p = buffer_in;
        size_t line_length = strlen(buffer_in_p);
        while (line_length + 1 == freesize && buffer_in_p[line_length - 2] != '\n')
        {
            //如果一行沒讀完，則最後一個字符不是換行，且讀滿緩衝區
            buffer_in_p += size - 1;
            freesize = size + 1;
            size += size;
            size_t offset = buffer_in_p - buffer_in;
            buffer_in = (char *) realloc(buffer_in, size * sizeof(char));
            buffer_in_p = buffer_in + offset;

            if (fgets(buffer_in_p, freesize, fp) == NULL)
                break;

            line_length = strlen(buffer_in_p);
        }

        if (cri (buffer_in, lst))
        {
            buffer_out = opencc_convert_utf8(od, buffer_in, (size_t) -1);
            if (buffer_out != (char *) -1)
            {
                fprintf(fpo, "%s", buffer_out);
                free(buffer_out);
            }
            else
            {
                opencc_perror(_("OpenCC error"));
                break;
            }
        }else{
            fprintf(fpo, "%s", buffer_in);
        }
    }
	
	opencc_close(od);
	
	free(buffer_in);
	
	fclose(fp);
	fclose(fpo);
}

void show_version()
{
  printf(_("\n"));
	printf(_("Open Chinese Convert (OpenCC) Command Line Tool\n"));
	printf(_("\n"));
	printf(_("Author: %s\n"), "BYVoid <byvoid.kcp@gmail.com>");
	printf(_("Bug Report: %s\n"), "http://code.google.com/p/opencc/issues/entry");
	printf(_("\n"));
}

void show_usage()
{
	show_version();
	printf(_("Usage:\n"));
	printf(_(" opencc [Options]\n"));
	printf(_("\n"));
	printf(_("Options:\n"));
	printf(_(" -i [file], --input=[file]   Read original text from [file].\n"));
	printf(_(" -o [file], --output=[file]  Write converted text to [file].\n"));
	printf(_(" -c [file], --config=[file]  Load configuration of conversion from [file].\n"));
	printf(_(" -p [list], --pass=[list]    pass the lines if contains words in the list,\n"));
	printf(_("                             using the following syntax like sed: '@op_jap@ed_jap@'\n"));
	printf(_(" -v, --version               Print version and build information.\n"));
	printf(_(" -h, --help                  Print this help.\n"));
	printf(_("\n"));
	printf(_("With no input file, reads standard input and writes converted stream to standard output.\n"));
	printf(_("Default configuration(%s) will be loaded if not set.\n"), OPENCC_DEFAULT_CONFIG_SIMP_TO_TRAD);
	printf(_("\n"));
}

int main(int argc, char ** argv)
{
#ifdef ENABLE_GETTEXT
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE_NAME, LOCALEDIR);
#endif

	static struct option longopts[] =
	{
		{ "version", no_argument, NULL, 'v' },
		{ "help", no_argument, NULL, 'h' },
		{ "input", required_argument, NULL, 'i' },
		{ "output", required_argument, NULL, 'o' },
		{ "config", required_argument, NULL, 'c' },
		{ "pass", required_argument, NULL, 'p' },
		{ 0, 0, 0, 0 },
	};

	static int oc;
	static char *input_file, *output_file, *config_file;
    static char *lst[256];
    int i=1;

	while((oc = getopt_long(argc, argv, "vh:i:o:c:p:", longopts, NULL)) != -1)
	{
		switch (oc)
		{
		case 'v':
			show_version();
			return 0;
		case 'h':
			show_usage();
			return 0;
		case '?':
			printf(_("Please use %s --help.\n"), argv[0]);
			return 1;
		case 'i':
			input_file = strdup(optarg);
			break;
		case 'o':
			output_file = strdup(optarg);
			break;
		case 'c':
			config_file = strdup(optarg);
			break;
        case 'p':
            {
                char del[]={optarg[0],'\0'};
                optarg++;
                lst[0]=strdup(strtok(optarg,del));
                for(;;){
                    char *s=strtok(NULL,del);
                    if(!s){
                        lst[i]=NULL;
                        break;
                    }
                    if(s[0])
                        lst[i++]=strdup(s);
                }
            }
            break;
		}
	}

	if (config_file == NULL)
	{
		config_file = strdup(OPENCC_DEFAULT_CONFIG_SIMP_TO_TRAD);
	}

	convert(input_file, output_file, config_file, (const char *const *)lst);

	free(input_file);
	free(output_file);
	free(config_file);

	return 0;
}
