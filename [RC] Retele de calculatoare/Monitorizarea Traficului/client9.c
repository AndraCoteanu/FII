/*  cod sursa de la care a pornit acest program: 
    https://profs.info.uaic.ro/~computernetworks/files/NetEx/S12/ServerConcThread/cliTcpNr.c
    Autorul sursei: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

#include "fcnoua.h"

void *raportare_accident(void *arg)
{
    char mesaj[256];
    while(quit==0)
    {
        if ((sda = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror ("Eroare la socket().\n");
            return NULL;
        }
        /* umplem structura folosita pentru realizarea conexiunii cu serverul */
        /* familia socket-ului */
        server.sin_family = AF_INET;
        /* adresa IP a serverului */
        server.sin_addr.s_addr = INADDR_ANY;
        /* portul de conectare */
        server.sin_port = htons (port);
         /* ne conectam la server */
        if (connect (sda, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
        {
            perror ("[client]Eroare la connect().\n");
            return NULL;
        }
        /*  citirea raspunsului dat de server 
            (apel blocant pana cand serverul raspunde) */
        if(read(sda, mesaj,256)>0)
        {
            system("@cls||clear");
            system("@cls||clear");
            printf("Notificare! %s\n\n",mesaj);
            close(sdv);
        }
        if(quit==1) {close(sdv); pthread_exit(0); return EXIT_SUCCESS; break;}
        close(sda);
    }
}
  
void *viteza1min(void *arg)
{
    time_t secunde1, secunde2;
    secunde1=time(NULL);
    while(quit==0)
    {
        if ((sdv = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror ("Eroare la socket().\n");
            return NULL;
        }
        /* umplem structura folosita pentru realizarea conexiunii cu serverul */
        /* familia socket-ului */
        server.sin_family = AF_INET;
        /* adresa IP a serverului */
        server.sin_addr.s_addr = INADDR_ANY;
        /* portul de conectare */
        server.sin_port = htons (port);
         /* ne conectam la server */
        if (connect (sdv, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
        {
            perror ("[client]Eroare la connect().\n");
            return NULL;
        }
        
        secunde1=time(NULL);
        while(secunde2-secunde1<=5 && quit==0)
        {   
            secunde2=time(NULL);
            if(secunde2-secunde1==5)
            {
                secunde1=secunde2;
                strcpy(msjth,"20");
                if (write (sdv,msjth,256) <= 0)
                {
                    //perror ("[client]Eroare la write() spre server.\n");
                    return NULL;
                }
                bzero(msjth,256);
                /*  citirea raspunsului dat de server 
                    (apel blocant pana cand serverul raspunde) */
                if (read (sdv, msjth,256) < 0)
                {
                    //perror ("[client]Eroare la read() de la server.\n");
                    return NULL;
                }
                speed(msjth);
                bzero(msjth,256);
            }
            if(quit==1) {close(sdv); pthread_exit(0); return EXIT_SUCCESS; break;}
        }
        close(sdv);
        if(quit==1) break;
    }
}

void *aplicatie(void *arg)
{
    fflush (stdout);
    memset (buf, 0, sizeof(buf));
    scanf("%s", buf);

    /* pregatim ecranul pentru a afisa raspunsul */
    system("@cls||clear");

    int nr=atoi(buf);
    if(meniu==1)
    {
        if(nr==0)  //quit
        {
            if (write (sd,buf,256) <= 0)
            {
                //perror ("[client]Eroare la write() spre server.\n");
                return NULL;
            }
            quit=1;
            printf("     \033[1;33mAti iesit din aplicatie.\033[0m\n");
        }
        else
        {
            comanda = verif_meniu1(logat,buf);
            if(comanda==1) //daca comanda este conforma cu meniul 1 atunci:
            {
                if(nr==1)  //login
                {
                    printf("Introduceti datele pentru autentificare: \n");
                    printf("username: ");
                    scanf("%s",user);
                    printf("parola: ");
                    scanf("%s",msj2);

                    strcpy(buf,"1 ");
                    strcat(buf,user);
                    strcat(buf," ");
                    strcat(buf,msj2);

                    if (write (sd,buf,256) <= 0)
                    {
                        //perror ("[client]Eroare la write() spre server.\n");
                        return NULL;
                    }
                    /* citirea raspunsului dat de server 
                    (apel blocant pana cand serverul raspunde) */
                    if (read (sd, msg,256) < 0)
                    {
                        //perror ("[client]Eroare la read() de la server.\n");
                        return NULL;
                    }
                    if(strcmp(msg,"1")==0)
                    {
                        stiri=0;
                        logat = 1;
                        printf("Sunteti autentificat.\n");
                    }
                    else if(strcmp(msg,"3")==0)
                    {
                        stiri=1;
                        logat = 1;
                        printf("Sunteti autentificat.\n");
                    }
                }
                else if(nr==2) //signin
                {
                    printf("Introduceti datele pentru crearea contului: \n");
                    printf("username: ");
                    scanf("%s",user);
                    printf("parola: ");
                    scanf("%s",msj1);
                    printf("Doriti sa va abonati la stiri meteo, sport si noutati despre preturile carburantilor? Da/Nu.\n");
                    scanf("%s",msj2);
                    /* se configureaza mesajul de trimitere la server */
                    strcpy(buf,"2 ");
                    strcat(buf,user);
                    strcat(buf," ");
                    strcat(buf,msj1);
                    strcat(buf," ");
                    strcat(buf,msj2);
                
                    /* trimiterea mesajului la server */
                    if (write (sd,buf,256) <= 0)
                    {
                        //perror ("[client]Eroare la write() spre server.\n");
                        return NULL;
                    }
                    /* citirea raspunsului dat de server 
                    (apel blocant pana cand serverul raspunde) */
                    if (read (sd, msg,256) < 0)
                    {
                        //perror ("[client]Eroare la read() de la server.\n");
                        return NULL;
                    }

                    if(strcmp(msg,"2")==0)
                    {
                        printf("Contul a fost creat cu succes. Sunteti autentificat.\n");
                        //news(msj2,msj1);
                        if(strcmp(msj2,"da")==0)
                        {
                            stiri=1;
                            printf("Felicitari! Veti putea accesa oricand noutatile in aplicatie.\n");
                        }
                        bzero(msg,256);
                        logat = 1;
                    }
                    else 
                    {
                        stiri=0;
                        printf("Username-ul pe care l-ati introdus este deja folosit de altcineva.\n");
                    }
                }
                
                /* se pregateste programul pentru urmatoarea comanda */
                if(logat==1)
                {
                    system("@cls||clear");
                    printf("\033[1;34m       V-ati conectat cu numele %s \033[0m \n",user);
                    if(stiri==0) meniu2();
                    else if(stiri==1) meniu2_stiri();
                    meniu=2;
                }
                else if(logat==0)
                {
                    system("@cls||clear");
                    printf("      Acest cont nu exista.\n");
                    meniu1();
                    meniu=1;
                }
                bzero(buf,256); 
            }
            else 
            {
                strcpy(wrcom,"5");
                /* trimiterea mesajului la server */
                if (write (sd,wrcom,256) <= 0)
                {
                    //perror ("[client]Eroare la write() spre server.\n");
                    return NULL;
                }
                /* citirea raspunsului dat de server 
                (apel blocant pana cand serverul raspunde) */
                if (read (sd, msg,256) < 0)
                {
                    //perror ("[client]Eroare la read() de la server.\n");
                    return NULL;
                }
                meniu1();
            }
        }
    }
    else if(meniu==2 && stiri==0) //client fara abonament stiri
    {
        if(nr==0)  //quit
        {
            if (write (sd,buf,256) <= 0)
            {
                //perror ("[client]Eroare la write() spre server.\n");
                return NULL;
            }
            quit=1;
            printf("     \033[1;33mAti iesit din aplicatie.\033[0m\n");
        }
        else
        {
            comanda = verif_meniu2(buf);
            if(comanda == 1)
            {
                if(nr==3)
                {
                    if (write (sd,buf,256) <= 0)
                    {
                        //perror ("[client]Eroare la write() spre server.\n");
                        return NULL;
                    }
                    /* citirea raspunsului dat de server 
                    (apel blocant pana cand serverul raspunde) */
                    if (read (sd, msg,256) < 0)
                    {
                        //perror ("[client]Eroare la read() de la server.\n");
                        return NULL;
                    }
                    printf("coming soon...\n");
                    meniu3();
                    meniu=3;
                }
                else if(nr==5)
                {
                    if (write (sd,buf,256) <= 0)
                    {
                        //perror ("[client]Eroare la write() spre server.\n");
                        return NULL;
                    }
                    /* citirea raspunsului dat de server 
                    (apel blocant pana cand serverul raspunde) */
                    if (read (sd, msg,256) < 0)
                    {
                        //perror ("[client]Eroare la read() de la server.\n");
                        return NULL;
                    }
                    logat = 0;
                    meniu = 1;
                    meniu1();
                    strcpy(user,"");
                }
            }
            else 
            {
                strcpy(wrcom,"5");
                /* trimiterea mesajului la server */
                if (write (sd,wrcom,256) <= 0)
                {
                    //perror ("[client]Eroare la write() spre server.\n");
                    return NULL;
                }
                /* citirea raspunsului dat de server 
                (apel blocant pana cand serverul raspunde) */
                if (read (sd, msg,256) < 0)
                {
                    //perror ("[client]Eroare la read() de la server.\n");
                    return NULL;
                }
                meniu2();
            }
        }
    }
    else if(meniu==2 && stiri==1) //client CU abonament la stiri
    {
        if(nr==0)  //quit
        {
            if (write (sd,buf,256) <= 0)
            {
                //perror ("[client]Eroare la write() spre server.\n");
                return NULL;
            }
            quit=1;
            printf("     \033[1;33mAti iesit din aplicatie.\033[0m\n");
        }
        else
        {
            comanda = verif_meniu2_stiri(buf);
            if(comanda == 1)
            {
                if(nr==3)
                {
                    if (write (sd,buf,256) <= 0)
                    {
                        //perror ("[client]Eroare la write() spre server.\n");
                        return NULL;
                    }
                    /* citirea raspunsului dat de server 
                    (apel blocant pana cand serverul raspunde) */
                    if (read (sd, msg,256) < 0)
                    {
                        //perror ("[client]Eroare la read() de la server.\n");
                        return NULL;
                    }
                    printf("coming soon...\n");
                    meniu3();
                    meniu=3;
                }
                else if(nr==4)
                {
                    if (write (sd,buf,256) <= 0)
                    {
                        //perror ("[client]Eroare la write() spre server.\n");
                        return NULL;
                    }
                    /* citirea raspunsului dat de server 
                    (apel blocant pana cand serverul raspunde) */
                    if (read (sd, msg,256) < 0)
                    {
                        //perror ("[client]Eroare la read() de la server.\n");
                        return NULL;
                    }
                    meniu = 4;
                    meniu4();
                }
                else if(nr==5)
                {
                    if (write (sd,buf,256) <= 0)
                    {
                        //perror ("[client]Eroare la write() spre server.\n");
                        return NULL;
                    }
                    /* citirea raspunsului dat de server 
                    (apel blocant pana cand serverul raspunde) */
                    if (read (sd, msg,256) < 0)
                    {
                        //perror ("[client]Eroare la read() de la server.\n");
                        return NULL;
                    }
                    logat = 0;
                    meniu = 1;
                    meniu1();
                    strcpy(user,"");
                }
            }
            else 
            {
                strcpy(wrcom,"5");
                /* trimiterea mesajului la server */
                if (write (sd,wrcom,256) <= 0)
                {
                    //perror ("[client]Eroare la write() spre server.\n");
                    return NULL;
                }
                /* citirea raspunsului dat de server 
                (apel blocant pana cand serverul raspunde) */
                if (read (sd, msg,256) < 0)
                {
                    //perror ("[client]Eroare la read() de la server.\n");
                    return NULL;
                }
                meniu2_stiri();
            }
        }
    }
    else if(meniu==3) 
    {
        comanda = verif_meniu3(buf);
        if(comanda==1)
        {
            if (write (sd,buf,256) <= 0)
            {
                //perror ("[client]Eroare la write() spre server.\n");
                return NULL;
            }
            /* citirea raspunsului dat de server 
            (apel blocant pana cand serverul raspunde) */
            if (read (sd, msg,256) < 0)
            {
                //perror ("[client]Eroare la read() de la server.\n");
                return NULL;
            }
            printf("coming soon...\n");
            meniu=2;
            if(stiri==0) meniu2();
            else meniu2_stiri();
        }
        else
        {
            strcpy(wrcom,"5");
            /* trimiterea mesajului la server */
            if (write (sd,wrcom,256) <= 0)
            {
                //perror ("[client]Eroare la write() spre server.\n");
                return NULL;
            }
            /* citirea raspunsului dat de server 
            (apel blocant pana cand serverul raspunde) */
            if (read (sd, msg,256) < 0)
            {
                //perror ("[client]Eroare la read() de la server.\n");
                return NULL;
            }
            meniu3();
        }
        
    }
    else if(meniu==4)
    {
        comanda = verif_meniu4(buf);
        if(comanda==1)
        {
            if(nr==10)
            {
                if (write (sd,buf,256) <= 0)
                {
                    //perror ("[client]Eroare la write() spre server.\n");
                    return NULL;
                }
                /* citirea raspunsului dat de server 
                (apel blocant pana cand serverul raspunde) */
                if (read (sd, msg,256) < 0)
                {
                    //perror ("[client]Eroare la read() de la server.\n");
                    return NULL;
                }
                meteo(msg);
                sleep(3);
                meniu4();
                meniu=4;
                
            }
            else if(nr==11)
            {
                if (write (sd,buf,256) <= 0)
                {
                    //perror ("[client]Eroare la write() spre server.\n");
                    return NULL;
                }
                /* citirea raspunsului dat de server 
                (apel blocant pana cand serverul raspunde) */
                if (read (sd, msg,256) < 0)
                {
                    //perror ("[client]Eroare la read() de la server.\n");
                    return NULL;
                }
                sport(msg);
                sleep(3);
                meniu4();
                meniu=4;
                
            }
            else if(nr==12)
            {
                if (write (sd,buf,256) <= 0)
                {
                    //perror ("[client]Eroare la write() spre server.\n");
                    return NULL;
                }
                /* citirea raspunsului dat de server 
                (apel blocant pana cand serverul raspunde) */
                if (read (sd, msg,256) < 0)
                {
                    //perror ("[client]Eroare la read() de la server.\n");
                    return NULL;
                }
                stiri_peco(msg);
                sleep(3);
                meniu4();
                meniu=4;
                
            }
            else if(nr==13)
            {
                if (write (sd,buf,256) <= 0)
                {
                    //perror ("[client]Eroare la write() spre server.\n");
                    return NULL;
                }
                /* citirea raspunsului dat de server 
                (apel blocant pana cand serverul raspunde) */
                if (read (sd, msg,256) < 0)
                {
                    //perror ("[client]Eroare la read() de la server.\n");
                    return NULL;
                }
                meniu=2;
                meniu2_stiri();
                
            }
        }
        else 
        {
            strcpy(wrcom,"5");
            /* trimiterea mesajului la server */
            if (write (sd,wrcom,256) <= 0)
            {
                //perror ("[client]Eroare la write() spre server.\n");
                return NULL;
            }
            /* citirea raspunsului dat de server 
            (apel blocant pana cand serverul raspunde) */
            if (read (sd, msg,256) < 0)
            {
                //perror ("[client]Eroare la read() de la server.\n");
                return NULL;
            }
            meniu4();
        }
    } 
}

static void *treat_viteza(void *arg)
{		
    /* golim bufferul */
    fflush (stdout);
    viteza1min((void*)arg);
    return(NULL);		
}

static void *treat(void *arg)
{		
    /* golim bufferul */
    fflush (stdout);
    aplicatie((void*)arg);
    return(NULL);		
}

static void* treat_accident(void* arg)
{		
    /* golim bufferul */
    fflush (stdout);
    raportare_accident((void*)arg);
    return(NULL);		
}

int main (int argc, char *argv[])
{
    /* exista toate argumentele in linia de comanda? */
    if (argc != 2)
    {
        printf ("Sintaxa: %s <port>\n", argv[0]);
        return -1;
    }

    int gd=DETECT,gm;
    tutorial(gd,gm);  

    /* se initializeaza unele variabile globale */
    meniu=1;
    logat=0;
    quit=0;
    nr=0;
    id_app=0;
    id_vit=0;

    system("@cls||clear");
    meniu1();

    /* stabilim portul */
    port = atoi (argv[1]);

    pthread_create(&viteza[id_vit], NULL, &treat_viteza, NULL);

    while (1) 
    {
        if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
            //perror ("Eroare la socket().\n");
            return NULL;
        }
        /* umplem structura folosita pentru realizarea conexiunii cu serverul */
        /* familia socket-ului */
        server.sin_family = AF_INET;
        /* adresa IP a serverului */
        server.sin_addr.s_addr = INADDR_ANY;
        /* portul de conectare */
        server.sin_port = htons (port);
        
       // printf("\nquit=%d, meniu=%d, logat=%d, id_app=%d\n",quit,meniu,logat,id_app);
        /* ne conectam la server */
        if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
        {
            //perror ("[client]Eroare la connect().\n");
            return NULL;
        }

        id_vit++;
        /* se creaza threadul care se ocupa de functionalitatile aplciatiei */
        pthread_create(&th[id_app], NULL, &treat, NULL); //alte functionalitati ale aplicatiei
        if(logat==1 && quit==0) 
        {
            /* se va crea un nou thread pentru a asigura continuitatea */
            pthread_create(&viteza[id_vit], NULL, &treat_viteza, NULL);
            pthread_create(&accident[id_acc], NULL, &treat_accident, NULL);
        }
        /* se asteapta threadul cu functionalitatile sa ruleze, apoi se reia while-ul */
        pthread_join(th[id_app], NULL);
        /* inchidem conexiunea */
        close (sd );
        id_app++;
        if(quit==1) 
        {   /* daca clientul a solicitat functia quit, atunci aplicatia se va inchide */
            pthread_exit(0); 
            return EXIT_SUCCESS; 
            break;
        }
    }
    close (sd);
    return 0;
}