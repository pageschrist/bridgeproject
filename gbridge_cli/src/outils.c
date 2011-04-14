#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
char *
grep (char *usr_str, char *expr)
{

  regex_t preg;			/* compiled regular expression : this is used for matching */
  char *result;
  size_t nmatch;
  regmatch_t match[1];
  int ret = 0, i = 0;		/* generic return value holder */

  char errbuf[80];		/* buffer for regerror() to return error messages in */
  int errbuf_len = 80;		/* buffer length */

	/*******************************  INIT *****************************************/
  match[0].rm_so = match[0].rm_eo = -1;
  nmatch = 1;
	/******************************* User input - BEGIN ****************************/
	/******************************* User input - END ****************************/

  /* compile the regex
   * input : expr (from user) 
   * output: preg (type: regex_t)
   * returns: 0 if success
   *          error code otherwise
   */
  ret = regcomp (&preg, expr, REG_EXTENDED);	/* extended regular expressions : see grep(1) */


  /* use regerror to report bad regular expressions */

  if (0 != ret)
    {
#ifdef DEBUG
      printf ("regcomp problem\n");
#endif
      regerror (ret, &preg, errbuf, errbuf_len);
#ifdef DEBUG
      printf ("\nerrbuf_len=%d\n", errbuf_len);
#endif
      puts (errbuf);
      printf ("\n");
      return (NULL);
    }
  /* Execute regex on user input
   * Inputs:
   *      preg    (type: regex_t)          : compiled regular expression
   *      usr_str (type: char array)       : user-input string
   *      nmatch  (type: size_t)           : number of matches
   * Inout:
   *      match   (type: regmatch_t array) : holds start/end offsets of regex matches
   */
  ret = regexec (&preg, usr_str, nmatch, match, 0	/* We are not using REG_NOTBOL or REG_NOTEOL here */
    );
#ifdef DEBUG
  printf ("\nnmatch=%d", nmatch);
#endif
  regfree (&preg);
  if (0 == ret)
    {
     /*affiche le nombre d'occurences matchant */ 
      /*printf ("%d\n", match[0].rm_eo - match[0].rm_so);*/
      result = (char *) malloc ((match[0].rm_eo - match[0].rm_so + 1) * sizeof (char));
      for (i = 0; i < (match[0].rm_eo - match[0].rm_so); i++)
	{
	  memcpy (result + i, usr_str + i + match[0].rm_so, sizeof (char));
	}
      return (result);
    }
  else
    {
#ifdef DEBUG
      printf ("regexec problem\n");
#endif
      regerror (ret, &preg, errbuf, errbuf_len);
#ifdef DEBUG
      printf ("\nerrbuf_len=%d\n", errbuf_len);
#endif
      puts (errbuf);
      printf ("\n");
      return (NULL);
    }




}

char *
strclean (char *usrchar, char *ref)
{
  char *newbuffer = NULL;
  int i = 0,j=0;
  for (i = 0; i < strlen (usrchar); i++)
    {
      if (*ref != usrchar[i])
	{
	  newbuffer=(char * ) realloc (newbuffer, 1);
	  memcpy (newbuffer+j , usrchar+i,sizeof(char));
	  j++;
	}


    }
      newbuffer=(char * ) realloc (newbuffer, 1);
      memcpy (newbuffer+j,"\0",sizeof(char));
      free (usrchar);
      return (newbuffer);
}
