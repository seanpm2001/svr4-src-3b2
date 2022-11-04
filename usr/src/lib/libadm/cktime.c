/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*LINTLIBRARY*/
#ident	"@(#)libadm:cktime.c	1.1"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern int	ckquit;

extern long	strtol();
extern void	putprmpt(),
		puterror(),
		puthelp();
extern int	getinput(),
		puttext();

#define PROMPT	"Enter the time of day"
#define ERRMSG	"Please enter the time day. Format is <%s>."
#define DEFAULT	"%H:%M"

#define TLEN 3
#define LH 00
#define UH 23
#define USH 12
#define LM 00
#define UM 59
#define LS 00
#define US 59
#define DELIM1 ':'
#define BLANK ' '

static char *
p_ndig(string, value)
char *string;
int *value;
{
	char *ptr;
	int accum = 0;
	int n = 2;

	if(!string) return (0);
	for (ptr = string; *ptr && n>0; n--, ptr++) {
		if(! isdigit(*ptr)) return (NULL);
		accum = (10 * accum) + (*ptr - '0');
		}
	if(n) return(NULL);
	*value = accum;
	return(ptr);
}

static char *
p_time(string, llim, ulim)
char *string;
int llim, ulim;
{
	char *ptr;
	int begin = -1;
	if(!(ptr = (char *)p_ndig(string, &begin)))
		return (NULL);
	if(begin >= llim && begin <= ulim)
		return(ptr);
	else return(NULL);
}

/* p_meridian will parse the string for the meridian - AM/PM or am/pm */

static char *
p_meridian(string)
char *string;
{
	static char *middle[] = {
		 "AM", "PM", "am", "pm"
	};
	int legit, n;
	char mid[TLEN];

	legit = 0;
	n = 0;
        mid[2] = '\0';
        (void) sscanf(string, "%2s", mid);
	while(!(legit) && (n < 4)) {
		if((strncmp(mid, middle[n], 2)) == 0)
			legit = 1;	/* found legitimate string */
		n++;
	}
	if(legit)
		return(string+2);
	return(NULL);
}

static char *
p_delim(string, dchoice)
char *string, dchoice;
{
	char dlm;

	if(! string) return(NULL);
	(void) sscanf(string, "%1c", &dlm);
	return((dlm == dchoice) ? string + 1 : NULL);
}

int
cktime_val(fmt, input)
char	*fmt, *input;
{
	char ltrl, dfl;
	int valid = 1; 	/* time of day string is valid for format */

	if(fmt == NULL)
		fmt = DEFAULT;
	ltrl = dfl = '\0';
	while(*fmt && valid) {
		if((*fmt) == '%') {
			switch(*++fmt) {
			  case 'H': 
				input = p_time(input, LH, UH);
				if(!input)
					valid = 0;
				break;

			  case 'M': 
				input = p_time(input, LM, UM);
				if(!input)
					valid = 0;
				break;

			  case 'S': 
				input = p_time(input, LS, US);
				if(!input)
					valid = 0;
				break;

			  case 'T': 
				input = p_time(input, LH, UH);
				if(!input) {
					valid = 0;
					break;
				}

				input = p_delim(input, DELIM1);
				if(!input) {
					valid = 0;
					break;
			   	}
				input = p_time(input, LM, UM);
				if(!input) {
					valid = 0;
					break;
			   	}
				input = p_delim(input, DELIM1);
				if(!input) {
					valid = 0;
					break;
				}
				input = p_time(input, LS, US);
				if(!input)
					valid = 0;
				break;

			case 'R': 
				input = p_time(input, LH, UH);
				if(!input) {
					valid = 0;
					break;
				}
				input = p_delim(input, DELIM1);
				if(!input) {
					valid = 0;
					break;
				}
				input = p_time(input, LM, UM);
				if(!input) {
					valid = 0;
					break;
				}
				break;

			case 'r':
				input = p_time(input, LH, USH);
				if(!input) {
					valid = 0;
					break;
				}
				input = p_delim(input, DELIM1);
				if(!input) {
					valid = 0;
					break;
				}
				input = p_time(input, LM, UM);
				if(!input) {
					valid = 0;
					break;
				}
				input = p_delim(input, DELIM1);
				if(!input) {
					valid = 0;
					break;
				}
				input = p_time(input, LS, US);
				if(!input) {
					valid = 0;
					break;
				}
				input = p_delim(input, BLANK);
				if(!input) {
					valid = 0;
					break;
				}
				input = p_meridian(input);
				if(!input)
					valid = 0;
				break;

			case 'I': 
				input = p_time(input, LH, USH);
				if(!input)
					valid = 0;
				break;

			case 'p': 
				input = p_meridian(input);
				if(!input)
					valid = 0;
				break;

			  default: 
				(void) sscanf(input++, "%1c", &ltrl);
			}
		} else {
			dfl = '\0';
			(void) sscanf(input, "%1c", &dfl);
			input++;
		}
		fmt++;
	}

	if(!(*fmt) && (*input))
		valid = 0;

	return(!valid);
}

void
cktime_err(fmt, error)
char	*fmt, *error;
{
	char	defmesg[128];

	(void) sprintf(defmesg, ERRMSG, fmt);
	puterror(stdout, defmesg, error);
}

void
cktime_hlp(fmt, help)
char	*fmt, *help;
{
	char	defmesg[128];

	(void) sprintf(defmesg, ERRMSG, fmt);
	puthelp(stdout, defmesg, help);
}

int
cktime(tod, fmt, defstr, error, help, prompt)
char *tod;
char *fmt;
char *prompt;
char *defstr, *error, *help;
{
	char	input[128],
		defmesg[128];

	if(fmt == NULL)
		fmt = DEFAULT;
	(void) sprintf(defmesg, ERRMSG, fmt);
	if(!prompt)
		prompt = "Enter a time of day";

start:
	putprmpt(stderr, prompt, NULL, defstr);
	if(getinput(input))
		return(1);

	if(!strlen(input)) {
		if(defstr) {
			(void) strcpy(tod, input);
			return(0);
		}
		puterror(stderr, defmesg, error);
		goto start;
	}
	if(!strcmp(input, "?")) {
		puthelp(stderr, defmesg, help);
		goto start;
	}
	if(ckquit && !strcmp(input, "q"))
		return(3);

	if(cktime_val(fmt, input)) {
		puterror(stderr, defmesg, error);
		goto start;
	}
	(void) strcpy(tod, input);
	return(0);
}
