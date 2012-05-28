/*-------------------------------------------------------------------------*/
/*  Filename: FitInDisplay.cpp                                             */
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

#include <wx/window.h>
#include <wx/display.h>
#include <wx/settings.h>

void MoveToFitInDisplay (wxRect &pWindowRect)
{
    const int Count           = wxDisplay::GetCount();
    const int BorderHeight    = wxSystemSettings::GetMetric(wxSYS_CAPTION_Y) / 2; //-- Keep at least half height of the window caption to allow dragging
    const int MinVisible      = 100;  //-- At least 100 pixels width must be visible
    int       ShorterDistance = INT_MAX;
    int       CloserDisplay   = -1;

    //-- Compute Window distance to each of the display
    //-- if distance is null nothing to do the Windows is visible
    for (int i = 0; i < Count; ++i)
    {
        wxRect CurRect      = wxDisplay(i).GetClientArea();
        int    HoriDistance = CurRect.GetLeft() - pWindowRect.GetRight() + MinVisible;
        if (HoriDistance <= 0)
        {
            HoriDistance = pWindowRect.GetLeft() - CurRect.GetRight() + MinVisible;
            if (HoriDistance < 0)
                HoriDistance = 0;
        }
        int  VertDistance = CurRect.GetTop() - pWindowRect.GetTop() - BorderHeight;
        if (VertDistance <= 0)
        {
            VertDistance = pWindowRect.GetTop() - CurRect.GetBottom() + MinVisible;
            if (VertDistance < 0)
                VertDistance = 0;
        }
        int Distance = HoriDistance + VertDistance;
        if (Distance == 0)
            return; /// <<-- RETURN: found a display that shows the window

        if (Distance < ShorterDistance)
        {
            ShorterDistance = Distance;
            CloserDisplay   = i;
        }
    }

    //-- The window does not fit in any display
    //-- shift its rectangle so that it fit in the closest one
    wxRect DisplayRect = wxDisplay(CloserDisplay).GetClientArea();
    if (DisplayRect.GetLeft() > pWindowRect.GetLeft())
        pWindowRect.SetLeft(DisplayRect.GetLeft());
    else if (DisplayRect.GetRight() < pWindowRect.GetRight())
        pWindowRect.SetLeft(DisplayRect.GetRight()-pWindowRect.GetWidth());
    if (DisplayRect.GetTop() > pWindowRect.GetTop())
        pWindowRect.SetTop(DisplayRect.GetTop());
    else if (DisplayRect.GetBottom() < pWindowRect.GetBottom())
        pWindowRect.SetTop(DisplayRect.GetBottom()-pWindowRect.GetHeight());
}
