/*-------------------------------------------------------------------------*/
/*  Filename: SessionList.h                                                */
/*                                                                         */
/*  Purpose:                                                               */
/*     Manage Session List Construction                                    */
/*     Define the pure interface used to manage sessions                   */
/*                                                                         */
/*  Author:                                                                */
/*     Copyright 2009-2011 Philippe Chevrier pch @ laposte.net             */
/*     from software originally written by Wolfgang Buescher (DL4YHF)      */
/*                                                                         */
/*  License:                                                               */
/*     New WxPic Code is licensed under GPLV3 conditions                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/


#ifndef TSESSIONLIST_H
#define TSESSIONLIST_H

#include <wx/ctrlsub.h>


//-- Interface that the creator must provide
class TSessionManager
{
public:

    enum TSessionState
    {
        sessionStateNONE,  //-- Session is not existing
        sessionStateIDLE,  //-- Session exists but is not used
        sessionStateCUR,   //-- The session is used by current session
        sessionStateDIRT,  //-- The session is used by current session and configuration is not saved
        sessionStateUSED,  //-- The session is used by another session
        sessionStateLAST,  //-- Dummy session indicating last session in the table
    };


    struct TSessionInfo
    {
        TSessionState State;
        wxString      Name;
    };


    //-- Return the table of Sessions
    //-- The table is terminated by a dummy session having the state sessionLAST
    //-- The returned table must be deleted[] by caller
    virtual TSessionInfo   *GetSessionTab (void) const = 0;

    //-- Attempt to take the specified session (or to create a new Session if pSession == -1)
    //-- and return the session index or -1 if failed
    //-- pSession is the index of the session in the table returned by GetSessionTab
    virtual int  SetSession  (int pSession) = 0;

    //-- Delete the given session
    //-- and return the session state before the operation
    //--    + If the session has been deleted successfully return sessionIDLE
    //--    + Else if it has been started by another instance return sessionUSED
    //--    + Or if it has been already deleted by another instance return sessionNONE
    //-- pSession is the index of the session in the table returned by GetSessionTab
    //-- Return false if the delete failed
    virtual TSessionState  DeleteSession (int pSession) = 0;

    //-- Save the current configuration in the current session
    virtual void SaveConfig (void) = 0;

    //-- Cancel the configuration changes by reloading the saved session configuration
    virtual void RevertConfig (void) = 0;

    //-- Rename the current session with the given name
    //-- Return false if another session has already taken this name
    virtual bool RenameConfig (const wxString &pNewName) = 0;

    //-- No need of virtual destructor
};


class TSessionListBuilder
{
public:
    /**/ TSessionListBuilder (const TSessionManager::TSessionInfo *pSessionInfoTab, wxControlWithItems* pSessionList)
    : aSessionInfoTab(pSessionInfoTab)
    , aSessionList   (pSessionList)
    , aSessionCount  (0)
    {
        pSessionList->Clear();
    }

    //-- Add a aSessionListBox entry
    void AddEntry (int pSession, const wxChar *pStatus = NULL);

    static int GetSelectedSession (const wxControlWithItems *pSessionList)
    {
        int SelectedItem = pSessionList->GetSelection();
        return (SelectedItem < 0)
                    ? SelectedItem
                    : (int)pSessionList->GetClientData(SelectedItem);
    }

private:
    const TSessionManager::TSessionInfo * const aSessionInfoTab;
    wxControlWithItems                  * const aSessionList;
    int                                         aSessionCount;
};

#endif // TSESSIONLIST_H
