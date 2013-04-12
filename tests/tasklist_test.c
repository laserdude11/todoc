#include "task.h"
#include "tasklist.h"
#include "seatest.h"

#include <string.h>

Tasklist* make_three_item_list(){
    Tasklist *list = tasklist_new();
    Task *first, *second, *third;
    first = task_new();
    second = task_new();
    third = task_new();
    task_append(first, "This is the first task. #first");
    task_append(second, "This is the second task. #notfirst");
    task_append(third, "This is the third task. #notfirst");
    tasklist_append(list, first);
    tasklist_append(list, second);
    tasklist_append(list, third);
    return list;
}

Tasklist* make_single_item_list(){
    Tasklist *list = tasklist_new();
    Task *newtask = task_new();
    task_append(newtask, "Something important I need to do.");
    tasklist_append(list, newtask);
    return list;
}

void tlist_append_test(){
    // Basic setup.
    Tasklist* list = make_single_item_list();
    /* Is the task in the correct spot? */
    assert_true(list->task != NULL);
    /* Is it the right task? */
    assert_string_equal(list->task->description, "Something important I need to do."); 

    /* Add another task. */
    Task* secondary = task_new();
    task_append(secondary, "A second thing.");
    tasklist_append(list, secondary);
    assert_true(list->next != NULL && list->next->task != NULL);
    assert_string_equal(list->next->task->description, "A second thing.");
    tasklist_free(list);
}

void tlist_get_test(){
    Tasklist* list = make_three_item_list();
    Task* secondItem = tasklist_get(list, 1);
    assert_string_equal(task_dump(secondItem), "This is the second task. #notfirst");
    tasklist_free(list);
}

void tlist_search_test(){
    Tasklist* list = make_three_item_list();
    Tasklist* notfirst = tasklist_search(list, "notfirst");
    assert_true(notfirst != NULL);
    Tasklist *index = notfirst;
    while (index != NULL){
        assert_true(strstr(index->task->description, "notfirst") != NULL);
        //assert_fail(strstr(index->task->description, "notfirst") == NULL);
        index = index->next;
    }
    tasklist_free(list);
}

void tlist_remove_test(){
    Tasklist* list = make_three_item_list();
    Task* task = tasklist_remove(list, 1);
    assert_string_equal("This is the second task. #notfirst", task->description);
    assert_string_equal("This is the third task. #notfirst", task_dump(tasklist_get(list, 1)));
    assert_string_equal("This is the first task. #first", task_dump(tasklist_remove(list, 0)));
}

void tlist_load_test(){
    Tasklist *list = tasklist_new();
    FILE* f = fopen("tests/testtodo.txt", "r");
    tasklist_read(list, f);    
    assert_string_equal(list->task->description, "A test task in a file.\n");
}

void tasklist_fixture(){
    test_fixture_start();
    run_test(tlist_append_test);
    run_test(tlist_get_test);
    run_test(tlist_remove_test);
    run_test(tlist_search_test);
    run_test(tlist_load_test);
    test_fixture_end();   
}
