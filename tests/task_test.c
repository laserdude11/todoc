
#include <stdbool.h>

#include "util/speedunit.h"
#include "util/dbg.h"

#include "task.h"
#include "date.h"

sp_test(append_test){
    Task* task = NULL;
    task = task_new();
    
    // Tests adding to blank task.
    task_append(task, "Items.");
    sp_assert(strcmp(task->description, "Items.") == 0, 
                        "Task description and input items are unequal.");
    
    // Tests adding to NULL pointer.
    int error = task_append(NULL, "derp.");
    // Make sure that the error is returned.
    sp_assert(error != 0, "Error code was not returned.");
    task_free(task);
    return NULL;
}

sp_test(dump_test){
    Task *task = task_new();
    task_append(task, "x (A) 2013-09-25 Testing task.");
    char* dump = (char*)task_dump(task);
		debug("taskdump = '%s'", dump);
    sp_assert(strcmp("x (A) 2013-09-25 Testing task.", dump) == 0, 
                        "Dumped task is not the same as given task information.");
    task_free(task);
    return NULL;
}

sp_test(keyword_test){
    Task *task = task_new();
    task_append(task, "Testing search.");
    int has = task_has_keyword(task, "search");
    sp_assert(has != false, "Search did not find the word.");
    task_free(task);
    return NULL;
}

sp_test(complete_test){
    Task * task = task_new();
    task_append(task, "Testing completion");
    task_set_complete(task, true);
    debug("Task = '%s'", task_dump(task));
    sp_assert(strcmp(task_dump(task), "x Testing completion") == 0, 
            "Task completion did not work");
    task_free(task);
    return NULL;
}

sp_test(parse_test){
    // Stupid proof!
    sp_assert(task_parse(NULL, "whoops") == 1, "Task is blank and did not fail");
    Task* t = task_new();
    sp_assert(task_parse(t, NULL) == 1, "Parsed text is null and did not fail");
    
    // Time to actually test parsing.
    char* wellformed = "This is a simple task with nothing on it.";
    task_parse(t, wellformed);
    debug("well formed task: %s", t->description);
    sp_streql(t->description, wellformed, "Basic description did not pass");
    
    // Priority testing. Priority is always the first 4 characters, 
    // like this:
    // '(A) '
    // "A" can be any uppercase char from A to Z.
    char* wellformedwithprio = "(A) a well formed task with priority.";
    char* wfwpminusprio = "a well formed task with priority.";
    task_clear(t);
    task_parse(t, wellformedwithprio);
    debug("task: '%s'", t->description);
    sp_streql(t->description, wfwpminusprio, 
            "Description with priority did not pass");

    char* horriblydeformed[] = {"(A )", "(a) ", "(A)task", NULL};
    int i = 0;
    char* current = horriblydeformed[0];
    do {
        task_parse(t, current);
        sp_streql(t->description, current, 
                "Parser accepted obviously bad priority input");
        i++;
        current = horriblydeformed[i];
    }while (current != 0);
    
    /** Completion testing.
     * Tasks are only complete if they begin with 'x '.
     */
    char *legit_complete = "x I'm complete!";
    task_clear(t);
    task_parse(t, legit_complete);
    sp_assert(t->complete == true, "Parser test for completion failed");
    sp_streql(t->description, "I'm complete!", "complete marker was not removed");

    /// Date testing.
    task_clear(t);
    char *t_with_start_date = "2014-10-12 This task has a start date.";
    task_parse(t, t_with_start_date);
    date *d = date_new(2014, 10, 12);
    sp_assert(date_cmp(t->date_started, d) == 0, "Created date is not correct");

    
    task_clear(t);
    char *t_with_completion_date = "x 2014-10-12 This task has a completion date.";
    task_parse(t, t_with_completion_date);
    sp_assert(date_cmp(t->date_completed, d) == 0, "Created date is not correct");

    task_clear(t);
    char *t_with_completion_and_start_date = "x 2014-10-12 2014-08-11 This task has a completion date";
    date *ds = date_new(2014, 8, 11);
    task_parse(t, t_with_completion_and_start_date);
    sp_assert(date_cmp(t->date_completed, d) == 0, "Created date is not correct");
    sp_assert(date_cmp(t->date_started, ds) == 0, "Created date is not correct");
    
    task_free(t);
    return NULL;
}

sp_test(task_compare_test){
    Task* done = task_new();
    Task* not_done = task_new();
    task_set_complete(done, true);
    task_set_complete(not_done, false);
    int comp = task_default_compare(done, not_done);
    sp_assert(comp > 0, "Comparison of done vs not done inaccurate");
    return NULL;
}

sp_suite(task_fixture){
    sp_run_test(append_test);
    sp_run_test(dump_test);
    sp_run_test(keyword_test);
    sp_run_test(complete_test);
    sp_run_test(task_compare_test);
    sp_run_test(parse_test);
    return NULL;
}
