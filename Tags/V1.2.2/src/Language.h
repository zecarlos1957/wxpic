///////////////////////////////////////////////////////////////////////////////
// Name:        Language.cpp
// Purpose:     Manage the Language selection for an application
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.03.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#ifndef LANGUAGE_H_INCLUDED
#define LANGUAGE_H_INCLUDED

#include <wx/arrstr.h>
#include <wx/intl.h>
#include "HelpCtrl.h"

#define HELP_DEFAULT_DIR_NAME     (_T("Help"))

//------------------------------------------------------------
//-- Manage the Language files and the Help Language
//-- The translation files must be in the form:
//-- Win32: <ApplicationDirectory>/Lang/<LanguageCode>/<ApplicationName>.mo
//-- Linux: <ApplicationDirectory>/../share/local/<LanguageCode>/LC_MESSAGES/<ApplicationName>.mo
//-- with <LanguageCode> a valid language code such as sp or fr_FR
//------------------------------------------------------------

class TLanguage
{
public:
    //-- This initialisation method must be called first
    //-- Return true upon success
    static bool                 Init                (void);

    //-- At very beginning of application execution (before any call to wxGetTranslation or GetHelpController)
    //-- Specify the language by its natural name.
    //-- It defines the value of <Language_Code> by receiving the corresponding name (French for fr_FR or else fr)
    //-- If the given LangageName is empty, the system default language is tried
    //-- It returns the language name if a translation file corresponding to the language name has been found
    //-- else it returns an empty string
    static void                 SetLanguage         (wxString &pLanguageName);

    //-- Set the Help using the given base directory and the language setup by SetLanguage
    //-- (so it should have been called before)
    //-- The pHelpDefDir is the default help directory that is used
    //-- if no current language Help Dir in the form <pHelpDefDir>/<currentLanguage> is found
    //-- The map file wxHelp must be present in any Help directory
    //-- If the provided directory path is empty it is replaced the Help SubDir of the application executable directory
    //-- Returns true on success, false on failure.
    static bool                 SetHelp             (const wxString &pHelpDefDir = wxEmptyString);

    //-- Return the best Help controller found for the set Language
    //-- (or NULL when no Help data found or not set by SetHelp)
    static wxHelpControllerBase*GetHelpController   (void) { return (theSingleton->aIsHelpValid) ? &(theSingleton->aHelpController) : NULL; };

    //-- Return the list of defined languages
    //-- All the returned values in the table can be succesfully passed to SetLanguage
    static wxArrayString        GetLanguageNameList (void);

    //-- Retrieve the text of the given file (no directory given)
    //-- in the current language if available else in the default language
    //-- These files are supposed to be written in the locale encoding associated to the language
    static wxString             GetFileText         (const wxString &pFilename) { return theSingleton->doGetFileText(pFilename); };

private:
    /**/ TLanguage     (void);
    void     setLanguage   (wxString   &pLanguageName);
    void     doSetLanguage (int pLanguage);
    bool     try2Languages (int pLanguage);
    bool     tryLanguage   (const wxString   &pLanguage);
    bool     tryHelpDir    (const wxString   &aDirName);
    wxString doGetFileText (const wxString   &pFilename);

    static TLanguage       *theSingleton;

    int                     aLanguageTab[wxLANGUAGE_USER_DEFINED];
    int                     aLanguageCount;
    int                     aLanguageSet;
    wxString                aLanguageFile;
    wxLocale                aLocale;
    wxBrowserHelpController aHelpController;
    bool                    aIsHelpValid;
};

#endif // LANGUAGE_H_INCLUDED
