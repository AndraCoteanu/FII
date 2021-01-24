/*  cod sursa de la care a pornit acest program: 
    https://profs.info.uaic.ro/~computernetworks/files/NetEx/S12/ServerConcThread/servTcpConcTh2.c
    Autorul sursei: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include "fsnoua.h"

/* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
static void *treat(void *); 
void raspunde(void *);

int main ()
{
    /* Declarari de (structuri de) date: */
    struct sockaddr_in server;	// structura folosita de server 
    struct sockaddr_in from;    // structura folosita de server
    int nr, verif_cl, sd, i;		//mesajul primit de trimis la client, descriptorul de socket 
    /* initializare date */
    nr_clienti=0;
    i=0;
    harta();
    harta_peco();
    /* crearea unui socket */
    sd = socket (AF_INET, SOCK_STREAM, 0);
    /* verificare daca se poate comunica prin socket */ 
    if (sd == -1)
    {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }
    /* utilizarea optiunii SO_REUSEADDR (permite refolosirea adresei locale pentru bind) */
    int ok=1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&ok,sizeof(ok));
    /* pregatirea structurilor de date (se golesc de orice informatii anterioare) */
    bzero (&server, sizeof(server));
    bzero (&from, sizeof(from));
    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
    /* acceptam orice adresa IP a clientului */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (port);
    /* atasam socketul (intre server si clienti) pentru a putea conecta clientii la server */
    int okbind;
    okbind = bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr));
    /* se verifica daca socketul a fost atasat bine */
    if (okbind == -1)
    {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }
    /* serverulasteapta conexiuni din partea clientilor */
    if (listen (sd, 2) == -1)  /* 2 = queue length for completely established sockets waiting to be accepted */
    {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }
    /* servim in mod concurent clientii (folosind thread-uri) */
    while (1)
    {
        int client;
        thread_info *td; //parametru functia executata de thread
        int length = sizeof (from);
        printf ("[server]Asteptam la portul %d...\n",port);
        fflush (stdout);
        /* acceptam un client (stare blocanta pana la realizarea conexiunii) */
        client = accept (sd, (struct sockaddr *) &from, &length);
        if ( client < 0) 
        {
            perror ("[server]Eroare la accept().\n");
            continue;
        }
        /* s-a realizat conexiunea, se astepta mesajul */
        td = (struct thread_info*)malloc(sizeof(struct thread_info));	
        td->idThread = i++;
        td->cl = client;
        verif_cl=0;
        for(int j=0;j<=nr_clienti;j++)
        {
            if(clienti[j]==client) verif_cl=1;
        }
        if(verif_cl==0) clienti[nr_clienti++]=client;
        printf("\n  \033[0;31m   sd = %d si client = %d \033[0m \n",sd,client);
        /* se creaza thread-ul care se va ocupa de noul proces */
        pthread_create(&th[i], NULL, &treat, td);
    }//while   
}

static void* treat(void *arg)
{		
    struct thread_info tdL; 
    tdL = *((struct thread_info*)arg);	
    printf("[Thread %d] - Asteptam mesajul...\n", tdL.idThread);
    /* golim bufferul */
    fflush (stdout);
    /*  pthread_self obtine id-ul thread-ului apelat */	
    /*  se marcheaza thread-ul ca fiind detasat 
        (cand se termina va trimite informatiile la sistem fara sa apeleze alt thread) */	 
    pthread_detach(pthread_self());		
    raspunde((struct thread_info*)arg);
    /* am terminat cu acest client, inchidem conexiunea */
    close ((intptr_t)arg);
    return(NULL);		
};

void raspunde(void *arg)
{
    int i=0, coordX, coordY, ok;
    char msj[256], buf[256], msj1[256], msj2[256], msj3[256], str[256], *tok;
	struct thread_info tdL; 
	tdL= *((struct thread_info*)arg);

    bzero(buf,256);

	if (read (tdL.cl, buf, 256) <= 0)
    {
        printf("[Thread %d]\n",tdL.idThread);
        perror ("Eroare la read() de la client.\n");
    }

	printf ("[Thread %d] Mesajul a fost receptionat...%s\n",tdL.idThread, buf);	      
    /*pregatim mesajul de raspuns */
    /* aici se vor apela celelalte functii */
    //speed(msj);
    if(strcmp(buf,"6")==0)   //accident
    {
        /* get_location */
        coordX = get_coordX();
        coordY = get_coordX();
        while(map[coordX][coordY]==0)
        {
            coordX = get_coordX();
            coordY = get_coordX();
        }
        street_name(str,coordX,coordY);

        sprintf(msj, "Accident pe %s!", str);

        for(int j=0;j<=nr_clienti;j++)
        {
            // printf("[Thread %d] Trimitem mesajul %s inapoi...\n",tdL.idThread, msj);	
          /*  if(recv(clienti[j], &tok, sizeof(tok), MSG_PEEK | MSG_DONTWAIT) > 0)
            {*/
                /* returnam mesajul clientului */
                if (write (clienti[j], msj, 256) <= 0)
                {
                    printf("[Client nr. %d] ",tdL.idThread);
                    perror ("[Thread] Eroare la write() catre client.\n");
                }
                else
                {
                    printf ("[Client nr. %d] Mesajul a fost transmis cu succes.\n",clienti[j]);	
                }
          //  }
        }
    }
    else if(strcmp(buf,"7")==0)   //drum in lucru
    {
        /* get_location */
        coordX = get_coordX();
        coordY = get_coordX();
        while(map[coordX][coordY]==0)
        {
            coordX = get_coordX();
            coordY = get_coordX();
        }
        street_name(str,coordX,coordY);

        sprintf(msj, "Drum in lucru pe %s!", str);

        for(int j=0;j<=nr_clienti;j++)
        {
            // printf("[Thread %d] Trimitem mesajul %s inapoi...\n",tdL.idThread, msj);	
          //  if(getpeername(clienti[j], (struct sockaddr *) &server, sizeof (struct sockaddr))>=0)
           // {
                /* returnam mesajul clientului */
                if (write (clienti[j], msj, 256) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread] Eroare la write() catre client.\n");
                }
                else
                {
                    printf ("[Thread %d] Mesajul a fost transmis cu succes.\n",tdL.idThread);	
                }
           // }
        }
    }
    else if(strcmp(buf,"8")==0)   //drum rau
    {
        /* get_location */
        coordX = get_coordX();
        coordY = get_coordX();
        while(map[coordX][coordY]==0)
        {
            coordX = get_coordX();
            coordY = get_coordX();
        }
        street_name(str,coordX,coordY);

        sprintf(msj, "Drum rau pe %s!", str);

        for(int j=0;j<=nr_clienti;j++)
        {
            // printf("[Thread %d] Trimitem mesajul %s inapoi...\n",tdL.idThread, msj);	
         //   if(getpeername(clienti[j], (struct sockaddr *) &server, sizeof (struct sockaddr))>=0)
          //  {
                /* returnam mesajul clientului */
                if (write (clienti[j], msj, 256) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread] Eroare la write() catre client.\n");
                }
                else
                {
                    printf ("[Thread %d] Mesajul a fost transmis cu succes.\n",tdL.idThread);	
                }
          //  }
        }
    }
    else if(strcmp(buf,"9")==0)   //ambuteiaj
    {
        /* get_location */
        coordX = get_coordX();
        coordY = get_coordX();
        while(map[coordX][coordY]==0)
        {
            coordX = get_coordX();
            coordY = get_coordX();
        }
        street_name(str,coordX,coordY);

        sprintf(msj, "Ambuteiaj pe %s! Se circula cu aproximativ 25 km/h mai incet decat de obicei.", str);

        for(int j=0;j<=nr_clienti;j++)
        {
            // printf("[Thread %d] Trimitem mesajul %s inapoi...\n",tdL.idThread, msj);	
           // if(getpeername(clienti[j], (struct sockaddr *) &server, sizeof (struct sockaddr))>=0)
           // {
                /* returnam mesajul clientului */
                if (write (clienti[j], msj, 256) <= 0)
                {
                    printf("[Thread %d] ",tdL.idThread);
                    perror ("[Thread] Eroare la write() catre client.\n");
                }
                else
                {
                    printf ("[Thread %d] Mesajul a fost transmis cu succes.\n",tdL.idThread);	
                }
           // }
        }
    }
    else if(strcmp(buf,"20")==0)
    {
        speed(msj);
        printf("[Thread %d] Trimitem mesajul %s inapoi...\n",tdL.idThread, msj);	

        /* returnam mesajul clientului */
        if (write (tdL.cl, msj, 256) <= 0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread] Eroare la write() catre client.\n");
        }
        else
        {
            printf ("[Thread %d] Mesajul a fost transmis cu succes.\n",tdL.idThread);	
        }
    }
    else if(strcmp(buf,"0")==0)
    {
        pthread_detach(th[tdL.idThread]);
        strcpy(msj,"");
        //pthread_exit(0);
        printf("[Thread %d] Trimitem mesajul %s inapoi...\n",tdL.idThread, msj);	

        /* returnam mesajul clientului */
        if (write (tdL.cl, msj, 256) <= 0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread] Eroare la write() catre client.\n");
        }
        else
        {
            printf ("[Thread %d] Mesajul a fost transmis cu succes.\n",tdL.idThread);	
        }
    }
    else
    {
        tok = strtok(buf, " ");
      /*  if(strcmp(tok,"accident")==0)
        {
            tok = strtok(NULL, " ");
            clientul = atoi(tok);
            clienti[nr_clienti++]=clientul;
            strcpy(msj,":)");
        }     
        else */if(strcmp(tok,"1")==0)   //login
        {
            tok = strtok(NULL, " ");
            strcpy(msj1,tok); //userul
            tok = strtok(NULL, " ");
            strcpy(msj2,tok);  //parola
            tok = strtok(NULL, " ");
            ok = login(msj1, msj2);
            printf("ok = %d\n",ok);
            sprintf(msj, "%d", ok);
        }
        else if(strcmp(tok,"2")==0)   //signin
        {
            tok = strtok(NULL, " ");
            strcpy(msj1,tok); //userul
            tok = strtok(NULL, " ");
            strcpy(msj2,tok); //parola
            tok = strtok(NULL, " ");
            strcpy(msj3,tok); //stiri
            tok = strtok(NULL, " ");
            ok = signin(msj1, msj2);
            news(msj3,msj1);
            printf("ok = %d\n",ok); //0 nu e bn, 2 e bn
            sprintf(msj, "%d", ok);    
        }
        else if(strcmp(tok,"3")==0)   //raportare
        {
            ok=1;
            sprintf(msj, "%d", ok);
        }
        else if(strcmp(tok,"4")==0)   //news
        {
            ok=1;
            sprintf(msj, "%d", ok);
        }
        else if(strcmp(tok,"5")==0)   //logout
        {
            ok=1;
            sprintf(msj, "%d", ok);
        }
        else if(strcmp(tok,"10")==0)   //meteo
        {
            vreme(msj);
        }
        else if(strcmp(tok,"11")==0)   //sport
        {
            id_sport(msj);
        }
        else if(strcmp(tok,"12")==0)   //combustibil
        {
            closest_peco(msj);
        }
        else if(strcmp(tok,"13")==0)   //ianpoi
        {
            ok=1;
            sprintf(msj, "%d", ok);
        }
   
        printf("[Thread %d] Trimitem mesajul %s inapoi...\n",tdL.idThread, msj);	

        /* returnam mesajul clientului */
        if (write (tdL.cl, msj, 256) <= 0)
        {
            printf("[Thread %d] ",tdL.idThread);
            perror ("[Thread] Eroare la write() catre client.\n");
        }
        else
        {
            printf ("[Thread %d] Mesajul a fost transmis cu succes.\n",tdL.idThread);	
        }
    }


    bzero(buf,256);
    bzero(msj1,256);
    bzero(msj2,256);
    bzero(msj3,256);
    bzero(msj,256);
}