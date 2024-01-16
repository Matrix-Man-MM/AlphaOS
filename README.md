# AlphaOS #
The Alpha Operating System.

# Features #
So far, all this OS does is print the name of the operating system to the screen as well as some initialization messages. It also loads a GDT (Global Descriptor Table). AlphaOS now loads an IDT (Interrupt Descriptor Table), we're really moving up in the OSDev world. Speaking of moving up, ISRs (Interrupt Service Routines) have now been initialized. Now, the IRQ system is in place, so AlphaOS can now handle interrupts if needed. The OS's timer has now been implemented. This thing now has basic keyboard support, so there's that. And you know the "printf" function from the C standard library? Well...that exists here now. AlphaOS now retrieves Multiboot header information and prints it to the screen. Paging exists now. The shift, ctrl, and alt keys are now functional. Capital letters and special characters can now be typed. AlphaOS now supports heap allocation.

# Why? #
I want to have a large project that I can work on when I'm bored.

# PROGRESS #
[x] VGA
[x] GDT
[x] IDT
[x] ISRs
[x] IRQ
[x] TIMER
[x] KEYBOARD
[x] PRINTF
[x] MULTIBOOT
[x] PAGING
[x] HEAP ALLOCATION
