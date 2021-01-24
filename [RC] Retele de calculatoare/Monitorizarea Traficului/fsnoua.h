#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 
#include <string.h>

#define SQLITE_OK 0
/* portul folosit */
#define port 4529

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thread_info{
	int idThread; //id-ul thread-ului
	int cl; //descriptorul intors de accept (numarul de ordine al actiunilor facute de clienti)
}thread_info;

pthread_t th[1000];    //identificatorii thread-urilor care se vor crea

int map[30][30], peco[30][30], copie[30][30], clienti[1000], nr_clienti, clientul;

/* initializare/memorare informatii */
void harta();
void harta_peco();
void street_name(char str[], int coordX, int coordY);

/* generare stiri */
void vreme(char meteo[]);
void closest_peco(char msjpeco[]);
int repeat(int v[], int n, int id);
void id_sport(char msj[]);

/* functii pentru manipularea bazei de date cu informatii despre utilizatori */
static int callback_login(void *data, int argc, char **argv, char **ColName);
static int callback_stiri(void *data, int argc, char **argv, char **ColName);
static int callback_exista_signin(void *data, int argc, char **argv, char **ColName);
static int callback_signin(void *data, int argc, char **argv, char **ColName);
/* la login:    daca exista cuvantul => eroare */
int login(char user[], char paswd[]);
/* la signin:   daca exista cuvantul => eroare */
int signin(char user[], char paswd[]);

/* functii pentru informatii legate de viteza cu care merge clientul */
int get_speed();
int get_coordX();
int legal_speed(int viteza, int coordX, int coordY);
void speed (char msj[]);

/* definirea functiilor declarate mai sus: */

void harta()
{
    /* matricea memoreaza un graf neorientat cu cost pe muchie */
    /* se initializeaza harta cu valoarea 0 pe toate pozitiile */
    int i,j;
    for(i=0;i<30;i++)
    {
        for(j=0;j<30;j++)
        {
            map[i][j]=0;
        }
    }

    /* se adauga strazile pe harta cu limita lor de viteza */
    /* nu exista strazi cu sens unic */
    map[1][2]   =map[2][1]  =80;    // soseaua moara de foc 80
    map[2][10]  =map[10][2] =50;    // strada pacurari 50
    map[2][11]  =map[11][2] =50;    // stapungerea silvestru 50
    map[2][12]  =map[12][2] =50;    // bulevardul alexandru cel bun 50
    map[10][4]  =map[4][10] =30;    // bulevardul carol I 30
    map[10][11] =map[11][10]=30;    // strada garii 30
    map[11][12] =map[12][11]=80;    // strada silvestru 80 
    map[10][17] =map[17][10]=80;    // bulevardul independentei 80
    map[4][3]   =map[3][4]  =50;    // bulevardul carol I 50
    map[4][17]  =map[17][4] =50;    // strada sarariei 50
    map[17][16] =map[16][17]=50;    // strada sarariei 50
    map[16][14] =map[14][16]=80;    // strada palat 80
    map[16][15] =map[15][16]=50;    // strada sfantul lazar 50
    map[14][9]  =map[9][14] =100;   // bulevardul profesor dimitrie mangeron 100
    map[14][19] =map[19][14]=50;    // strada nicolina 50 
    map[9][8]   =map[8][9]  =40;    // strada elena doamna 40
    map[9][18]  =map[18][9] =50;    // bulevardul tudor vladimirescu 50 
    map[9][25]  =map[25][9] =90;    // bulevardul chimiei 90
    map[8][6]   =map[6][8]  =70;    // bulevardul tudor vladimirescu 70
    map[8][7]   =map[7][8]  =60;    // strada vasile lupu 60
    map[6][5]   =map[5][6]  =100;   // bulevardul constantin alexandru rosetti 100
    map[7][6]   =map[6][7]  =30;    // strada ciric 30
    map[18][19] =map[19][18]=60;    // bulevardul primaverii 60
    map[18][20] =map[20][18]=80;    // calea chisinaului 80
    map[20][21] =map[21][20]=60;    // strada bucium 60
    map[20][24] =map[24][20]=100;   // bulevardul virgil sahleanu 100 
    map[21][22] =map[22][21]=80;    // soseaua bucium 80
    map[21][23] =map[23][21]=50;    // strada trei fantani 50
    map[23][24] =map[24][23]=70;    // soseaua iasi-tomesti 70
    map[23][26] =map[26][23]=50;    // strada profesor petru olteanu 50 
    map[25][7]  =map[7][25] =50;    // strada aurel vlaicu 50
    map[25][24] =map[24][25]=30;    // strada sfantul ioan 30
    map[19][13] =map[13][19]=80;    // bulevardul nicolae iorga 80
    map[13][12] =map[12][13]=80;    // strada silvestru 80
    map[13][14] =map[14][13]=50;    // soseaua nationala 50
    map[15][8]  =map[8][15] =30;    // strada smardan 30
    map[15][14] =map[14][15]=50;    // strada sfantul lazar 50
}

void harta_peco() 
{
    /* matricea memoreaza un graf neorientat cu cost pe muchie */
    /* se initializeaza harta benzinariilor */
    int i,j;
    for(i=0;i<30;i++)
    {
        for(j=0;j<30;j++)
        {
            peco[i][j]=0;
        }
    }

    /* fiecare benzinarie are un cod unic de indentificare */
    /* se adauga pe harta codurile corespunzatoare fiecarei locatii */

    /* MOL (fara gpl): */
    peco[4][17] =peco[17][4]    =1;
    peco[20][21]=peco[21][20]   =1;
    peco[9][25] =peco[25][9]    =1;

    /* Petrom: */
    peco[2][10] =peco[10][2]    =2;
    peco[18][19]=peco[19][18]   =2;


    /* OMV: */
    peco[5][6]  =peco[6][5]     =3;
    peco[14][9] =peco[9][14]    =3;
    peco[7][25] =peco[25][7]    =3;

    /* Lukoil: */
    peco[1][2]  =peco[2][1]     =4;
    peco[13][19]=peco[19][13]   =4;
    peco[20][24]=peco[24][20]   =4;
    peco[6][7]  =peco[7][6]     =4;

    /* Rompetrol: */
    peco[10][17]=peco[17][10]   =5;
    peco[21][23]=peco[23][21]   =5;

    /* Socar (fara gpl): */ 
    peco[21][22]=peco[22][21]   =6;
    peco[23][26]=peco[26][23]   =6;
}

void vreme(char meteo[])
{
    /* se genereaza informatii despre vreme si sunt codificate intr-o forma pe care clientul o poate intelege */

    int id, temp, vant, precip, real_feel, directie;
    /* se genereaza id-ul vremii */
    srand (time(NULL));
    id=rand()%9+1;
    /* se genereaza diferenta dintre temperatura si real feel */
    real_feel=rand()%4;
    if(id==1)       //soare 
    {
        /* se genereaza temperatura */
        temp=rand()%30+18;
        /* se genereaza precopitatiile */
        precip=rand()%10;
        /* se genereaza viteza vantului */
        vant=rand()%10+1;
        while(precip>100)
        {
            /* se verifica procentajul precipitatiilor sa nu depaseasca 100 */
            precip=rand()%10;
        }
        /* se actualizeaza temperatura resimtita in functie de temperatura inregistrata */
        real_feel=real_feel+temp;
        /* se genereaza directia vantului */
        directie=rand()%8+1;
        /* mesajul va fi codat astfel: id|temp|real_feel|precipitatii|vant|directie unde '|' = un spatiu ' ' */
        sprintf(meteo,"%d %d %d %d %d %d",id,temp,real_feel,precip,vant,directie);
    }
    else if(id==2)  //soare si partial innorat
    {
        /* detalii/comentarii despre codul de mai jos se gasesc in if(id==1){} */
        temp=rand()%28+15;
        precip=rand()%40+10;
        while(precip>100)
        {
            precip=rand()%40+10;
        }
        vant=rand()%15+1;
        real_feel=real_feel+temp;
        directie=rand()%8+1;
        sprintf(meteo,"%d %d %d %d %d %d",id,temp,real_feel,precip,vant,directie);
    }
    else if(id==3)  //nori
    {
        /* detalii/comentarii despre codul de mai jos se gasesc in if(id==1){} */
        temp=rand()%26+5;
        precip=rand()%70+20;
        while(precip>100)
        {
            precip=rand()%70+20;
        }
        vant=rand()%20+1;
        real_feel=real_feel+temp;
        directie=rand()%8+1;
        sprintf(meteo,"%d %d %d %d %d %d",id,temp,real_feel,precip,vant,directie);
    }
    else if(id==4)  //ploaie 
    {
        /* detalii/comentarii despre codul de mai jos se gasesc in if(id==1){} */
        temp=rand()%25+2;
        precip=rand()%99+52;
        while(precip>100)
        {
            precip=rand()%99+52;
        }
        vant=rand()%35+10;
        real_feel=real_feel+temp;
        directie=rand()%8+1;
        sprintf(meteo,"%d %d %d %d %d %d",id,temp,real_feel,precip,vant,directie);
    }
    else if(id==5)  //furtuna
    {
        /* detalii/comentarii despre codul de mai jos se gasesc in if(id==1){} */
        temp=rand()%20+2;
        precip=rand()%99+70;
        while(precip>100)
        {
            precip=rand()%99+70;
        }
        vant=rand()%40+20;
        real_feel=real_feel+temp;
        directie=rand()%8+1;
        sprintf(meteo,"%d %d %d %d %d %d",id,temp,real_feel,precip,vant,directie);
    }
    else if(id==6)  //ninsoare si ploi
    {
        /* detalii/comentarii despre codul de mai jos se gasesc in if(id==1){} */
        temp=rand()%15;
        precip=rand()%98+50;
        while(precip>100)
        {
            precip=rand()%98+50;
        }
        vant=rand()%20+5;
        real_feel=real_feel+temp;
        directie=rand()%8+1;
        sprintf(meteo,"%d %d %d %d %d %d",id,temp,real_feel,precip,vant,directie);
    }
    else if(id==7)  //ninsoare
    {
        /* detalii/comentarii despre codul de mai jos se gasesc in if(id==1){} */
        temp=rand()%20;
        precip=rand()%98+50;
        while(precip>100)
        {
            precip=rand()%98+50;
        }
        vant=rand()%40+10;
        real_feel=real_feel+temp;
        directie=rand()%8+1;
        sprintf(meteo,"%d -%d -%d %d %d %d",id,temp,real_feel,precip,vant,directie);
    }
    else if(id==8)  //ploi inghetate
    {
        /* detalii/comentarii despre codul de mai jos se gasesc in if(id==1){} */
        temp=rand()%25;
        precip=rand()%99+50;
        while(precip>100)
        {
            precip=rand()%99+50;
        }
        vant=rand()%30+5;
        real_feel=real_feel+temp;
        directie=rand()%8+1;
        sprintf(meteo,"%d -%d -%d %d %d %d",id,temp,real_feel,precip,vant,directie);
    }
    else if(id==9)  //ceata
    {
        /* detalii/comentarii despre codul de mai jos se gasesc in if(id==1){} */
        temp=rand()%13;
        precip=rand()%50+30;
        while(precip>100)
        {
           precip=rand()%50+30;
        }
        vant=rand()%3+1;
        real_feel=real_feel+temp;
        directie=rand()%8+1;
        sprintf(meteo,"%d %d %d %d %d %d",id,temp,real_feel,precip,vant,directie);
    }
}

void street_name(char str[], int coordX, int coordY)
{
    /* verifica coordonatele primite si returneaza numele strazii aflat in acea locatie in variabila str[] */
    if(coordX==1 && coordY==2)
    {
        strcpy(str, "Soseaua Moara de Foc");
    }
    else if(coordX==2 && coordY==10)
    {
        strcpy(str, "Strada Pacurari");
    }
    else if(coordX==2 && coordY==11)
    {
        strcpy(str, "Strapungerea Silvestru");
    }
    else if(coordX==2 && coordY==12)
    {
        strcpy(str, "Bulevardul Alexandru cel Bun");
    }
    else if(coordX==10 && coordY==4)
    {
        strcpy(str, "Bulevardul Carol I");
    }
    else if(coordX==10 && coordY==11)
    {
        strcpy(str, "Strada Garii");
    }
    else if(coordX==11 && coordY==12)
    {
        strcpy(str, "Strada Silvestru");
    }
    else if(coordX==10 && coordY==17)
    {
        strcpy(str, "Bulevardul Independentei");
    }
    else if(coordX==4 && coordY==3)
    {
        strcpy(str, "Bulevardul Carol I");
    }
    else if(coordX==4 && coordY==17)
    {
        strcpy(str, "Strada Sarariei");
    }
    else if(coordX==17 && coordY==16)
    {
        strcpy(str, "Strada Sarariei");
    }
    else if(coordX==16 && coordY==14)
    {
        strcpy(str, "Strada Palat");
    }
    else if(coordX==16 && coordY==15)
    {
        strcpy(str, "Strada Sfantul Lazar");
    }
    else if(coordX==14 && coordY==9)
    {
        strcpy(str, "Bulevardul Profesor Dimitrie Mangeron");
    }
    else if(coordX==14 && coordY==19)
    {
        strcpy(str, "Strada Nicolina");
    }
    else if(coordX==9 && coordY==8)
    {
        strcpy(str, "Strada Elena Doamna");
    }
    else if(coordX==9 && coordY==18)
    {
        strcpy(str, "Bulevardul Tudor Vladimirescu");
    }
    else if(coordX==9 && coordY==25)
    {
        strcpy(str, "Bulevardul Chimiei");
    }
    else if(coordX==8 && coordY==6)
    {
        strcpy(str, "Bulevardul Tudor Vladimirescu");
    }
    else if(coordX==8 && coordY==7)
    {
        strcpy(str, "Strada Vasile Lupu");
    }
    else if(coordX==6 && coordY==5)
    {
        strcpy(str, "Bulevardul Constantin Alexandru Rosetti");
    }
    else if(coordX==7 && coordY==6)
    {
        strcpy(str, "Strada Ciric");
    }
    else if(coordX==18 && coordY==19)
    {
        strcpy(str, "Bulevardul Primaverii");
    }
    else if(coordX==18 && coordY==20)
    {
        strcpy(str, "Calea Chisinaului");
    }
    else if(coordX==20 && coordY==21)
    {
        strcpy(str, "Strada Bucium");
    }
    else if(coordX==20 && coordY==24)
    {
        strcpy(str, "Bulevardul Virgil Sahleanu");
    }
    else if(coordX==21 && coordY==22)
    {
        strcpy(str, "Soseaua Bucium");
    }
    else if(coordX==21 && coordY==23)
    {
        strcpy(str, "Strada Trei Fantani");
    }
    else if(coordX==23 && coordY==24)
    {
        strcpy(str, "Soseaua Iasi-Tomesti");
    }
    else if(coordX==23 && coordY==26)
    {
        strcpy(str, "Strada Profesor Petru Olteanu");
    }
    else if(coordX==25 && coordY==7)
    {
        strcpy(str, "Soseaua Aurel Vlaicu");
    }
    else if(coordX==25 && coordY==24)
    {
        strcpy(str, "Strada Sfantul Ioan");
    }
    else if(coordX==19 && coordY==13)
    {
        strcpy(str, "Bulevardul Nicolae Iorga");
    }
    else if(coordX==13 && coordY==12)
    {
        strcpy(str, "Strada Silvestru");
    }
    else if(coordX==13 && coordY==14)
    {
        strcpy(str, "Soseaua Nationala");
    }
    else if(coordX==15 && coordY==8)
    {
        strcpy(str, "Strada Smardan");
    }
    else if(coordX==15 && coordY==14)
    {
        strcpy(str, "Strada Sfantul Lazar");
    }
    else if(coordX==2 && coordY==1)
    {
        strcpy(str, "Soseaua Moara de Foc");
    }
    else if(coordX==10 && coordY==2)
    {
        strcpy(str, "Strada Pacurari");
    }
    else if(coordX==11 && coordY==2)
    {
        strcpy(str, "Strapungerea Silvestru");
    }
    else if(coordX==12 && coordY==2)
    {
        strcpy(str, "Bulevardul Alexandru cel Bun");
    }
    else if(coordX==4 && coordY==10)
    {
        strcpy(str, "Bulevardul Carol I");
    }
    else if(coordX==11 && coordY==10)
    {
        strcpy(str, "Strada Garii");
    }
    else if(coordX==12 && coordY==11)
    {
        strcpy(str, "Strada Silvestru");
    }
    else if(coordX==17 && coordY==10)
    {
        strcpy(str, "Bulevardul Independentei");
    }
    else if(coordX==3 && coordY==4)
    {
        strcpy(str, "Bulevardul Carol I");
    }
    else if(coordX==17 && coordY==4)
    {
        strcpy(str, "Strada Sarariei");
    }
    else if(coordX==16 && coordY==17)
    {
        strcpy(str, "Strada Sarariei");
    }
    else if(coordX==14 && coordY==16)
    {
        strcpy(str, "Strada Palat");
    }
    else if(coordX==15 && coordY==16)
    {
        strcpy(str, "Strada Sfantul Lazar");
    }
    else if(coordX==9 && coordY==14)
    {
        strcpy(str, "Bulevardul Profesor Dimitrie Mangeron");
    }
    else if(coordX==19 && coordY==14)
    {
        strcpy(str, "Strada Nicolina");
    }
    else if(coordX==8 && coordY==9)
    {
        strcpy(str, "Strada Elena Doamna");
    }
    else if(coordX==18 && coordY==9)
    {
        strcpy(str, "Bulevardul Tudor Vladimirescu");
    }
    else if(coordX==25 && coordY==9)
    {
        strcpy(str, "Bulevardul Chimiei");
    }
    else if(coordX==6 && coordY==8)
    {
        strcpy(str, "Bulevardul Tudor Vladimirescu");
    }
    else if(coordX==7 && coordY==8)
    {
        strcpy(str, "Strada Vasile Lupu");
    }
    else if(coordX==5 && coordY==6)
    {
        strcpy(str, "Bulevardul Constantin Alexandru Rosetti");
    }
    else if(coordX==6 && coordY==7)
    {
        strcpy(str, "Strada Ciric");
    }
    else if(coordX==19 && coordY==18)
    {
        strcpy(str, "Bulevardul Primaverii");
    }
    else if(coordX==20 && coordY==18)
    {
        strcpy(str, "Calea Chisinaului");
    }
    else if(coordX==21 && coordY==20)
    {
        strcpy(str, "Strada Bucium");
    }
    else if(coordX==24 && coordY==20)
    {
        strcpy(str, "Bulevardul Virgil Sahleanu");
    }
    else if(coordX==22 && coordY==21)
    {
        strcpy(str, "Soseaua Bucium");
    }
    else if(coordX==23 && coordY==21)
    {
        strcpy(str, "Strada Trei Fantani");
    }
    else if(coordX==24 && coordY==23)
    {
        strcpy(str, "Soseaua Iasi-Tomesti");
    }
    else if(coordX==26 && coordY==23)
    {
        strcpy(str, "Strada Profesor Petru Olteanu");
    }
    else if(coordX==7 && coordY==25)
    {
        strcpy(str, "Soseaua Aurel Vlaicu");
    }
    else if(coordX==24 && coordY==25)
    {
        strcpy(str, "Strada Sfantul Ioan");
    }
    else if(coordX==13 && coordY==19)
    {
        strcpy(str, "Bulevardul Nicolae Iorga");
    }
    else if(coordX==12 && coordY==13)
    {
        strcpy(str, "Strada Silvestru");
    }
    else if(coordX==14 && coordY==13)
    {
        strcpy(str, "Soseaua Nationala");
    }
    else if(coordX==8 && coordY==15)
    {
        strcpy(str, "Strada Smardan");
    }
    else if(coordX==14 && coordY==15)
    {
        strcpy(str, "Strada Sfantul Lazar");
    }
    //return str;
}

static int callback_login(void *data, int argc, char **argv, char **ColName) 
{
    /* daca nu exista cuvantul cautat => eroare */
   int i,ok=0;
   fprintf(stderr, "%s", (const char*)data);    //afiseaza ca functia a fost apelata 
   for(i = 0; i<argc; i++)                      //se afiseaza valorile argumentelor din tabel care au fost cautate
   {
        printf("%s = ",ColName[i]);
        if(atoi(argv[i])==0)
        {
            ok=1;
            printf("0\n");
        }
        else if (argv[i]!=NULL)
        {
            printf("%s\n", argv[i]);
        }
        else
        {
            printf("NULL");
        } 
   }
   printf("\n");
   return ok;
}

static int callback_stiri(void *data, int argc, char **argv, char **ColName) 
{
    //returneaza 0 pentru nu si >0 pentru da 
   int i,ok;
   fprintf(stderr, "%s", (const char*)data);    //afiseaza ca functia a fost apelata 
   for(i = 0; i<argc; i++)                      //se afiseaza valorile argumentelor din tabel care au fost cautate
   {
        printf("%s = %s",ColName[i],argv[i]);
        if(strcmp(argv[i],"da")==0)
        {
            ok = 3;
        }
        else if (strcmp(argv[i],"nu")==0)
        {
            ok = 0;
        }
        i=argc;
   }
   printf("\n");
   return ok;
}

static int callback_exista_signin(void *data, int argc, char **argv, char **ColName) 
{
   int i,ok=1;
   fprintf(stderr, "%s", (const char*)data);    //afiseaza ca functia a fost apelata 
   for(i = 0; i<argc; i++)                      //se afiseaza valorile argumentelor din tabel care au fost cautate
   {
        printf("%s = ",ColName[i]);
        if(atoi(argv[i])==0)
        {
            ok=0;
            printf("0\n");
        }
        else if (argv[i]!=NULL)
        {
            printf("%s\n", argv[i]);
        }
        else
        {
            printf("NULL");
        } 
   }
   printf("\n");
   return ok;
}

static int callback_signin(void *data, int argc, char **argv, char **ColName) 
{
   return 1;
}

int login(char user[], char paswd[])
{
    sqlite3 *db;
    int     rc, rd, ok;
    /* se deschide baza de date */
    rc = sqlite3_open("trafic.db", &db);
    if(rc != SQLITE_OK) 
    {
        fprintf(stderr, "Nu se poate accesa baza de date: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    else
    {
        fprintf(stderr, "Baza de date a fost accesata cu succes.\n\n");
    }
    /* daca s-a deschis cu succes: */
    char        *ErrMsg, *sql, *sql2;
    int         logat = 0;
    const char  *mesaj_callback = "Se apeleaza functia callback:\n";
    sql = (char *)malloc(256);
    sql2 = (char *)malloc(256);
    /* se creaza o instanta sql */
    sprintf(sql, "SELECT count(nume) from users WHERE nume='%s' and paswd='%s'", user, paswd);

    /* se executa instanta sql */
    rc = sqlite3_exec(db, sql, callback_login, (void*)mesaj_callback, &ErrMsg);
 
    /* daca se produce o eroare atunci: */
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "Acest user nu exista.\n");
        logat=0;
        sqlite3_free(ErrMsg);
    }
    /* daca nu se produce nicio eroare => userul exista */
    else 
    {   
        /* verificam daca userul este abonat la stiri */
        /*  1-nu 
            3-da  */
        /* se creaza o instanta sql */
        sprintf(sql2, "SELECT stiri from users WHERE nume='%s' and paswd='%s'", user, paswd);

        /* se executa instanta sql */
        rd = sqlite3_exec(db, sql2, callback_stiri, (void*)mesaj_callback, &ErrMsg);

        if(rd != SQLITE_OK)
        {
            logat = 3;
        }
        else
        {
            logat = 1;
            sqlite3_free(ErrMsg);
        }
    
        fprintf(stdout, "Operatiile au fost facute.\n");
    }
    /* se inchide baza de date */
    sqlite3_close(db);
    /* se elibereaza memoria ocupata */
    free(sql);
    free(sql2);
    return logat;
}

int signin(char user[], char paswd[])
{
    sqlite3 *db;
    int     rc,rd,ok;
    /* se deschide baza de date */
    rc = sqlite3_open("trafic.db", &db);
    if(rc != SQLITE_OK) 
    {
        fprintf(stderr, "Nu se poate accesa baza de date: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    else
    {
        fprintf(stderr, "Baza de date a fost accesata cu succes.\n\n");
    }
    /* daca s-a deschis cu succes: */
    char        *ErrMsg, *sql_verif, *sql;
    int         logat=0;
    const char  *mesaj_callback = "Se apeleaza functia callback:\n";
    sql_verif = (char *)malloc(256);
    sql = (char *)malloc(256);
    /* se creaza o instanta sql */
    sprintf(sql_verif, "SELECT count(nume) from users WHERE nume='%s'", user);
    
    /* se executa instanta sql */
    rc = sqlite3_exec(db, sql_verif, callback_exista_signin, (void*)mesaj_callback, &ErrMsg);
 
    if(rc != SQLITE_OK)
    {
        fprintf(stderr, "Acest user deja exista.\n");
        logat=0;
        sqlite3_free(ErrMsg);
    }
    else
    {
        logat=0;
        fprintf(stdout, "Se poate face signin.\n");
        /* se va introduce userul in baza de date si se va autentifica */

        /* se creaza o instanta sql */
        sprintf(sql, "INSERT INTO users VALUES ('%s', '%s', 'nu')", user, paswd);

        /* se executa instanta sql */
        rd = sqlite3_exec(db, sql, callback_signin, (void*)mesaj_callback, &ErrMsg);
        
        if(rd != SQLITE_OK)
        {
            logat=0;
            printf("nu e bn =))\n");
        }
        else
        {
            logat=2;
            printf("bravo =))\n");
        }
   
    }
    /* se inchide baza de date */
    sqlite3_close(db);
    /* se elibereaza memoria ocupata */
    free(sql_verif);
    free(sql);
    return logat;
}

void news(char stiri[], char user[])
{
    sqlite3 *db;
    int rc,ok;
    /* se deschide baza de date */
    rc = sqlite3_open("trafic.db", &db);
    if(rc != SQLITE_OK) 
    {
        fprintf(stderr, "Nu se poate accesa baza de date: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Baza de date a fost accesata cu succes.\n\n");
    }
    /* daca s-a deschis cu succes: */
    char        *ErrMsg, *sql = malloc(256);
    const char  *mesaj_callback = "Se apeleaza functia callback:\n";

    /* se creaza o instanta sql */
    sprintf(sql, "UPDATE users SET stiri='%s' where nume='%s'", stiri, user);

    /* se executa instanta sql */
    rc = sqlite3_exec(db, sql, callback_signin, (void*)mesaj_callback, &ErrMsg);
 
    if(rc != SQLITE_OK)
    {
        printf("%s\n",ErrMsg);
        sqlite3_free(ErrMsg);
    }
    else
    {
        fprintf(stdout, "Operatiile au fost facute.\n");
    }
    /* se inchide baza de date */
    sqlite3_close(db);
    /* se elibereaza memoria ocupata */
    free(sql);
}

int get_speed()
{
    /* se primeste automat viteza autovehiculuilui (in intervalul 0-180) */
    int s;
    srand(time(NULL));
    s=rand();
    while(s<0 || s>180)
    {
        s=rand();
    }
    return s;
}

int get_coordX()
{
    /* se genereaza una din coordonate */ 
    srand(time(NULL));
    int coordX = rand()%26+1;
    return coordX;
}

void closest_peco(char msjpeco[])
{
    /* se va lucra pe o copie a hartii benzinariilor */
    int i,j;
    for(i=0;i<30;i++)
    {
        for(j=0;j<30;j++)
        {
            copie[i][j]=peco[i][j];
        }
    }

    int     coordX, coordY, nr_peco=0;
    char    mesaj[256];
    /* se gaseste locatia clientului */
    srand(time(NULL));
    coordX = get_coordX();
    coordY = get_coordX();
    while(map[coordX][coordY]==0)
    {
        /* se verifica locatia (sa fie una existenta) */
        coordX = get_coordX();
        coordY = get_coordX();
    }

    /* se memoreaza numele strazii pe care se afla clientul */
    street_name(msjpeco,coordX,coordY);
    strcat(msjpeco,"#");

    /* se verifica daca pe strada unde se afla clientul este vreo benzinarie */
    if(copie[coordX][coordY]>0)
    {
        /* daca da se incrementeaza nr_peco */
        nr_peco=1;
        /* se adauga id-ul benzinariei gasite la mesajul pentru client */
        sprintf(mesaj, " %d",copie[coordX][coordY]);
        strcat(msjpeco,mesaj);
        /*  pentru a nu ne afisa aceasi benzinarie de mai multe ori,
            cand gasim una apropiata de locatia clientului o stergem de pe copia hartii */
        copie[coordX][coordY]=copie[coordY][coordX]=0;
    }

    /* se parcurge matricea cu benzinarii concentric cu locatia clientului */
    for(i=1;i<=25;i++)
    {   /* daca s-au gasit 3 benzinarii, algoritmul se opreste si mesajul este gata pentru a fi trimis clientului */
        if(nr_peco==3) i=30;
        for(j=1;j<=i;j++)
        {
            if(nr_peco<3 && coordX>j && coordY>j && copie[coordX-j][coordY-j]>0)
            {
                nr_peco++;
                sprintf(mesaj, " %d",copie[coordX-j][coordY-j]);
                strcat(msjpeco,mesaj);
                copie[coordX-j][coordY-j]=copie[coordY-j][coordX-j]=0;
            }
            else if(nr_peco<3 && coordX>j && copie[coordX-j][coordY]>0)
            {
                nr_peco++;
                sprintf(mesaj, " %d",copie[coordX-j][coordY]);
                strcat(msjpeco,mesaj);
                copie[coordX-j][coordY]=copie[coordY][coordX-j]=0;
            }
            else if(nr_peco<3 && coordX>j && coordY+j<=26 && copie[coordX-j][coordY+j]>0)
            {
                nr_peco++;
                sprintf(mesaj, " %d",copie[coordX-j][coordY+j]);
                strcat(msjpeco,mesaj);
                copie[coordX-j][coordY+j]=copie[coordY+j][coordX-j]=0;
                
            }
            else if(nr_peco<3 && coordY>j && copie[coordX][coordY-j]>0)
            {
                nr_peco++;
                sprintf(mesaj, " %d",copie[coordX][coordY-j]);
                strcat(msjpeco,mesaj);
                copie[coordX][coordY-j]=copie[coordY-j][coordX]=0;
                
            }
            else if(nr_peco<3 && coordY+j<=26 && copie[coordX][coordY+j]>0)
            {
                nr_peco++;
                sprintf(mesaj, " %d",copie[coordX][coordY+j]);
                strcat(msjpeco,mesaj);
                copie[coordX][coordY+j]=copie[coordY+j][coordX]=0;
                
            }
            else if(nr_peco<3 && coordX+j<=26 && coordY>j && copie[coordX+j][coordY-j]>0)
            {
                nr_peco++;
                sprintf(mesaj, " %d",copie[coordX+j][coordY-j]);
                strcat(msjpeco,mesaj);
                copie[coordX+j][coordY-j]=copie[coordY-j][coordX+j]=0;
                
            }
            else if(nr_peco<3 && coordX+j<=26 && copie[coordX+j][coordY]>0)
            {
                nr_peco++;
                sprintf(mesaj, " %d",copie[coordX+j][coordY]);
                strcat(msjpeco,mesaj);
                copie[coordX+j][coordY]=copie[coordY][coordX+j]=0;
                
            }
            else if(nr_peco<3 && coordX+j<=26 && coordY+j<=26 && copie[coordX+j][coordY+j]>0)
            {
                nr_peco++;
                sprintf(mesaj, " %d",copie[coordX+j][coordY+j]);
                strcat(msjpeco,mesaj);
                copie[coordX+j][coordY+j]=copie[coordY+j][coordX+j]=0;
                
            }
        }
        if(nr_peco==3) i=30;
    }
}

int legal_speed(int viteza, int coordX, int coordY)
{
    int x = map[coordX][coordY]+40;
    if(viteza > x)
    {
        /* 3 = se poate suspenda permisul */
        return 3;
    }
    else if(viteza > map[coordX][coordY])
    {
        /* 0 = a depasit limita legala pe acea portiune de strada */
        return 0;
    }
    else if(map[coordX][coordY]>40 && viteza<map[coordX][coordY]/2.5)
    {
        /* 2 = merge prea incet */
        return 2;
    }
    else if(map[coordX][coordY]<=40 && viteza<17)
    {
        /* 2 = merge prea incet */
        return 2;
    }
    /* 1 = merge cu viteza legala */
    return 1;
}

void speed (char msj[])
{
    int     viteza, coordX, coordY, ok;
    char    cuv[256], str[256];
    /* se primeste viteza autovehiculului */
    viteza = get_speed();

    /* se primeste locatia clientului */
    coordX = get_coordX();
    coordY = get_coordX();
    while(map[coordX][coordY]==0)
    {
        coordX = get_coordX();
        coordY = get_coordX();
    }
    /* se gaseste numele strazii */
    street_name(str,coordX,coordY);
    /* se veifica viteza */
    ok = legal_speed(viteza,coordX,coordY);
    /* se creaza mesajul pentru client */
    /* mesaj = ok|viteza|strada, unde ok = viteza legala depasita sau nu si '|' = un spatiu ' ' */
    sprintf(cuv,"%d", ok);
    strcpy(msj,cuv);
    strcat(msj, " ");
    sprintf(cuv,"%d", viteza);
    strcat(msj,cuv);
    strcat(msj," ");
    strcat(msj,str);
}

int repeat(int v[], int n, int id)
{
    /* se parcurge vectorul v cu lungimea n si se verifica daca exista deja valoarea variabilei id */
    int i;
    for(i=0;i<=n;i++)
    {
        if(v[i]==id) return 0; //valoarea deja exista
    }
    return 1; //valoarea nu exista si poate fi adaugata
}

void id_sport(char msj[])
{
    srand (time(NULL));
    int nr, k, v[30], lungime, ok;
    /* determina cate stiri vor aparea pe ecran */
    nr = rand()%8+3;
    while(nr<3 || nr>8)
    {
        nr = rand()%8+3;
    }
    sprintf(msj,"%d",nr);
    lungime=0;
    while(nr!=0)
    {   /* determina id-ul stirilor ce vor aparea */
        k=rand()%20+1;
        if(lungime==0) 
        {
            /* prima stire generata va fi inserata in vector fara alte verificari */
            v[lungime++]=k;
            sprintf(msj,"%s %d",msj,k);
            nr--;
        }
        else 
        {
            /* se verifica daca stirea nu a fost deja inseara in vector */
            ok=repeat(v,lungime,k);
            if(ok==1)
            {
                /* daca stirea nu se va repeta, atunci poate fi introdusa in mesajul pentru client */
                v[lungime++]=k;
                sprintf(msj,"%s %d",msj,k);
                nr--;
            }
        }
    }
}