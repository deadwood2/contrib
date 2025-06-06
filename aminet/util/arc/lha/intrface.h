/*
 * intrface.h -- part of LHa for UNIX
 *
 *	modified : Dec. 5, 1991, Masaru Oki.
 */

typedef short  node;

struct interfacing {
	FILE *infile;
	FILE *outfile;
	unsigned long original;
	unsigned long packed;
	int dicbit;
	int method;
};

extern struct interfacing interface;

/* from error.c
extern void error();
extern void fileerror(); */

/* from slide.c */ 
extern node *next;

extern int encode_alloc(int method);
extern void encode(struct interfacing *interface);
extern void decode(struct interfacing *interface);

/* from crcio.c */
extern unsigned short crc;

extern void make_crctable(void);
extern unsigned short calccrc(unsigned char *p,
                       unsigned int n);

/* from append.c */
extern void start_indicator(char *name,
                     long size,
                     char *msg,
                     long def_indicator_threshold);
extern void finish_indicator2(char *name,
                       char *msg,
                       int pcnt);
extern void finish_indicator(char *name,
                      char *msg);
