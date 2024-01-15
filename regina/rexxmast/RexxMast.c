#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/regina.h>
#include <proto/rexxsyslib.h>

#include <rexx/rexxcall.h>
#include <exec/ports.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <rexx/storage.h>
#include <rexx/errors.h>
#include <rexx/rxslib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEBUG 0
#include <aros/debug.h>

static LONG StartFile(struct RexxMsg *);
static void StartFileSlave(struct RexxMsg *);
static void AddLib(struct RexxMsg *);
static void RemLib(struct RexxMsg *);
static void AddCon(struct RexxMsg *);
static void RemCon(struct RexxMsg *);

struct Library *CrtBase = NULL;
struct Library *ReginaBase = NULL;
static void OpenLibs();
static void CloseLibs();

static UBYTE progdir[256];

int main(int argc, char **argv)
{
   struct MsgPort *port;
   struct RexxMsg *msg;
   BOOL done = FALSE;
   ULONG mask, signals;
   BPTR lock;
   struct EasyStruct es =
   {
      sizeof(struct EasyStruct),
      0,
      "RexxMast error",
      NULL,
      "OK"
   };

   OpenLibs();

   if (argc==3 && strcmp("SUBTASK", argv[1])==0)
   {
      struct RexxMsg *msg;
      
      sscanf(argv[2], "%p", &msg);
      StartFileSlave(msg);
      ReplyMsg((struct Message *)msg);
      CloseLibs();
      return 0;
   }

   lock = Lock("PROGDIR:", SHARED_LOCK);
   NameFromLock(lock, progdir, sizeof(progdir));
   D(bug("Got PROGDIR:='%s'\n", progdir));
   UnLock(lock);

   port = CreatePort("REXX", 0);
   es.es_Title = "RexxMast message";
   mask = SIGBREAKF_CTRL_C | (1<<port->mp_SigBit);
   do
   {
      signals = Wait(mask);
      if (signals & SIGBREAKF_CTRL_C)
         done = TRUE;
      if (signals & (1<<port->mp_SigBit))
      {
         while ((msg = (struct RexxMsg *)GetMsg(port)) != NULL)
         {
            BOOL reply=TRUE;


            if (!IsRexxMsg(msg))
            {
               es.es_TextFormat = "Received message that is not a Rexx message";
               EasyRequest(NULL, &es, NULL);
            }
            else
            {
               static UBYTE *text[] =
                  { "RXCOMM", "RXFUNC", "RXCLOSE", "RXQUERY", "UNKNOWN1",
                    "UNKNOWN2", "RXADDFH", "RXADDLIB", "RXREMLIB", 
                    "RXADDCON", "RXREMCON", "RXTCOPN", "RXTCCLS",
                    "TOO HIGH"
                  };
               LONG action = msg->rm_Action & RXCODEMASK;
               D(bug("Got message with action %x\n", action));
               switch (action)
               {
               case RXFUNC:
               case RXCOMM:
                  if (StartFile(msg) < 0)
                  {
                     D(bug("Error executing command '%s'\n", (char *)msg->rm_Args[0]));
                     msg->rm_Result1 = RC_ERROR;
                     msg->rm_Result2 = (IPTR)ERR10_100;
                  }
                  else
                  {
                     D(bug("Command '%s' executed\n", (char *)msg->rm_Args[0]));
                     reply = FALSE;
                  }
                  break;

               case RXADDCON:
                  AddCon(msg);
                  break;
		    
               case RXREMCON:
                  RemCon(msg);
                  break;
		    
               case RXADDFH:
               case RXADDLIB:
                  AddLib(msg);
                  break;

               case RXREMLIB:
                  RemLib(msg);
                  break;

               case RXADDRSRC:
               case RXREMRSRC:
               case RXCHECKMSG:
               case RXSETVAR:
               case RXGETVAR:
                  if (!IsReginaMsg(msg))
                  {
                     msg->rm_Result1 = RC_ERROR;
                     msg->rm_Result2 = (IPTR)ERR10_010;
                  }
                  else if (action==RXCHECKMSG)
                  {
                     msg->rm_Result1 = RC_OK;
                  }
                  else
                  {
                     /* Forward to the appropriate port */
                     PutMsg( (struct MsgPort *)msg->rm_Private1, (struct Message *)msg );
                     reply = FALSE;
                  }
                  break;
		    
               case RXCLOSE:
                  done = TRUE;
               default:
                  es.es_TextFormat = "Rexx message with command \"%s\" received";
                  if (action <= RXTCCLS)
                     EasyRequest(NULL, &es, NULL, text[(action >> 24) - 1]);
                  else
                     EasyRequest(NULL, &es, NULL, text[(RXTCCLS >> 24)]);
                  break;
               }
            }
            if (reply)
            {
               D(bug("Replying message\n"));
               ReplyMsg((struct Message *)msg);
            }
            else
               D(bug("Not replying to message\n"));
         }
      }
   } while(!done);
   
   DeletePort(port);
   CloseLibs();
   
   return 0;
}

static LONG StartFile(struct RexxMsg *msg)
{
   char text[300];

   sprintf(text, "\"%s%sRexxMast\" SUBTASK %p", progdir,
           progdir[strlen(progdir)-1] == ':' ? "" : "/",
           (void*)msg
   );
  
   /* FIXME: thread should be used to handle more then one message at a time, not SystemTags */
   return SystemTags(text, SYS_Asynch, TRUE, SYS_Input, NULL,
                     SYS_Output, NULL, TAG_DONE
   );
}

static void StartFileSlave(struct RexxMsg *msg)
{
   UBYTE *comm = (UBYTE *)msg->rm_Args[0];
   BPTR lock, oldlock = BNULL;
   unsigned int len=0, extlen, commlen = LengthArgstring(comm);
   BPTR input = BNULL, output = BNULL, error = BNULL;
   struct Process *process = (struct Process *)msg->rm_Node.mn_ReplyPort->mp_SigTask;

   /* Input arguments for calling the RexxStart procedure */
   LONG argcount = 0;
   RXSTRING rxargs[15], rxresult;
   PSZ progname = NULL;
   PRXSTRING instore = NULL;
   USHORT rc;
   
   /* Set input/output to the task that sent the message */
   if (!(msg->rm_Action & RXFF_NOIO))
   {
      lock = BNULL;
      oldlock = BNULL;
      if (process->pr_Task.tc_Node.ln_Type == NT_PROCESS)
      {
         input = process->pr_CIS;
         output = process->pr_COS;
         error = process->pr_CES;
         lock = DupLock(process->pr_CurrentDir);
         if (lock != BNULL)
         {
            lock = CurrentDir(lock);
            oldlock = DupLock(lock);
            UnLock(lock);
         }
      }
      if (msg->rm_Stdin != BNULL)
         input = msg->rm_Stdin;
      if (msg->rm_Stdout != BNULL)
         output = msg->rm_Stdout;
      
      input = SelectInput(input);
      output = SelectOutput(output);
      error = SelectErrorOutput(error);
      updatestdio();
   }

   if ((msg->rm_Action & RXFF_STRING))
   {
      void *t;

      progname = strdup("intern");
      instore = malloc(2*sizeof(RXSTRING));
      t = malloc(commlen);
      memcpy(t, comm, commlen);
      MAKERXSTRING(instore[0], t, commlen);
      MAKERXSTRING(instore[1], NULL, 0);
   }
   else if (comm[0] == '\'' || comm[0] == '"')
   {
      char c = comm[0];
      int i;
      void *t;
        
      for (i = 1; comm[i] != c && i < commlen; i++)
         ;

      progname = strdup("intern");
      instore = malloc(2*sizeof(RXSTRING));
      t = malloc(i-1);
      memcpy(t, comm + 1, i - 1);
      MAKERXSTRING(instore[0], t, i-1);
      MAKERXSTRING(instore[1], NULL, 0);
   }
   else
   {
      BOOL iscommand = ((msg->rm_Action & RXCODEMASK) == RXCOMM);
      UBYTE *s = comm;

      if (iscommand)
      {
         /* For a command the characters up to the first space is the progname */
         while (*s != 0 && !isspace(*s)) s++;
         len = s - comm;
      }
      else
      {
         /* For a function whole ARG0 is the progname */
         len = LengthArgstring((UBYTE *)msg->rm_Args[0]);
      }

      extlen = msg->rm_FileExt==NULL ? 5 : strlen(msg->rm_FileExt);
      progname = malloc(len + 6 + extlen);

      memcpy(progname, (char *)msg->rm_Args[0], len);
      progname[len] = 0;
      lock = Lock(progname, ACCESS_READ);
      if (lock == BNULL)
      {
         strcat(progname, msg->rm_FileExt==NULL ? ".rexx" : (const char *)msg->rm_FileExt);
         lock = Lock(progname, ACCESS_READ);
      }
      if (lock == BNULL && strchr(progname, ':') == NULL)
      {
         strcpy(progname, "REXX:");
         strncat(progname, (char *)msg->rm_Args[0], len);
         lock = Lock(progname, ACCESS_READ);
      }
      if (lock == BNULL)
      {
         strcat(progname, msg->rm_FileExt==NULL ? ".rexx" : (const char *)msg->rm_FileExt);
         lock = Lock(progname, ACCESS_READ);
      }
      if (lock == BNULL)
      {
         msg->rm_Result1 = 5;
         msg->rm_Result2 = 1;
         free(progname);
         if (oldlock != BNULL)
            UnLock(CurrentDir(oldlock));
         return;
      }
      UnLock(lock);
      
      if (iscommand)
      {
         while (isspace(*s))
            s++;
         
         if (!(msg->rm_Action & RXFF_TOKEN))
         {
            if (*s == 0)
               argcount = 0;
            else
            {
               argcount = 1;
               len = LengthArgstring((UBYTE *)msg->rm_Args[0]) - (s - (UBYTE *)msg->rm_Args[0]);
               MAKERXSTRING(rxargs[0], s, len);
            }
         }
         else
         {
            UBYTE *s2;
            while (*s != 0)
            {
               s2 = s;
               while((*s2 != 0) && !isspace(*s2)) s2++;
               MAKERXSTRING(rxargs[argcount], s, s2-s);
		
               while((*s2 != 0) && isspace(*s2) ) s2++;
               s = s2;
               argcount++;
            }
         }
      }
      else /* is a function call */
      {
         int arguments = msg->rm_Action & RXARGMASK;
         
         for (argcount = 0; argcount < arguments; argcount++)
         {
            UBYTE *argstr = (UBYTE *)msg->rm_Args[1+argcount];
            if (argstr == NULL)
               MAKERXSTRING(rxargs[argcount], NULL, 0);
            else
               MAKERXSTRING(rxargs[argcount], argstr, LengthArgstring(argstr));
         }
      }
   }

   MAKERXSTRING(rxresult, NULL, 0);
   D(bug("[RexxMast/StartFileSlave()] Executing '%s'\n", progname));
   RexxStart(argcount, rxargs, progname, instore, msg->rm_CommAddr, RXFUNCTION, NULL, &rc, &rxresult);
   D(bug("[RexxMast/StartFileSlave()] Return rc=%d, rxresult='%s'\n",
         rc, rxresult
   ));
   /* Return to the old input/output if it was changed */
   if (!(msg->rm_Action & RXFF_NOIO))
   {
      if (input != BNULL)
         SelectInput(input);
      if (output != BNULL)
         SelectOutput(output);
      if (error != BNULL)
         SelectErrorOutput(error);
      updatestdio();
      if (oldlock != BNULL)
         UnLock(CurrentDir(oldlock));
   }
    
   fflush(stdout);
   msg->rm_Result1 = rc;
   if (rc==0 && (msg->rm_Action & RXFF_RESULT) && RXVALIDSTRING(rxresult))
      msg->rm_Result2 = (IPTR)CreateArgstring(RXSTRPTR(rxresult), RXSTRLEN(rxresult));
   else
      msg->rm_Result2 = 0;
   
   if (RXSTRPTR(rxresult) != NULL)
      free(RXSTRPTR(rxresult));
   if (progname != NULL)
      free(progname);
   if (instore != NULL)
   {
      free(RXSTRPTR(instore[0]));
      free(instore);
   }
}

static void AddLib(struct RexxMsg *msg)
{
   struct RexxRsrc *rsrc;
  
   if (msg->rm_Args[0] == 0 || msg->rm_Args[1] == 0)
   {
      msg->rm_Result1 = 20;
      msg->rm_Result2 = 0;
      return;
   }
  
   rsrc = (struct RexxRsrc *)AllocMem(sizeof(struct RexxRsrc), MEMF_ANY | MEMF_CLEAR);
   rsrc->rr_Size = sizeof(struct RexxRsrc);
   rsrc->rr_Node.ln_Pri = atoi((char *)msg->rm_Args[1]);
   rsrc->rr_Node.ln_Name = CreateArgstring((UBYTE *)msg->rm_Args[0], strlen((char *)msg->rm_Args[0]));
   
   if ((msg->rm_Action & RXCODEMASK) == RXADDLIB)
   {
      rsrc->rr_Node.ln_Type = RRT_LIB;
      if (msg->rm_Args[2] == 0)
      {
         msg->rm_Result1 = 20;
         msg->rm_Result2 = 0;
         FreeMem(rsrc, rsrc->rr_Size);
         return;
      }
      rsrc->rr_Arg1 = (LONG)atoi((char *)msg->rm_Args[2]);
      rsrc->rr_Arg2 = (msg->rm_Args[3] == 0) ? (LONG)0 : (LONG)atoi((char *)msg->rm_Args[3]);
   }
   else
      rsrc->rr_Node.ln_Type = RRT_HOST;

   LockRexxBase(0);
   if (FindName(&RexxSysBase->rl_LibList, rsrc->rr_Node.ln_Name))
   {
      msg->rm_Result1 = 5;
      msg->rm_Result2 = 0;
      FreeMem(rsrc, rsrc->rr_Size);
   }
   else
   {
      Enqueue(&RexxSysBase->rl_LibList, (struct Node *)rsrc);
      msg->rm_Result1 = 0;
      msg->rm_Result2 = 0;
      RexxSysBase->rl_NumLib++;
   }
   UnlockRexxBase(0);
}

static void RemLib(struct RexxMsg *msg)
{
   struct RexxRsrc *rsrc;
  
   if (msg->rm_Args[0] == 0)
   {
      msg->rm_Result1 = 20;
      msg->rm_Result2 = 0;
      return;
   }
  
   LockRexxBase(0);
   rsrc = (struct RexxRsrc *)FindName(&RexxSysBase->rl_LibList, (STRPTR)msg->rm_Args[0]);
   if (rsrc == NULL)
   {
      msg->rm_Result1 = 5;
      msg->rm_Result2 = 0;
   }
   else
   {
      Remove((struct Node *)rsrc);
      RexxSysBase->rl_NumLib--;
      DeleteArgstring(rsrc->rr_Node.ln_Name);
      FreeMem(rsrc, rsrc->rr_Size);
      msg->rm_Result1 = 0;
      msg->rm_Result2 = 0;
   }
   UnlockRexxBase(0);
}

static void AddCon(struct RexxMsg *msg)
{
   struct RexxRsrc *rsrc;
  
   LockRexxBase(0);
   rsrc = (struct RexxRsrc *)FindName(&RexxSysBase->rl_ClipList, (char *)msg->rm_Args[0]);
   if (rsrc == NULL)
   {
      rsrc = (struct RexxRsrc *)AllocMem(sizeof(struct RexxRsrc), MEMF_ANY | MEMF_CLEAR);
      rsrc->rr_Node.ln_Type = RRT_CLIP;
      rsrc->rr_Node.ln_Name = CreateArgstring((UBYTE *)msg->rm_Args[0], strlen((char *)msg->rm_Args[0]));
      rsrc->rr_Size = sizeof(struct RexxRsrc);
      rsrc->rr_Arg1 = (IPTR)CreateArgstring((UBYTE *)msg->rm_Args[1], (ULONG)msg->rm_Args[2]);
      
      AddTail(&RexxSysBase->rl_ClipList, (struct Node *)rsrc);
   }
   else
   {
      DeleteArgstring((UBYTE *)rsrc->rr_Arg1);
      rsrc->rr_Arg1 = (IPTR)CreateArgstring((UBYTE *)msg->rm_Args[1], (ULONG)msg->rm_Args[2]);
   }
   UnlockRexxBase(0);
  
   msg->rm_Result1 = 0;
   msg->rm_Result2 = 0;
   return;
}

static void RemCon(struct RexxMsg *msg)
{
   struct RexxRsrc *rsrc;
   
   LockRexxBase(0);
   rsrc = (struct RexxRsrc *)FindName(&RexxSysBase->rl_ClipList, (STRPTR)msg->rm_Args[0]);
   if (rsrc == NULL)
   {
      msg->rm_Result1 = 0;
   }
   else
   {
      Remove(&rsrc->rr_Node);
      DeleteArgstring(rsrc->rr_Node.ln_Name);
      DeleteArgstring((UBYTE *)rsrc->rr_Arg1);
      FreeMem(rsrc, rsrc->rr_Size);
      
      msg->rm_Result1 = 0;
   }
   UnlockRexxBase(0);
   
   msg->rm_Result2 = 0;
   return;
}

void __progonly_allow_shareable(struct Library *base);
void __progonly_disallow_shareable(struct Library *base);

static void OpenLibs()
{
   CrtBase = OpenLibrary("crt.library", 3L);

   __progonly_allow_shareable(CrtBase);

   ReginaBase = OpenLibrary("regina.library", 4L);

   __progonly_disallow_shareable(CrtBase);
}

static void CloseLibs()
{
   CloseLibrary(ReginaBase);
   ReginaBase = NULL;
   CloseLibrary(CrtBase);
   CrtBase = NULL;
}
