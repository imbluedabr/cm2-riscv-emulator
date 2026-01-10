#ifndef MEMORYMAP_H
#define MEMORYMAP_H

// Note that the post interface is just a indicator to the user,
// you can decide how to encode the fail cause.
#define POST_SUCCESS 2
#define POST_FAILURE 1
#define POST_RESULT     (0xFFF8) // Power-On Self Test Result (0 = pass, 1 = fail)
#define POST_FAIL_CAUSE (0xFFF7) // Power-On Self Test Fail Cause

// TTY Console
#define TTY_LOC     (0xFFF6) // TTY Console Location
#define TTY_CHAR    (0xFFF5) // TTY Console Location
#define TTY_WRITE   (0xFFF4) // Write to TTY Console
#define TTY_CLEAR   (0xFFF3) // Clear TTY Console

// User Input
#define USER_READY  (0xFFF2) // User Input Ready Flag !!! READING RESETS THIS TO 0 IF IT'S 1 !!!
#define USER_ASCII  (0xFFF1) // User Input ASCII Character

// Memory Mapped Math Units
#define MULT_X      (0xFFF0) // Divider X Input
#define MULT_Y      (0xFFEC) // Divider Y Input
#define MULT_LO     (0xFFE8) // Divider Result Output LOW 32 bits
#define MULT_HI     (0xFFE4) // Divider Result Output HI 32 bits

#define DIV_X      (0xFFE0) // Divider X Input
#define DIV_Y      (0xFFDC) // Divider Y Input
#define DIV_QUO    (0xFFD8) // Divider Quotient Output
#define DIV_REM    (0xFFD4) // Divider Remainder Output

#define RANDOM_BYTE (0xFFD3) // RNG 0-255

#define RGB_X (0xFFD2) 
#define RGB_Y (0xFFD1) 
#define RGB_R (0xFFD0) 
#define RGB_G (0xFFCF) 
#define RGB_B (0xFFCE) 
#define RGB_CLEARSCREEN (0xFFCD) 
#define RGB_WRITE (0xFFCC)


#define LED_LIGHT (0xFFCB)

#define DISK_ADDRESS (0xFFC8)
#define DISK_DATA (0xFFC6)
#define DISK_OUT (0xFFC4)
#define DISK_WRITE (0xFFC3)

#endif // MEMORYMAP_H