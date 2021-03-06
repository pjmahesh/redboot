//=============================================================================
//
//      test_menu.c - Cyclone Diagnostics
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Scott Coulter, Jeff Frazier, Eric Breeden
// Contributors:
// Date:        2001-01-25
// Purpose:     
// Description: 
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

/*****************************************************************************
* test_menu.c - Menu dispatching routine 
* 
* modification history
* --------------------
* 30aug00 ejb Ported to IQ80310 Cygmon
*/

/*
DESCRIPTION:

A table-driven menu dispatcher
*/

#include "test_menu.h"
#include "iq80310.h"

#define QUIT			-1
#define MAX_INPUT_LINE_SIZE	80

extern long decIn(void);

/*
 * Internal routines
 */
static int menuGetChoice (MENU_ITEM	menuTable[],
			  int		numMenuItems,
			  char		*title,
			  unsigned long	options);
static void printMenu (MENU_ITEM	menuTable[],
		       int		numMenuItems,
		       char		*title);


/***************************************************************************
*
* menu - a table-driven menu dispatcher
*
* RETURNS:
*
*	The menu item argument, or NULL if the item chosen is QUIT.
*/
MENU_ARG menu (
    MENU_ITEM	menuTable[],
    int		numMenuItems,
    char		*title,
    unsigned long	options
    )
{
    int	item;		/* User's menu item choice */

    /*
     * Get the user's first choice.  Always display the menu the first time.
     */
    item = menuGetChoice (menuTable, numMenuItems, title, MENU_OPT_NONE);
    if (item == QUIT)
	return (NULL);

    /*
     * If the user just wants a value returned, return the argument.  If the
     * argument is null, return the item number itself.
     */
    if (options & MENU_OPT_VALUE) {
	if (menuTable[item].arg == NULL)
	    return ((void *)item);
	else
	    return (menuTable[item].arg);
    }

    /*
     * Process menu items until the user selects QUIT
     */
    while (TRUE) {
	/*
	 * Call the action routine for the chosen item.  If the argument is
	 * NULL, pass the item number itself.
	 */
	if (menuTable[item].actionRoutine != NULL) {
	    if (menuTable[item].arg == NULL) {
		printf("\n");
		(*menuTable[item].actionRoutine) ((void *)item);
	    } else {
		printf("\n");
		(*menuTable[item].actionRoutine)(menuTable[item].arg);
	    }
	}

	/*
	 * Get the next choice, using any display options the user specified.
	 */
	item = menuGetChoice (menuTable, numMenuItems, title, options);
	if (item == QUIT)
	    return (NULL);
    }
}


/***************************************************************************
*
* menuGetChoice - Get the user's menu choice.
*
* If display is not suppressed, display the menu, then prompt the user for
* a choice.  If the choice is out of range or invalid, display the menu and
* prompt again.  Continue to display and prompt until a valid choice is made.
*
* RETURNS:
*	The item number of the user's menu choice. (-1 if they chose QUIT)
*/

static int
menuGetChoice (MENU_ITEM	menuTable[],
	       int		numMenuItems,
	       char		*title,
	       unsigned long	options
	       )
{
    int	choice;

    /*
     * Suppress display of the menu the first time if we're asked
     */
    if (!(options & MENU_OPT_SUPPRESS_DISP))
	printMenu (menuTable, numMenuItems, title);

    /*
     * Prompt for a selection.  Redisplay the menu and prompt again
     * if there's an error in the selection.
     */
    choice = -1;
	
    while (choice < 0 || choice > numMenuItems) {
	printf ("\nEnter the menu item number (0 to quit): ");
	choice = decIn ();
	if (choice < 0 || choice > numMenuItems)
	    printMenu (menuTable, numMenuItems, title);
    }

    if (choice == 0)
	return (QUIT);

    return (choice - 1);

}


/***************************************************************************
*
* printMenu - Print the menu
*
*
*/

static void
printMenu (MENU_ITEM	menuTable[],
	   int		numMenuItems,
	   char		*title
	   )
{
    int		i;

    printf("\n%s\n\n", title);

    for (i = 0; i < numMenuItems; i++)
	printf ("%2d - %s\n", i+1, menuTable[i].itemName);

    printf(" 0 - quit");

}
