/*-------------------------------------------------------------------------*/
/*  Filename: FitInDisplay.h                                               */
/*                                                                         */
/*  Purpose:                                                               */
/*     Ajust the given Window coordinates so that they fit in the display  */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2011 Philippe Chevrier pch @ laposte.net                  */
/*                                                                         */
/*  License:                                                               */
/*     Licensed under GPLV3 conditions                                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#ifndef FITINDISPLAY_H_INCLUDED
#define FITINDISPLAY_H_INCLUDED

//-- Ajust the given Windows rectangle so that the top of the window (border) fits in the display
void MoveToFitInDisplay (wxRect &pWindowRect);

#endif // FITINDISPLAY_H_INCLUDED
