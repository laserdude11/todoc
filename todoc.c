// Todo.c: the main file. 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include "task.h"
#include "tasklist.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_BUILD 3

int add_task(char* filename, char* string){
    tasklist* list = tasklist_new();
    Task* task = task_new();
    
    FILE* file = fopen(filename, "r");
    if (file == NULL){
        file = fopen(filename, "w+");
        if (file == NULL){
            perror("Add task");
            goto error;
        }
    }

    if (tasklist_read(list, file) != 0){
        puts("ERROR: could not read tasklist.");
        goto error;
    }
    
    task_append(task, string);   
    dllist_append(list, task);

    freopen(filename, "r+", file);
    if (tasklist_dump(list, file) != 0){
        puts("ERROR: could not write tasklist.");
        goto error;
    }
    
    printf("Added: %s\n", string);
    tasklist_destroy(list);
    return 0;

error:
    tasklist_destroy(list);
    return 1;
}


/** List the tasks in the file given.
 */
int list_tasks(char* filename){
    // Open up the file, bro.

    FILE* file = fopen(filename, "r");
    if (file == NULL){
        perror("Could not open file");
        errno = 0;
        return -1;
    }
    
    tasklist* list = tasklist_new();
    // TODO: Error handling for reading file.
    tasklist_read(list, file);
    fclose(file);
    tasklist_display(list);
    // TODO: Display the amount of tasks.
    printf("---\n");
    tasklist_destroy(list);
    return 0;
}

/** List the tasks with a match in the string.
 *
 */
void list_tasks_matching(char* filename, char* string){
    if (string == NULL) string = "";

    FILE* file = fopen(filename, "r");
    if (file == NULL){
        perror("Could not open file");
        errno = 0;
        return;
    }

	tasklist* list = tasklist_new();
    tasklist_read(list, file);
    tasklist* matches = tasklist_search(list, string);
    int count = tasklist_display(matches);
    if (count == 0){
        puts("No matches.");
    } else {
        printf("Number of matches: %d.\n", count);
    }
    fclose(file);
    // Make sure we don't destroy the data.
    tasklist_free(matches);
    tasklist_destroy(list);
}

// Removes a task given the file and the index.
int remove_task(char* filename, int number){
    
    FILE* file = fopen(filename, "r");
    if (file == NULL){
        perror("Cannot remove task");
        errno = 0;
        goto error;
    }

    // Open up the tasklist.
    tasklist* list = tasklist_new();
    if (tasklist_read(list, file) != 0){
        puts("ERROR: could not read tasklist.");
        goto error;
    }

    fclose(file);
    fopen(filename, "w");
    Task* t = tasklist_get(list, number-1);
    if (t == NULL){
        puts("There is no task with that number.");
        goto error;
    }

    printf("Are you sure you want to remove %d: %s? (y/n)\n", number, task_dump(t));
    char answer[5];
    char* ans = answer;
    fgets(ans, 5, stdin);
    if (strcmp(answer, "y\n") == 0){
        tasklist_remove(list, number-1);
        if (tasklist_dump(list, file) != 0){
            puts("ERROR: could not write tasklist.");
        }
    }
    printf("Task %d deleted.\n", number);
    task_free(t);
    tasklist_free(list);
    error:
        fclose(file);
        return 1;
}

/** Complete a task.
 *
 */
void set_complete_task(char* filename, int number, bool comp)
{    
    tasklist* list = tasklist_new();
    FILE* file = fopen(filename, "r");
    tasklist_read(list, file);
    fclose(file);

    Task* task = tasklist_get(list, number-1);
    task_set_complete(task, comp);

    file = fopen(filename, "w");
    tasklist_dump(list, file);
    fclose(file);
    if (comp == true){
        printf("Marked task '%s' (%d) as complete.\n", task->description, number);
    } else {
        printf("Marked task '%s' (%d) as incomplete.\n", task->description, number); 
    }
    tasklist_free(list);
}

// Help functions.
void print_version(){
    printf("todoc version %d.%d.%d by Ben Davenport-Ray and contributors.\n", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
}

void print_short_help(){ 
    puts("todoc [options] <command> [number] [description]");
}

void print_help(){
    print_short_help();
    puts("Manage tasks in a todo.txt file.\n");
    puts("Options:\n"
         " -f file \t\tSet the file used.\n"
         " -h \t\t\tShow the extended help.\n\n"
         "Commands:\n"
         " -a|add 'task'\t\tadds the task to the list\n"
         " -d|do index\t\tmarks the task at the index as done\n"
         " -u|undo index\t\tmarks the task at the index as incomplete\n"
         " -r|remove index\tremoves the task at the index\n"
         " -s|search 'query'\tsearches for the text in the tasklist\n"
         " -l|list\t\tshows all the tasks\n");
}

bool strings_equal(char* subject, char* first, char* sec){
    if(strcmp(subject, first) == 0 ||
       strcmp(subject, sec) == 0){
        return true;
    } else {
        return false;
    }
}

int main(int argc, char* argv[]){

    char* taskfilename = "todo.txt";
//    char* donefile = "done.txt";

    //bool verbose = false;

    int status = EXIT_SUCCESS;

    // parse the command line arguments.
    if (argc < 2){
        print_short_help();
        exit(status);
    }
    int i;
		//start the loop at 1 because argv[0] refers to the binary file
    for (i = 1; i < argc; i++){
        // Add a new task.
        if (strings_equal(argv[i], "add", "-a")){
            if (add_task(taskfilename, argv[i+1]) != 0){
                status = EXIT_FAILURE;   // We done goofed.
            }
            i++;  // Skip the next argument.
            continue;
        }

        // Remove a task.
        if (strings_equal(argv[i], "rm", "-r")){
            int index = atoi(argv[i+1]);
            if (remove_task(taskfilename, index) != 0){
                status = EXIT_FAILURE;
            }
            i++;
            continue;
        }

        // Complete a task.
        if (strings_equal(argv[i], "do", "-d")){
            int index = atoi(argv[i+1]);
            set_complete_task(taskfilename, index, true);
            i++;
            continue;
        }

        // Uncomplete a task.
        if (strings_equal(argv[i], "undo", "-u")){
            int index = atoi(argv[i+1]);
            set_complete_task(taskfilename, index, false);
            i++;
            continue;
        }
        
        // List the tasks matching the string in the file.
        if (strings_equal(argv[i], "search", "-s")){
            list_tasks_matching(taskfilename, argv[i+1]);
            i++;
            continue;
        }
        
        // Show all the tasks in the file.
        if(strings_equal(argv[i], "ls", "-l")){
            list_tasks(taskfilename);
            continue;
        }
        
        // Show the help dialog.
        if (strcmp(argv[i], "-h") == 0){
            print_help();
        }

        // Set the taskfilename flag.
        if (strcmp(argv[i], "-f") == 0){
            taskfilename = argv[i+1]; 
            i++;
            continue;
        }

        // Set the verbosity flag.
        if (strcmp(argv[i], "-v") == 0){
            // verbose = true;
        }

    }

    return status;
}
