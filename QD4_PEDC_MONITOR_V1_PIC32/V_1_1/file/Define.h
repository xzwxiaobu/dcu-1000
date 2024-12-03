
#ifndef _DEFINE_H
#define _DEFINE_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif

/****************** DATA TYPES Complier Specific ******************************/
typedef int8u  *      pINT8U;
typedef int16u *      pINT16U;


typedef union
{
	struct
	{
		int8u byte0;
		int8u byte1;
	};	
	struct
	{
		int16u word;
	};	
	struct
	{
		int8u B0  : 1;
		int8u B1  : 1;
		int8u B2  : 1;
		int8u B3  : 1;
		int8u B4  : 1;
		int8u B5  : 1;
		int8u B6  : 1;
		int8u B7  : 1;
		int8u B8  : 1;
		int8u B9  : 1;
		int8u B10 : 1;
		int8u B11 : 1;
		int8u B12 : 1;
		int8u B13 : 1;
		int8u B14 : 1;
		int8u B15 : 1;
	};

}BIT16;

typedef union
{
	struct
	{
		int8u byte;
	};

	struct
	{
		int8u B0  : 1;
		int8u B1  : 1;
		int8u B2  : 1;
		int8u B3  : 1;
		int8u B4  : 1;
		int8u B5  : 1;
		int8u B6  : 1;
		int8u B7  : 1;
	};
}BIT8;

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#define SetBit(Var,Place)     (Var|=(1<<Place))
#define ClrBit(Var,Place)     ( Var &= ((1<<Place)^255) )
#define ValBit(Var,Place)     ( Var & (1<<Place) )

#define DelayNop()   {Nop(); Nop(); Nop(); Nop(); }

#endif

