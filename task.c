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
        "Формат входного файла: <id>,<level>,<str>, где <id> положительное число, обозначающее идентицикационный номер события, <level> - уровень важности события, представленный числом 1-5, <str> - описание произошедшего события, представленное строкой\n\n"
        "Options:\n"
        "\t-h\tShow this message.\n"
        "\t-a\tChoose sort algorithm (qsort | quick_sort | selection_sort)\n"
        "\t-f\tChoose sort field (id | level | str)\n"
        "\t-o\tChoose sort order (up | down)\n\n", 
        command);
}
int compare_qsort_id_up(const Event *p1, const Event *p2)
{
	return p2->id -p1->id;
}
int compare_qsort_id_down(const Event *p1, const Event *p2)
{
	return p1->id- p2->id;
}
int compare_qsort_level_up(const Event *p1, const Event *p2)
{
	return p2->level -p1->level;
}
int compare_qsort_level_down(const Event *p1, const Event *p2)
{
	return p1->level - p2->level;
}
int compare_qsort_str_up(const Event *p1, const Event *p2)
{
	/*char* str1 = p1->str;
	char*str2 = p2->str;
	while(*str1==*str2)
	{
		str1++;
		str2++;
	}
	return *str2 - *str1;*/
	return *p2->str - *p1->str;
}
int compare_qsort_str_down(const Event *p1, const Event *p2)
{
	
	/*char* str1 = p1->str;
	char*str2 = p2->str;
	while(*str1==*str2)
	{
		str1++;
		str2++;
	}
	return *str1 - *str2;*/
	return *p1->str - *p2->str;
}

void selection_sort(Event* event, int number_event, int(*compare)(const Event*, const Event*))
{
	int min;
	Event tmp;
	for(int i = 0; i<number_event-1; i++)
	{
		min = i;
		for(int j = i; j< number_event; j++)
		{
			if(compare(&event[j], &event[min])<0)
			{
				min = j;
			}
		}
		tmp = event[i];
		event[i] = event[min];
		event[min] = tmp;
	}
}

void quick_sort(Event* event, int number_events, int(*compare)(const Event*, const Event*))
{
	int i = 0;
	int j = number_events - 1;
	Event mid = event[number_events / 2];

	do
	{
		while(compare(&event[i], &mid)<0)
			i++;
		while(compare(&event[j], &mid)>0)
			j--;

		if(i <= j)
		{
			Event tmp = event[i];
			event[i] = event[j];
			event[j] = tmp;

			i++;
			j--;
		}
	} while(i<=j);

	if (j>0)
		quick_sort(event, j+1, compare);
	if ( i < number_events)
		quick_sort(&event[i], number_events - i, compare);
}
int main(int argc, char *argv[])
{
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
			{
				algo = optarg;
			}
			else
		                err = c;
			break;

	        case 'f':
        		if ((strcmp(optarg, "id") == 0) || (strcmp(optarg, "level") == 0) || (strcmp(optarg, "str") == 0))
			{
				field = optarg;
			}
			else
				err = c;
			break;

	        case 'o':
        		if ((strcmp(optarg, "up") == 0) || (strcmp(optarg, "down") == 0))
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
	if(strcmp(algo, "qsort")==0)
	{
		if(strcmp(field, "id")==0)
		{
			if(strcmp(order, "up")==0)
			{
				qsort(event, number_events, sizeof(*event), (int(*)(const void*, const void*))compare_qsort_id_up);
			}
			else
			{
				qsort(event, number_events, sizeof(*event), (int(*)(const void*, const void*))compare_qsort_id_down);
			}
		}
		else if(strcmp(field, "level")==0)
		{
			if(strcmp(order, "up")==0)
			{
				qsort(event, number_events, sizeof(*event), (int(*)(const void*, const void*))compare_qsort_level_up);
			}
			else
			{
				qsort(event, number_events, sizeof(*event), (int(*)(const void*, const void*))compare_qsort_level_down);
			}
		}
		else
		{
			if(strcmp(order, "up")==0)
			{
				qsort(event, number_events, sizeof(*event), (int(*)(const void*, const void*))compare_qsort_str_up);
			}
			else
			{
				qsort(event, number_events, sizeof(*event), (int(*)(const void*, const void*))compare_qsort_str_down);
			}
		}
	}
	else if(strcmp(algo, "quick_sort")==0)
	{	
		if(strcmp(field, "id")==0)
		{
			if(strcmp(order, "up")==0)
			{
				quick_sort(event, number_events, compare_qsort_id_up);
			}
			else
			{
				quick_sort(event, number_events, compare_qsort_id_down);
			}
		}
		else if(strcmp(field, "level")==0)
		{
			if(strcmp(order, "up")==0)
			{
				quick_sort(event, number_events, compare_qsort_level_up);
			}
			else
			{
				quick_sort(event, number_events, compare_qsort_level_down);
			}
		}
		else
		{
			if(strcmp(order, "up")==0)
			{
				quick_sort(event, number_events, compare_qsort_str_up);
			}
			else
			{
				quick_sort(event, number_events, compare_qsort_str_down);
			}
		}
	}
	else
	{	
		if(strcmp(field, "id")==0)
		{
			if(strcmp(order, "up")==0)
			{
				selection_sort(event, number_events, compare_qsort_id_up);
			}
			else
			{

				selection_sort(event, number_events, compare_qsort_id_down);
			}
		}
		else if(strcmp(field, "level")==0)
		{
			if(strcmp(order, "up")==0)
			{
				selection_sort(event, number_events, compare_qsort_level_up);
			}
			else
			{
				selection_sort(event, number_events, compare_qsort_level_down);
			}
		}
		else
		{
			if(strcmp(order, "up")==0)
			{
				selection_sort(event, number_events, compare_qsort_str_up);
			}
			else
			{
				selection_sort(event, number_events, compare_qsort_str_down);
			}
		}
	}


	for(int i = 0; i< number_events; i++)
	{
	        fprintf(file_output, "---\n");
	        fprintf(file_output, "%i\n", event[i].id);
	        fprintf(file_output, "%i\n", event[i].level);
	        fprintf(file_output, "%s\n", event[i].str);
	}
	

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

	return 0;
}
