#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <linux/input.h>
#include <limits.h>
#include <string.h>

/*
    Macro that defines name of file to read keyboard events.
    Distinguishes aarch64 and other architectures in order
    to allow program run on TAs Virtual Machines and my local one.
*/
#ifdef __aarch64__
#define KBD_EVENTS_FILENAME "/dev/input/by-path/pci-0000:00:04.0-usb-0:3:1.0-event-kbd"
#else
#define KBD_EVENTS_FILENAME "/dev/input/by-path/platform-i8042-serio-0-event-kbd"
#endif

/* Define for convenient output event's data */
#define EVENT_FORMAT "%s 0x%x (%u)\n"

#define SHORTCUT_KEYS_COUNT (USHRT_MAX + 1)
#define SHORTCUT_MAX_SET_SIZE  100
#define SHORTCUT_MAX_DESC_SIZE 100

/* Function to execute exit shortcut, when it is trigerred */
void exit_shortcut() {
    exit(EXIT_SUCCESS);                            
}

/* Function to execute "Pass exam" shortcut, when it is trigerred */
void passed_exam_shortcut() {
    printf("I passed the Exam!\n");                                 
}

/* Function to execute "Cappuccino" shortcut, when it is trigerred */
void cappuccino_shortcut() {
    printf("Get some cappuccino!\n");
}

/* Function to execute custom author's shortcut, when it is trigerred */
void custom_shortcut() {
    printf("My tg channel is https://t.me/eternal_th0ughts\n");
}

/* Function to check whether occurred event is key press */
bool is_keypress(struct input_event event) {
    return event.type == EV_KEY && event.value == 1; 
}

/* Function to check whether occurred event is key release */
bool is_keyrelease(struct input_event event) {
    return event.type == EV_KEY && event.value == 0;
}

/* Function to check whether occurred event is key repeat */
bool is_keyrepeat(struct input_event event) {
    return event.type == EV_KEY && event.value == 2;
}


/* Function to log necessary events */
void log_event(struct input_event event) {
   if (is_keypress(event)) {
       printf(EVENT_FORMAT, "PRESSED", event.code, event.code);                 
   } else if (is_keyrepeat(event)) {                                
       printf(EVENT_FORMAT, "REPEATED", event.code, event.code);               
   } else if (is_keyrelease(event)) {
       printf(EVENT_FORMAT, "RELEASED", event.code, event.code);              
   }
}

/** Description of shortcut type */
typedef struct {
    bool keys[SHORTCUT_KEYS_COUNT];
    char description[SHORTCUT_MAX_DESC_SIZE];
    char keys_description[SHORTCUT_MAX_DESC_SIZE];
    void (*action)();
} shortcut_t;

/** Constructor for shortcut */
void shortcut_new(shortcut_t* instance, const char* description, const char* keys_description) {
    for (size_t idx = 0; idx < SHORTCUT_KEYS_COUNT; ++idx) {
        instance->keys[idx] = false;
    }
    instance->action = NULL;
    strcpy(instance->description, description);
    strcpy(instance->keys_description, keys_description);
}

/** Add key to shortcut */
void shortcut_add_key(shortcut_t* shortcut, unsigned short key) {
    shortcut->keys[key] = true;
}

/** Add action for shortcut */
void shortcut_add_action(shortcut_t* shortcut, void (*action)()) {
    shortcut->action = action;
}

/** Execute action for shortcut */
void shortcut_run_action(shortcut_t* shortcut) {
    (*shortcut->action)();
}

/** Print shortcut's desription */
void shortcut_print_description(shortcut_t* shortcut) {
    printf("Shortcut [ %s ] --- %s\n", 
            shortcut->keys_description, shortcut->description);
}

/** Description of shortcuts set */
typedef struct {
    shortcut_t* shortcuts_set[SHORTCUT_MAX_SET_SIZE];
    size_t size;
} shortcuts_set_t;

/** Constructor for shortcuts set */
void shortcuts_set_new(shortcuts_set_t* instance) {
    instance->size = 0;
    for (size_t idx = 0; idx < SHORTCUT_MAX_SET_SIZE; ++idx) {
        instance->shortcuts_set[idx] = NULL;
    }
}

/** Add shortcut to set */
void shortcuts_set_add_shortcut(shortcuts_set_t* set, shortcut_t* shortcut) {
    set->shortcuts_set[set->size++] = shortcut;
}

/** Keyboard manager description */
typedef struct {
    bool is_pressed[SHORTCUT_KEYS_COUNT];
    shortcuts_set_t shortcuts;
} keyboard_manager_t;

/** Keyboard maanger constructor */
void keyboard_manager_new(keyboard_manager_t* instance) {
    shortcuts_set_new(&instance->shortcuts);
    for (size_t idx = 0; idx < SHORTCUT_KEYS_COUNT; ++idx) {
        instance->is_pressed[idx] = false;
    }
}

/** Method of keyboard manager to process event */
void keyboard_manager_process_event(keyboard_manager_t* manager, struct input_event event) {
   if (is_keypress(event)) {
       manager->is_pressed[event.code] = true;   

   } else if (is_keyrelease(event)) {
       manager->is_pressed[event.code] = false;                                          
   }
}

/** Method of keyboard manager to check whether the shortcut is active */
bool keyboard_manager_is_active_shortcut(keyboard_manager_t* manager, shortcut_t* shortcut) {
    for (size_t idx = 0; idx < SHORTCUT_KEYS_COUNT; ++idx) {
        if (manager->is_pressed[idx] && !shortcut->keys[idx]) {
            return false;
        } else if (!manager->is_pressed[idx] && shortcut->keys[idx]) {
            return false;
        }
    }
    return true;
}

/** Check all the shortcuts in manager */
void keyboard_manager_check_shortcuts(keyboard_manager_t* manager) {
    for (size_t idx = 0; idx < manager->shortcuts.size; ++idx) {
        if (keyboard_manager_is_active_shortcut(manager, manager->shortcuts.shortcuts_set[idx])) {
            shortcut_run_action(manager->shortcuts.shortcuts_set[idx]);
        }
    }
}

/** Print description of all shortcuts in manager */
void keyboard_manager_print_shortcuts(keyboard_manager_t* manager) {
    for (size_t idx = 0; idx < manager->shortcuts.size; ++idx) {
        shortcut_print_description(manager->shortcuts.shortcuts_set[idx]);
    }
}

/** Listen and process all the events */
void keyboard_manager_listen_events(keyboard_manager_t* manager) {
    keyboard_manager_print_shortcuts(manager);
    FILE* events = fopen(KBD_EVENTS_FILENAME, "r");                                   
    for (;;) {                                                                  
       struct input_event event;
       fread(&event, sizeof(event), 1, events);                                

       log_event(event);
       keyboard_manager_process_event(manager, event);
       keyboard_manager_check_shortcuts(manager);
    }
}

int main() {
    // Create shortcuts
    shortcut_t exam, exit_shrt, cappuccino, custom;
    
    // Construct shortcuts
    shortcut_new(&exam, "allows easily pass exam", "E + P");
    shortcut_new(&exit_shrt, "terminates the program", "E + X");
    shortcut_new(&cappuccino, "allows to take a cup of coffee", "C + A + P");
    shortcut_new(&custom, "allows to get problem solver's telegram channel", "M + 0 + T + 9");

    // Add keys and action to exam shortcut
    shortcut_add_key(&exam, KEY_E);
    shortcut_add_key(&exam, KEY_P);
    shortcut_add_action(&exam, &passed_exam_shortcut);

    // Add keys and action to exit shortcut
    shortcut_add_key(&exit_shrt, KEY_E);
    shortcut_add_key(&exit_shrt, KEY_X);
    shortcut_add_action(&exit_shrt, &exit_shortcut);

    // Add keys and action to cappuccino shortcut
    shortcut_add_key(&cappuccino, KEY_C);
    shortcut_add_key(&cappuccino, KEY_A);
    shortcut_add_key(&cappuccino, KEY_P);
    shortcut_add_action(&cappuccino, &cappuccino_shortcut);

    // Add keys and action to custom shortcut
    shortcut_add_key(&custom, KEY_M);
    shortcut_add_key(&custom, KEY_0);
    shortcut_add_key(&custom, KEY_T);
    shortcut_add_key(&custom, KEY_9);
    shortcut_add_action(&custom, &custom_shortcut);

    // Initialize keyboard manager
    keyboard_manager_t manager;
    keyboard_manager_new(&manager);

    // Add shortcuts to keyboard
    shortcuts_set_add_shortcut(&manager.shortcuts, &exam);
    shortcuts_set_add_shortcut(&manager.shortcuts, &exit_shrt);
    shortcuts_set_add_shortcut(&manager.shortcuts, &cappuccino);
    shortcuts_set_add_shortcut(&manager.shortcuts, &custom);

    // Start kb listening
    keyboard_manager_listen_events(&manager);
    return 0;
}
