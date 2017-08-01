//=============================================================================
// Projet : Winrell (server)
// Fichier : threads.c
//
//=============================================================================

#include "server.h"


//=============================================================================
//                           Thread : Serveur
//=============================================================================

DWORD  WINAPI Serveur(LPVOID lParam)
{
    init();
   
    app();

    end();
   
    return EXIT_SUCCESS;
}

//=============================================================================
//                           Thread : ShowScreen
//=============================================================================

DWORD  WINAPI ShowScreen(LPVOID lParam)
{
    int clientIndex, clientId, clientsNumberSave;
    char cmd[20]; // 20 carat�res devraient suffir, ex: Send_Pict 70 (faut savoir optimis� quand c'est possible)
    
    // On r�cup�re le param�tre du thread / l'indice/index du client
    int clientIndexParam = (int)lParam;
    
    // Sauvegarde des donn�es du client (indice/index, id, clientsNumber)
    clientIndex = clientIndexParam;
    clientId = clients[clientIndex].id;
    clientsNumberSave = actualClientsNumber;
    
    // Tant que le client ne s'est pas d�connect� et qu'on peut toujours visionner l'�cran du client
    while(clientIndex != -1 && clients[clientIndex].show_screen)
    {
        // Si stop == FALSE + le client ne re�oit rien + le client n'envoie rien + on ne controle pas la sourie, on peut y aller, si nn on ne fait rien � part attendre
        if (!clients[clientIndex].show_screen_infos.stop && !clients[clientIndex].recv_file && !clients[clientIndex].send_file && !clients[clientIndex].show_screen_infos.mouseControl)
        {
            // On pr�pare la commande
            sprintf(cmd, "Send_Pict %d", clients[clientIndex].show_screen_infos.quality);
            
            // On ex�cute la commande
            write_client(&clients[clientIndex], cmd, TRUE);
        }
        
        // On attend le temp donn�
        Sleep(clients[clientIndex].show_screen_infos.speed * 1000);
            
        // mise � jour de l'indice du client (peut �tre que le client ou qu'un client avant nous s'est d�connect�)
        clientIndex = updateClientIndexById(clientId, clientIndex, &clientsNumberSave);
    }
    
    return EXIT_SUCCESS;
}

//=============================================================================
//                        Thread : CheckDoubleClic
//=============================================================================

DWORD  WINAPI CheckDoubleClic(LPVOID lParam)
{
    // R�cup�ration du/des param�tres
    Clic *clic = (Clic *)lParam;
    
    // Petite pause (pour attendre la confirmation du double clic)
    Sleep(300);
    
    // On met � jour l'indice/index du client (peut �tre que le client ou qu'un client avant nous s'est d�connect�)
    (*clic).clientIndex = updateClientIndexById((*clic).clientId, (*clic).clientIndex, &(*clic).clientsNumberSave);
    
    // Si le client est toujours connect�
    if ((*clic).clientIndex != -1) // c'est juste pour la s�curit� et �viter un plantage, vu que le thread ne dure que pour moin d'une seconde
    {
        // Si le nombre de clic est inf�rieur � 2 + la variable doubleClic == FALSE
        if (clients[(*clic).clientIndex].show_screen_infos.clicks < 2 && !clients[(*clic).clientIndex].show_screen_infos.doubleClic)
        {
            char cmd[50];
            
            // construction de la commande
            sprintf(cmd, "L_Mouse_Clic %d %d %d %d", (*clic).pt.x, (*clic).pt.y, (*clic).rc.right - (*clic).rc.left, (*clic).rc.bottom - (*clic).rc.top);
            
            // ex�cution de la commande
            write_client(&clients[(*clic).clientIndex], cmd, TRUE);
                    
            // on appel le thread qui va s'occuper de raffraichir l'�cran
            DWORD dwThreadId;
            HANDLE hThread = CreateThread(NULL, 0, RefreshScreenShow, (LPVOID)clients[(*clic).clientIndex].id, 0, &dwThreadId);
        }
        
        // On r�nitialise doubleClic et clicks
        clients[(*clic).clientIndex].show_screen_infos.doubleClic = FALSE;
        clients[(*clic).clientIndex].show_screen_infos.clicks = 0;
        
        // Si nn c'etait surement un double clic (il sera effectuer directement apr�s la r�c�ption du message WM_LBUTTONDBLCLK)
    }
    
    // On lib�re la structure allou�e pour passer les param�tres
    free(clic);
    
    return EXIT_SUCCESS;
}

//=============================================================================
//                        Thread : RefreshScreenShow
//=============================================================================

DWORD  WINAPI RefreshScreenShow(LPVOID lParam)
{
    int clientId = (int)lParam;
    
    // On attend une demi-seconde
    Sleep(500);
    
    // On r�cup�re l'index du client � partir de son id
    int clientIndex = getClientIndexById(clientId);
    
    // Si le client est toujorus connect�
    if (clientIndex != -1)
    {
        char cmd[20];
        
        // On pr�pare la commande pour raffra�chir l'�cran/le screen show
        sprintf(cmd, "Send_Pict %d", clients[clientIndex].show_screen_infos.quality);
        // On ex�cute la commande
        write_client(&clients[clientIndex], cmd, TRUE);
    }
    
    return EXIT_SUCCESS;
}

//=============================================================================
//                           Thread : Telechargement
//=============================================================================

DWORD  WINAPI Telechargement(LPVOID lParam)
{
    Client *clientActuel = (Client *)lParam;
    int i = recv_file((*clientActuel).id);
    
    return EXIT_SUCCESS;
}

//=============================================================================
//                           Thread : Envoie
//=============================================================================

DWORD  WINAPI Envoie(LPVOID lParam)
{
    Client *clientActuel = (Client *)lParam;
    send_file((*clientActuel).id);
       
    return EXIT_SUCCESS;
}
