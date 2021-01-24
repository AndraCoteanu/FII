#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <term.h>
#include <time.h>
#include <graphics.h>
#include <arpa/inet.h>
#include <pthread.h>

/*caps = light, lowercase = normal */
#define NC "\033[0m"  /*no color*/
#define RED "\033[0;31m"  
#define red "\033[1;31m" 
#define PURPLE "\033[1;35m" 
#define purple "\033[0;35m"
#define BLUE "\033[1;34m"

pthread_t th[1000],viteza[1000],accident[1000];

struct sockaddr_in server;	//structura folosita pentru conectare

int     port,sd, sdv, sda, logat, meniu, comanda, quit, stiri, nr, id_app, id_vit, id_acc;
char    buf[256], msg[256], msj1[256], msj2[256], user[256], *tok, wrcom[256], msjth[256];

/* grafica de la deschiderea aplicatiei*/
void car();
void tutorial(int gd, int gm);
/* genereaza o lista de preturi pentru fiecare combustibil in parte */
void benzina();
void diesel();
void gpl();
/* lista preturilor va fi afisata cu ajutorul functiilor de mai jos pentru fiecare benzinarie in parte*/
void MOL();
void petrom();
void OMV();
void lukoil();
void rompetrol();
void socar();
/*  
    id benzinarie - nume benzinarie
    1 - MOL (fara gpl) 
    2 - Petrom
    3 - OMV
    4 - Lukoil
    5 - Rompetrol 
    6 - Socar (fara gpl)
*/
void benzinarie(int peco_id);
void stiri_peco(char msjpeco[]);
/* prezentarea meniurilor */
void meniu1();
void meniu2_stiri();
void meniu2();
void meniu3();
void meniu4();
/* verificarea comenzilor introduse de utilizator */
int verif_meniu1(int logat, char buf[]);
int verif_meniu2_stiri(char buf[]);
int verif_meniu2(char buf[]);
int verif_meniu3(char buf[]);
int verif_meniu4(char buf[]);
/* afisarea vitezei clientului */
void speed(char msg[]);
/* decodificarea si afisarea informatiilor despre vreme*/
void meteo(char msj[]);
void soare(char msj[]);
void soareish(char msj[]);
void nori(char msj[]);
void ploaie(char msj[]);
void furtuna(char msj[]);
void ninsoare_ploi(char msj[]);
void ninsoare(char msj[]);
void ploi_inghetate(char msj[]);
void ceata(char msj[]);
/* stiri sport */
void sport(char msj[]);

/* definirea functiilor declarate mai sus */

void benzina() 
{
    /* se genereaza o lista de preturi pentru benzina */
    srand (time(NULL));
    int pret, virgula;
    pret=rand()%5+4;
    while(pret>5 || pret<4)
    {
        pret=rand()%5+4;
    }
    virgula=rand()%99+50;
    while(virgula>99 || virgula<50)
    {
        virgula=rand()%99+50;
    }
    printf("    \033[0;34mbenzina standard (95):\033[0m %d,%d RON/litru\n",pret,virgula); //intre 4,50 - 5,00 lei/litru
    virgula=rand()%50;
    while(virgula>50)
    {
        virgula=rand()%50;
    }
    printf("    \033[0;34mbenzina superioara (98):\033[0m 5,%d RON/litru\n",virgula); //intre 5,00 - 5,50 lei/litru
}

void diesel()  
{
    /* se genereaza o lista de preturi pentru motorina */
    srand (time(NULL));
    int pret,virgula;
    pret=rand()%5+4;
    while(pret>5 || pret<4)
    {
        pret=rand()%5+4;
    }
    virgula=rand()%99+60;
    while(virgula>99 || virgula<60)
    {
        virgula=rand()%99+60;
    }
    printf("    \033[0;34mmotorina standard (51):\033[0m %d,%d RON/litru\n",pret,virgula); //intre 4,60 - 5,00 lei/litru
    virgula=rand()%50;
    while(virgula>50)
    {
        virgula=rand()%50;
    }
    printf("    \033[0;34mmotorina superioara (55):\033[0m 5,%d RON/litru\n",virgula); //intre 5,00 - 5,50 lei/litru
}

void gpl()
{
    /* se genereaza o lista de preturi pentru gpl */
    srand (time(NULL));
    int pret;
    pret=rand()%99+10;
    while(pret<10 || pret>99)
    {
        pret=rand()%99+10;
    }
    printf("    \033[0;34mgpl:\033[0m 2,%d RON/litru\n",pret); //intre 2,10 - 2,99 lei/litru
}

void MOL()
{
    printf("\n       \033[1;35m [MOL] Lista preturi:\n\n\033[0m");
    benzina();
    printf("\n");
    diesel();
    printf("\n");
}

void petrom()
{
    printf("\n       \033[1;35m [Petrom] Lista preturi:\n\n\033[0m");
    benzina();
    printf("\n");
    diesel();
    printf("\n");
    gpl();
    printf("\n");
}

void OMV()
{
    printf("\n       \033[1;35m [OMV] Lista preturi:\n\n\033[0m");
    benzina();
    printf("\n");
    diesel();
    printf("\n");
    gpl();
    printf("\n");
}

void lukoil()
{
    printf("\n       \033[1;35m [Lukoil] Lista preturi:\n\n\033[0m");
    benzina();
    printf("\n");
    diesel();
    printf("\n");
    gpl();
    printf("\n");
}

void rompetrol()
{
    printf("\n       \033[1;35m [Rompetrol] Lista preturi:\n\n\033[0m");
    benzina();
    printf("\n");
    diesel();
    printf("\n");
    gpl();
    printf("\n");
}

void socar()
{
    printf("\n       \033[1;35m [Socar] Lista preturi:\n\n\033[0m");
    benzina();
    printf("\n");
    diesel();
    printf("\n");
}

void benzinarie(int peco_id) 
{
    /* pentru fiecare id se apeleaza la benzinaria corespunzatoare pentru a obtine lista preturilor */
    if(peco_id==1)
    {
        MOL();
    }
    else if(peco_id==2)
    {
        petrom();
    }
    else if(peco_id==3)
    {
        OMV();
    }
    else if(peco_id==4)
    {
        lukoil();
    }
    else if(peco_id==5)
    {
        rompetrol();
    }
    else if(peco_id==6)
    {
        socar();
    }
}

void stiri_peco(char msjpeco[])
{
    /*  decodifica mesajul de la server si 
        pentru benzinariile care se afla in apropiere se primeste lista de preturi si aceasta este afisata */
    char *tok;
    tok=strtok(msjpeco,"#");
    printf("\n     Va aflati pe %s. In apropiere aveti: \n\n", tok); 
    tok=strtok(NULL," ");
    while(tok!=NULL)
    {
        int id=atoi(tok);
        benzinarie(id);
        tok=strtok(NULL," ");
    }
}

void car()
{
    /* masinuta de la deschiderea programului */
	setlinestyle(0, 0, 3);
	circle(245,300,30);     //
	circle(245,300,29);
	circle(245,300,28);
    circle(385,300,30);     //
	circle(385,300,29); 
	circle(385,300,28); 
	circle(245,300,7);      //
	circle(245,300,6);
    circle(385,300,7);      //
	circle(385,300,6);
	line(275,300,355,300);
	line(170,300,215,300);
	rectangle(165,300,188,285);
	line(170,285,170,250);
	line(170,250,230,240);
	line(230,240,250,175);
	line(250,175,350,175);
	line(350,175,385,240);
	line(385,240,435,243);
	line(435,243,440,300);
	line(415,300,460,300);
	line(460,300,460,293);
	line(460,293,440,293);
	line(230,240,235,270);
	line(230,240,385,240);
	line(385,240,380,270);
	line(310,175,310,300);
	rectangle(270,245,300,246);
	rectangle(343,245,373,246);
}

void tutorial(int gd, int gm)
{
    /* masina isi va schimba culorile in timp ce aplicatia "se incarca"/pregateste de deschidere */
	initgraph(&gd,&gm,NULL);
	outtextxy(230,400,"se deschide aplicatia . . .");	
	for(int i=1;i<=45;i++)
	{
		setcolor(i%15+1);
		car();
		delay(300);
		if(i==7)
		{
			/* se sterge mesajul afisat anterior */
			setcolor(BLACK);
			setlinestyle(0, 0, 3);
			rectangle(217,387,413,423);
			rectangle(214,384,410,420);
			rectangle(211,381,407,417);
			rectangle(208,378,404,414);
			rectangle(205,375,401,411);
			rectangle(202,372,398,408);
			rectangle(199,369,395,405);
			rectangle(196,366,392,402);
			rectangle(193,363,389,399);
			setlinestyle(0, 0, 1);

			/* se afiseaza noul mesaj dupa "incarcarea aplicatiei" */
            /* utilizatorului i se explica pe scurt cum sa foloseasca aplicatia */
			outtextxy(270,70,"Bine ai venit!");
			outtextxy(230,90,"TUTORIAL DE UTILIZARE:");
			outtextxy(50,110,"Pentru selectarea unei comenzi se va insera de la tastatura numarul acesteia.");
            outtextxy(185,125,"Aplicatia se va deschide in terminal.");
		}
	}
	delay(100);
    closegraph();
    /* se curata terminalul si se porneste aplicatia pentru client */
    system("@cls||clear") ;
}

void meniu1()
{
    /* meniul principal al aplicatiei */
    printf("\n\n\n\033[1;35m     MONITORIZAREA TRAFICULUI\n\n");
    printf("\033[0m               🚗\n");
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         | \033[0m 1. Login\033[1;35m  |\n");  /*--> meniu2*/
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         |\033[0m 2. Sign in \033[1;35m|\n");  /*--> meniu2*/
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         | \033[0m 0. Quit  \033[1;35m |\n");  /*iese din aplicatie*/
    printf("\033[1;35m          ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻\n");
    printf("\n\033[1;34m     Introduceti o comanda: \033[0m\n");
}

void meniu2_stiri()
{
    /* comenzile generale dupa autentificare pentru utilizatorii abonati la stiri */
    printf("\033[0m                     🚗\n");
    printf("\033[1;35m          |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m          | \033[0m3. Raportare incident\033[1;35m |\n");  /*--> meniu3*/
    printf("\033[1;35m          |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m          |        \033[0m4. News       \033[1;35m |\n");  /* -->meniu4*/
    printf("\033[1;35m          |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m          |       \033[0m5. Logout      \033[1;35m |\n");  /* --> meniu1*/
    printf("\033[1;35m          |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m          |       \033[0m 0. Quit       \033[1;35m |\n");  /*iese din aplicatie*/
    printf("\033[1;35m           ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻\n");
    printf("\n\033[1;34m         Introduceti o comanda: \033[0m\n");
}

void meniu2()
{
    /* comenzile generale dupa autentificare pentru clientii fara abonament la stiri */
    printf("\033[0m                     🚗\n");
    printf("\033[1;35m          |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m          | \033[0m3. Raportare incident\033[1;35m |\n");  /*--> meniu3*/
    printf("\033[1;35m          |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m          |       \033[0m5. Logout      \033[1;35m |\n");  /* --> meniu1*/
    printf("\033[1;35m          |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m          |       \033[0m 0. Quit       \033[1;35m |\n");  /*iese din aplicatie*/
    printf("\033[1;35m           ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻\n");
    printf("\n\033[1;34m         Introduceti o comanda: \033[0m\n");
}

void meniu3()
{
    /* utilizatorul vrea sa raporteze un incident in trafic */
    /* se va raporta doar tipul de incident, locatia va fi preluata automat de aplicatie */
    /*dupa raportarea incidentului se va intoarce la meniu2*/
    printf("                  🚨\n");
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         |   \033[0m6. Accident    \033[1;35m|\n");
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         | \033[0m7. Drum in lucru \033[1;35m|\n");
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         |   \033[0m8. Drum rau    \033[1;35m|\n");
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         |   \033[0m9. Ambuteiaj   \033[1;35m|\n");
    printf("\033[1;35m          ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻\n");
    printf("\n\033[1;34m         Raportati un incident:\033[0m \n");
}

void meniu4()
{
    /* la afisarea stirilor de meteo sau sport, clientul va avea optiunea "Inapoi" care il va intoarce la meniul2 */
    printf("                 🚗\n");
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         |    \033[0m10. Meteo\033[1;35m    |\n");
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         |    \033[0m11. Sport  \033[1;35m  |\n");
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         | \033[0m12. Combustibil \033[1;35m|\n");  /* --> meniu5*/
    printf("\033[1;35m         |⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻|\n");
    printf("\033[1;35m         |    \033[0m13. Inapoi  \033[1;35m |\n"); /* --> meniu2*/
    printf("\033[1;35m          ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻\n");
    printf("\n\033[1;34m         Introduceti o comanda:\033[0m \n");
}

int verif_meniu1(int logat, char buf[])
{  
    /* verifica daca comanda introdusa este din meniul 1 */
    int x=atoi(buf);
    if(logat==0 && x>2)
    {
        printf("Comanda introdusa nu poate fi accesata. \nTrebuie sa va autentificati.\n");
        return 0;
    }
    else if(logat==0 && x<0)
    {
        printf("Comanda introdusa nu poate fi accesata. \nTrebuie sa va autentificati.\n");
        return 0;
    }
    return 1;
}

int verif_meniu2_stiri(char buf[])
{
    int x=atoi(buf);
    if(x!=0)
    {
        if(x<3)
        {
            printf("Comanda introdusa nu poate fi accesata. Incercati una din cele afisate pe ecran.\n");
            return 0;
        }
        else if(x>5) 
        {
            printf("Comanda introdusa nu poate fi accesata. Incercati una din cele afisate pe ecran.\n");
            return 0;
        }
    }
    return 1;
}

int verif_meniu2(char buf[])
{
    int x=atoi(buf);
    if(x!=0)
    {
        if(x<3)
        {
            printf("Comanda introdusa nu poate fi accesata. Incercati una din cele afisate pe ecran.\n");
            return 0;
        }
        else if(x==4)
        {
            printf("Comanda introdusa nu poate fi accesata. Incercati una din cele afisate pe ecran.\n");
            return 0;
        }
        else if(x>5) 
        {
            printf("Comanda introdusa nu poate fi accesata. Incercati una din cele afisate pe ecran.\n");
            return 0;
        }
    }
    return 1;
}

int verif_meniu3(char buf[])
{
    int x=atoi(buf);
    if(x<6)
    {
        printf("Comanda introdusa nu poate fi accesata. Incercati una din cele afisate pe ecran.\n");
        return 0;
    }
    else if(x>9)
    {
        printf("Comanda introdusa nu poate fi accesata. Incercati una din cele afisate pe ecran.\n");
        return 0;
    }
    return 1;
}

int verif_meniu4(char buf[])
{
   int x=atoi(buf);
    if(x<10)
    {
        printf("Comanda introdusa nu poate fi accesata. Incercati una din cele afisate pe ecran.\n");
        return 0;
    }
    else if(x>13)
    {
        printf("Comanda introdusa nu poate fi accesata. Incercati una din cele afisate pe ecran.\n");
        return 0;
    }
    return 1;
}

void speed(char msg[])
{
    /* procesam mesajul de la server */
    int     ok, i;
    char    strada[256], *tok, *viteza;
    
    /* este viteza legala? */
    tok = strtok(msg," ");
    ok = atoi(tok);
    
    /* care este viteza clientului? */ 
    viteza = strtok(NULL," ");
    
    /* afisam mesajul primit */
    printf("Notificare!!! Sunteti pe ");

    /* citim strada */
    tok = strtok(NULL," ");
    while(tok != NULL)
    {
        printf("%s ", tok);
        tok = strtok(NULL," ");
    }

    /* afisarea eventualelor avertismente pentru sofer */
    printf("si circulati cu %s km/h.\n", viteza);
    if(ok==0)
    {
        printf("\033[1;31m  ATENTIE! viteza prea mare\033[0m\n");
    }
    else if(ok==2)
    {
        printf("\033[0;31m  ATENTIE! viteza prea mica\033[0m\n");
    }
    else if(ok==3)
    {
        printf("\033[1;31m  ATENTIE! viteza prea mare. Vi se poate suspenda permisul\033[0m\n");
    }  
}

void soare(char msj[])
{
    printf("\n            🌞 Soare  ");
    char *cuv;
    int k=1,nr;
    cuv=strtok(msj, " ");
    cuv=strtok(NULL, " ");
    while(cuv!=NULL)
    {
        nr = atoi(cuv);
        if(k==1)        //temp
        {
            printf("%d°\n",nr);
        }
        else if(k==2)   //real_feel
        {
            printf("            real feel: %d°\n",nr);
        }
        else if(k==3)   //precipitatii
        {
            printf("           precipitatii: %d%%\n",nr);
        }
        else if(k==4)   //vant
        {
            printf("            vant:  %d km/h\n",nr);
        }
        else if(k==5)   //directie
        {
            printf("         directia vantului: ");
            if(nr==1)
            {
                printf("N\n");
            }
            else if(nr==2)
            {
                printf("S\n");
            }
            else if(nr==3)
            {
                printf("E\n");
            }
            else if(nr==4)
            {
                printf("V\n");
            }
            else if(nr==5)
            {
                printf("NE\n");
            }
            else if(nr==6)
            {
                printf("NV\n");
            }
            else if(nr==7)
            {
                printf("SE\n");
            }
            else if(nr==8)
            {
                printf("SV\n");
            }
        }
        k++;
        cuv = strtok(NULL," ");
    }
    printf("\n");
}

void soareish(char msj[])
{
    printf("\n    🌤  Soare si partial inorat  ");
    char *cuv;
    int k=1,nr;
    cuv=strtok(msj, " ");
    cuv=strtok(NULL, " ");
    while(cuv!=NULL)
    {
        nr = atoi(cuv);
        if(k==1)        //temp
        {
            printf("%d°\n",nr);
        }
        else if(k==2)   //real_feel
        {
            printf("             real feel: %d°\n",nr);
        }
        else if(k==3)   //precipitatii
        {
            printf("           precipitatii: %d%%\n",nr);
        }
        else if(k==4)   //vant
        {
            printf("             vant: %d km/h\n",nr);
        }
        else if(k==5)   //directie
        {
            printf("         directia vantului: ");
            if(nr==1)
            {
                printf("N\n");
            }
            else if(nr==2)
            {
                printf("S\n");
            }
            else if(nr==3)
            {
                printf("E\n");
            }
            else if(nr==4)
            {
                printf("V\n");
            }
            else if(nr==5)
            {
                printf("NE\n");
            }
            else if(nr==6)
            {
                printf("NV\n");
            }
            else if(nr==7)
            {
                printf("SE\n");
            }
            else if(nr==8)
            {
                printf("SV\n");
            }
        }
        k++;
        cuv = strtok(NULL," ");
    }
    printf("\n");
}

void nori(char msj[])
{
    printf("\n           🌥  Inorat  ");
    char *cuv;
    int k=1,nr;
    cuv=strtok(msj, " ");
    cuv=strtok(NULL, " ");
    while(cuv!=NULL)
    {
        nr = atoi(cuv);
        if(k==1)        //temp
        {
            printf("%d°\n",nr);
        }
        else if(k==2)   //real_feel
        {
            printf("           real feel: %d°\n",nr);
        }
        else if(k==3)   //precipitatii
        {
            printf("          precipitatii: %d%%\n",nr);
        }
        else if(k==4)   //vant
        {
            printf("            vant: %d km/h\n",nr);
        }
        else if(k==5)   //directie
        {
            printf("         directia vantului: ");
            if(nr==1)
            {
                printf("N\n");
            }
            else if(nr==2)
            {
                printf("S\n");
            }
            else if(nr==3)
            {
                printf("E\n");
            }
            else if(nr==4)
            {
                printf("V\n");
            }
            else if(nr==5)
            {
                printf("NE\n");
            }
            else if(nr==6)
            {
                printf("NV\n");
            }
            else if(nr==7)
            {
                printf("SE\n");
            }
            else if(nr==8)
            {
                printf("SV\n");
            }
        }
        k++;
        cuv = strtok(NULL," ");
    }
    printf("\n");
}

void ploaie(char msj[])
{
    printf("\n           🌧  Ploaie  ");
    char *cuv;
    int k=1,nr;
    cuv=strtok(msj, " ");
    cuv=strtok(NULL, " ");
    while(cuv!=NULL)
    {
        nr = atoi(cuv);
        if(k==1)  //temp
        {
            printf("%d°\n",nr);
        }
        else if(k==2)  //real_feel
        {
            printf("           real feel: %d°\n",nr);
        }
        else if(k==3)  //precipitatii
        {
            printf("          precipitatii: %d%%\n",nr);
        }
        else if(k==4)  //vant
        {
            printf("            vant: %d km/h\n",nr);
        }
        else if(k==5)  //directie
        {
            printf("        directia vantului: ");
            if(nr==1)
            {
                printf("N\n");
            }
            else if(nr==2)
            {
                printf("S\n");
            }
            else if(nr==3)
            {
                printf("E\n");
            }
            else if(nr==4)
            {
                printf("V\n");
            }
            else if(nr==5)
            {
                printf("NE\n");
            }
            else if(nr==6)
            {
                printf("NV\n");
            }
            else if(nr==7)
            {
                printf("SE\n");
            }
            else if(nr==8)
            {
                printf("SV\n");
            }
        }
        k++;
        cuv = strtok(NULL," ");
    }
    printf("\n       \033[1;33mATENTIE! Carosabil umed.\033[0m \n\n");
}

void furtuna(char msj[])
{
    printf("\n           ⛈  Furtuna  ");
    char *cuv;
    int k=1,nr;
    cuv=strtok(msj, " ");
    cuv=strtok(NULL, " ");
    while(cuv!=NULL)
    {
        nr = atoi(cuv);
        if(k==1)  //temp
        {
            printf("%d°\n",nr);
        }
        else if(k==2)  //real_feel
        {
            printf("            real feel: %d°\n",nr);
        }
        else if(k==3)  //precipitatii
        {
            printf("          precipitatii: %d%%\n",nr);
        }
        else if(k==4)  //vant
        {
            printf("            vant: %d km/h\n",nr);
        }
        else if(k==5)  //directie
        {
            printf("         directia vantului: ");
            if(nr==1)
            {
                printf("N\n");
            }
            else if(nr==2)
            {
                printf("S\n");
            }
            else if(nr==3)
            {
                printf("E\n");
            }
            else if(nr==4)
            {
                printf("V\n");
            }
            else if(nr==5)
            {
                printf("NE\n");
            }
            else if(nr==6)
            {
                printf("NV\n");
            }
            else if(nr==7)
            {
                printf("SE\n");
            }
            else if(nr==8)
            {
                printf("SV\n");
            }
        }
        k++;
        cuv = strtok(NULL," ");
    }
    printf("\n  \033[1;33mAtentie! Posibile descarcari electrice.\033[0m\n\n");
}

void ninsoare_ploi(char msj[])
{
    printf("\n           🌨  Ninsoare si ploi  ");
    char *cuv;
    int k=1,nr;
    cuv=strtok(msj, " ");
    cuv=strtok(NULL, " ");
    while(cuv!=NULL)
    {
        nr = atoi(cuv);
        if(k==1)  //temp
        {
            printf("%d°\n",nr);
        }
        else if(k==2)  //real_feel
        {
            printf("                real feel: %d°\n",nr);
        }
        else if(k==3)  //precipitatii
        {
            printf("               precipitatii: %d%%\n",nr);
        }
        else if(k==4)  //vant
        {
            printf("                vant: %d km/h\n",nr);
        }
        else if(k==5)  //directie
        {
            printf("             directia vantului: ");
            if(nr==1)
            {
                printf("N\n");
            }
            else if(nr==2)
            {
                printf("S\n");
            }
            else if(nr==3)
            {
                printf("E\n");
            }
            else if(nr==4)
            {
                printf("V\n");
            }
            else if(nr==5)
            {
                printf("NE\n");
            }
            else if(nr==6)
            {
                printf("NV\n");
            }
            else if(nr==7)
            {
                printf("SE\n");
            }
            else if(nr==8)
            {
                printf("SV\n");
            }
        }
        k++;
        cuv = strtok(NULL," ");
    }
    printf("\n    \033[1;33mATENTIE! Averse diverse. Carosabil umed.\033[0m \n\n");
}

void ninsoare(char msj[])
{
    printf("\n           🌨  Ninsoare  ");
    char *cuv;
    int k=1,nr;
    cuv=strtok(msj, " ");
    cuv=strtok(NULL, " ");
    while(cuv!=NULL)
    {
        nr = atoi(cuv);
        if(k==1)  //temp
        {
            printf("%d°\n",nr);
        }
        else if(k==2)  //real_feel
        {
            printf("            real feel: %d°\n",nr);
        }
        else if(k==3)  //precipitatii
        {
            printf("           precipitatii: %d%%\n",nr);
        }
        else if(k==4)  //vant
        {
            printf("             vant: %d km/h\n",nr);
        }
        else if(k==5)  //directie
        {
            printf("         directia vantului: ");
            if(nr==1)
            {
                printf("N\n");
            }
            else if(nr==2)
            {
                printf("S\n");
            }
            else if(nr==3)
            {
                printf("E\n");
            }
            else if(nr==4)
            {
                printf("V\n");
            }
            else if(nr==5)
            {
                printf("NE\n");
            }
            else if(nr==6)
            {
                printf("NV\n");
            }
            else if(nr==7)
            {
                printf("SE\n");
            }
            else if(nr==8)
            {
                printf("SV\n");
            }
        }
        k++;
        cuv = strtok(NULL," ");
    }
    printf("\n  \033[1;33mATENTIE! Carosabil acoperit cu zapada.\033[0m \n\n");
}

void ploi_inghetate(char msj[])
{
    printf("\n           ❄️💧 Ploi inghetate  ");
    char *cuv;
    int k=1,nr;
    cuv=strtok(msj, " ");
    cuv=strtok(NULL, " ");
    while(cuv!=NULL)
    {
        nr = atoi(cuv);
        if(k==1)  //temp
        {
            printf("%d°\n",nr);
        }
        else if(k==2)  //real_feel
        {
            printf("                real feel: %d°\n",nr);
        }
        else if(k==3)  //precipitatii
        {
            printf("              precipitatii: %d%%\n",nr);
        }
        else if(k==4)  //vant
        {
            printf("                vant: %d km/h\n",nr);
        }
        else if(k==5)  //directie
        {
            printf("             directia vantului: ");
            if(nr==1)
            {
                printf("N\n");
            }
            else if(nr==2)
            {
                printf("S\n");
            }
            else if(nr==3)
            {
                printf("E\n");
            }
            else if(nr==4)
            {
                printf("V\n");
            }
            else if(nr==5)
            {
                printf("NE\n");
            }
            else if(nr==6)
            {
                printf("NV\n");
            }
            else if(nr==7)
            {
                printf("SE\n");
            }
            else if(nr==8)
            {
                printf("SV\n");
            }
        }
        k++;
        cuv = strtok(NULL," ");
    }
    printf("\n         \033[1;33mATENTIE! Carosabil alunecos.\033[0m \n\n");
}

void ceata(char msj[])
{
    printf("\n           🌫  Ceata  ");
    char *cuv;
    int k=1,nr;
    cuv=strtok(msj, " ");
    cuv=strtok(NULL, " ");
    while(cuv!=NULL)
    {
        nr = atoi(cuv);
        if(k==1)  //temp
        {
            printf("%d°\n",nr);
        }
        else if(k==2)  //real_feel
        {
            printf("           real feel: %d°\n",nr);
        }
        else if(k==3)  //precipitatii
        {
            printf("          precipitatii: %d%%\n",nr);
        }
        else if(k==4)  //vant
        {
            printf("           vant: %d km/h\n",nr);
        }
        else if(k==5)  //directie
        {
            printf("         directia vantului: ");
            if(nr==1)
            {
                printf("N\n");
            }
            else if(nr==2)
            {
                printf("S\n");
            }
            else if(nr==3)
            {
                printf("E\n");
            }
            else if(nr==4)
            {
                printf("V\n");
            }
            else if(nr==5)
            {
                printf("NE\n");
            }
            else if(nr==6)
            {
                printf("NV\n");
            }
            else if(nr==7)
            {
                printf("SE\n");
            }
            else if(nr==8)
            {
                printf("SV\n");
            }
        }
        k++;
        cuv = strtok(NULL," ");
    }
    printf("\n     \033[1;33mATENTIE! Vizibilitate redusa.\033[0m \n\n");
}

void meteo(char msj[])
{
    /* verifica id-ul vremii primit si apeleaza functia pentru afisare comform acestuia */
    char *tok, meteo[256];
    int nr;
    strcpy(meteo,msj);
    tok = strtok(meteo," ");
    nr = atoi(tok);
    if(nr==1)
    {
        soare(msj);
    }
    else if(nr==2)
    {
        soareish(msj);
    }
    else if(nr==3)
    {
        nori(msj);
    }
    else if(nr==4)
    {
        ploaie(msj);
    }
    else if(nr==5)
    {
        furtuna(msj);
    }
    else if(nr==6)
    {
        ninsoare_ploi(msj);
    }
    else if(nr==7)
    {
        ninsoare(msj);
    }
    else if(nr==8)
    {
        ploi_inghetate(msj);
    }
    else if(nr==9)
    {
        ceata(msj);
    }
    strcpy(msj,meteo);
}

void sport(char msj[])
{
    /* decodifica mesajul de la server si afiseaza informatiile corespunzatoare */
    char *id;
    int nr;
    id=strtok(msj," "); //se citeste cate stiri vor fi afisate
    nr=atoi(id);
    printf("\n\033[0;32m            Cele mai recente %d stiri din sport: \033[0m\n\n",nr);
    id=strtok(NULL," "); //se printeaza stirile
    while(id!=NULL)
    {
        nr=atoi(id);
        if(nr==1)
        {
            printf("  ⚽️    CFR Cluj dă semne de revenire, după victoria obținută în fața lui Dinamo, la București, scor 2-0.\n\n");
        }
        else if(nr==2)
        {
            printf("  🎾    Maria Sharapova (33 de ani), care s-a retras din lumea tenisului în 2020, a lăsat să se înțeleagă că se va mărita cu iubitul ei, Alexander Gilkes (41 de ani).\n\n");
        }
        else if(nr==3)
        {
            printf("  ⚽️    FCSB va fi nevoită să se mute din București la ultimul meci din 2020 - roș-albaștrii vor întâlni luni pe teren propriu pe Sepsi Sfântu Gheorghe, dar nu vor putea juca pe Arena Națională.\n\n");
        }
        else if(nr==4)
        {
            printf("  ⚽️    Dinamo va avea un nou stadion în viitorul apropiat, lucru confirmat astăzi de Marcel Vela, ministrul de Interne.\n\n");
        }
        else if(nr==5)
        {
            printf("  ⚽️    Toni Petrea a primit o veste bună înaintea derby-ului cu Universitatea Craiova - căpitanul echipei, Florin Tănase, ar putea reveni pe teren.\n\n");
        }
        else if(nr==6)
        {
            printf("  ⚽️    Ianis Hagi traversează din nou o perioadă în care stă mai mult pe banca de rezerve și ar putea schimba din nou campionatul după ce s-a perindat prin Italia, Belgia și Scoția.\n\n");
        }
        else if(nr==7)
        {
            printf("  ⚽️    Antoine Griezmann (29 de ani) a avut o apariție exotică la partida pe care Barcelona a câștigat-o cu Real Sociedad, scor 2-1, într-o restanță din La Liga.\n\n");
        }
        else if(nr==8)
        {
            printf("  🎾    Tenismenul Jack Sock s-a căsătorit cu Miss Carolina de Nord în plină pandemie.\n\n");
        }
        else if(nr==9)
        {
            printf("  🎾    Australian Open inedit. S-a stabilit data începerii turneului de la Melbourne: 8 februarie 2021.\n\n");
        }
        else if(nr==10)
        {
            printf("  🎾    \"Ar fi egoism să spun că eu am câștigat!\" Insider la triumful Simonei Halep la Wimbledon.\n\n");
        }
        else if(nr==11)
        {
            printf("  ⛸     Un fost campion american la patinaj artistic s-a sinucis.\n\n");
        }
        else if(nr==12)
        {
            printf("  ⛸     O româncă din Italia a câştigat un titlu european la patinaj artistic pe role.\n\n");
        }
        else if(nr==13)
        {
            printf("  ⛸     O rusoaică, în vârstă de 15 ani, a stabilit un record mondial la patinaj artistic.\n\n");
        }
        else if(nr==14)
        {
            printf("  ⛸     Fostul campion mondial la patinaj artistic Brian Boitano a anuntat ca este homosexual.\n\n");
        }
        else if(nr==15)
        {
            printf("  ⛸     O clujeanca este campioana nationala la patinaj artistic.\n\n");
        }
        else if(nr==16)
        {
            printf("  🏊    \"Cand adversarii dorm, noi ne antrenam!\" Un pusti de 15 ani poate fi cel mai tanar participant la Jocurile Olimpice de la Tokyo.\n\n");
        }
        else if(nr==17)
        {
            printf("  🏅    Un fost contabil, trecut pe la hochei si balet, si cu medalii la karate, e mai tare ca Phelps: \"Am invatat 150 de copii orfani sa inoate!\"\n\n");
        }
        else if(nr==18)
        {
            printf("  🏊    Cu adevarat o poveste de film: are 90 de ani, s-a apucat de inot in urma cu 3 ani, iar acum va participa la Campionatul Mondial.\n\n");
        }
        else if(nr==19)
        {
            printf("  🏊    Putea sa fie rivala lui Potec, dar a ajuns colega lui Neagu! A dat inotul pe handbal si acum vrea sa castige Liga Campionilor cu CSM!\n\n");
        }
        else if(nr==20)
        {
            printf("  💃    Balet cu mingea de fotbal! O balerina e in centrul atentiei la Mondial!\n\n");
        }
        id=strtok(NULL," ");
    }
}
