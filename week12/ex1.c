#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <linux/input.h>
#include <limits.h>

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

/* An array that stores whether key with certain code is pressed now */
bool is_pressed[USHRT_MAX + 1];
/* A file to read keyboard events and file to log them */
FILE* events;

/* Function to execute exit shortcut, when it is trigerred */
void exit_shortcut() {
    if (is_pressed[KEY_E] && is_pressed[KEY_X]) {
        fclose(events);
        // Stop the execution
        exit(EXIT_SUCCESS);                            
    }
}

/* Function to execute "Pass exam" shortcut, when it is trigerred */
void passed_exam_shortcut() {
    if (is_pressed[KEY_P] && is_pressed[KEY_E]) {
        printf("I passed the Exam!\n");                                 
    }
}

/* Function to execute "Cappuccino" shortcut, when it is trigerred */
void cappuccino_shortcut() {
    if (is_pressed[KEY_C] && is_pressed[KEY_A] && is_pressed[KEY_P]) {
        printf("Get some cappuccino!\n");
    }
}

/* Function to execute custom author's shortcut, when it is trigerred */
void custom_shortcut() {
    if (is_pressed[KEY_M] && is_pressed[KEY_0] 
            && is_pressed[KEY_T] && is_pressed[KEY_9]) {
        printf("Shortcut by m0t9_ is pressed\n");
    }
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

/* Function to check the occurrance of all the shortcuts */
void check_shortcuts() {
    exit_shortcut();
    passed_exam_shortcut();
    cappuccino_shortcut();
    custom_shortcut();
}

/* Function to print the description of all the shortcuts */
void shortcuts_desc() {
    printf("[ SHORTCUTS DESCRIPTION ]\n");
    printf("Shortcut [ E + X ] to exit a program\n");
    printf("Shortcut [ E + P ] to pass the exam\n");
    printf("Shortcut [ C + A + P ] to get a cappuccino\n");
    printf("Shortcut [ M + 0 + T + 9 ] to get authors handle\n");
}

/* Function to log necessary events */
void log_event(struct input_event event) {
   if (is_keypress(event)) {
       printf(EVENT_FORMAT, "PRESSED", event.code, event.code);                 // Log "pressed" event
   } else if (is_keyrepeat(event)) {                                
       printf(EVENT_FORMAT, "REPEATED", event.code, event.code);                // Log "repeated" event
   } else if (is_keyrelease(event)) {
       printf(EVENT_FORMAT, "RELEASED", event.code, event.code);                // Log "released" event
   }
}

/* Function to process occured event if it is keypress or keyrelease */
void process_event(struct input_event event) {
   if (is_keypress(event)) {
       is_pressed[event.code] = true;                                           // If key press --- mark pressed key with true
   } else if (is_keyrelease(event)) {
       is_pressed[event.code] = false;                                          // If key release --- mark released key with false
   }
}

int main() {
    events = fopen(KBD_EVENTS_FILENAME, "r");                                   // Open file with keyboard events
    shortcuts_desc();                                                           // Print shortcuts description
    for (;;) {                                                                  // Endless loop listening for keyboard events
       struct input_event event;
       fread(&event, sizeof(event), 1, events);                                 // Read keyboard event
       log_event(event);                                                        
       process_event(event);
       check_shortcuts();                                                       // Check the occurrance of the shortcuts
    }
    return 0;
}
