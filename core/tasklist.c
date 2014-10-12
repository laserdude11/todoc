/* tasklist.c
 * Methods that perform operations on tasklists.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "task.h"
#include "tasklist.h"
#include "util/dbg.h"

// Make a new tasklist.
Tasklist* tasklist_new(){
    Tasklist* this = (Tasklist*)malloc(sizeof(Tasklist));
    this->list = List_new();
    return this;
}

void destroytask(void* item){
    Task* t = (Task*)item;
    task_free(t);    
}

// Destroy the given tasklist.
void tasklist_destroy(Tasklist* list){
    if (list == NULL) return;
    List_do(list->list, &destroytask);
    tasklist_free(list);
}

void tasklist_free(Tasklist* list){
    List_free(list->list);
    free(list);
}

// Add a new task to the end of this list.
int tasklist_append(Tasklist* this, Task* t){
    if (this == NULL){
        // Make sure it's heard.
        return 1;
    }
    List_append(this->list, t);
    return 0;
}

// Get the task at the index.
Task* tasklist_get(Tasklist* list, int index){
    Task* t = (Task*)List_get(list->list, index);
    return t;
}

// Remove the task at the index.
Task* tasklist_remove(Tasklist* list, int index){
    return ( (Task*)List_remove(list->list, index) );
}

// Construct a tasklist of tasks that match the filter.
Tasklist* tasklist_search(Tasklist* list, char* filter){

    bool filterfunc(void* v){
				Task* tmptsk = (Task*)v;
				char* str = (char*)tmptsk->description;
        if (strstr(str, filter) != NULL) return true;
        else return false;
    }
    
    List* matches = List_filter(list->list, *filterfunc);
    Tasklist* t = tasklist_new();
    t->list = matches;
    return t;
}

// Print out a tasklist to the console.
int tasklist_display(Tasklist* list){
    List* sorted = List_sort(list->list, &task_default_compare);
    int count = 0;
    void print_task(void* item){
        Task* t = (Task*)item;
        task_show(t);
        count ++;
    }
    List_do(sorted, &print_task);
    return count;
}

// Write a tasklist to a file.
int tasklist_dump(Tasklist *list, FILE* f){
    void d(void* item){
        Task* t = (Task*)item;
        fprintf(f, "%s\n", task_dump(t));
    }
    List_do(list->list, &d);
    return 0;
}

// Read a tasklist from a file.
int tasklist_read(Tasklist *list, FILE* f){
    check(list, "Task list does not exist.");
    check(f, "Something wrong happened with the file.");

    // Go through the file one line at a time.
    char buffer[256];
    int lineno = 0;
    while (fgets(buffer, 255, f) != NULL){
        lineno ++; 
        // Remove the ending newline.
        char* start = buffer;
        int newline = strcspn(buffer, "\n");
        start[newline] = '\0';

        // Make a task to hold the line.
        Task* temp = task_new();
        task_set_lineno(temp, lineno);
        task_parse(temp, buffer);

        // Add it to the list.
        tasklist_append(list, temp);
    }

    return 0;

error:
    return -1;

}
