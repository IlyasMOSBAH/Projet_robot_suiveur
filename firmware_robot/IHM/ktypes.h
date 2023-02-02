#ifndef __KTYPES_H__
#define __KTYPES_H__

/* FILE: ktypes.h */
/****************************************************************************************/
/* MODULE NAME: None                                                                    */
/*                                                                                      */
/* AUTHOR     : Jacques-Olivier KLEIN                                                   */
/*                                                                                      */
/* EMAIL      : jacques-olivier.klein@u-psud.fr                                         */
/*                                                                                      */
/* INSTITUTION: IUT de CACHAN - 9 av. de la div. Leclerc - 94230 CACHAN                 */
/*                                                                                      */
/* DATE       : 03/07/2012                                                              */  
/* CPU        : FREESCALE MC9S12XS128MAA                                                */
/* BOARD      : CACHAN Xboard+MicroB12                                                  */
/* OS         : None                                                                    */
/*                                                                                      */
/* DESCRIPTION: Definition des types de longueurs fixes                                 */
/****************************************************************************************/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef  unsigned char BOOL ;

typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned long  UINT32;
typedef   signed char  SINT8;
typedef   signed short SINT16;
typedef   signed long  SINT32;

typedef unsigned char  INT8U;
typedef unsigned short INT16U;
typedef unsigned long  INT32U;
typedef   signed char  INT8S;
typedef   signed short INT16S;
typedef   signed long  INT32S;

typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned long  U32;
typedef   signed char  S8;
typedef   signed short S16;
typedef   signed long  S32;

typedef volatile unsigned char  VUINT8;


#endif

