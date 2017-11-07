#include "afi.h"
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv) {
	afi_Dict *dict = afi_newDict();
	afi_Entry *entry = afi_defEntry("TEST", NULL, 0);
	dict = afi_addEntry(dict, entry);
	dict = afi_addEntry(dict, afi_defEntry("TEST2", NULL, 0));
	afi_Entry *found_entry = afi_find(dict, "TEST");
	assert(entry == found_entry);
	puts(found_entry->name);
	return 0;
}
