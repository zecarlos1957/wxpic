///////////////////////////////////////////////////////////////////////////////
// Name:        Language.cpp
// Purpose:     Manage the Language selection for an application
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.03.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#include "Language.h"

#include <wx/stdpaths.h>
#include <wx/strconv.h>
#include <wx/file.h>

TLanguage *TLanguage::theSingleton = NULL;

/* static */
bool TLanguage::SetLangDefDir (const wxFileName &pLanguageDefFile)
{
    if (theSingleton != NULL)
        return false;
    theSingleton = new TLanguage(pLanguageDefFile);
    return (theSingleton != NULL) && (theSingleton->aLanguageCount > 0);
}


/* static */
void TLanguage::SetLanguage (wxString &pLanguageName)
{
    theSingleton->setLanguage(pLanguageName);
}

void TLanguage::setLanguage (wxString &pLanguageName)
{
    //-- Si un langage a déjà été défini on ne fait rien
    if (aLanguageSet == wxLANGUAGE_UNKNOWN)
    {
        //-- Index invalide correspondant à pas de langage trouvé
        int Language = aLanguageCount;
        if (!pLanguageName.IsEmpty())
        {
            //-- Si un Langage est demandé on le recherche parmi les langages existants
            for (int i = 0; i < aLanguageCount; ++i)
            {
                Language = aLanguageTab[i];
                if (wxLocale::GetLanguageInfo(Language)->Description == pLanguageName)
                    //-- Le langage à l'index courant a bien le nom demandé
                    //-- On a trouvé le bon index
                    break;
            }
            //-- Si on a pas trouvé de language correspondant au nom donné
            //-- On efface ce nom ce qui signifie pas de language trouvé
            if (Language == aLanguageCount)
                pLanguageName = wxEmptyString;
        }
        //-- Aucun langage n'a été demandé on va tester le langage par défaut du système
        else if (try2Languages (wxLANGUAGE_DEFAULT))
        {
            //-- Le langage par défaut du système est disponible
            //-- C'est lui qui sera défini
            Language = wxLANGUAGE_DEFAULT;
            pLanguageName = wxLocale::GetLanguageInfo(Language)->Description;
        }
        //-- Si le langage est valide on va le définir comme langage courant
        if (!pLanguageName.IsEmpty())
            doSetLanguage(Language);
    }
}


void TLanguage::doSetLanguage (int pLanguage)
{
    //-- Mémoriser le langage courant
    aLanguageSet = pLanguage;
    //-- Définit la locale avec ce langage (encodage et fichier de traduction)
    aLocale.Init(aLanguageSet, wxLOCALE_CONV_ENCODING);
    aLocale.AddCatalog(aLanguageDefFile.GetName());
}


/*static*/
bool TLanguage::SetHelp (const wxString &pHelpDefDir)
{
    //-- Construit le fichier de map par défaut quand aucun fichier n'est spécifié
    	wxString HelpDir = pHelpDefDir;
		if (HelpDir.IsEmpty()){
			wxFileName Temp;
			Temp.Assign(wxStandardPaths::Get().GetExecutablePath());
			Temp.AppendDir(HELP_DEFAULT_DIR_NAME);
			HelpDir = Temp.GetPath();
			}
		if( wxFile::Exists(HelpDir) ){ //Silences error if diretory doesn't exists.
			theSingleton->aIsHelpValid = theSingleton->aHelpController.Initialize(HelpDir);
			return true;
			}
	return false;
}


/* static */
wxArrayString TLanguage::GetLanguageNameList(void)
{
    wxArrayString Result;
    for (int i = 0; i < theSingleton->aLanguageCount; ++i)
    {
        const wxLanguageInfo *LangInfo = wxLocale::GetLanguageInfo(theSingleton->aLanguageTab[i]);
        Result.Add(LangInfo->Description);
    }
    return Result;
}


/**/ TLanguage::TLanguage(const wxFileName &pLanguageDefFile)
: aLanguageCount  (0)
, aLanguageSet    (wxLANGUAGE_UNKNOWN)
, aLanguageDefFile(pLanguageDefFile)
, aIsHelpValid    (false)
{
    //-- Construit le répertoire par défaut quand aucun répertoire de traduction n'est spécifié
    if (aLanguageDefFile.GetPath().IsEmpty())
    {
        aLanguageDefFile.Assign(wxStandardPaths::Get().GetExecutablePath());
        aLanguageDefFile.AppendDir(LANGUAGE_DEFAULT_DIR_NAME);
    }
    //-- Dans tous les cas l'extension des fichiers de traduction est .mo
    aLanguageDefFile.SetExt(_T("mo"));
    //-- Indique que les catalogues doivent être cherchés dans le répertoire de traduction
    aLocale.AddCatalogLookupPathPrefix(aLanguageDefFile.GetPath());

    //-- Passer tous les langages en revue pour savoir s'ils ont leur fichier de traduction
    //-- Si c'est le cas le language est ajouté dans la liste des langages disponibles
    for(int i = 0; i < wxLANGUAGE_USER_DEFINED; ++i)
    {
        if (i == wxLANGUAGE_DEFAULT)
            continue;
        if (try2Languages(i))
            aLanguageTab[aLanguageCount++] = i;
    }
}


bool TLanguage::try2Languages (int pLanguage)
{
    bool                  Result = false;
    const wxLanguageInfo *LangInfo = wxLocale::GetLanguageInfo(pLanguage);

    //-- D'abord le langage doit être valide
    if (LangInfo != NULL)
    {
        //-- Il doit avoir un nom !
        wxString LangTag (LangInfo->CanonicalName);
        Result = !LangTag.IsEmpty();
        if (Result)
        {
            //-- Alors on peut essayer de trouver le fichier de traduction
            Result = tryLanguage(LangTag);
            if (!Result)
            {
                //-- Si on n'a pas trouvé le fichier de traduction
                //-- Et si la langue est une variante nationale
                //-- On peut vérifier si le langage générique existe
                int SubLangPos = LangTag.Find(_T('_'));
                if (SubLangPos != wxNOT_FOUND)
                {
                    //-- Construction du nom du langage générique
                    LangTag.Truncate(SubLangPos);

                    //-- Le langage générique étant généralement générique de plusieurs variante régionale
                    //-- on vérifie si le générique n'a pas déjà été vérifié pour une variante précédente
                    static wxString LastSubLanguage;
                    if (LangTag != LastSubLanguage)
                    {
                        //-- C'est la première fois on peut vérifier la langue générique
                        Result = tryLanguage(LangTag);
                        //-- Mémoriser que ce langague générique a été testé
                        LastSubLanguage = LangTag;
                    }
                }
            }
        }
    }
    return Result;
}


bool TLanguage::tryLanguage (const wxString &pLanguage)
{
    aLanguageDefFile.AppendDir(pLanguage);
    bool Result = aLanguageDefFile.FileExists();
    aLanguageDefFile.RemoveLastDir();
    return Result;
}


wxString TLanguage::doGetFileText (const wxString &pFilename)
{
    wxFileName Filename;
    bool       Found = false;
    wxString   Result;
    wxFile     TextFile;
    char      *FileData;

    Filename = aLanguageDefFile;
    Filename.SetFullName(pFilename);
    if (aLanguageSet != wxLANGUAGE_UNKNOWN)
    {
        //-- Récupération du nom du répertoire de la langue
        wxString LangTag (wxLocale::GetLanguageInfo(aLanguageSet)->CanonicalName);
        Filename.AppendDir(LangTag);
        Found = Filename.FileExists();
        if (!Found)
        {
            Filename.RemoveLastDir();
            int SubLangPos = LangTag.Find(_T('_'));
            if (SubLangPos != wxNOT_FOUND)
            {
                LangTag.Truncate(SubLangPos);
                Filename.AppendDir(LangTag);
                Found = Filename.FileExists();
                if (!Found)
                    Filename.RemoveLastDir();
            }
        }
    }
    if (!Found && Filename.FileExists())
        Found = true;
    if (Found)
    {
        TextFile.Open(Filename.GetFullPath());
        Found = TextFile.IsOpened();
    }
    wxFileOffset Length;
    if (Found)
    {
        Length   = TextFile.Length();
#ifdef _UNICODE
        FileData = (char*)malloc(Length+1);
        Found    = (FileData != NULL);
    }
    if (Found)
    {
        FileData[Length] = '\0';
        if (TextFile.Read(FileData, Length) == Length)
        {
            wchar_t *StringBuf = Result.GetWriteBuf(Length+1);
            if (StringBuf != NULL)
            {
                Length = wxConvLocal.ToWChar(StringBuf, Length+1, FileData);
                if ((int)Length > 0)
                    --Length;
                else
                    Length = 0;
                Result.UngetWriteBuf(Length);
            }
        }
        free(FileData);
#else
        wchar_t *StringBuf = Result.GetWriteBuf(Length+1);
        if (StringBuf != NULL)
        {
            if (TextFile.Read(StringBuf, Length) != Length)
                Length = 0;
            Result.UngetWriteBuf(Length);
        }
#endif
    }
    return Result;
}
