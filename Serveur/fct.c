//=============================================================================
// Projet : Winrell (server)
// Fichier : fct.c (fonctions)
//
//=============================================================================

//#include "server.h" // d�ja inclus dans "exploreTypes.h"
#include "exploreTypes.h"
#include "commands.h"


//=============================================================================
//                    Fonction d'affichage sur la RichConsole
//
//=============================================================================

void appendToRichConsole(char *title, const char *buffer)
{
     char *temp = get_time(2), chaine[strlen(temp)+strlen(title)+strlen(buffer)+8];
     
     sprintf(chaine, "[%s] %s : %s\n", temp, title, buffer);
     free(temp);
     /* Sauvegarde + affichage */
     enregistrerDansLog(chaine, fichierLog, FALSE);
     if (!HIDE_CONSOLE_MSG)
        afficherSurRichConsole(chaine);
     //if ((!enregistrerDansLog(chaine, fichierLog, FALSE) || !lireLogEtAfficherSurRichConsole(fichierLog)))
     //{
        //MessageBox(hwFenetre, "Erreur de sauvegarde/affichage !", "Fermeture de " NOM_APP, MB_OK | MB_ICONWARNING);
        //exit(EXIT_FAILURE);
     //}
}

//=============================================================================
//         Fonction qui enregistre les donn�es dans le fichier sp�cifi�
//
//=============================================================================

BOOL enregistrerDansLog(char *log, char *fichier, BOOL ecraserAncien)
{
   HANDLE hFichier;
   DWORD FileSize, nbcharRead;
   
   FileSize = lstrlen(log);
   if (ecraserAncien)
      hFichier = CreateFile(fichier, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   else
      hFichier = CreateFile(fichier, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFichier != INVALID_HANDLE_VALUE)
   {
      /* Mettre le pointeur du fichier � la fin */
      if (!ecraserAncien)
         SetFilePointer(hFichier, 0, NULL, FILE_END);
      /* Ecrire la lettre dans le fihier log */
      WriteFile(hFichier, log, FileSize, &nbcharRead, NULL);
      /* Fermer le fichier */
      CloseHandle(hFichier);
      return TRUE;
   }
   else
      return FALSE;
}

//=============================================================================
//             Fonction qui lit les donn�es du fichier sp�cifi�
//                     et affiche sur la RichConsole
//
//=============================================================================

BOOL lireLogEtAfficherSurRichConsole(char *fichier)
{
   HANDLE hFichier;
   DWORD FileSize, nbcharRead;
   CHAR *buffer;

   hFichier = CreateFile(fichier, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFichier != INVALID_HANDLE_VALUE) /* Si fichier trouv� */
   {
      FileSize = GetFileSize(hFichier, NULL);
      buffer = (PCHAR)LocalAlloc(LMEM_FIXED, FileSize+1);
      ReadFile(hFichier, buffer, FileSize, &nbcharRead, NULL) ; /* Lecture */
      buffer[FileSize] = 0;
      SendMessage(hwRichConsole, WM_SETTEXT, 0, (LPARAM)buffer); /* Affichage � l'�cran */
      if (REFRESH_CONSOLE)
      {
         changerCouleurRichEdit(hwRichConsole, TextColor); // couleur du texte
         //SendMessage(hwRichConsole, WM_HSCROLL, SB_LEFT, 0); /* On se positionne � gauchhe */
      }
      SendMessage(hwRichConsole, WM_VSCROLL, SB_BOTTOM, 0); /* On descend � la derni�re ligne */
      LocalFree(buffer);
      CloseHandle(hFichier);
      return TRUE;
   }
   else
      return FALSE;
}

//=============================================================================
//      Fonction qui affiche les donn�es sp�cifi�es sur la RichConsole
//
//=============================================================================

void afficherSurRichConsole(char *texteAafficher)
{
   int old_len = GetWindowTextLength(hwRichConsole), len = old_len + lstrlen(texteAafficher) + 1;
   char *texte = malloc(len);
   
   lstrcpy(texte, ""); /* Initialisation */
   if (old_len > 0)
      SendMessage(hwRichConsole, WM_GETTEXT, old_len+1, (LPARAM)texte); /* On r�cup�re le texte existant dans la RichConsole */
   lstrcat(texte, texteAafficher); /* On lui ajoute le nouveau texte */
   
   SendMessage(hwRichConsole, WM_SETTEXT, 0, (LPARAM)texte); /* Affichage � l'�cran */
   
   /* On raffraichie la couleur du texte (pour que �a fonctionne sur Wine) */
   if (REFRESH_CONSOLE)
   {
      changerCouleurRichEdit(hwRichConsole, TextColor); // couleur du texte
      //SendMessage(hwRichConsole, WM_HSCROLL, SB_LEFT, 0); /* On se positionne � gauchhe */
   }
   
   /* Colorisation */
   richConsoleColorization(texte, len);
   
   free(texte);
   
   SendMessage(hwRichConsole, WM_VSCROLL, SB_BOTTOM, 0); /* On descend � la derni�re ligne */
}

//=============================================================================
//      Fonction qui r�nitialise la couleur du texte de la RichConsole
//
//=============================================================================

void resetRichConsoleTextColor()
{
   int len = GetWindowTextLength(hwRichConsole) + 1;
   char *texte = malloc(len);
   
   GetWindowText(hwRichConsole, texte, len);
   
   /* On raffraichie la couleur du texte (pour que �a fonctionne sur Wine) */
   changerCouleurRichEdit(hwRichConsole, TextColor); // couleur du texte
   
   /* Colorisation */
   richConsoleColorization(texte, len);
   
   free(texte);
   
   SendMessage(hwRichConsole, WM_VSCROLL, SB_BOTTOM, 0); /* On descend � la derni�re ligne */
}

//=============================================================================
//          Fonction qui s'occupe de colorier la RichConsole
//
//=============================================================================

void richConsoleColorization(char *texte, int len)
{
   if (ENABLE_COLORIZATION)
   {
      /* Colorisation */
      CHARRANGE CurrentSelection;
      CHARRANGE Selection;
      int i, lines = 0;
   
      /* On r�cup�re la s�lection courante afin de la r�tablir apr�s le traitement */
      SendMessage(hwRichConsole, EM_EXGETSEL, 0, (LPARAM) &CurrentSelection);
   
      for (i = 0; i < len - 1; i++)
      {
         // send | recv
         if ((texte[i] == 's' && texte[i+1] == 'e' && texte[i+2] == 'n' && texte[i+3] == 'd') || (texte[i] == 'r' && texte[i+1] == 'e' && texte[i+2] == 'c' && texte[i+3] == 'v'))
         {
            // Indice de d�but (cpMin) et un indice de fin (cpMax) de la s�lection
            Selection.cpMin = i - lines;
            Selection.cpMax = Selection.cpMin+4; // 4 lettres > send | recv
            // On marque le mot avec sa couleur
            changerCouleurSelectionRichEdit(hwRichConsole, Selection, texte[i] == 's' ? SEND_COLOR : RECV_COLOR);
         }
         // error
         /*else if (texte[i] == 'e' && texte[i+1] == 'r' && texte[i+2] == 'r' && texte[i+3] == 'o' && texte[i+4] == 'r')
         {
            // Indice de d�but (cpMin) et un indice de fin (cpMax) de la s�lection
            Selection.cpMin = i - lines;
            Selection.cpMax = Selection.cpMin+5; // 5 lettres > error
            // On marque le mot avec sa couleur
            changerCouleurSelectionRichEdit(hwRichConsole, Selection, ERROR_COLOR);
         }*/
         else if (texte[i] == '\n')
            lines++;
      }
      
      /* Restauration de la s�lection */
      SendMessage(hwRichConsole, EM_EXSETSEL, 0, (LPARAM) &CurrentSelection);
   }
}

//=============================================================================
//                 Fonction d'ajout de client(s) � la listView
//
//=============================================================================

void ajouterALaListView(char *texte, int tailleTexte, char *ip)
{
   int i = 0, j = 0, itemPos;
   char user[100], machine[100];
   LVITEM lvi;
   
   ZeroMemory(&lvi, sizeof(LV_ITEM));
   lvi.mask = LVIF_TEXT | LVIF_IMAGE; //LVIF_PARAM
   
   while (texte[i] != '-' && i < tailleTexte) /* Si le caract�re actuel n'est pas un '-' */
   {
      machine[j] = texte[i]; /* On r�cup�re le nom de la machine */
      i++;
      j++;
   }
   machine[j] = '\0'; /* Fin du nom (machine) */
   j = 0;
   for (i++; i < tailleTexte; i++)
   {
      user[j] = texte[i]; /* On r�cup�re le sujet du rappel */
      j++;
   }
   user[j] = '\0'; /* Fin du nom (machine) */
   
   /* On remplie la listView */
   
   /* user */
   if (lstrlen(user) <= 0) lstrcpy(user, "-");
   lvi.iItem = actualClientsNumber; /* Emplacement d'insersion (0 == tout en haut de la liste) */
   lvi.iSubItem = 0;
   //lvi.lParam = LVM_SORTITEMS;
   if (!strcmp(ip, "127.0.0.1")) // si c'est un client local
       lvi.iImage = 0;
   else
       lvi.iImage = 1;
   lvi.pszText = user;
   itemPos = ListView_InsertItem(hwClientsList, &lvi); /* Position actuelle dans la ListView */
   
   /* machine */
   if (lstrlen(machine) <= 0) lstrcpy(machine, "-");
   lvi.iItem = itemPos;
   lvi.iSubItem = 1;
   lvi.pszText = machine;
   ListView_SetItem(hwClientsList, &lvi);
   
   /* ip */
   if (lstrlen(ip) <= 0) lstrcpy(ip, "-");
   lvi.iItem = itemPos;
   lvi.iSubItem = 2;
   lvi.pszText = ip;
   ListView_SetItem(hwClientsList, &lvi);

   appendToRichConsole("ip", ip); // on sauvegarde et affiche l'ip aussi
   
   /* connexion time */
   char *time = get_time(2);
   lvi.iItem = itemPos;
   lvi.iSubItem = 3;
   lvi.pszText = time;
   ListView_SetItem(hwClientsList, &lvi);
   free(time);
   
   /* R�organise la taille des colones. */
   //ListView_SetColumnWidth(hwClientsList, 0, LVSCW_AUTOSIZE);
   //ListView_SetColumnWidth(hwClientsList, 1, LVSCW_AUTOSIZE);
}

//=============================================================================
//                  Fonction qui r�cup�re le temp actuel
//
//=============================================================================

char *get_time(short format)
{
   char *temp;
   temp = malloc(32);
   SYSTEMTIME Time;
   GetSystemTime(&Time);
   if (format == 2)
      sprintf(temp, "%02d:%02d:%02d", Time.wHour, Time.wMinute, Time.wSecond);
   else
      sprintf(temp, "[%02d-%02d-%d](%02d-%02d-%02d)", Time.wDay, Time.wMonth, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond);
   return temp;
}

//=============================================================================
//     Fonction qui change la couleur de la selection dans l'edit sp�cifi�
//
//=============================================================================

void changerCouleurRichEdit(HWND hEdit, COLORREF couleur)
{
   /* Format du texte */
   CHARFORMAT2 Format;
   ZeroMemory(&Format, sizeof(CHARFORMAT2));
   Format.cbSize = sizeof(CHARFORMAT2);
   Format.crTextColor = couleur; /* La couleur a utilis� */
   Format.dwEffects = CFE_BOLD; /* Gras */
   Format.dwMask = CFM_BOLD | CFM_COLOR;
   /* On indique au Rich Edit que l'on va utiliser le format : Format */
   SendMessage(hEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&Format);
}

//=============================================================================
//     Fonction qui change la couleur de la selection dans l'edit sp�cifi�
//
//=============================================================================

void changerCouleurSelectionRichEdit(HWND hEdit, CHARRANGE Selection, COLORREF couleur)
{
   /* Formatage du texte */
   CHARFORMAT2 Format;
   ZeroMemory(&Format, sizeof(CHARFORMAT2));
   Format.cbSize = sizeof(CHARFORMAT2);
   Format.crTextColor = couleur; /* La couleur a utilis� */
   Format.dwMask = CFM_COLOR;	/* Le formatage va concerner la couleur du texte */
   /* On indique au Rich Edit que l'on va utiliser la s�lection : Selection */
   SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM) &Selection);
   /* On indique au Rich Edit que l'on va utiliser le format : Format */
   SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &Format);
}

//=============================================================================
//                Fonction qui v�rifie la version de l'os
//
//=============================================================================

BOOL verifierOsWindowsXPorLater()
{
    OSVERSIONINFO osvi;
    BOOL bIsWindowsXPorLater;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    bIsWindowsXPorLater = ( (osvi.dwMajorVersion > 5) || ( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1) ));

    if(bIsWindowsXPorLater)
        return TRUE;
    else
       return FALSE;
}

//=============================================================================
//      Fonction qui affiche un ballon dans la TrayIcon (la notification)
//
//=============================================================================

void afficherTrayIconBallon(char *titre, char *contenu, unsigned int temp, BOOL icone)
{
   lstrcpy(TrayIcon.szInfoTitle, titre); /* Titre */
   lstrcpy(TrayIcon.szInfo, contenu); /* Contenu du ballon */
   TrayIcon.uTimeout = temp;  /* Temp d'attente */
   if (icone)
      TrayIcon.dwInfoFlags = NIIF_INFO;
   else
      TrayIcon.dwInfoFlags = NIIF_NONE;
   Shell_NotifyIcon(NIM_MODIFY, &TrayIcon); /* Modification */
}

//=============================================================================
//       Fonction qui r�cup�re le nom du programme depuis son chemin
//
//=============================================================================

char *recupNomDuProgramme(char *path)
{
   char *nomDuProgramme, nomInverser[200];
   nomDuProgramme = malloc(strlen(path));

   /* On r�cup�re le nom (invers�) */
   int i, j;
   for (i = 0, j = strlen(path) - 1; j >= 0; i++, j--)
   {
      if (path[j] != '\\')
         nomInverser[i] = path[j];
      else
         break;
   }
   nomInverser[i] = '\0';
   /* On inverse le nom */
   for (i = 0, j = strlen(nomInverser) - 1; j >= 0; i++, j--)
   {
       nomDuProgramme[i] = nomInverser[j];
   }
   nomDuProgramme[i] = '\0';
   
   return nomDuProgramme;
}

//=============================================================================
//              Fonction qui cr�e les colones de la ListView
//=============================================================================

void creerListViewColone(HANDLE htmp, UINT largeurColone, char *nomColone, UINT idColone)
{
   LVCOLUMN lvc;
   lvc.mask = LVCF_TEXT | LVCF_WIDTH;
   lvc.cx = largeurColone;
   lvc.pszText = nomColone;
   ListView_InsertColumn(htmp, idColone, &lvc);         
}

//=============================================================================
//          Fonction de remplissage de la liste des commandes
//
//=============================================================================

void remplirListeDesCommandes(HWND hListe)
{
   int i, itemPos;
   char cmdNbr[3];
   
   for (i = 0; i < (sizeof cmd / sizeof *cmd); i++)
   {
      LVITEM lvi;
      ZeroMemory(&lvi, sizeof(LV_ITEM));
      lvi.mask = LVIF_TEXT | LVIF_PARAM;
      lvi.iItem = i;
      lvi.lParam = LVM_SORTITEMS;
      
      itemPos = ListView_InsertItem(hListe, &lvi); /* Position actuelle dans la ListView */
      
      sprintf(cmdNbr, "%d", i + 1);
      ListView_SetItemText(hListe, itemPos, 0, cmdNbr); /* Num�ro de commande */
      ListView_SetItemText(hListe, itemPos, 1, cmd[i].nom); /* cmdPrefixe/nom */
      ListView_SetItemText(hListe, itemPos, 2, cmd[i].param1); /* param1 */
      ListView_SetItemText(hListe, itemPos, 3, cmd[i].param2); /* param2 */
      ListView_SetItemText(hListe, itemPos, 4, cmd[i].param3); /* param3 */
      ListView_SetItemText(hListe, itemPos, 5, cmd[i].param4); /* param4 */
      ListView_SetItemText(hListe, itemPos, 6, cmd[i].param5); /* param5 */
      ListView_SetItemText(hListe, itemPos, 7, cmd[i].param6); /* param6 */
   }
}

//=============================================================================
//    Fonction qui se charge de v�rifier l'existence du fichier sp�cifi� 
//
//=============================================================================

BOOL verifierExistenceFichier(char *path)
{
   FILE *Fichier = NULL;
   
   Fichier = fopen(path, "r");
   
   if (Fichier != NULL)
   {
      fclose(Fichier);
      return TRUE;
   }
   else
      return FALSE;     
}

//=============================================================================
//               Fonction qui cr�e des tooltips/bules d'infos
//=============================================================================

void createTooltipFor(HWND hButton, HWND hwnd, HWND hTTip, char *contenu)
{ 
   RECT rect;
   TOOLINFO ti;
   
   GetClientRect(hwnd, &rect); 
   ti.cbSize = sizeof(TOOLINFO); 
   ti.uFlags = TTF_SUBCLASS; 
   ti.hwnd = hButton;
   ti.hinst = hInst; 
   ti.uId = 0; 
   ti.lpszText = contenu; 
   ti.rect.left = rect.left; 
   ti.rect.top = rect.top; 
   ti.rect.right = rect.right; 
   ti.rect.bottom = rect.bottom; 
   SendMessage(hTTip, TTM_ADDTOOL, 0, (LPARAM) &ti);
}

//=============================================================================
// Fonction qui retourne TRUE si la chaine pass� en parametre est num�rique, si nn FALSE
//=============================================================================

BOOL isFullyNumeric(char *string)
{
    BOOL isNumeric = TRUE; // on suppose que la chaine contient/est un nombre num�rique
    int i;
    
    // on parcourt la chaine
    for (i = 0; i < strlen(string); i++)
    {
        // si on trouve un seul caract�re non num�rique
        if (string[i] != '0' && string[i] != '1' && string[i] != '2' && string[i] != '3' && string[i] != '4' && string[i] != '5' &&
            string[i] != '6' && string[i] != '7' && string[i] != '8' && string[i] != '9')
        {
           isNumeric = FALSE; // la chaine n'est pas num�rique
           break; // on sort de la boucle
        }
    }
    
    return isNumeric;
}

//=============================================================================
//      Fonction qui renvoie l'extension du fichier pass� en parametre
//
// @ si c'est un dossier la valeur "" est affect� � ptExt
//=============================================================================

void getExtension(char *f_name_with_path_or_not, char *ptExt)
{
     char f_name[200];
     
     // si le nom du fichier fourni contient son chemin aussi, on extrait le nom
     if (strstr(f_name_with_path_or_not, "\\") != NULL)
     {
         char *name = recupNomDuProgramme(f_name_with_path_or_not); // extraction du nom du fichier
         lstrcpy(f_name, name); // affectation du nouveau nom
         free(name); // lib�ration du pointeur name
     }
     else // si nn, le nom du fichier fourni ne contient pas son chemin
         lstrcpy(f_name, f_name_with_path_or_not); // affectation du nouveau nom
     
     // si c'est un dossier (son nom ne contient aucun point)
     // @@ il se peut qu'un nom de dossier contient un point, mais ce n'est pas un probl�me, on traite ce cas apr�s en bas
     // @@ le dernier petit conflit qui reste i�i, c'est que si le fichier n'a pas d'extension, il sera consid�r� comme un dossier
     // , et on peux rien y faire, � part reconstruire tout le syst�me qu'on vien de b�tir, et c'est pas de n�tre faute, mais pour
     // s'assurer que c'est un fichier non pas un dossier c'est simple, on ouvre la fen�tre d'informations et on trouvera qu'il a bien
     // une taille (n�tre fichier masqu� en dossier) :pp, et si jamais vous voulez le t�l�charger utilisez la ligne de commande ;))
     if (strstr(f_name, ".") == NULL)
     {
         lstrcpy(ptExt, ""); // on affecte une chaine vide
         return; // on sort de la fonction
     }
     
     // si nn
     int i, j = 0;
     char extension[6] = ""; // extension .xxxxxx maximum, 6 caract�re maxi (osef du point, on ne l'inclus pas)
     
     // on r�cup�re l'extension
     for (i = strlen(f_name) - 1; i >=0 && j < 6; i--)
     {
        if (f_name[i] == '.')
           break; // on sort de la boucle
        else
           extension[j] = f_name[i];
           
        j++;
     }
     
     extension[j] = 0; // ou '\0', fin de chaine
     
     if (i < 0) i = 0; // si on a parcouru tout le nom (f_name), i sera == -1 (il ne faut pas utiliser un indice de tableau n�gatif)
     // si on n'a pas trouv� le point qui indique que c'est une extension, c'est plut�t un dossier alors
     // ou bien si l'extension ne contient que des chiffres, ce n'est pas une extension alors
     // c'est plutot un nom de dossier qui contient un/des points
     if (f_name[i] != '.' || isFullyNumeric(extension))
     {
         lstrcpy(ptExt, ""); // on affecte une chaine vide
         return; // on sort de la fonction
     }
     
     // maintenant on doit inv�rss� ptExt car l'extension a �t� �crite � l'envers
     // et c'est normal, on a commenc� de la fin quand on a parcouru f_name
     i = 0;
     for (j = strlen(extension) - 1; j >= 0; j--)
     {
         ptExt[i] = extension[j];
         i++;
     }
     
     ptExt[i] = 0; // fin de chaine
}

//=============================================================================
// Fonction qui renvoie l'indice de l'exploreType du fichier/dossier pass� en parametre
//=============================================================================

unsigned short getExploreType(char *f_name, BOOL isDir)
{
    int i, j = 0;
    
    char ext[6] = ""; // si c'est un dossier => ext == ""
    
    // si ce n'est pas un dossier
    if (! isDir)
    {
        // on r�cup�re l'extension
        getExtension(f_name, ext);
    }
    
    // @ si c'est un dossier ext sera == "" (� une chaine vide)
    // et �a coincide exactement avec ce qu'on a dans le tableau exploreTypes
    
    // on parcourt le tableau exploreTypes
    for (i = 0; i < (sizeof exploreTypes / sizeof *exploreTypes); i++)
    {
        if (!strcmp(ext, exploreTypes[i].extension)) // si on identifie l'extension
           return i;
    }
    
    // si on arrive i�i c'est que l'extension n'a pa �t� identifi� => c'est un fichier inconnu alors, on renvoie son indice
    return 1;
}

//=============================================================================
// Fonction qui retourne l'id de l'ic�ne du type de fichier pass� en parametre
//=============================================================================

int getExploreIcon(char *description)
{
    int i;
    
    // on parcourt le tableau exploreTypes
    for (i = 0; i < (sizeof exploreTypes / sizeof *exploreTypes); i++)
    {
        if (!strcmp(description, exploreTypes[i].description)) // si on identifie le type/description
           return exploreTypes[i].iconeId;
    }
    
    // si jamais on ne trouve pas le type (m�me si �a ne devrais pas arriver) => on renvoie l'icone d'un fichier inconnu alors
    return exploreTypes[1].iconeId;
}

//=============================================================================
//                 Fonction d'ajout � l'explore listView
//
//=============================================================================

void addToExploreListView(char *nom, int itemPos, BOOL isDir)
{
   LVITEM lvi;
   ZeroMemory(&lvi, sizeof(LV_ITEM));
   lvi.mask = LVIF_TEXT | LVIF_IMAGE; //LVIF_PARAM
   /* Cr�ation de l'image liste */
   HICON hIcon;
   HIMAGELIST hExploreImgList = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , 14, 14); // 14 == max icones
   ImageList_SetBkColor(hExploreImgList, GetSysColor(COLOR_WINDOW));
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DRIVE));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_FOLDER));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_FILE));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TEXT_FILE));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_IMAGE));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SON));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_VIDEO));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_EXE));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DOC_WORD));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DOC_EXCEL));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DOC_POWERPOINT));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DOC_ACCESS));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ZIP_RAR));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PDF));
   ImageList_AddIcon(hExploreImgList, hIcon);
   // @@ si vous ajoutez des ic�nes � l'imageList, changez le nombre max d'image en haut (6�me ligne apr�s l'accolade de la fonction)
   // on lui associe l'image liste
   ListView_SetImageList(hwListeExploration, hExploreImgList, LVSIL_SMALL);
   
   /* nom */
   int tailleNom = strlen(nom);
   short indice = -1;
   
   lvi.iItem = itemPos; /* Emplacement d'insersion (0 == tout en haut de la liste) */
   lvi.iSubItem = 0;
   if (nom[tailleNom - 1] == '\\') // si c'est un drive/partition
      lvi.iImage = 0;
   else // c'est un fichier ou dossier alors
   {
      // on r�cup�re l'exploreType (l'indice de l'extension "qu'on va extraire du nom" dans le tableau exploreTypes)
      indice = getExploreType(nom, isDir);
      
      if (exploreTypes[indice].iconeId == IDI_FOLDER)
         lvi.iImage = 1; // dossier => indice 1
      else if (exploreTypes[indice].iconeId == IDI_FILE)
         lvi.iImage = 2; // fichier (inconnu) => indice 2
      else if (exploreTypes[indice].iconeId == IDI_TEXT_FILE)
         lvi.iImage = 3; // fichier texte => indice 3
      else if (exploreTypes[indice].iconeId == IDI_IMAGE)
         lvi.iImage = 4; // image => indice 4
      else if (exploreTypes[indice].iconeId == IDI_SON)
         lvi.iImage = 5; // son => indice 5
      else if (exploreTypes[indice].iconeId == IDI_VIDEO)
         lvi.iImage = 6; // vid�o => indice 6
      else if (exploreTypes[indice].iconeId == IDI_EXE)
         lvi.iImage = 7; // ex� => indice 7
      else if (exploreTypes[indice].iconeId == IDI_DOC_WORD)
         lvi.iImage = 8; // ex� => indice 8
      else if (exploreTypes[indice].iconeId == IDI_DOC_EXCEL)
         lvi.iImage = 9; // ex� => indice 9
      else if (exploreTypes[indice].iconeId == IDI_DOC_POWERPOINT)
         lvi.iImage = 10; // ex� => indice 10
      else if (exploreTypes[indice].iconeId == IDI_DOC_ACCESS)
         lvi.iImage = 11; // ex� => indice 11
      else if (exploreTypes[indice].iconeId == IDI_ZIP_RAR)
         lvi.iImage = 12; // ex� => indice 12
      else if (exploreTypes[indice].iconeId == IDI_PDF)
         lvi.iImage = 13; // ex� => indice 13
   }
   lvi.pszText = nom;
   ListView_InsertItem(hwListeExploration, &lvi);
   
   /* type */
   lvi.iItem = itemPos;
   lvi.iSubItem = 1;
   lvi.pszText = indice == -1 ? "Partition" : exploreTypes[indice].description;
   ListView_SetItem(hwListeExploration, &lvi);
   
   /* extension */
   char ext[6] = ""; // si c'est une partition => ext == ""
   lvi.iItem = itemPos;
   lvi.iSubItem = 2;
   if (indice != -1)
   {
      // si c'est un fichier inconnu (?)
      if (! lstrcmp(exploreTypes[indice].extension, "?"))
         getExtension(nom, ext); // on r�cup�re son extension r�elle
      else
         lstrcpy(ext, exploreTypes[indice].extension);
   }
   lvi.pszText = ext;
   ListView_SetItem(hwListeExploration, &lvi);
}

//=============================================================================
//        Fonction qui d�coupe le r�sultat selon le separateur donn�
//                  et le rajoute dans l'exploreListView
//=============================================================================

BOOL setExploreResult(char *result, char separateur, BOOL isDrive)
{
   int len = strlen(result);
   /*
   if (len == 0)
      return FALSE;
   */
   
   int i, j = 0, count = 0;
   char nom[256];
   
   // on vide la listeView (si elle est d�j� vide aucun prob.)
   ListView_DeleteAllItems(hwListeExploration);
   
   // on d�coupe le r�sultat
   for (i = 0; i < len; i++)
   {
       if (result[i] == separateur)
       {
           nom[j] = 0; // ou '\0'
           if (isDrive)
           {
               addToExploreListView(nom, count, FALSE);
           }
           else
           {
               addToExploreListView(nom, count, result[i+1] == '1' ? TRUE : FALSE);
               i += 2; // on saute les caract�res : 0| ou 1|
           }
           j = 0; // pour un nouveau nom
           count++;
       }
       else
       {
           nom[j] = result[i];
           j++;
       }
   }
   
   // On affiche le nombre de r�sultat sur la StatusBar
   sprintf(nom, "%d �l�ment(s) trouv�(s)", count); // on utilise 'nom' temporairement i�i
   SetWindowText(hwExploreStatusBar, nom);
   
   if (count > 0)
      return TRUE;
   else
      return FALSE;
}

//=============================================================================
//            Fonction qui retourne la taille du fichier sp�cif�
//=============================================================================

long tailleFichier(char *fichier)
{
   HANDLE hFichier;
   DWORD FileSize;
   
   hFichier = CreateFile(fichier, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFichier != INVALID_HANDLE_VALUE) /* Si fichier trouv� */
   {
       FileSize = GetFileSize(hFichier, NULL);
       CloseHandle(hFichier);
       return FileSize;
   }
   else
       return 0;
}

//=============================================================================
//  Fonction qui se charge de v�rifier l'existence et lire le fichier sp�cifi� 
//=============================================================================

BOOL lireFichier(char *fichier, char *buffer)
{
   HANDLE hFichier;
   DWORD FileSize, nbcharRead;
   //CHAR *buffer;
   
   hFichier = CreateFile(fichier, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFichier != INVALID_HANDLE_VALUE) /* Si fichier trouv� */
   {
      FileSize = GetFileSize(hFichier, NULL);
      //buffer = (PCHAR)LocalAlloc(LMEM_FIXED, FileSize+1);
      ReadFile(hFichier, buffer, FileSize, &nbcharRead, NULL) ; /* Lecture */
      buffer[FileSize] = 0;
      //LocalFree(buffer);
      CloseHandle(hFichier);
      return TRUE;
   }
   else
      return FALSE;
}

//=============================================================================
//     Fonction qui tranche la derni�re partie d'un path � chaque appel 
//=============================================================================

BOOL pathBack(char *path)
{
     int i, pathLen = strlen(path);
     
     for (i = pathLen - 1; i >= 0; i--)
     {
         if (path[i] == '\\')
         {
            // si 'C:\\xxxxxx' ou 'C:\\'
            if (path[i - 1] == ':')
            {
               if (i == pathLen - 1) // si juste 'C:\\'
                  return FALSE; // on ne peux rien trancher (pour ne pas avoir d'erreur)
               else // si nn si 'C:\\xxxxxx'
                  path[i + 1] = 0; // on enl�ve juste les 'xxxxxx'
            }
            else // si nn si 'C:\\xxxx\\xxxxx'
               path[i] = 0; // ou '\0' (on tranche le path, on enl�ve les derniers '\\xxxxx')
            
            return TRUE;
         }
     }
     
     return FALSE;
}

//=============================================================================
//      Fonction qui change la couleur des lignes/rows d'une listView
//=============================================================================

BOOL customdraw_handler(HWND hwnd, WPARAM wParam, LPARAM lParam, COLORREF color)
{
    LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

    switch (pnm->hdr.code)
    {
        case NM_CUSTOMDRAW:
        {
            LPNMLVCUSTOMDRAW  lplvcd;
            lplvcd = (LPNMLVCUSTOMDRAW)lParam;

            switch(lplvcd->nmcd.dwDrawStage)
            {
                case CDDS_PREPAINT :
                        SetWindowLong(hwnd, DWL_MSGRESULT, CDRF_NOTIFYITEMDRAW); // Comment this line out if this is not a dialog box
                        return CDRF_NOTIFYITEMDRAW;
                    break;
                case CDDS_ITEMPREPAINT:
                    {
                        int row; 
                        row = lplvcd->nmcd.dwItemSpec;
               
                        if((row + 1) % 2 == 0)
                            lplvcd->clrTextBk = color; // change the text background color
                    
                        return CDRF_NEWFONT;
                    }
                    break;
            } // fin 2eme switch
        }
        break;
    }
    
return FALSE;
}

//=============================================================================
//                 Fonction d'ajout � la tasks listView
//
//=============================================================================

void addToTasksListView(char *task, int itemPos)
{
   LVITEM lvi;
   ZeroMemory(&lvi, sizeof(LV_ITEM));
   lvi.mask = LVIF_TEXT | LVIF_IMAGE; //LVIF_PARAM
   /* Cr�ation de l'image liste */
   HICON hIcon;
   HIMAGELIST hTasksImgList = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , 1, 1); // 1 == max icones
   ImageList_SetBkColor(hTasksImgList, GetSysColor(COLOR_WINDOW));
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_EXE));
   ImageList_AddIcon(hTasksImgList, hIcon);
   // on lui associe l'image liste
   ListView_SetImageList(hwTaskList, hTasksImgList, LVSIL_SMALL);
   
   /* task */
   lvi.iItem = itemPos; // Emplacement d'insersion (0 == tout en haut de la liste)
   lvi.iSubItem = 0;
   lvi.iImage = 0;
   lvi.pszText = task;
   ListView_InsertItem(hwTaskList, &lvi);
   
   /* type */
   char type[26] = "-";
   
   if (!lstrcmp(task, "System") || !lstrcmp(task, "svchost.exe") ||
       !lstrcmp(task, "explorer.exe") || !lstrcmp(task, "taskhost.exe") ||
       !lstrcmp(task, "taskmgr.exe") || !lstrcmp(task, "winlogon.exe") ||
       !lstrcmp(task, "dwm.exe"))
       lstrcpy(type, "System");
   
   lvi.iItem = itemPos;
   lvi.iSubItem = 1;
   lvi.pszText = type;
   ListView_SetItem(hwTaskList, &lvi);
}

//=============================================================================
//        Fonction qui d�coupe le r�sultat selon le separateur donn�
//                  et le rajoute dans la TasksListView
//=============================================================================

BOOL setTasksResult(char *result, char separateur)
{
   int len = strlen(result);
   /*
   if (len == 0)
      return FALSE;
   */
   
   int i, j = 0, count = 0;
   char task[256];
   
   // on vide la listeView (si elle est d�j� vide aucun prob.)
   ListView_DeleteAllItems(hwTaskList);
   
   // on d�coupe le r�sultat
   for (i = 0; i < len; i++)
   {
       if (result[i] == separateur)
       {
           task[j] = 0; // ou '\0'
           addToTasksListView(task, count);
           j = 0; // pour un nouveau nom
           count++;
       }
       else
       {
           task[j] = result[i];
           j++;
       }
   }
   
   // On affiche le nombre de r�sultat sur la StatusBar
   sprintf(task, "%d task(s) trouv�(s)", count); // on utilise 'task' temporairement i�i
   SetWindowText(hwTaskListStatusBar, task);
   
   if (count > 0)
      return TRUE;
   else
      return FALSE;
}

//=============================================================================
//     Fonction qui retourne l'index du client dont l'id est sp�cifi�
//
// @ retourne -1 si client non trouv�
//=============================================================================

int getClientIndexById(int clientId)
{
   int i;
   
   for (i = 0; i < actualClientsNumber; i++)
   {
       if (clients[i].id == clientId) // si notre client est toujours connect� on trouvera son id
           return i; // on retourne l'index
   }
       
   // si nn c vraiment notre client qui s'est d�connect�
   return -1;
}

//=============================================================================
//     Fonction qui met � jour l'index du client dont l'id est sp�cifi�
//=============================================================================

int updateClientIndexById(int clientId, int currentClientIndex, int *oldClientsNumber)
{
   /*
   // Si un client vient de se d�connecter (le nombre de clients � changer)
   if ((*oldClientsNumber) > actualClientsNumber)
   {
      (*oldClientsNumber) = actualClientsNumber; // on met � jour le nombre de clients aussi
      return getClientIndexById(clientId);
   }
   else if ((*oldClientsNumber) < actualClientsNumber) // Si nn si un client vient de se connecter
   {
       (*oldClientsNumber) = actualClientsNumber; // on met � jour le nombre de clients
       return currentClientIndex;
   }
   else // si nn si le nombre de clients n'a pas chang�
      return currentClientIndex;
   */
   
   // par contre dans le else, si jamais le client d�co et qu'un autre reco sur place ou lui m�me 
   // avant d'appeler cette fonction/avant de faire l'update, l'ancien nombre de client sera
   // �gal au nombre de client actuel et du coup on aura un faux id, c vrai que l'index pourra
   // �tre juste si on a un seul client mais l'id nan, ce qui va g�n�rer une erreur au prochain update.
   // De toute fa�on la possibilit� que �a arrive n'est pas trop grande, et puis la plus simple des solutions
   // serait d'utiliser un return getClientIndexById(clientId); au lieu de tout ce code, ce qui veux dire
   // que cette fonction ne sera plus n�cessaire, on pourra la supprimer, mais bon, j'ai pas envie de faire �a
   // pour l'instant, si jamais quelqu'un lit ce que j'ai �crit, garde en t�te cette possibilit�, voila ;))
   
   return getClientIndexById(clientId);
}

//=============================================================================
//     Fonction qui retourne l'index du client dont le handle est sp�cifi�
//
// @ retourne -1 si client non trouv�
//=============================================================================

int getClientIndexByShowScreenWindowHwnd(HWND hwnd)
{
    int i;
   
    for (i = 0; i < actualClientsNumber; i++)
    {
        if (clients[i].show_screen_infos.hwnd == hwnd) // si on trouve le handle
            return i; // on retourne l'index
    }
    
    // si nn si le client s'est d�connect�
    return -1;
}

//=============================================================================
//                   Fonction d'exploration des fichiers
//=============================================================================

BOOL browseForFile(HWND hwnd, char *title, char *selectedFilePath)
{
   OPENFILENAME ofn;
   CHAR szFile[MAX_PATH]={0};

   ZeroMemory(&ofn, sizeof(OPENFILENAME));
   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFile = szFile;
   ofn.nMaxFile = MAX_PATH;
   ofn.lpstrFilter = "Tout les fichiers (*.*)\0*.*\0";
   ofn.nFilterIndex = 1;
   ofn.lpstrInitialDir = APP_PATH;
   ofn.lpstrTitle = title;
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

   if (GetOpenFileName(&ofn) == TRUE)
   {
      lstrcpy(selectedFilePath, szFile);
      SetCurrentDirectory(APP_PATH); // on rend le r�pertoire par d�faut � celui de d�part (pour �viter que les fichiers re�us soit mit ailleur)
      return TRUE;
   }
   
   return FALSE;
}

//=============================================================================
//        Fonction qui d�coupe le r�sultat selon le separateur donn�
//          et le rajoute au control de la fen�tre Update Config
//=============================================================================

BOOL setConfigResult(char *result, char separateur)
{
   int len = strlen(result);
   /*
   if (len == 0)
      return FALSE;
   */
   
   int i, j = 0, count = 0;
   char parametre[256];
   
   // on d�coupe le r�sultat
   for (i = 0; i <= len; i++)
   {
       if (result[i] == separateur || i == len)
       {
           parametre[j] = 0; // ou '\0'
           j = 0; // pour un nouveau nom
           i++; // pour d�passer le '\n' qui vient apr�s un '\r'
           
           // On remplie les controls de la fen�tre
           switch (count)
           {
               case 0:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_EMAIL), parametre);
                    break;
               case 1:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_SERVEUR), parametre);
                    break;
               case 2:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_PORT), parametre);
                    break;
               case 3:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_TEMP_RECEPTION), parametre);
                    break;
               case 4:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_CURRENT_DIR), parametre);
                    break;
               case 5:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_TASK_NAME), parametre);
                    break;
               case 6:
                    if (parametre[0] == '1')
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_RUN_ON_BOOT), CB_SETCURSEL, 0, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_RUN_ON_BOOT), 1);
                       // 1 i�i est non 0 (le vrai indice) car Run On Boot n'est pas pr�c�d� d'un Stop qui indique la n�gation comme
                       // les autres options qui suivent
                    }
                    else
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_RUN_ON_BOOT), CB_SETCURSEL, 1, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_RUN_ON_BOOT), 0);
                    }
                    break;
               case 7:
                    if (parametre[0] == '1')
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_ESPION), CB_SETCURSEL, 0, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_ESPION), 0);
                    }
                    else
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_ESPION), CB_SETCURSEL, 1, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_ESPION), 1);
                    }
                    break;
               case 8:
                    if (parametre[0] == '1')
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_SENDER), CB_SETCURSEL, 0, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_SENDER), 0);
                    }
                    else
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_SENDER), CB_SETCURSEL, 1, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_SENDER), 1);
                    }
                    break;
               case 9:
                    if (parametre[0] == '1')
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_ESPIONMDP), CB_SETCURSEL, 0, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_ESPIONMDP), 0);
                    }
                    else
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_ESPIONMDP), CB_SETCURSEL, 1, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_ESPIONMDP), 1);
                    }
                    break;
               case 10:
                    if (parametre[0] == '1')
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_CLIENT), CB_SETCURSEL, 0, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_CLIENT), 0);
                    }
                    else
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_CLIENT), CB_SETCURSEL, 1, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_CLIENT), 1);
                    }
                    break;
               case 11:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_VERSION), parametre);
                    break;       
           }
           count++;
       }
       else
       {
           parametre[j] = result[i];
           j++;
       }
   }
   
   if (count > 0)
      return TRUE;
   else
      return FALSE;
}


//=============================================================================
// Fonction qui affiche l'image correspandante � la s�l�ction de la combobox sp�cifi�
//=============================================================================

BOOL setComboBoxIcon(HWND hwComboBox, int currentSel)
{
    if (currentSel == 1)
    {
       SendMessage(hwComboBox, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_TRUE), IMAGE_ICON, 12, 12, 0));
       return TRUE;
    }
    else
    {
       SendMessage(hwComboBox, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_FALSE), IMAGE_ICON, 12, 12, 0));
       return FALSE;
    }
}

//=============================================================================
// Fonction qui convertie une taille/size en chaine de caract�re significatif
//=============================================================================

void sizeToString(long size, char *buffer)
{
    char stringSize[100];
    
    if (size / 1024 > 0)
    {
        if (size / 1048576 > 0)
            sprintf(stringSize, "%.2f Mo", (double)size / 1048576);
        else
            sprintf(stringSize, "%.2f Ko", (double)size / 1024);
    }
    else
        sprintf(stringSize, "%d byte", size); // byte en anglais == octet
        
    lstrcpy(buffer, stringSize);
}

//=============================================================================
//   Fonction qui retourne le pourcentage de progression (p = x * 100 / max)
//=============================================================================

int getIntProgress(long totalSize, ssize_t currentSize)
{
    return (double)currentSize * 100 / totalSize;
}

//=============================================================================
//   Fonction qui calcule la diff�rence entre 2 size donn�e et retourne TRUE si la diff. == 1 Mo
//=============================================================================

BOOL diffMoreThanOneMegabits(ssize_t currentSize, ssize_t sizeSave)
{
    // if the difference equal or more than 1 m�gabits
    if ((currentSize - sizeSave) / 1048576 >= 1) // conversion octet/byte => Mo
       return TRUE;
    else
       return FALSE;
}

//=============================================================================
// Fonction qui enl�ve le nom du program et son extension pour extraire son path
//=============================================================================

void extractPath(char *path)
{
     int i, pathLen = strlen(path);
     
     for (i = pathLen - 1; i >= 0; i--)
     {
         if (path[i] == '\\')
         {
             path[i] = 0; // ou '\0' (on tranche le path, on enl�ve les derniers '\\xxxxx.exe')
             return;
         }
     }
}

//=============================================================================
// Fonction qui active/d�sactive les controls/buttons pass� en tableau selon la valeur sp�cifi�e
//=============================================================================

void enableDisableButtons(HWND hwTable[], unsigned short tableSize, BOOL value) // ou bien HWND *hwTable
{
     int i;
     
     for (i = 0; i < tableSize; i++)
         EnableWindow(hwTable[i], value);
}

//=============================================================================
// Fonction qui retourne TRUE si le boutton n'est pas le boutton Delete, si nn FALSE
//=============================================================================

BOOL isNotDelete(int btnId)
{
     if (btnId == IDB_NEW_FOLDER || btnId == IDB_RENAME || btnId == IDB_COPY || btnId == IDB_CUT || btnId == IDB_PASTE)
         return TRUE;
     else
         return FALSE;
}

//=============================================================================
//        Fonction qui d�coupe le r�sultat selon le separateur donn�
//              et le rajoute dans la fen�tre d'informations
//=============================================================================

BOOL setInformationsResult(char *result, char separateur, char premierSeparateur)
{
   int len = strlen(result);
   /*
   if (len == 0)
      return FALSE;
   */
   
   int i = 0, j = 0, count = 0;
   char param[100];
   
   // on d�passe le premier s�parateur
   while (result[i] != premierSeparateur)
       i++;
   
   // on d�coupe le r�sultat
   i++; // pour sauter/commencer apr�s le premier s�parateur
   for (i; i <= len; i++)
   {
       if (i == len || result[i] == separateur)
       {
           param[j] = 0; // ou '\0'
           // Traitement des parametres
           switch (count)
           {
               case 0: // Taille
                 {
                    long f_size = atol(param);
                    if (f_size != -1) // si ce n'est pas : un dossier ou bien le chemin est introuvable
                    {
                       sizeToString(f_size, param);
                       SetDlgItemText(hwFenInformations, TXT_INFO_F_SIZE, param);
                    }
                 }
                    break;
               case 1: // Attribut System
                    if (param[0] == '1')
                       CheckDlgButton(hwFenInformations, IDC_INFO_F_SYSTEM, BST_CHECKED);
                    break;
               case 2: // Attribut Cach�
                    if (param[0] == '1')
                       CheckDlgButton(hwFenInformations, IDC_INFO_F_HIDDEN, BST_CHECKED);
                    break;
           }   
           j = 0; // pour un nouveau parametre
           count++;
       }
       else
       {
           param[j] = result[i];
           j++;
       }
   }
   
   if (count > 0)
      return TRUE;
   else
      return FALSE;
}

//=============================================================================
// Fonction qui v�rifie le client sp�cifi� et affiche les messages d'erreur appropri�s
//=============================================================================

BOOL checkClient(HWND hDlg, int *clientIndex, int clientId, BOOL updateClientIndex, BOOL checkSendRecvFileToo, int idButton)
{
    // si on doit mettre � jour l'index du client
    if (updateClientIndex)
    {
        // On met � jour l'index de notre client, @@ car la fen�tre/ou un MessageBox peut rester ouvert longtemp, et le client pourra d�co pendant ce temp
        //clientIndex = updateClientIndexById(clientId, clientIndex, &clientsNumberSave);
        (*clientIndex) = getClientIndexById(clientId);
        
        // S'il n'y a plus de client dont on est entrain d'explorer le drive (fichiers/dossiers)
        if ((*clientIndex) == -1)
        {
            MessageBox(hDlg, "Le client s'est d�connect� !", NOM_APP, MB_OK | MB_ICONWARNING);
            /* Activation du boutton associ� � la fen�tre � la fermeture */
            if (idButton > 0) EnableWindow(GetDlgItem(hwFenetre, idButton), TRUE);
            EndDialog(hDlg, 0); // fermeture de la fen�tre 
            return FALSE;
        }
    }
    
    // si on doit v�rifier si le client est en train d'envoyer ou recevoir un fichier
    if (checkSendRecvFileToo)
    {
       // v�rifications
       if (clients[(*clientIndex)].send_file)
       {
           MessageBox(hDlg, "Le client est en train d'envoyer un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
           return FALSE;
       }
       else if (clients[(*clientIndex)].recv_file)
       {
           MessageBox(hDlg, "Le client re�oit un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
           return FALSE;
       }
    }
    
    return TRUE; // aucune erreur lors des v�rifications
}

//=============================================================================
//                 Fonction d'ajout � la ToDo listView
//
//=============================================================================

void addToToDoListView(char *id, char *commmande, char *time, int itemPos)
{
   LVITEM lvi;
   ZeroMemory(&lvi, sizeof(LV_ITEM));
   lvi.mask = LVIF_TEXT | LVIF_IMAGE; //LVIF_PARAM
   /* Cr�ation de l'image liste */
   HICON hIcon;
   HIMAGELIST hToDoImgList = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , 1, 1); // 1 == max icones
   ImageList_SetBkColor(hToDoImgList, GetSysColor(COLOR_WINDOW));
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TODO));
   ImageList_AddIcon(hToDoImgList, hIcon);
   // on lui associe l'image liste
   ListView_SetImageList(hwToDoList, hToDoImgList, LVSIL_SMALL);
   
   /* id */
   lvi.iItem = itemPos; // Emplacement d'insersion (0 == tout en haut de la liste)
   lvi.iSubItem = 0;
   lvi.iImage = 0;
   lvi.pszText = id;
   ListView_InsertItem(hwToDoList, &lvi);
   
   /* commande */
   lvi.iItem = itemPos;
   lvi.iSubItem = 1;
   lvi.pszText = commmande;
   ListView_SetItem(hwToDoList, &lvi);
   
   /* time */
   lvi.iItem = itemPos;
   lvi.iSubItem = 2;
   lvi.pszText = time;
   ListView_SetItem(hwToDoList, &lvi);
}

//=============================================================================
//        Fonction qui d�coupe le r�sultat selon le separateur donn�
//                  et le rajoute dans la ToDoListView
//=============================================================================

BOOL setToDoResult(char *result, char separateur)
{
   int len = strlen(result);
   /*
   if (len == 0)
      return FALSE;
   */
   
   int i, j = 0, count = 0;
   char id[10];
   char commande[BUF_SIZE];
   char time[10];
   BOOL GET_ID = FALSE, GET_TIME = FALSE, GET_COMMANDE = FALSE;
   
   // on vide la listeView (si elle est d�j� vide aucun prob.)
   ListView_DeleteAllItems(hwToDoList);
   
   // on d�coupe le r�sultat
   for (i = 0; i <= len; i++)
   {
       // si c'est le d�but (s�parateur d'id '[') ou la fin
       if (result[i] == '[' || i == len)
       {
           if (GET_COMMANDE)
           {
               GET_COMMANDE = FALSE;
               commande[j] = 0; // ou '\0'
               addToToDoListView(id, commande, time, count);
               j = 0;
               count++;
           }
           
           GET_ID = TRUE;
       }
       // si nn, si c'est la fin de l'id ']'
       else if (result[i] == ']' && GET_ID) // && GET_ID == TRUE)
       {
           id[j] = 0; // ou '\0'
           j = 0;
           GET_ID = FALSE;
           GET_TIME = TRUE;
       }
       // si nn, si c'est la fin du temps restant avant ex�cution/time
       else if (result[i] == separateur && GET_TIME) // == '|' && GET_TIME == TRUE)
       {
           time[j] = 0; // ou '\0'
           j = 0;
           GET_TIME = FALSE;
           GET_COMMANDE = TRUE;
       }
       else
       {
           if (GET_ID)
               id[j] = result[i];
           else if (GET_TIME)
               time[j] = result[i];
           else //else if (GET_COMMANDE)
               commande[j] = result[i];
           j++;
       }
   }
   
   // On affiche le nombre de r�sultat sur la StatusBar
   sprintf(commande, "%d ToDo(s) trouv�(s)", count); // on utilise 'commande' temporairement i�i
   SetWindowText(hwToDoListStatusBar, commande);
   
   if (count > 0)
      return TRUE;
   else
      return FALSE;
}

//=============================================================================
// Fonction qui v�rifie les commandes et affiche les messages d'erreur appropri�s
//=============================================================================

BOOL checkCommande(HWND hDlg, char *commande)
{
    // Si la v�rification est d�sactiv�e => return TRUE
    if (! CHECK_COMMANDS) return TRUE;
    
    int i = 0;
    char cmdPrefixe[100];
    
    // On r�cup�re le prefixe/nom de la commande
    sscanf(commande, "%s", cmdPrefixe);
    
    for (i = 0; i < (sizeof cmd / sizeof *cmd); i++)
    {
        // On v�rifie le pr�fixe de la commande d'abord
        if (!lstrcmp(cmdPrefixe, cmd[i].nom)) // si trouv�
        {
            // On r�cup�re le nombre de param�tre par d�faut de la commande
            int paramNbr = (!lstrcmp(cmd[i].param1, "NULL") ? 0 : 
                           ((!lstrcmp(cmd[i].param2, "NULL") || cmd[i].param2[0] == '|') ? 1 : 
                           ((!lstrcmp(cmd[i].param3, "NULL") || cmd[i].param3[0] == '|') ? 2 : 
                           ((!lstrcmp(cmd[i].param4, "NULL") || cmd[i].param4[0] == '|') ? 3 : 
                           ((!lstrcmp(cmd[i].param5, "NULL") || cmd[i].param5[0] == '|') ? 4 : 
                           ((!lstrcmp(cmd[i].param6, "NULL") || cmd[i].param6[0] == '|') ? 5 : 6
                           ))))));
            // On compte un param�tre tout ce qui est diff�rent de NULL ou ne commence pas par un s�parateur '|'
            // NB: le 1er param�tre ne peut pas commencer par un s�parateur
            
            // On v�rifie le nombre de param�tres
            if (strCharOccur(commande, ' ') == paramNbr) // si c'est bon
            {
                return TRUE;
            }
            else
            {
                //MessageBox(hDlg, "Le nombre de param�tre de cette commande est incorrect !", NOM_APP, MB_OK | MB_ICONWARNING);
                appendToRichConsole("erreur", "Le nombre de param�tre de cette commande est incorrect !");
                return FALSE;
            }
        }
    }
    
    //MessageBox(hDlg, "Cette commande n'existe pas !", NOM_APP, MB_OK | MB_ICONWARNING);
    appendToRichConsole("erreur", "Cette commande n'existe pas !");
    
    return FALSE;
}

//=============================================================================
//  Fonction qui renvoie le nombre d'occurence d'un caractere dans une chaine
//=============================================================================

int strCharOccur(char *chaine, char caractere)
{
    int occurence = 0, i;
    
    for (i = 0; i < strlen(chaine); i++)
    {
        if (chaine[i] == caractere)
           occurence++;
    }
    
    return occurence;
}

//=============================================================================
//             Fonction qui active l'onglet (Tab) sp�cifi�
//=============================================================================

void setSettingsTab(HWND hwnd, BOOL statut, unsigned short tab)
{
   if (tab == GENERAL_TAB) /* Onglet G�n�ral (Tab 1) */
   {
      if (statut == SHOW_TAB)
      {
         /* On affiche les �l�ments de l'onglet G�n�ral */
         ShowWindow(GetDlgItem(hwnd, IDC_CRYPT_DECRYPT), SW_SHOW);
         ShowWindow(GetDlgItem(hwnd, IDC_CHECK_COMMANDS), SW_SHOW);
         ShowWindow(GetDlgItem(hwnd, IDC_ENABLE_COLORIZATION), SW_SHOW);
      }
      else
      {
         /* On cache les �l�ments de l'onglet G�n�ral */
         ShowWindow(GetDlgItem(hwnd, IDC_CRYPT_DECRYPT), SW_HIDE);
         ShowWindow(GetDlgItem(hwnd, IDC_CHECK_COMMANDS), SW_HIDE);
         ShowWindow(GetDlgItem(hwnd, IDC_ENABLE_COLORIZATION), SW_HIDE);
      }
   }
   else if (tab == EXPLORE_DRIVE_TAB) /* Explore Drive (Tab 2) */
   {
      if (statut == SHOW_TAB)
      {
         /* On affiche les �l�ments de l'onglet Explore Drive */
         ShowWindow(GetDlgItem(hwnd, IDC_SHOW_EXPLORE_TYPE), SW_SHOW);
         ShowWindow(GetDlgItem(hwnd, IDC_SHOW_EXPLORE_EXT), SW_SHOW);
      }
      else
      {
         /* On cache les �l�ments de l'onglet Explore Drive */
         ShowWindow(GetDlgItem(hwnd, IDC_SHOW_EXPLORE_TYPE), SW_HIDE);
         ShowWindow(GetDlgItem(hwnd, IDC_SHOW_EXPLORE_EXT), SW_HIDE);
      }    
   }
}
