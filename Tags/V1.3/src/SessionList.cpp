/*-------------------------------------------------------------------------*/
/*  Filename: SessionList.cpp                                              */
/*                                                                         */
/*  Purpose:                                                               */
/*     Manage Session List Construction                                    */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2009-2011 Philippe Chevrier pch @ laposte.net             */
/*     from software originally written by Wolfgang Buescher (DL4YHF)      */
/*                                                                         */
/*  License:                                                               */
/*     New WxPic Code is licensed under GPLV3 conditions                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#include "SessionList.h"

void TSessionListBuilder::AddEntry (int pSession, const wxChar *pStatus)
{
    wxString Label = aSessionInfoTab[pSession].Name;
    if (pStatus != NULL)
    {
        if (pSession == 0)
            Label += _(" [Default]");

        Label += wxString(_T(" (")) + pStatus + _T(')');
    }
    aSessionList->Append(Label, (void*)pSession);
    if ((aSessionInfoTab[pSession].State == TSessionManager::sessionStateCUR)
    ||  (aSessionInfoTab[pSession].State == TSessionManager::sessionStateDIRT))
        aSessionList->SetSelection(aSessionCount);
    ++aSessionCount;
}

