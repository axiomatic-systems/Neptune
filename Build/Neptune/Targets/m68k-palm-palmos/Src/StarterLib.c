/*
 * File: StarterLib.c
 *
 */ 
 

#include <PalmOS.h>
#include <Window.h>
#include <Font.h>


/*********************************************************************
 * Internal Functions
 *********************************************************************/


// Our one and only internal function
void StarterLibMain (void);
 
 
/*
 * FUNCTION: StarterLibMain
 *
 * DESCRIPTION:
 *
 * This routine is simply a placeholder for your static library  
 *		routines. Currently, it will display "Hello World!".
 *
 * PARAMETERS:
 *		none
 * RETURNED:
 *     	nothing
 */

void StarterLibMain (void)
{
	const Char chars[] = "Hello World!";
	Coord x = 50;
	Coord y = 60;
	FontID prevFontID;
	
	// Set the font to large bold font
	prevFontID = FntSetFont(largeBoldFont);
	
	// Draw the chars
	WinDrawChars(chars, sizeof(chars)-1, x, y);
	
	//Reset the font to the previous font
	FntSetFont(prevFontID);
} 
	