/*      
   GOAT Games Menu System
        
   code.c

   Copyright (C)2005 Cryptic Allusion, LLC

   License: KOS/BSD

*/

// Converted from Gary's code_test2.phps

// #define TEST

#ifdef TEST

#define uint32 unsigned long

typedef struct {
	char	name[16];
	uint32	score;
} score_entry_t;

int gid;
int goat_get_game_id() { return gid; }

#else
#	include "libmenuint.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

static char digits[36] = {
	// Base 10
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	// Base 16
	'a', 'b', 'c', 'd', 'e', 'f',
	// Base 36
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
	'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

static char conv_to_digit(int i) {
	assert( i>=0 && i<36 );
	return toupper(digits[i]);
}

static int conv_fr_digit(char c) {
	int i;
	for (i=0; i<36; i++) {
		if (digits[i] == tolower(c))
			return i;
	}
	assert( 0 );
	return 0;
}

static void strrev(char * str) {
	char * tmp = strdup(str);
	int i, len = strlen(str);
	for (i=0; i<len; i++)
		str[i] = tmp[(len-1) - i];
	free(tmp);
}

static void base_convert(char * str, int oldbase, int newbase) {
	int num10 = 0, i, len = strlen(str);
	for (i=len-1; i>=0; i--) {
		int power = (len-1) - i;
		int old_digit = conv_fr_digit(str[i]);
		int j;
		for (j=0; j<power; j++)
			old_digit *= oldbase;
		num10 += old_digit;
	}
	i = 0;
	while (num10 > 0) {
		str[i++] = conv_to_digit(num10 % newbase);
		num10 = num10 / newbase;
	}
	str[i] = 0;
	strrev(str);
}

static void str_pad(char * str, int len, char w) {
	int olen = strlen(str);
	if (olen >= len)
		return;

	char *tmp = strdup(str);
	int i;
	for (i=0; i<len-olen; i++)
		str[i] = w;
	str[i] = 0;
	strcat(str, tmp);
	free(tmp);
}

static void strtoupper(char * str) {
	while (*str) {
		*str = toupper(*str);
		str++;
	}
}

char base36_reduce(char * str) {
	// Returns single digit reduction (addition) in Base36
	char checksum[50];
	strcpy(checksum, str);

	do {
		int ichecksum = 0;
		int len = strlen(checksum);
		int n;
		for (n=0; n<len; n++) {
			// Loop through each character
			int the_char = checksum[n];
				// Retrieve character
			the_char = conv_fr_digit(the_char);
				// Convert to Base10
			ichecksum += the_char;
				// Add char value to running total
		}

		sprintf(checksum, "%d", ichecksum);
		base_convert(checksum, 10, 36);
			// convert checksum to Base36
	} while(checksum[1]);

	return checksum[0];
}

void base36_offset(char * str, int offset) {
	// Function to shift Base36 number strings

	int input_length = strlen(str), ch;

	for (ch=0; ch<input_length; ch++) {
		// Loop through each character
		int the_char = conv_fr_digit(str[ch]);
		the_char += offset;
			// Offset value

		// Check for wrapping
		if (the_char < 0) {
			the_char += 36;
				// Add 36
		} else if (the_char > 35) {
			the_char -= 36;
				// Subtract 36
		}

		str[ch] = conv_to_digit(the_char);
	}
}

char * goat_make_code(score_entry_t * score) {
	// Encode Game ID
	uint32 game_id = goat_get_game_id();
	assert( game_id < 256 );
	char final_game_id[6];
	sprintf(final_game_id, "%02X", game_id);
		// Convert to Base16 from Base10
		// Ensure 2 digits are present
	strrev(final_game_id);
		// Reverse the string
	base_convert(final_game_id, 16, 36);
		// Convert to Base36 from Base16
	str_pad(final_game_id, 2, '0');
		// Ensure output is 2 chars

	// Encode initials
	char initials[4] = {0};
	strncpy(initials, score->name, 3); initials[3] = 0;
	strtoupper(initials);

	// Combine initials into one string
		// Ensure 4 digits are present
	char final_initials[5] = {
		'0',
		conv_to_digit(initials[2] - 65),
		conv_to_digit(initials[1] - 65),
		conv_to_digit(initials[0] - 65), 0
	};

	// Encode Score
	char final_score[8];
	sprintf(final_score, "%06X", score->score);
		// Convert to Base16
		// Ensure 6 digits are present
	strrev(final_score);
		// Reverse the string
	base_convert(final_score, 16, 36);
		// Convert to Base36
	str_pad(final_score, 6, '0');
		// Ensure 6 digits are present

	// Final code
	// -----------------------------------------------
	char the_code[50];
	sprintf(the_code, "%s%s%s", final_game_id, final_initials, final_score);
		// Combine the above codes into one string
	int offset1 = rand() % 35 + 1;
		// Choose random offset (1 - 35)
	int offset2 = offset1 - 3;
		// Calculate offset #2
	char the_code1[50], the_code2[50];
	strcpy(the_code1, the_code);
	base36_offset(the_code1, offset1);
		// Offset code
	strcpy(the_code2, the_code);
	base36_offset(the_code2, offset2);
		// Offset code

	// Merge codes
	// -----------------------------------------------
	char final_code[50];
	sprintf(final_code, "%d", offset1);
	base_convert(final_code, 10, 36);
		// Start code with offset #1

	int n, o = 1;
	for (n=0; n<12; n++) {
		if (n % 2) {
			final_code[o++] = the_code1[n];
				// Get next character from code #1
		} else {
			final_code[o++] = the_code2[n];
				// Get next character from code #2
		}
	}
	final_code[o] = 0;
	
	// Calculate check digit
	// -----------------------------------------------
	char the_digit = base36_reduce(final_code);
		// Call function to calculate check digit

	final_code[o++] = the_digit;
	final_code[o++] = 0;

	return strdup(final_code);
}

#ifdef TEST

int main(int argc, char **argv) {
	argc--;
	/* if (argc < 3) {
		printf("usage: codetest <gameid> <initials> <score>\n");
		return 0;
	} */

	srand(time(NULL));

	score_entry_t se;
	/* gid = strtoul(argv[1], NULL, 10);
	strcpy(se.name, argv[2]);
	se.score = strtoul(argv[3], NULL, 10); */
	gid = 5;
	strcpy(se.name, "dlp");
	se.score = 512512;

	char * foo = goat_make_code(&se);

	printf("Code is '%s'\n", foo);
	free(foo);

	return 0;
}

#endif
