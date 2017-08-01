//=============================================================================
// Projet : Winrell (server)
// Fichier : showScreen.c
//
//=============================================================================

#include "server.h"


//=============================================================================
//                            variable globale
//=============================================================================

static int next_btn_id = 400; // D�claration statique

//=============================================================================
//                             ShowScreenProc
//
//=============================================================================

LRESULT CALLBACK ShowScreenProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            // Sauvegarde de l'indice/index du client
            int clientIndex = lvItemActuel;
            // Affichage de l'icone
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_VIDEO)));
            // On indique qu'on visionne l'�cran du client
            clients[clientIndex].show_screen = TRUE;
            // On initialise le reste des param�tres
            clients[clientIndex].show_screen_infos.hwnd = hwnd;
            lstrcpy(clients[clientIndex].show_screen_infos.imagePath, "");
            clients[clientIndex].show_screen_infos.stop = FALSE;
            clients[clientIndex].show_screen_infos.saveImages = FALSE;
            clients[clientIndex].show_screen_infos.speed = 3; // normal
            clients[clientIndex].show_screen_infos.quality = 70; // normal
            clients[clientIndex].show_screen_infos.mouseControl = FALSE;
            clients[clientIndex].show_screen_infos.doubleClic = FALSE;
            clients[clientIndex].show_screen_infos.clicks = 0;
            // Ajout du menu
            AddShowScreenMenu(hwnd, clientIndex);
            // Modification du titre de la fen�tre
            char fenTitle[128];
            sprintf(fenTitle, "Show Screen > %s", clients[clientIndex].name);
            SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)fenTitle);
            // Lancement du thread
            DWORD dwThreadId;
            clients[clientIndex].show_screen_infos.hThread = CreateThread(NULL, 0, ShowScreen, (LPVOID)clientIndex, 0, &dwThreadId);
        }
            break;
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
            SetCapture(hwnd);
            break;
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        {
            // On r�cup�re la position de click
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            RECT rc;
            
            // On v�rifie que le click est � l'int�rieur de la fen�tre non pas ailleur
            GetClientRect(hwnd, &rc);
            if(PtInRect(&rc, pt)) // S'il est vraiment � l'int�rieur de la fen�tre
            {
                // R�cup�ration de l'indice du client
                int clientIndex = getClientIndexByShowScreenWindowHwnd(hwnd);
                
                // Si le client s'est d�connect�
                if (clientIndex == -1)
                {
                    MessageBox(hwnd, "Le client s'est d�connect� !", NOM_APP, MB_OK | MB_ICONWARNING);
                    // Fermeture de la fen�tre
                    EndDialog(hwnd, 0);
                    break;
                }
                
                // Si le contr�le de la sourie est activ�
                if (clients[clientIndex].show_screen_infos.mouseControl)
                {
                    // v�rifications
                    /*
                    if (clients[clientIndex].send_file)
                        MessageBox(hwnd, "Le client est en train d'envoyer un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                    else if (clients[clientIndex].recv_file)
                        MessageBox(hwnd, "Le client re�oit un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                    else
                    {*/
                        // construction de la commande
                        char cmd[50];
                
                        // si c'est un clic droit
                        if (uMsg == WM_RBUTTONUP)
                            sprintf(cmd, "R_Mouse_Clic %d %d %d %d", pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top);
                        // si nn si clic gauche
                        else if (uMsg == WM_LBUTTONUP)
                        {
                            // on incr�mente le nombre de clicks
                            clients[clientIndex].show_screen_infos.clicks++;
                        
                            // Si c'est le premier clic (si c'est le 2�me pas besoin de rappeler le thread)
                            if (clients[clientIndex].show_screen_infos.clicks == 1)
                            {
                                // on pr�pare les param�tres � passer au thread
                                Clic *clic = (Clic *) malloc(sizeof(Clic));
                                (*clic).pt = pt;
                                (*clic).rc = rc;
                                (*clic).clientIndex = clientIndex;
                                (*clic).clientId = clients[clientIndex].id;
                                (*clic).clientsNumberSave = actualClientsNumber;
                        
                                // on appel le thread qui va v�rifier s'il y'a eu un double clic avant de clicker
                                DWORD dwThreadId;
                                HANDLE hThread = CreateThread(NULL, 0, CheckDoubleClic, (LPVOID)clic, 0, &dwThreadId);
                            }
                        
                            break; // pour ne rien ex�cuter (c'est le thread qui le fera pour nous si ce n'est pas un double clic)
                        }
                        // si nn si double clic gauche
                        else if (uMsg == WM_LBUTTONDBLCLK)
                        {
                            // on indique qu'on vien de double click�
                            clients[clientIndex].show_screen_infos.doubleClic = TRUE;
                            sprintf(cmd, "D_L_Mouse_Clic %d %d %d %d", pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top);
                        }
                    
                        // ex�cution de la commande
                        write_client(&clients[clientIndex], cmd, TRUE);
                    
                        // on appel le thread qui va s'occuper de raffraichir l'�cran
                        DWORD dwThreadId;
                        HANDLE hThread = CreateThread(NULL, 0, RefreshScreenShow, (LPVOID)clients[clientIndex].id, 0, &dwThreadId);
                    //} // fin else
                } // fin 2�me if
            } // fin 1er if
            ReleaseCapture();
        }
            break;
        case WM_SIZE:
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case WM_GETMINMAXINFO:
        {
             // set minimum window size
             LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
             lpMMI->ptMinTrackSize.x = 300;
             lpMMI->ptMinTrackSize.y = 300;
        }
        case WM_PAINT:
            {
                // R�cup�ration de l'indice du client
                int clientIndex = getClientIndexByShowScreenWindowHwnd(hwnd);
                
                // Si le client est toujours connect�
                if (clientIndex != -1)
                {
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);
                    
                    // On dessine l'image
                    HANDLE hImage = OpenGraphic(clients[clientIndex].show_screen_infos.imagePath, &clients[clientIndex].show_screen_infos.ImageInfo);
                    DisplayGraphic(hwnd, hdc, &clients[clientIndex].show_screen_infos.ImageInfo);
                    CloseGraphic(hImage, &clients[clientIndex].show_screen_infos.ImageInfo);
                
                    EndPaint(hwnd, &ps);
                }
                else // le client s'est d�connect�
                {// On ne rentrera i�i que si WM_SIZE est d�clench�, car apr�s la r�c�ption de l'image on est s�r que le client est tj connect�
                    if (MessageBox(hwnd, "Le client s'est d�connect� !", NOM_APP, MB_OK | MB_ICONWARNING | DS_CENTER) == IDOK)
                    {
                        // Destruction de la fen�tre (pour eviter les r�p�titions de message)
                        DestroyWindow(hwnd);
                    }
                }
            }
            break;
        case WM_COMMAND:
        {
            // R�cup�ration de l'indice du client
            int clientIndex = getClientIndexByShowScreenWindowHwnd(hwnd);
            
            // Si le client s'est d�connect�
            if (clientIndex == -1)
            {
                MessageBox(hwnd, "Le client s'est d�connect� !", NOM_APP, MB_OK | MB_ICONWARNING);
                // Fermeture de la fen�tre
                EndDialog(hwnd, 0);
                break;
            }
            
            int wparam = LOWORD(wParam);
            
            if (wparam == clients[clientIndex].show_screen_infos.btn_id.stopContinue)
            {
                if (!clients[clientIndex].show_screen_infos.stop)
                {
                    clients[clientIndex].show_screen_infos.stop = TRUE;
                    ModifyMenu(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_BYCOMMAND | MF_STRING, wparam, "Continue");
                }
                else
                {
                    clients[clientIndex].show_screen_infos.stop = FALSE;
                    ModifyMenu(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_BYCOMMAND | MF_STRING, wparam, "Stop");
                }
            }
            else if (wparam == clients[clientIndex].show_screen_infos.btn_id.save)
            {
                int state = GetMenuState(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_BYCOMMAND); 
                
                if (state == MF_CHECKED)
                    CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_UNCHECKED);
                else
                    CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_CHECKED);
                    
                clients[clientIndex].show_screen_infos.saveImages = clients[clientIndex].show_screen_infos.saveImages ? FALSE : TRUE;
            }
            else if (wparam == clients[clientIndex].show_screen_infos.btn_id.close)
            {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            else if (wparam == clients[clientIndex].show_screen_infos.btn_id.speed_fast)
            {
                // On s�l�ctionne Fast
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_CHECKED);
                
                // On d�s�l�ctionne les autres
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.speed_normal, MF_UNCHECKED);
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.speed_slow, MF_UNCHECKED);
            
                // On change le speed
                clients[clientIndex].show_screen_infos.speed = 1;
            }
            else if (wparam == clients[clientIndex].show_screen_infos.btn_id.speed_normal)
            {
                // On s�l�ctionne Normal
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_CHECKED);
                
                // On d�s�l�ctionne les autres
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.speed_fast, MF_UNCHECKED);
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.speed_slow, MF_UNCHECKED);
            
                // On change le speed
                clients[clientIndex].show_screen_infos.speed = 3;
            }
            else if (wparam == clients[clientIndex].show_screen_infos.btn_id.speed_slow)
            {
                // On s�l�ctionne Slow
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_CHECKED);
                
                // On d�s�l�ctionne les autres
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.speed_fast, MF_UNCHECKED);
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.speed_normal, MF_UNCHECKED);
            
                // On change le speed
                clients[clientIndex].show_screen_infos.speed = 5;
            }
            else if (wparam == clients[clientIndex].show_screen_infos.btn_id.quality_high)
            {
                // On s�l�ctionne High
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_CHECKED);
                
                // On d�s�l�ctionne les autres
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.quality_normal, MF_UNCHECKED);
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.quality_low, MF_UNCHECKED);
            
                // On change la qualit�
                clients[clientIndex].show_screen_infos.quality = 100;
            }
            else if (wparam == clients[clientIndex].show_screen_infos.btn_id.quality_normal)
            {
                // On s�l�ctionne Normal
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_CHECKED);
                
                // On d�s�l�ctionne les autres
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.quality_high, MF_UNCHECKED);
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.quality_low, MF_UNCHECKED);
            
                // On change la qualit�
                clients[clientIndex].show_screen_infos.quality = 70;
            }
            else if (wparam == clients[clientIndex].show_screen_infos.btn_id.quality_low)
            {
                 // On s�l�ctionne Low
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_CHECKED);
                
                // On d�s�l�ctionne les autres
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.quality_high, MF_UNCHECKED);
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.quality_normal, MF_UNCHECKED);
            
                // On change la qualit�
                clients[clientIndex].show_screen_infos.quality = 5;
            }
            else if (wparam == clients[clientIndex].show_screen_infos.btn_id.mouse_control_on)
            {
                 // On s�l�ctionne On
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_CHECKED);
                
                // On d�s�l�ctionne Off
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.mouse_control_off, MF_UNCHECKED);
            
                // On change mouseControl
                clients[clientIndex].show_screen_infos.mouseControl = TRUE;
            }
            else if (wparam == clients[clientIndex].show_screen_infos.btn_id.mouse_control_off)
            {
                 // On s�l�ctionne Off
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, wparam, MF_CHECKED);
                
                // On d�s�l�ctionne On
                CheckMenuItem(clients[clientIndex].show_screen_infos.hMenu, clients[clientIndex].show_screen_infos.btn_id.mouse_control_on, MF_UNCHECKED);
            
                // On change mouseControl
                clients[clientIndex].show_screen_infos.mouseControl = FALSE;
            }
        }
            break;
        case WM_CLOSE:
        {
            // R�cup�ration de l'indice du client
            int clientIndex = getClientIndexByShowScreenWindowHwnd(hwnd);
            
            // Si le client est toujours connect�
            if (clientIndex != -1)
            {
                clients[clientIndex].show_screen = FALSE;
                // Destruction du Thread et de la section critique
                CloseHandle(clients[clientIndex].show_screen_infos.hThread);
                // Suppression de la derni�re image re�u
                if (!clients[clientIndex].show_screen_infos.saveImages)
                    DeleteFile(clients[clientIndex].show_screen_infos.imagePath);
            }
            // Fermeture de la fen�tre
            EndDialog(hwnd, 0);
        }
            break;
    }

    return 0;
}

//=============================================================================
//            Fonction qui cr�e la/les dialogBox ShowScreen
//=============================================================================

LRESULT CreateShowScreenDialogBox()
{
   HGLOBAL hmem; //handle m�moire globale
   LPDLGTEMPLATE lpdt; //pointeur sur structure dialog template
   LPWORD lpw; // pointeur sur un WORD
   LPWSTR lpnom; // pointeur sur chaine UNICODE pour le nom de la boite
   LRESULT result;
    
   //allouer de la m�moire  pour notre dialog template
   hmem = GlobalAlloc(GPTR, 512); //GPTR=taille fixe initialis�e � 0
   if (!hmem) return;
                            
   //convertir le handle en pointeur DLGTEMPLATE sur le d�but de la m�moire allou�e  
   lpdt = ( LPDLGTEMPLATE) hmem;
                            
   // D�finir les propri�t�s de la boite de dialogue
   lpdt->dwExtendedStyle = WS_EX_TOPMOST;
   lpdt->style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | DS_CENTER | CS_DBLCLKS;
   lpdt->x = 0; //position x
   lpdt->y = 0; //position y
   lpdt->cx = L_SC_FENETRE; //largeur
   lpdt->cy = H_SC_FENETRE; //hauteur
                            
   //obtenir pointeur juste apr�s la structure DLGTEMPLATE
   lpw = (LPWORD) (lpdt + 1);
                            
   //obtenir pointeur sur la zone du nom de la boite de dialogue
   lpnom = (LPWSTR) lpw+2;
                            
   //convertir le nom en UNICODE et le mettre dans la zone nom
   MultiByteToWideChar (CP_ACP, 0, "Show Screen", -1, lpnom, 128);
                            
   // lancer la boite de dialogue
   result = DialogBoxIndirect(hInst, lpdt, NULL, (DLGPROC)ShowScreenProc);
                            
   // lib�ration de la m�moire allou�e
   GlobalFree(hmem);
   
   return result;
}

//=============================================================================
//         Fonction qui ajoute un menu � la fen�tre ShowScreen
//=============================================================================

void AddShowScreenMenu(HWND hwnd, int clientIndex)
{
    // G�n�ration des ids des bouttons du menu
    clients[clientIndex].show_screen_infos.btn_id.stopContinue = next_btn_id;
    next_btn_id++;
    clients[clientIndex].show_screen_infos.btn_id.save = next_btn_id;
    next_btn_id++;
    clients[clientIndex].show_screen_infos.btn_id.close = next_btn_id;
    next_btn_id++;
    clients[clientIndex].show_screen_infos.btn_id.speed_fast = next_btn_id;
    next_btn_id++;
    clients[clientIndex].show_screen_infos.btn_id.speed_normal = next_btn_id;
    next_btn_id++;
    clients[clientIndex].show_screen_infos.btn_id.speed_slow = next_btn_id;
    next_btn_id++;
    clients[clientIndex].show_screen_infos.btn_id.quality_high = next_btn_id;
    next_btn_id++;
    clients[clientIndex].show_screen_infos.btn_id.quality_normal = next_btn_id;
    next_btn_id++;
    clients[clientIndex].show_screen_infos.btn_id.quality_low = next_btn_id;
    next_btn_id++;
    clients[clientIndex].show_screen_infos.btn_id.mouse_control_on = next_btn_id;
    next_btn_id++;
    clients[clientIndex].show_screen_infos.btn_id.mouse_control_off = next_btn_id;
    next_btn_id++;
    // Cr�ation du menu
    HMENU hMenu, hSousMenu;
    hMenu = CreateMenu();
    // Screen Show
    hSousMenu = CreateMenu();
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.stopContinue, "Stop");
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.save, "Save");
    AppendMenu(hSousMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.close, "Close");
    AppendMenu(hMenu, MF_POPUP, (UINT)hSousMenu, "Screen Show");
    // Speed
    hSousMenu = CreateMenu();
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.speed_fast, "Fast");
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.speed_normal, "Normal");
    CheckMenuItem(hSousMenu, clients[clientIndex].show_screen_infos.btn_id.speed_normal, MF_CHECKED);
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.speed_slow, "Slow");
    AppendMenu(hMenu, MF_POPUP, (UINT)hSousMenu, "Speed");
    // Quality
    hSousMenu = CreateMenu();
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.quality_high, "High");
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.quality_normal, "Normal");
    CheckMenuItem(hSousMenu, clients[clientIndex].show_screen_infos.btn_id.quality_normal, MF_CHECKED);
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.quality_low, "Low");
    AppendMenu(hMenu, MF_POPUP, (UINT)hSousMenu, "Quality");
    // Mouse Control
    hSousMenu = CreateMenu();
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.mouse_control_on, "On");
    AppendMenu(hSousMenu, MF_STRING, clients[clientIndex].show_screen_infos.btn_id.mouse_control_off, "Off");
    CheckMenuItem(hSousMenu, clients[clientIndex].show_screen_infos.btn_id.mouse_control_off, MF_CHECKED);
    AppendMenu(hMenu, MF_POPUP, (UINT)hSousMenu, "Mouse Control");
    
    // Sauvegarde du handle du Menu
    clients[clientIndex].show_screen_infos.hMenu = hMenu;
    
    // Ajout du menu
    SetMenu(hwnd, hMenu);
}

//=============================================================================
//    Fonctions qui d�ssine une image JPG/GIF � l'int�rieur d'une fen�tre
//
// @ 3 fonctions
//=============================================================================

void *OpenGraphic(char *name, IMG_INFO *ImageInfo)
{
  IPicture *Ipic = NULL;
  SIZE sizeInHiMetric,sizeInPix;
  const int HIMETRIC_PER_INCH = 2540;
  HDC hDCScreen = GetDC(NULL);
  HRESULT hr;
  int nPixelsPerInchX = GetDeviceCaps(hDCScreen, LOGPIXELSX);
  int nPixelsPerInchY = GetDeviceCaps(hDCScreen, LOGPIXELSY);
  unsigned short OlePathName[512];

  ReleaseDC(NULL,hDCScreen);
  mbstowcs(OlePathName,name,strlen(name)+1);
  hr = OleLoadPicturePath(OlePathName, NULL, 0, 0, &IID_IPicture, (void *)(&Ipic));
  
  if (hr)
    return 0;
  if (Ipic)
  {
    // get width and height of picture
    hr = Ipic->lpVtbl->get_Width(Ipic,&sizeInHiMetric.cx);
    if (!SUCCEEDED(hr))
      goto err;
    Ipic->lpVtbl->get_Height(Ipic,&sizeInHiMetric.cy);
    if (!SUCCEEDED(hr))
      goto err;

    // convert himetric to pixels
    sizeInPix.cx = (nPixelsPerInchX * sizeInHiMetric.cx +
              HIMETRIC_PER_INCH / 2) / HIMETRIC_PER_INCH;
    sizeInPix.cy = (nPixelsPerInchY * sizeInHiMetric.cy +
              HIMETRIC_PER_INCH / 2) / HIMETRIC_PER_INCH;
    (*ImageInfo).sizeInPix = sizeInPix;
    (*ImageInfo).sizeInHiMetric = sizeInHiMetric;
    (*ImageInfo).Ipic = Ipic;
    (*ImageInfo).Path = name;
    return Ipic;

  }
err:
  return 0;
}

void DisplayGraphic(HWND hwnd,HDC pDC, IMG_INFO *ImageInfo)
{
  IPicture *Ipic = (*ImageInfo).Ipic;
  DWORD dwAttr = 0;
  HBITMAP Bmp,BmpOld;
  RECT rc;
  HRESULT hr;
  HPALETTE pPalMemOld;

  if (Ipic != NULL)
  {
    // get palette
    OLE_HANDLE hPal = 0;
    HPALETTE hPalOld=NULL,hPalMemOld=NULL;
    hr = Ipic->lpVtbl->get_hPal(Ipic,&hPal);

    if (!SUCCEEDED(hr))
      return;
    if (hPal != 0)
    {
      hPalOld = SelectPalette(pDC,(HPALETTE)hPal,FALSE);
      RealizePalette(pDC);
    }

    // Fit the image to the size of the client area. Change this
    // For more sophisticated scaling
    GetClientRect(hwnd,&rc);
    // transparent?
    if (SUCCEEDED(Ipic->lpVtbl->get_Attributes(Ipic,&dwAttr)) || (dwAttr & PICTURE_TRANSPARENT))
    {
      // use an off-screen DC to prevent flickering
      HDC MemDC = CreateCompatibleDC(pDC);
      Bmp = CreateCompatibleBitmap(pDC,(*ImageInfo).sizeInPix.cx,(*ImageInfo).sizeInPix.cy);

      BmpOld = SelectObject(MemDC,Bmp);
      pPalMemOld = NULL;
      if (hPal != 0)
      {
        hPalMemOld = SelectPalette(MemDC,(HPALETTE)hPal, FALSE);
        RealizePalette(MemDC);
      }
/* Use this to show the left corner
      rc.left = rc.top = 0;
      rc.right = ImageInfo.sizeInPix.cx;
      rc.bottom = ImageInfo.sizeInPix.cy;
*/
      // display picture using IPicture::Render
      hr = Ipic->lpVtbl->Render(Ipic,MemDC, 0, 0, rc.right, rc.bottom, 0, (*ImageInfo).sizeInHiMetric.cy,
                               (*ImageInfo).sizeInHiMetric.cx, -(*ImageInfo).sizeInHiMetric.cy, &rc);

      BitBlt(pDC, 0, 0, (*ImageInfo).sizeInPix.cx, (*ImageInfo).sizeInPix.cy, MemDC, 0, 0, SRCCOPY);

      SelectObject(MemDC,BmpOld);

      if (pPalMemOld) SelectPalette(MemDC,pPalMemOld, FALSE);
      DeleteObject(Bmp);
      DeleteDC(MemDC);

    }
    else
    {
      // display picture using IPicture::Render
      Ipic->lpVtbl->Render(Ipic,pDC, 0, 0, rc.right, rc.bottom, 0, (*ImageInfo).sizeInHiMetric.cy,
                          (*ImageInfo).sizeInHiMetric.cx, -(*ImageInfo).sizeInHiMetric.cy, &rc);
    }

    if (hPalOld != NULL) SelectPalette(pDC,hPalOld, FALSE);
    if (hPal) DeleteObject((HPALETTE)hPal);
  }
}

void CloseGraphic(void *Ipict, IMG_INFO *ImageInfo)
{
  IPicture *ip = (IPicture *)Ipict;

  if (ip == NULL)
    ip = (*ImageInfo).Ipic;
  if (ip == NULL)
    return;
  ip->lpVtbl->Release(ip);
  memset(ImageInfo, 0, sizeof((*ImageInfo)));
}
