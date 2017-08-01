//=============================================================================
// Projet : WinSk
// Fichier : gui.c
//
//=============================================================================

#include "client.h"


//=============================================================================
//                              Constantes
//=============================================================================

/* Couleurs des textes sp�ciaux de la RichConsole */
#define TEXT_COLOR            RGB(255, 255, 255)
#define OK_COLOR              RGB(0, 255, 0)
#define WARNING_COLOR         RGB(255, 242, 0)
#define DETECTED_COLOR        RGB(255, 127, 39)
#define KO_COLOR              RGB(255, 0, 0)

//=============================================================================
//                          Procedure de la fenetre
//=============================================================================

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    static HWND hwCommandeLine;
    static HWND hwButtonSend, hwBtClearConsole;
    static COLORREF BkEditcolor = RGB(0, 0, 0); /* couleur noir */
    static COLORREF TextColor = TEXT_COLOR; //RGB(255, 255, 255); /* couleur blanche */
    static HFONT policeButton;
    
    switch (uMsg)
    {
       case WM_CREATE:
            {
            /* Cr�ation des controls. */
            /* RichConsole */
            hwRichConsole = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, ""
             , WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_DISABLENOSCROLL | WS_VSCROLL | ES_READONLY
             , 10, 10, 530, 280, hwnd, NULL, hInst, NULL);
            SendMessage(hwRichConsole, EM_SETBKGNDCOLOR, 0, BkEditcolor); // couleur de fond noir
            /* CommandeLine et autres */
            hwCommandeLine = CreateWindow("edit", "", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
                                           10, 300, 400, 25, hwnd, (HMENU)IDC_COMMANDELINE, hInst, NULL);
            oldProc = (WNDPROC) SetWindowLong(hwCommandeLine, GWL_WNDPROC, (LONG)editProc); /* gestion des evennements de la commande line */
            PostMessage(hwCommandeLine, EM_LIMITTEXT, BUF_SIZE, 0); /* limite du textBox */
            hwButtonSend = CreateWindow("button", "Send", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                        420, 300, 80, 25, hwnd, (HMENU) IDB_SEND, hInst, NULL);
            /* Cls (Clear Console) */
            hwBtClearConsole = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        510, 300, 30, 25, hwnd, (HMENU) IDB_CLEAR_CONSOLE, hInst, NULL);
            SendMessage(hwBtClearConsole, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_CMD), IMAGE_ICON, 16, 16, 0));
            /* Police des controls */
            policeButton = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");
            SendMessage(hwRichConsole, WM_SETFONT, (long)policeButton, 0);
            SendMessage(hwButtonSend, WM_SETFONT, (long)policeButton, 0);
            SendMessage(hwBtClearConsole, WM_SETFONT, (long)policeButton, 0);
            SendMessage(hwCommandeLine, WM_SETFONT, (long)policeButton, 0);
            /* Format & couleur du texte */
            //changerCouleurRichEdit(hwRichConsole, TextColor);
            /* Cr�ation/Lancement des Threads */
            lancerThreads();
            } 
            break;
       case WM_CLOSE:
            DestroyWindow(hwnd); /* Message pour detruire la fenetre */
            break;
       case WM_COMMAND:
             /* Gestion des actions (clics buttons, menu, ecriture, ...) */
             switch (LOWORD(wParam))
             {
                 case IDB_CLEAR_CONSOLE:
                      {
                      SendMessage(hwRichConsole, WM_SETTEXT, 0, (LPARAM)""); /* Vidage de l'�cran */
                      }
                      break;
                 case IDB_SEND:
                      {
                      // V�rification(s) avant envoie
                      int texteSize = GetWindowTextLength(hwCommandeLine);
                      if (texteSize > 0)
                      {
                          if (CONNECTED_TO_SERVER)
                          {
                              char *texte = (char*)LocalAlloc(LMEM_FIXED, texteSize+1);
                              GetWindowText(hwCommandeLine, texte, texteSize+1);
                              write_server(serverSock, texte);
                              SendMessage(hwCommandeLine, WM_SETTEXT, 0, (LPARAM)"");
                          }
                          else
                          {
                              appendToRichConsole("CONNECTED TO SERVER", CONNECTED_TO_SERVER ? "OK" : "KO");
                          }
                      }
                      else
                      {
                          appendToRichConsole("Text field is empty", "");
                      }
                      SetFocus(hwCommandeLine);
                      }
                      break;
             }
             break;
        case WM_DESTROY:
             /* Destruction des Threads et de la section critique */
             CloseHandle(hThread[0]);
             CloseHandle(hThread[1]);
             CloseHandle(hThread[2]);
             CloseHandle(hThread[3]);
             //CloseHandle(hThread[4]);
             /* Destruction de la fenetre */
             PostQuitMessage(0);
             break;
        default:
             return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    return 0;
}

//=============================================================================
//         Proc�dure qui g�re les �vennements de la commande line
//                         (appui sur entr�...)
//=============================================================================

LRESULT CALLBACK editProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   int id = GetDlgCtrlID(hwnd);
   
   if (IDC_COMMANDELINE == id)
   {
      switch (message)
      {
   	     case WM_KEYDOWN:
            if (wParam == VK_RETURN)
            {
               // Gestion de la touche entr�e.
               PostMessage(hwFenetre, WM_COMMAND, IDB_SEND, 0); // simmule l'appui sur le boutton send
            }
         break;
      }
   }
   
   return CallWindowProc(oldProc, hwnd, message, wParam, lParam);
}

//=============================================================================
//                    Fonction d'affichage sur la RichConsole
//
//=============================================================================

void appendToRichConsole(char *title, const char *buffer)
{
     char *temp = get_time(2), chaine[strlen(temp)+strlen(title)+strlen(buffer)+8];
     
     sprintf(chaine, "[%s] %s %s %s\n", temp, title, strlen(buffer) > 0 ? ":" : "", buffer);
     free(temp);
     /* affichage */
     afficherSurRichConsole(chaine);
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
   changerCouleurRichEdit(hwRichConsole, TEXT_COLOR);
   
   /* Colorisation OK/KO/WARNING/DETECTED */
   CHARRANGE CurrentSelection;
   CHARRANGE Selection;
   int i, lines = 0;
   
   /* On r�cup�re la s�lection courante afin de la r�tablir apr�s le traitement */
   SendMessage(hwRichConsole, EM_EXGETSEL, 0, (LPARAM) &CurrentSelection);
   
   for (i = 0; i < len - 1; i++)
   {
      // OK | KO
      if ((((texte[i] == 'O' && texte[i+1] == 'K') || (texte[i] == 'K' && texte[i+1] == 'O')) && texte[i-1] == ' '))
      {
         // Indice de d�but (cpMin) et un indice de fin (cpMax) de la s�lection
         Selection.cpMin = i - lines;
         Selection.cpMax = Selection.cpMin+2; // 2 lettres > OK |KO
         // On marque le mot avec sa couleur
         changerCouleurSelectionRichEdit(hwRichConsole, Selection, texte[i] == 'O' ? OK_COLOR : KO_COLOR);
      }
      // WARNING
      else if (texte[i] == 'W' && texte[i+1] == 'A' && texte[i+2] == 'R' && texte[i+3] == 'N' && texte[i+4] == 'I' && texte[i+5] == 'N' && texte[i+6] == 'G' && texte[i-1] == ' ')
      {
         // Indice de d�but (cpMin) et un indice de fin (cpMax) de la s�lection
         Selection.cpMin = i - lines;
         Selection.cpMax = Selection.cpMin+7; //Selection.cpMin+strlen("WARNING");
         // On marque le mot avec sa couleur
         changerCouleurSelectionRichEdit(hwRichConsole, Selection, WARNING_COLOR);
      }
      // DETECTED
      else if (texte[i] == 'D' && texte[i+1] == 'E' && texte[i+2] == 'T' && texte[i+3] == 'E' && texte[i+4] == 'C' && texte[i+5] == 'T' && texte[i+6] == 'E' && texte[i+7] == 'D' && texte[i-1] == ' ')
      {
         // Indice de d�but (cpMin) et un indice de fin (cpMax) de la s�lection
         Selection.cpMin = i - lines;
         Selection.cpMax = Selection.cpMin+8; //Selection.cpMin+strlen("DETECTED");
         // On marque le mot avec sa couleur
         changerCouleurSelectionRichEdit(hwRichConsole, Selection, DETECTED_COLOR);
      }
      else if (texte[i] == '\n')
         lines++;
   }
   
   /* Restauration de la s�lection */
   SendMessage(hwRichConsole, EM_EXSETSEL, 0, (LPARAM) &CurrentSelection);
   
   free(texte);
   
   SendMessage(hwRichConsole, WM_VSCROLL, SB_BOTTOM, 0); /* On descend � la derni�re ligne */
}

//=============================================================================
//     Fonction qui change la couleur de tout le texte dans l'edit sp�cifi�
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
//                Fonction qui v�rouille l'interface GUI
//
//=============================================================================

void lockGUI()
{
   EnableWindow(GetDlgItem(hwFenetre, IDB_SEND), FALSE); /* Boutton Send d�sactiv� */
   EnableWindow(GetDlgItem(hwFenetre, IDC_COMMANDELINE), FALSE); /* Ligne de commande d�sactiv�e */
}

//=============================================================================
//                Fonction qui d�v�rouille l'interface GUI
//
//=============================================================================

void unlockGUI()
{
   EnableWindow(GetDlgItem(hwFenetre, IDB_SEND), TRUE); /* Boutton Send activ� */
   EnableWindow(GetDlgItem(hwFenetre, IDC_COMMANDELINE), TRUE); /* Ligne de commande activ�e */
}
