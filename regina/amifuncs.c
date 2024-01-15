/*
 *  Amiga REXX functions for regina
 *  Copyright � 2002-2011, Staf Verhaegen
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* This files contains functions that are implemented in ARexx
 * but that are not standard REXX functions. This file contains
 * the functions that are only to be used on amiga or compatibles.
 * arxfuncs.c contains the ARexx functions that are usable on all
 * platforms.
 */
#if defined(_AMIGA) || defined(__AROS__)
#define AROS_ALMOST_COMPATIBLE
#include "rexx.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#include "envir.h"
#include <dos/dos.h>
#include <exec/lists.h>
#include <exec/ports.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <rexx/rxslib.h>
#include <rexx/storage.h>
#include <rexx/errors.h>
#ifdef __AROS__
#include <rexx/rexxcall.h>
#endif

#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/rexxsyslib.h>

#include "rxiface.h"

#ifdef __AROS__

#define DEBUG 0
#include <aros/debug.h>

#else

#define D(x)

#endif

#ifdef __AROS__

/* We can't use AROS_LC1NR, since 'offset'
 * is not constant.
 */
#define CallRsrcFunc(libbase, offset, rsrc) \
   ({ \
      int _offset=abs(offset)/6; \
      AROS_LVO_CALL1NR(VOID, \
         AROS_LCA(struct RexxRsrc *, rsrc, A0), \
	 struct Library *, libbase, _offset, rexxcall \
      ); \
  })

#elif defined(__MORPHOS__)

#include <ppcinline/macros.h>

#define CallRsrcFunc(libbase, offset, rsrc) \
   LP1NR((-offset), __rsrcfunc, struct RexxRsrc *, rsrc, a0, \
         struct Library *, libbase, 0, 0, 0, 0, 0, 0 \
   )

#else

#error define CallRsrcFunc

#endif

#ifdef __AROS__

#define RX_PRIVATETYPE IPTR

#else

#define rm_Private1 rm_TaskBlock
#define rm_Private2 rm_LibBase
#define RX_PRIVATETYPE APTR

#endif

#ifndef BNULL
#define BNULL ((BPTR)NULL)
#endif

#ifndef RXADDRSRC
#define RXADDRSRC  0xF0000000 /* Will register a resource node to call the clean up function
			      * from when the rexx script finishes
			      * The rexx implementation is free to use the list node fields
			      * for their own purpose. */
#define RXREMRSRC  0xF1000000 /* Will unregister an earlier registered resource node */
#define RXCHECKMSG 0xF2000000 /* Check if private fields are from the Rexx interpreter */
#define RXSETVAR   0xF3000000 /* Set a variable with a given to a given value */
#define RXGETVAR   0xF4000000 /* Get the value of a variable with the given name */
#endif

#ifdef __AROS__
#define RX_RESULTTYPE IPTR
#define RX_ARGTYPE IPTR
#else
#define RX_RESULTTYPE LONG
#define RX_ARGTYPE STRPTR
#endif

#if !defined(__GNUC__)
#undef GetHead
#define GetHead(_l)  \
    (((struct List *)_l)->lh_Head->ln_Succ ? ((struct List *)_l)->lh_Head : (struct Node *)0)

#undef GetSucc
#define GetSucc(_n)  \
    (((struct Node *)_n)->ln_Succ->ln_Succ ? ((struct Node *)_n)->ln_Succ : (struct Node *)0)
#endif

struct amiga_envir {
  struct envir envir;
  struct MsgPort *port;
};

#define PTRS_SIZE 16
typedef struct _amiga_tsd_t {
   struct amiga_envir portenvir;
   struct RxsLib *rexxsysbase;
   BPTR startlock;
   struct List resources; /* List to store resources to clean up at the end */
   struct MsgPort *listenport, *replyport;
   BYTE maintasksignal, subtasksignal;
   struct Task *parent, *child;
   UBYTE *value; /* Here a temporary argstring will be stored for RXGETVAR */
   void *ai;
   void *ptrs[PTRS_SIZE];
   struct Library *reginabase;
} amiga_tsd_t;


GLOBAL_PROTECTION_VAR(createtask)
tsd_t *subtask_tsd;
 
#define RexxSysBase (((amiga_tsd_t *)TSD->ami_tsd)->rexxsysbase)

/* On AROS delete the allocated resources that are not recycled by the
 * normal C exit handling
 */
void exit_amigaf( amiga_tsd_t *atsd )
{
   struct RexxRsrc *rsrc;
  
#ifdef CallRsrcFunc
   ForeachNode( &atsd->resources, rsrc )
      CallRsrcFunc( rsrc->rr_Base, rsrc->rr_Func, rsrc );
#else
#warning Fix calling resources
#endif
  
   DeleteMsgPort( atsd->replyport );
   Signal( atsd->child, 1<<atsd->subtasksignal );
   if ( atsd->rexxsysbase != NULL )
      CloseLibrary( (struct Library *)atsd->rexxsysbase );
   UnLock( CurrentDir( atsd->startlock ) );
  
   free(atsd);
}

#ifndef RXLIB
static void exit_amigaf_wrapper( void )
{
    tsd_t *TSD = __regina_get_tsd();
    exit_amigaf( (amiga_tsd_t *)TSD->ami_tsd );
}
#endif

streng *createstreng( tsd_t *TSD, char *value, int length )
{
   streng *retval;

#ifdef CHECK_MEMORY
   retval = TSD->MTMalloc( TSD, sizeof(streng) );
   if ( retval != NULL )
   {
      retval->value = TSD->MTMalloc( TSD, length );
      if ( retval->value == NULL )
      {
         TSD->MTFree( retval );
         retval = NULL;
      }
      else
         memcpy( retval->value, value, length );
   }
#else
   retval = TSD->MTMalloc( TSD, sizeof(streng)-4*sizeof(char)+length );
   if ( retval != NULL )
      memcpy( retval->value, value, length );
#endif
   retval->len = length;
   retval->max = length;

   return retval;
}

struct Library *__aros_getbase_ReginaBase(void);
void __aros_setoffsettable(void *base);

/* ReginaHandleMessages will be executed in a subtask and will be
 * able to handle messages send to it asynchronously
 */
void ReginaHandleMessages(void)
{
   tsd_t *TSD = subtask_tsd;
   amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;
   BOOL done;
   ULONG mask, signals;
   struct RexxMsg *msg;
   struct MsgPort *listenport;
   
   /* Share parents regina base with this task, so that rellib C library functions get required base */
   __aros_setoffsettable(atsd->reginabase);

   listenport = CreateMsgPort();
   atsd->listenport = listenport;
   if ( listenport == NULL )
      atsd->child = NULL;
   else
      atsd->subtasksignal = AllocSignal( -1 );
   Signal( atsd->parent, 1<<atsd->maintasksignal );
  
   mask = 1<<atsd->subtasksignal | 1<<atsd->listenport->mp_SigBit;
   done = listenport == NULL;
   while ( !done )
   {
      signals = Wait( mask );
      
      done = (signals & 1<<atsd->subtasksignal) != 0;

      while ( (msg = (struct RexxMsg *)GetMsg( atsd->listenport )) != NULL )
      {
         if ( IsRexxMsg( msg ) )
            switch ( msg->rm_Action & RXCODEMASK )
            {
            case RXADDRSRC:
               AddTail( &atsd->resources, (struct Node *)ARG0( msg ) );
               msg->rm_Result1 = RC_OK;
               break;
	  
            case RXREMRSRC:
               Remove( (struct Node *)ARG0( msg ) );
               msg->rm_Result1 = RC_OK;
               break;

            case RXSETVAR:
               if ( ( msg->rm_Action & RXARGMASK ) != 2 )
               {
                  msg->rm_Result1 = RC_ERROR;
                  msg->rm_Result2 = (IPTR)ERR10_017;
               }
               else
               {
                  streng *name, *value;
                  
                  /* Using own allocation so I can get a NULL return value when allocation
                   * and not Exiterror is called
                   */
                  name = createstreng( TSD, (char *)msg->rm_Args[0], LengthArgstring( (UBYTE *)msg->rm_Args[0] ) );
                  value = createstreng( TSD, (UBYTE *)msg->rm_Args[1], LengthArgstring( (UBYTE *)msg->rm_Args[1] ) );
                     
                  if ( name == NULL || value == NULL )
                  {
                     if ( name != NULL ) Free_stringTSD( name );
                     if ( value != NULL ) Free_stringTSD( value );
                     msg->rm_Result1 = RC_ERROR;
                     msg->rm_Result2 = ERR10_003;
                  }
                  else
                  {
                     setvalue( TSD, name, value, -1 );
                     Free_stringTSD( name );
                     msg->rm_Result1 = RC_OK;
                     msg->rm_Result2 = (IPTR)NULL;
                  }
               }
               break;

            case RXGETVAR:
               if ( ( msg->rm_Action & RXARGMASK ) != 1 )
               {
                  msg->rm_Result1 = RC_ERROR;
                  msg->rm_Result2 = (IPTR)ERR10_017;
               }
               else
               {
                  amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;
                  streng *name;
                  const streng *value;
                  
                  name = createstreng( TSD, (char *)msg->rm_Args[0], LengthArgstring( (UBYTE *)msg->rm_Args[0] ) );
                  if ( name == NULL )
                  {
                     msg->rm_Result1 = RC_ERROR;
                     msg->rm_Result2 = (IPTR)ERR10_003;
                  }
                  else
                  {
                     value = isvariable( TSD, name );
                     Free_stringTSD( name );
                     
                     if ( value != NULL )
                     {
                        if ( atsd->value != NULL) DeleteArgstring( (UBYTE *)atsd->value );
                        atsd->value = CreateArgstring( (STRPTR)value->value, value->len );

                        msg->rm_Result1 = RC_OK;
                        msg->rm_Result2 = (IPTR)atsd->value;
                     }
                     else
                     {
                        msg->rm_Result1 = RC_ERROR;
                        msg->rm_Result2 = (IPTR)ERR10_039;
                     }
                  }
               }
               break;
	    
            default:
               msg->rm_Result1 = RC_ERROR;
               msg->rm_Result2 = ERR10_010;
               break;
            }
         ReplyMsg( (struct Message *)msg );
      }
   }

   FreeSignal(atsd->subtasksignal);
   if ( listenport != NULL )
      DeletePort( listenport );
}

/* Init amiga specific thread data, this function is called during initialisation
 * of the thread specific data
 */
int init_amigaf ( tsd_t *TSD )
{
   amiga_tsd_t *atsd = (amiga_tsd_t *)malloc( sizeof(amiga_tsd_t) );
   BPTR old;
   int i;
   environpart *ep;

   if (atsd==NULL) return 0;

   TSD->ami_tsd = (void *)atsd;

   /* Initialization of amiga environ handling message ports.
    * This has to be kept up-to-date with changes made to
    * add_envir()/clear_environpart() in envir.c
    */
   atsd->portenvir.envir.e.name = NULL;
   atsd->portenvir.envir.type = ENVIR_AMIGA;
   ep = &atsd->portenvir.envir.e.input;
   ep->currnum = -1;
   ep->maxnum = -1;
   ep->hdls[2] = ep->hdls[1] = ep->hdls[0] = -1;
   ep->flags.isinput = 1;
   ep = &atsd->portenvir.envir.e.output;
   ep->currnum = -1;
   ep->maxnum = -1;
   ep->hdls[2] = ep->hdls[1] = ep->hdls[0] = -1;
   ep = &atsd->portenvir.envir.e.error;
   ep->currnum = -1;
   ep->maxnum = -1;
   ep->hdls[2] = ep->hdls[1] = ep->hdls[0] = -1;
   ep->flags.iserror = 1;

   atsd->rexxsysbase = (struct RxsLib *)OpenLibrary( "rexxsyslib.library", 44 );
   if ( atsd->rexxsysbase == NULL )
      return 0;
   old = CurrentDir(BNULL);
   atsd->startlock = DupLock( old );
   CurrentDir(old);
#ifndef RXLIB
   /* When in shared library, mt_amigalib.c will take care of cleaning up */
   if (atexit( exit_amigaf_wrapper ) == -1)
      return 0;
#endif
   NewList( &atsd->resources );
   atsd->replyport = CreatePort( NULL, 0 );
   atsd->maintasksignal = AllocSignal( -1 );
   atsd->parent = FindTask( NULL );
   atsd->reginabase = __aros_getbase_ReginaBase();

   atsd->value = NULL;

   THREAD_PROTECT(createtask)
   subtask_tsd = TSD;
#ifdef __MORPHOS__
   atsd->child = NewCreateTask( TASKTAG_NAME, "Regina Helper", TASKTAG_PC, (APTR)ReginaHandleMessages,
      TASKTAG_CODETYPE, CODETYPE_PPC, TAG_END
   );
#else
   atsd->child = CreateTask( "Regina Helper", 0, (APTR)ReginaHandleMessages, 8192 );
#endif
   if ( atsd->child != NULL )
      Wait(1<<atsd->maintasksignal);
   THREAD_UNPROTECT(createtask)
   FreeSignal(atsd->maintasksignal);
    
   if ( atsd->child == NULL )
      return 0;

   for(i = 0; i < PTRS_SIZE; i++)
      atsd->ptrs[i] = NULL;

   return 1;
}


/*
 * Support function for exec lists
 */
static streng *getlistnames( tsd_t *TSD, struct List *list, const streng *sep )
{
   int first = 1;
   struct Node *ln;
   streng *retval, *tmpstr;
   
   retval = Str_cre_TSD( TSD, "" );
   ForeachNode( list, ln )
   {
      if ( !first )
      {
         tmpstr = Str_cat_TSD( TSD, retval, sep );
         if ( tmpstr != retval )
         {
            Free_string_TSD( TSD, retval );
            retval = tmpstr;
         }
      }
      else
      {
         first = 0;
      }
      tmpstr = Str_catstr_TSD( TSD, retval, ln->ln_Name );
      if ( tmpstr != retval )
      {
         Free_string_TSD( TSD, retval );
         retval = tmpstr;
      }
   }
   
   return retval;
}


/*
 * SHOW a function the names available in different resource lists
 */
streng *amiga_show( tsd_t *TSD, cparamboxptr parm1 )
{
   cparamboxptr parm2 = NULL, parm3 = NULL;
   streng *name = NULL, *sep, *retval = NULL;
  
   checkparam( parm1, 1, 3, "SHOW" );
   parm2 = parm1->next;
   if ( parm2 != NULL )
      parm3 = parm2->next;

   if ( parm2 != NULL && parm2->value != NULL && parm2->value->len != 0 )
      name = parm2->value;
  
   if ( parm3 == NULL || parm3->value == NULL || parm3->value->len == 0 )
      sep = Str_cre_TSD( TSD, " " );
   else
      sep = Str_dup_TSD( TSD, parm3->value );
  
   switch( getoptionchar( TSD, parm1->value, "SHOW", 1, "", "CFLP" ) )
   {
   case 'F':
      retval = arexx_show( TSD, parm1 );
      break;
    
   case 'C':
      LockRexxBase( 0 );
      if ( name == NULL )
         retval = getlistnames( TSD, &RexxSysBase->rl_ClipList, sep );
      else
      {
         char *s = str_of( TSD, name );
         struct Node *ln = FindName( &RexxSysBase->rl_ClipList, s );
         retval = int_to_streng( TSD, ln != NULL );
         Free_TSD( TSD, s );
      }
      UnlockRexxBase( 0 );
      break;
    
   case 'L':
      LockRexxBase( 0 );
      if ( name == NULL )
         retval = getlistnames( TSD, &RexxSysBase->rl_LibList, sep );
      else
      {
         char *s = str_of( TSD, name );
         struct Node *ln = FindName( &RexxSysBase->rl_LibList, s );
         retval = int_to_streng( TSD, ln != NULL );
         Free_TSD( TSD, s );
      }
      UnlockRexxBase( 0 );
      break;
    
   case 'P':
      Forbid();
      if ( name == NULL )
         retval = getlistnames( TSD, &SysBase->PortList, sep );
      else
      {
         char *s = str_of( TSD, name );
         struct Node *ln = FindName( &SysBase->PortList, s );
         retval = int_to_streng( TSD, ln != NULL );
         Free_TSD( TSD, s );
      }
      Permit();
      break;
   }
   Free_string_TSD( TSD, sep );
  
   return retval;
}


/* amiga_find_envir will try to find the port with the given and
 * create an envir function for that
 */
struct envir *amiga_find_envir( const tsd_t *TSD, const streng *name )
{
   amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;
   char *s;
   struct MsgPort *port; 
   
   s = str_of( TSD, name );
   port = FindPort( s );
   FreeTSD( s );
  
   if (port == NULL)
      return NULL;

   if ( atsd->portenvir.envir.e.name != NULL )
      Free_stringTSD( atsd->portenvir.envir.e.name );
    
   atsd->portenvir.envir.e.name = Str_dupTSD( name );
   atsd->portenvir.port = port;

   return (struct envir *)&(atsd->portenvir);
}

/* createreginamessage will create a RexxMsg filled with the necessary fields
 * for regina specific things
 */
struct RexxMsg *createreginamessage( const tsd_t *TSD )
{
   amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;
   struct RexxMsg *msg;
   
   msg = CreateRexxMsg( atsd->replyport, NULL, NULL );
   if ( msg != NULL )
   {
      msg->rm_Private1 = (RX_PRIVATETYPE)atsd->listenport;
      msg->rm_Private2 = (RX_PRIVATETYPE)TSD;
   }
   return msg;
}

/* The function sendandwait will send a rexx message to a certain
 * port and wait till it returns. It the mean time also other
 * message (like variable can be handled
 * The replyport of the msg has to be atsd->listenport
 */
void sendandwait( const tsd_t *TSD, struct MsgPort *port, struct RexxMsg *msg )
{
   amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;
   struct RexxMsg *msg2;

   PutMsg( port, (struct Message *)msg );

   msg2 = NULL;
   while ( msg2 != msg )
   {
      WaitPort( atsd->replyport );
      msg2 = (struct RexxMsg *)GetMsg( atsd->replyport );
      if ( msg2 != msg )
         ReplyMsg( (struct Message *)msg2 );
   }
}



streng *AmigaSubCom( const tsd_t *TSD, const streng *command, struct envir *envir, int *rc )
{
   struct RexxMsg *msg;
   struct MsgPort *port = ((struct amiga_envir *)envir)->port;
   streng *retval = NULL;

   D(bug("environment: %s\n", tmpstr_of( (tsd_t *)TSD, envir->e.name)));

   msg = createreginamessage( TSD );
   /* Always ask for result, wether to set RESULT or not will be decided later */
   msg->rm_Action = RXCOMM | RXFF_RESULT;
   msg->rm_Args[0] = (RX_ARGTYPE)CreateArgstring( (STRPTR)command->value, command->len );
   fflush(stdout);
   msg->rm_Stdin = Input();
   msg->rm_Stdout = Output();

   sendandwait( TSD, port, msg );

   *rc = msg->rm_Result1;
   if (msg->rm_Result1 == 0)
   {
      if (msg->rm_Result2 == 0)
         retval = Str_crestrTSD( "" );
      else
      {
         retval = Str_ncre_TSD( TSD, (UBYTE *)msg->rm_Result2, LengthArgstring( (UBYTE *)msg->rm_Result2 ) );
         DeleteArgstring( (UBYTE *)msg->rm_Result2 );
      }
   }
   else
      retval = Str_crestrTSD( "" );

   DeleteArgstring( (UBYTE *)msg->rm_Args[0]);
   DeleteRexxMsg( msg );
    
   return retval;
}


/*
 * Here follows now the support function for ARexx style function hosts and libraries:
 * addlib and remlib.
 * Also here the try_func_amiga is defined which is called when a function is called
 * in an ARexx script.
 */

/* When addlib is called with two arguments the first argument is considered as a function
 * host name. When it is called with three or four arguments a function library is assumed
 */
streng *amiga_addlib( tsd_t *TSD, cparamboxptr parm1 )
{
   cparamboxptr parm2 = NULL, parm3 = NULL, parm4 = NULL;
   struct MsgPort *rexxport;
   struct RexxMsg *msg;
   int pri, offset, version, error, count;
   char *name;
   streng *retval;
  
   checkparam( parm1, 2, 4, "ADDLIB" );
   parm2 = parm1->next;
   pri = streng_to_int( TSD, parm2->value, &error );
   if (error || abs(pri) > 100 )
      exiterror( ERR_INCORRECT_CALL, 11, "ADDLIB", 2, tmpstr_of( TSD, parm2->value ) );
   
   parm3 = parm2->next;
   if ( parm3 != NULL && parm3->value != NULL && parm3->value->len == 0 )
      exiterror( ERR_INCORRECT_CALL, 21, "ADDLIB", 3 );
   if ( parm3 == NULL || parm3->value == NULL )
      offset = -30;
   else
   {
      offset = streng_to_int( TSD, parm3->value, &error );
      if ( error || offset >= 0 )
         exiterror( ERR_INCORRECT_CALL, 11, "ADDLIB", 3, tmpstr_of( TSD, parm3->value ) );
   }
  
   if ( parm3 != NULL )
      parm4 = parm3->next;
   if ( parm4 == NULL || parm4->value == NULL || parm4->value->len == 0 )
      version = 0;
   else
   {
      version = streng_to_int( TSD, parm4->value, &error );
      if ( error )
         exiterror( ERR_INCORRECT_CALL, 11, "ADDLIB", 4, tmpstr_of( TSD, parm4->value ) );
      if ( version < 0 )
         exiterror( ERR_INCORRECT_CALL, 13, "ADDLIB", 4, tmpstr_of( TSD, parm4->value ) );
   }

   name = str_of( TSD, parm1->value );
   msg = createreginamessage( TSD );
   if ( msg == NULL )
   {
      Free_TSD( TSD, name );
      exiterror( ERR_STORAGE_EXHAUSTED, 0 );
   }
   if ( parm3 == NULL || parm3->value == NULL || parm3->value->len == 0 )
   {
      msg->rm_Action = RXADDFH;
      msg->rm_Args[0] = (RX_ARGTYPE)name;
      msg->rm_Args[1] = (RX_ARGTYPE)pri;
      count = 2;
      if ( !FillRexxMsg( msg, 2, 1<<1 ) )
      {
         Free_TSD( TSD, name );
         DeleteRexxMsg( msg );
         exiterror( ERR_STORAGE_EXHAUSTED, 0 );
      }
   }
   else
   {
      msg->rm_Action = RXADDLIB;
      msg->rm_Args[0] = (RX_ARGTYPE)name;
      msg->rm_Args[1] = (RX_ARGTYPE)pri;
      msg->rm_Args[2] = (RX_ARGTYPE)offset;
      msg->rm_Args[3] = (RX_ARGTYPE)version;
      count = 4;
      if ( !FillRexxMsg( msg, 4, 1<<1 | 1<<2 | 1<<3 ) )
      {
         Free_TSD( TSD, name );
         DeleteRexxMsg( msg );
         exiterror( ERR_STORAGE_EXHAUSTED, 0 );
      }
   }

   rexxport = FindPort( "REXX" );
   if (rexxport == NULL)
   {
      Free_TSD( TSD, name );
      DeleteRexxMsg( msg );
      exiterror( ERR_EXTERNAL_QUEUE, 0 );
   }
   sendandwait( TSD, rexxport, msg );
    
   Free_TSD( TSD, name );
   ClearRexxMsg( msg, count );
    
   retval = ( msg->rm_Result1 == 0 ) ? Str_cre_TSD( TSD, "1" ) : Str_cre_TSD( TSD, "0" );

   DeleteRexxMsg( msg );
  
   return retval;
}

streng *amiga_remlib( tsd_t *TSD, cparamboxptr parm1 )
{
   struct MsgPort *rexxport;
   struct RexxMsg *msg;
   streng *retval;
   
   checkparam( parm1, 1, 1, "REMLIB" );
   
   msg = createreginamessage( TSD );
   if ( msg == NULL )
      exiterror( ERR_STORAGE_EXHAUSTED, 0 );

   msg->rm_Action = RXREMLIB;
   msg->rm_Args[0] = (RX_ARGTYPE)CreateArgstring( parm1->value->value, parm1->value->len );
  
   rexxport = FindPort( "REXX" );
   if (rexxport == NULL )
   {
      DeleteArgstring( (UBYTE *)msg->rm_Args[0] );
      DeleteRexxMsg( msg );
      exiterror( ERR_EXTERNAL_QUEUE, 0 );
   }
   sendandwait( TSD, rexxport, msg );

   retval = ( msg->rm_Result1 == 0 ) ? Str_cre_TSD( TSD, "1" ) : Str_cre_TSD( TSD, "0" );

   DeleteArgstring( (UBYTE *)msg->rm_Args[0] );
   DeleteRexxMsg( msg );
  
   return retval;
}

streng *try_func_amiga( tsd_t *TSD, const streng *name, cparamboxptr parms, char called )
{
   struct MsgPort *port;
   struct RexxMsg *msg;
   struct RexxRsrc *rsrc;
   struct Library *lib;
   ULONG result1;
   IPTR result2 = (IPTR)0;
   UBYTE *retstring;
   unsigned int parmcount;
   cparamboxptr parmit;
   streng *retval = NULL;

   msg = createreginamessage( TSD );
   if ( msg == NULL )
      exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  
   msg->rm_Action = RXFUNC | RXFF_RESULT;
   msg->rm_Args[0] = (RX_ARGTYPE)CreateArgstring( (char *)name->value, name->len );

   for (parmit = parms, parmcount = 0; parmit != NULL; parmit = parmit->next)
   {
      if ( parmit->value != NULL && parmit->value->len > 0 )
      {
         parmcount++;
         msg->rm_Args[parmcount] = (RX_ARGTYPE)CreateArgstring( parmit->value->value, parmit->value->len );
      }
   }
   msg->rm_Action |= parmcount;
   msg->rm_Stdin = Input( );
   msg->rm_Stdout = Output( );
  
   LockRexxBase(0);
   
   for (rsrc = (struct RexxRsrc *)GetHead(&RexxSysBase->rl_LibList), result1 = 1;
        rsrc != NULL && result1 == 1;
        rsrc = (struct RexxRsrc *)GetSucc(rsrc)
   )
   {
      switch (rsrc->rr_Node.ln_Type)
      {
#ifdef __AROS__
      case RRT_LIB:
         lib = OpenLibrary(rsrc->rr_Node.ln_Name, rsrc->rr_Arg2);
         if (lib == NULL)
         {
            UnlockRexxBase(0);
            ClearRexxMsg( msg, parmcount + 1 );
            DeleteRexxMsg( msg );
            exiterror( ERR_EXTERNAL_QUEUE, 0 );
         }
         /* Can not pass &result2 directly because on systems where
          * sizeof(IPTR)>sizeof(UBYTE *) this goes wrong
          */
         result1 = RexxCallQueryLibFunc(msg, lib, rsrc->rr_Arg1, &retstring);
         CloseLibrary(lib);
         result2 = (IPTR)retstring;
         break;
#endif
      
      case RRT_HOST:
         port = FindPort(rsrc->rr_Node.ln_Name);
         if (port == NULL)
         {
            UnlockRexxBase(0);
            ClearRexxMsg( msg, parmcount + 1 );
            DeleteRexxMsg( msg );
            exiterror( ERR_EXTERNAL_QUEUE, 0 );
         }
         sendandwait( TSD, port, msg);
         
         result1 = (ULONG)msg->rm_Result1;
         result2 = msg->rm_Result2;
         
      default:
         assert(FALSE);
      }
   }
   
   UnlockRexxBase(0);
   ClearRexxMsg( msg, parmcount + 1 );
   DeleteRexxMsg( msg );
  
   if ( result1 == 0 )
   {
      if ( (UBYTE *)result2 == NULL )
         retval = nullstringptr();
      else
      {
         retval = Str_ncre_TSD( TSD, (const char *)result2, LengthArgstring( (UBYTE *)result2 ) );
         DeleteArgstring( (UBYTE *)result2 );
      }
   }
   else if ( result1 == 1 )
      retval = NULL;
   else
      exiterror( ERR_EXTERNAL_QUEUE, 0 );
   
   return retval;
}



/* The clip handling functions for AROS/amiga: setclip, getclip */

streng *amiga_setclip( tsd_t *TSD, cparamboxptr parm1 )
{
   cparamboxptr parm2;
   struct MsgPort *rexxport;
   struct RexxMsg *msg;
   streng *retval;
  
   checkparam( parm1, 1, 2, "SETCLIP" );
   parm2 = parm1->next;
   
   msg = createreginamessage( TSD );
   if ( msg == NULL )
      exiterror( ERR_STORAGE_EXHAUSTED, 0 );
  
   if ( parm2 == NULL || parm2->value == NULL || parm2->value->len == 0 )
   {
      msg->rm_Action = RXREMCON;
      msg->rm_Args[0] = (RX_ARGTYPE)str_of( TSD, parm1->value );
   }
   else
   {
      msg->rm_Action = RXADDCON;
      msg->rm_Args[0] = (RX_ARGTYPE)str_of( TSD, parm1->value );
      msg->rm_Args[1] = (RX_ARGTYPE)parm2->value->value;
      msg->rm_Args[2] = (RX_ARGTYPE)parm2->value->len;
   }
   
   rexxport = FindPort( "REXX" );
   if ( rexxport == NULL )
   {
      Free_TSD( TSD, (void *)msg->rm_Args[0] );
      DeleteRexxMsg( msg );
      exiterror( ERR_EXTERNAL_QUEUE, 0 );
   }
   sendandwait( TSD, rexxport, msg );
   
   Free_TSD( TSD, (void *)msg->rm_Args[0] );
   
   retval = ( msg->rm_Result1 == 0 ) ? Str_cre_TSD( TSD, "1" ) : Str_cre_TSD( TSD, "0" );
   DeleteRexxMsg( msg );
   
   return retval;
}

streng *amiga_getclip( tsd_t *TSD, cparamboxptr parm1 )
{
   struct RexxRsrc *rsrc;
   char *name;
  
   checkparam( parm1, 1, 1, "GETCLIP" );
  
   name = str_of( TSD, parm1->value );
  
   LockRexxBase(0);
   rsrc = (struct RexxRsrc *)FindName( &RexxSysBase->rl_ClipList, name );
   UnlockRexxBase(0);
  
   Free_TSD( TSD, name );
  
   if ( rsrc == NULL )
      return nullstringptr();
   else
   {
      void *arg1 = (void *)rsrc->rr_Arg1;
      return Str_ncre_TSD( TSD, arg1, LengthArgstring( arg1 ) );
   }
}

streng *amiga_pragma( tsd_t *TSD, cparamboxptr parm1 )
{
   cparamboxptr parm2;
   streng *retval = NULL;
   static char buf[1024];
   
   checkparam( parm1, 1, 2, "PRAGMA" );
   parm2 = parm1->next;
  
   switch( getoptionchar( TSD, parm1->value, "PRAGMA", 1, "", "DPIS" ) )
   {
   case 'D':
      {
         BPTR lock = CurrentDir( BNULL );
         
         NameFromLock( lock, buf, 1023 );
         CurrentDir( lock );
         retval = Str_cre_TSD( TSD, buf );
         if ( parm2 != NULL && parm2->value != NULL && parm2->value->len != 0 )
         {
            struct FileInfoBlock *fib;
            char *name = str_of( TSD, parm2->value );
            
            lock = Lock( name, ACCESS_READ );
            
            Free_TSD( TSD, name );
            
            fib = AllocDosObject( DOS_FIB, NULL );
            if ( fib == NULL )
            {
               if ( lock != BNULL )
                  UnLock( lock );
               exiterror( ERR_STORAGE_EXHAUSTED, 0 );
            }

            if ( lock != BNULL )
               Examine( lock, fib );
      
            if ( lock == BNULL || fib->fib_DirEntryType <= 0 )
            {
               FreeDosObject( DOS_FIB, fib );
               Free_string_TSD( TSD, retval );
               retval = nullstringptr();
            }
            else
            {
               UnLock( CurrentDir( lock ) );
               FreeDosObject( DOS_FIB, fib );
            }
         }
      }
      break;

   case 'P':
      {
         struct Task *task = FindTask( NULL );
         retval = int_to_streng( TSD, (int)task->tc_Node.ln_Pri );
         if ( parm2 != NULL && parm2->value != NULL && parm2->value->len != 0 )
         {
            int pri, error;
            pri = streng_to_int( TSD, parm2->value, &error );
            if ( error )
               exiterror( ERR_INCORRECT_CALL, 11, "PRAGMA", 2, tmpstr_of( TSD, parm2->value ) );
            if ( abs(pri) > 127 )
               exiterror( ERR_INCORRECT_CALL, 0 );
            SetTaskPri( task, pri );
         }
      }
      break;
      
   case 'I':
      {
         char s[10];
         sprintf(s, "%8lx", (unsigned long)FindTask( NULL ) );
         if ( parm2 != NULL && parm2->value != NULL )
            exiterror( ERR_INCORRECT_CALL, 4, "PRAGMA", 1 );
         retval = Str_cre_TSD( TSD, s );
      }
      break;
      
   case 'S':
      {
         struct Process *process = (struct Process *)FindTask( NULL );
         ULONG size = (ULONG)((char *)process->pr_Task.tc_SPUpper - (char *)process->pr_Task.tc_SPLower);
         retval = int_to_streng( TSD, size );
         /* FIXME: second argument ignored because stack size increase is not implemented
          */
      }
      break;
   }
  
   return retval;
}


/* Support functions for os_amiga.c
   They are defined here so data can be stored in amiga_tsd_t, that is only
   defined in this file.
*/
int __amiga_ptr2int(const tsd_t *TSD, void *ptr)
{
   amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;
   int i;

   for ( i = 0; i < PTRS_SIZE && atsd->ptrs[i] != NULL; i++ )
      /* NOP */;

   if ( i == PTRS_SIZE )
      return -1;
   else
   {
      atsd->ptrs[i] = ptr;
      return i + 1;
   }
}

void *__amiga_getptr(const tsd_t *TSD, int index)
{
   amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;

   return atsd->ptrs[index - 1];
}

void __amiga_clearptr(const tsd_t *TSD, int index)
{
   amiga_tsd_t *atsd = (amiga_tsd_t *)TSD->ami_tsd;

   atsd->ptrs[index - 1] = NULL;
}
#endif
