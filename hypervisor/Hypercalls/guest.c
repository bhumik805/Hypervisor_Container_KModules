#include <stddef.h>
#include <stdint.h>

uint32_t exit_count = 0;
static void outb(uint16_t port, uint8_t value)
{
	asm("outb %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}
static void inb(uint16_t port, uint8_t *value)
{
     asm volatile("in %1, %0" : "=a"(*value) : "Nd"(port));
}
void outl(uint16_t port, uint32_t val)
{
	asm volatile("outl %0, %1" : /* No output */ : "a"(val), "Nd"(port));
}
void HC_print8bit(uint8_t val)
{
	outb(0xE9, val);
}


void HC_print32bit(uint32_t val)
{
	outl(0xEA, val);
	// Fill in here
	// val++;		// Remove this
}

uint32_t HC_numExits()
{
	// here access the shared memory exit count
	uint32_t value;
	inb(0xEC, &value);
	return value;

}

void HC_printStr(char *str)
{
	// u`` // Remove this
}

char *HC_numExitsByType()
{
	// Fill in here

	
}

uint32_t HC_gvaToHva(uint32_t gva)
{
	// Fill in here
	gva++;	  // Remove this
	return 0; // Remove this
}

void
	__attribute__((noreturn))
	__attribute__((section(".start")))
	_start(void)
{
	const char *p;

	for (p = "Hello 695!\n"; *p; ++p)
		HC_print8bit(*p);

	/*----------Don't modify this section. We will use grading script---------*/
	/*---Your submission will fail the testcases if you modify this section---*/
	HC_print32bit(2048);
	HC_print32bit(4294967295);

	uint32_t num_exits_a, num_exits_b;
	num_exits_a = HC_numExits();

	char *str = "CS695 Assignment 2\n";
	HC_printStr(str);

	num_exits_b = HC_numExits();

	HC_print32bit(num_exits_a);
	HC_print32bit(num_exits_b);

	char *firststr = HC_numExitsByType();
	uint32_t hva;
	hva = HC_gvaToHva(1024);
	HC_print32bit(hva);
	hva = HC_gvaToHva(4294967295);
	HC_print32bit(hva);
	char *secondstr = HC_numExitsByType();

	HC_printStr(firststr);
	HC_printStr(secondstr);
	/*------------------------------------------------------------------------*/

	*(long *)0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a"(42) : "memory");
}
