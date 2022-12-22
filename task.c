#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <wchar.h>


typedef struct
{
	int id;
	int level;
	char* str;
}Event;


Event event_new( int id,  int level,  char* str)
{
	Event local;
	local.id = id;
	local.level = level;
	local.str = strdup(str);
	return local;
}


void event_printf( Event local)
{
	printf("%i ", local.id);
	printf("%i ", local.level);
	printf("%s", local.str);
}


void help(char *command)
{
	printf(
        "Usage: %s [-hafo] <input_file> <output_file>\n\n"
        "Input file line format: <name>,<phone_number>,<time> where name is string, phone number is string with length less than 17 which consist of numerics and spaces (first symbol can be '+'), time is positive long integer\n\n"
        "Options:\n"
        "\t-h\tShow this message.\n"
        "\t-a\tChoose sort algorithm (qsort | name_1 | name_2)\n"
        "\t-f\tChoose sort field (model | name | mileage)\n"
        "\t-o\tChoose sort order (asc | desc)\n\n", 
        command);
}
/*void event_output(Event local)
{
	fprintf(file_output, "---\n");
	fprintf(file_output, "%i\n", local.id);
	fprintf(file_output, "%i\n", local.level);
	fprintf(file_output, "%s\n", local.str);
}*/

int main(int argc, char *argv[])
{

	//char * allow ="QqWwEeRrTtYyUuIiOoPpAaSsDdFfGgHhJjKkLlZzXxCcVvBbNnMm- ";
	char * numbers = "1234567890";

	char* str = NULL;
	size_t len = 0;
        ssize_t nread = 0;
	char* line = NULL;
	char* level_str = NULL;
	char* id_str = NULL;

	int c;
	char *algo = NULL;
	char *field = NULL;
	char *order = NULL;
        char err = '\0';
	
	Event* event = NULL;
	int number_events = 0;

	FILE * file_input;
	FILE * file_output;
	

	while (( c = getopt(argc, argv, "ha:f:o:")) != -1)
	{
        	switch (c)
	        {

        	case 'a':
	        	if ((strcmp(optarg, "qsort") == 0) || (strcmp(optarg, "quick_sort") == 0) || (strcmp(optarg, "selection_sort") == 0))
			{//str?
				algo = optarg;
			        printf("%s\n", algo);
			}
			else
		                err = c;
			break;

	        case 'f':
        		if ((strcmp(optarg, "id") == 0) || (strcmp(optarg, "level") == 0) || (strcmp(optarg, "str") == 0))
			{
				field = optarg;
				printf("%s\n", field);
			}
			else
				err = c;
			break;

	        case 'o':
        		if ((strcmp(optarg, "asc") == 0) || (strcmp(optarg, "desc") == 0))
			{
				order = optarg;
				printf("%s\n", optarg);
			}
			else
				err = c;
			break;
		case 'h':
			help(argv[0]);
			return 0;

 		case '?':
			printf("Unknown option: %c\n", optopt);
			break;
		case ':':
			printf("Missing arg for %c\n", optopt);

		}
	}

	if (err)
	{
        	if (err != '?')
	        	printf("Invalid value for -%c\n", err);
        	return 1;
	}


	char * input_filename = argv[optind];
	char * output_filename = argv[optind + 1];


	if ((input_filename == NULL) || (output_filename == NULL))
	{
		help(argv[0]);
		perror("Missed filename");
		return 1;
	}

	if ((file_input = fopen(input_filename, "r")) == NULL)
	{
		fprintf(stderr, "Unable to open file %s to read\n", input_filename);
		return 2;
	}

	if ((file_output = fopen(output_filename, "w")) == NULL)
	{
		fclose(file_input);
		fprintf(stderr, "Unable to open file %s to write\n", output_filename);
		return 2;
	}


        for(int i = 0; nread != -1; i++) 
	{
		nread = getline( &line, &len, file_input);
		if( strcmp(line, "---\n") != 0 )
		{
			continue;
		}
		

		nread = getline( &id_str, &len, file_input);
		if ( nread == -1 )
		{
			fprintf(stderr, "End of file or error \n\n");
			continue;
		}
		if ( strspn(id_str, numbers) != nread - 1)
		{
			fprintf(stderr, "Неправильный id\n\n");
			continue;
		}
		if(atoi(id_str)<0)
		{
			fprintf(stderr, "id не соответствует стандарту\n\n");
			continue;
		}
		int id = atoi(id_str);
		

		nread = getline( &level_str, &len, file_input);
		if ( nread == -1 )
		{
			fprintf(stderr, "End of file or error \n\n");
			continue;
		}
		if((nread-1 != 1)||(atoi(level_str)<=0)||(atoi(level_str)>5))
		{
			fprintf(stderr, "Уровень важности не соотвествует существующим\n\n");
			continue;
		}
		int level = atoi(level_str);

			
		nread = getline( &line, &len, file_input);
		if ( nread == -1 )
		{
			fprintf(stderr, "End of file or error \n\n");
			continue;
		}
		
		number_events++;
		event = realloc(event, number_events * sizeof(Event));
		event[number_events - 1] = event_new(id, level, line);

        }

	
	for(int i =0; i < number_events; i++)
	{
		event_printf(event[i]);
		printf("\n");
	}

	for(int i = 0; i< number_events; i++)
	{
		//event_output(event[i]);
	        fprintf(file_output, "---\n");
	        fprintf(file_output, "%i\n", event[i].id);
	        fprintf(file_output, "%i\n", event[i].level);
	        fprintf(file_output, "%s\n", event[i].str);
	}
	

	//free CARS
	for(int i =0; i < number_events; i++)
	{
		free(event[i].str);
	}
	free(event);

	fclose(file_input);
	fclose(file_output);
	free(id_str);
	free(level_str);
	free(line);

	return 1;
}
