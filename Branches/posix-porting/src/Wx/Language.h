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
#include <wx/filename.h>
#include "HelpCtrl.h"

#define HELP_DEFAULT_DIR_NAME     (_T("Help"))
#define LANGUAGE_DEFAULT_DIR_NAME (_T("Lang"))

class TLanguage
{
public:
    //-- Yet before calling SetLanguage, GetLanguageNameList, ...
    //-- This initialisation method must be called to define the place where the translation files and help files are
    //-- If the pLanguageDefDir parameter is <Directory>/<Filename>.<Ext>
    //-- The translation files must be in the form
    //-- <Directory>/<LanguageCode>/<Filename>.mo with <LanguageCode> a valid language code such as sp or fr_FR
    //-- If the provided directory path is empty it is replaced
    //-- by the application executable dir and Name with additional directory /Lang in between
    static bool                 SetLangDefDir       (const wxFileName &pLanguageDefDir = wxFileName());

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
    static void                 SetHelp             (const wxString &pHelpDefDir = wxEmptyString);

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
    /**/ TLanguage     (const wxFileName &pLanguageDefFile);
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
    wxFileName              aLanguageDefFile;
    wxLocale                aLocale;
    wxBrowserHelpController aHelpController;
    bool                    aIsHelpValid;
};

#endif // LANGUAGE_H_INCLUDED
