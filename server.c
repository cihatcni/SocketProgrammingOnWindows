/**
@file
SEMESTER PROJECT
Mesajlaşma Programı - Server
@author
İsim : Cihat Bozkurt
Soyisim : Cüni
Öğrenci Numarası : 15011041
Tarih: 08.01.2018
E-Mail: cihatc52@gmail.com
Compiler : GCC
IDE: Geany 
Operating System : Windows 10
CPU : 2.60 Ghz
Ram : 8 GB
*/


#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>

typedef struct {
	int userID;			//Kullanici id
	char username[20];	//Kullanici adi
	char name[20];		//Isım
	char surname[20];	//Soyisim
	char pnumber[11];	//Telefon Numarasi
} USERLIST ;


void mesajAl(SOCKET[],int,USERLIST[]);			//Kullanicidan mesajlari alir.
void rehberGonder(SOCKET, int,USERLIST[]);		//Rehberş kullaniciya gonderir.
void gelenKutusu(SOCKET,int);					//Kullaniciya gelen mesajlari iletir.
int karsilastir (char[],char[]);				//String karsilastirmasi yapar.
void rehberiYazdir (FILE *,int ,USERLIST[]);	//Rehberi yazdirir.

 
int main() {
	/**
	 * @param serverSocket : Kullanicilarin baglanacagi soket.
	 * @param user[] = Kullanicilara baglanilacak soketler.
	 * @param tempSocket : Geçici soket işlemlerini tutar.
	 * @param userlist : Kullanicilarin bilgilerini tutar.
	 * @param contacts : Rehberin bulundugu dosya.
	 * @param adresBoyut: server structunun boyutunu tutar.
	 * @param replySize : Gelen mesajın büyüklüğünü tutar.
	 * @param userCount : Kullanici sayisini tutar.
	 * @param maxUserCount : Baglanabilecek maks. kullanici sayisini tutar.
	 * @param ret : karsilastir fonksiyonundan gelen sonucu tutar.
	 * @param message : Gonderilecek mesaji tutar.
	 * @param userReply : Kullanicidan gelen mesaji tutar.
	 * @param secim : Kullanicidan gelen secimi tutar.
	 * */
	
    WSADATA wsa;
    SOCKET serverSocket, user[20],tempSocket;
    USERLIST userlist[20];
    FILE *contacts;
    struct sockaddr_in server , client;
    int adresBoyut,replySize,userCount=0,i,maxUserCount=20,ret;
    char *message, userReply[100],secim='0';
    fd_set readfds;

    
    for(i=0; i<maxUserCount; i++) //Socket bilgilerini sıfırlar.
        user[i] = 0;
 
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) { //Kütüphaneyi başlatır.
        printf("Hata olustu : %d",WSAGetLastError());
        return 1;
    }
	
	//Socketi başlatır.
    if((serverSocket = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
        printf("Soket olusturulamadi : %d" , WSAGetLastError());
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; //Herhangi bir adres
    server.sin_port = htons(2018); //Serverin acilacaği port girişi
	
	//Dinleme işleminin başlatılması
    if(bind(serverSocket ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR) {
        printf("Bind baslatilamadi : %d" , WSAGetLastError());
        return 2;
    }

    listen(serverSocket , 3);
    
    contacts = fopen ("contacts.txt","r+");
    
    i=0;
	while (!feof(contacts)) { //Rehberdeki bilgileri yükler.
		fscanf(contacts,"%d%s%s%s%s",&userlist[i].userID, userlist[i].username, 
							userlist[i].name, userlist[i].surname, userlist[i].pnumber);
		user[i]=0;
		i++;
		userCount++;
	}
	
	rehberiYazdir (contacts,userCount,userlist);
    
    printf("Server basariyla baslatildi.\nBaglantilar bekleniyor.\n");
     
    adresBoyut = sizeof(struct sockaddr_in);
    while (TRUE) {
		FD_ZERO(&readfds); 					//fd_setteki değeri temizler.
		FD_SET(serverSocket, &readfds);		//Soketteki değeri fd_sete atar.
		
		for (i=0; i<maxUserCount; i++)  {   //Client varsa soket bilgisini fd_sete atar.
            if(user[i] > 0) {
                FD_SET(user[i], &readfds);
            }
        }
		
		if (select(0,&readfds,NULL,NULL,NULL) == SOCKET_ERROR )  { //fd_setteki bilgileri select yapısına aktarır.
            printf("Select hatasi");
            return 3;
        }
        //select() çalıştırıldıktan sonra readfds seçmiş olduğumuz dosya 
        //tanımlayıcılardan hangisinin okumak için hazır olduğunu 
        //yansıtacak şekilde güncellenir. 
		
		if (FD_ISSET(serverSocket, &readfds))  {	
			//Yeni bağlantıyı kabul etme kısmı
			tempSocket = accept(serverSocket , (struct sockaddr *)&client, &adresBoyut); 
			
			 //Kullanıcı adı alınması ve ilk mesajın atılması.
			if((replySize = recv(tempSocket, userReply , 100 , 0)) == SOCKET_ERROR) {
				printf("Mesaj alinamadi.\n");
			}
     
			userReply[replySize] = '\0';
			printf("%s isimli kullanici servera baglandi.\n", userReply); 
			
			i=0; ret=1;
			while (i<userCount && ret!=0) { 						//Kullanici adinin rehberde daha onceden
				ret=0;												//bulunup bulunmadigi kontrolü yapılır.
				ret = karsilastir(userlist[i].username,userReply);
				i++;
			}
			if (ret==0) {
				user[i-1]=tempSocket; // Kullanıcıyı numarasına göre listeye ekler.
				message = "Merhaba, uygulamamiza tekrar hosgeldiniz.\n";
				send(tempSocket, message , strlen(message) , 0);													
			}
			else { //Yeni kullanicin bilgileri alinir ve rehbere aktarilir.
				userCount++;
				user[i]=tempSocket;
				strcpy(userlist[i].username,userReply);
				message="0";
				send(tempSocket, message , strlen(message) , 0);
				message = "Merhaba, uygulamamiza ilk kez baglandiginiz\nicin bazi bilgileri doldurmaniz gerek.\n";
				send(tempSocket, message , strlen(message) , 0);
				replySize = recv(tempSocket, userReply , 100 , 0);
				userReply[replySize] = '\0';
				strcpy(userlist[i].name,userReply);
				replySize = recv(tempSocket, userReply , 100 , 0);
				userReply[replySize] = '\0';
				strcpy(userlist[i].surname,userReply);
				replySize = recv(tempSocket, userReply , 100 , 0);
				userReply[replySize] = '\0';
				strcpy(userlist[i].pnumber,userReply);
				userlist[i].userID=i;
				fprintf(contacts,"\n%d %s %s %s %s",userlist[i].userID, userlist[i].username, 
							userlist[i].name, userlist[i].surname, userlist[i].pnumber);	
				rehberiYazdir (contacts,userCount,userlist);		
			}
			
			printf("Guncel rehberde kayitli kullanici sayisi : %d\n",userCount);
			fclose(contacts);
	
		}
			
		for (i=0; i<userCount+1; i++) { 
			 if (FD_ISSET(user[i], &readfds)) { //Eğer kullanıcı fd_sette varsa
				 if(secim == '0') {
				 replySize= recv(user[i],&secim,1,0); //Kullanicinin secimin alinmasi
				}
				 if((replySize == SOCKET_ERROR) || (replySize == 0)) { //Hala bağlı mı kontrolü
					 printf("User[%d] cikis yapti.\n",i);
					 closesocket(user[i]);
					 user[i]=0;
				 }
				 else { //Kullanıcıyla işlemler burada olacak.
					switch(secim) {
						case '1' :
						mesajAl(user,i,userlist);			
						break;
					
						case '2' :
						gelenKutusu(user[i],i);
						break;
					
						case '3' : 
						rehberGonder(user[i],userCount,userlist) ;
						break;	
					}
					secim = '0';
				 }
			 }
		 }		
	}

    closesocket(serverSocket);
    WSACleanup();
     
    return 0;
}

int karsilastir (char username[],char name[]) {
	/**
	 * @param sayac : Farkli harf sayisini tutar.
	 * @param i : Harf sirasini tutar.
	 * Bu fonksiyon iki stringin harflerini karsilastirir.
	 * Gelen kullanici adini rehberde var mi diye kontrol eder.
	 */
	int sayac=0,i=0;
	
	if(strlen(name)==strlen(username)) {
		while(i!=strlen(name)) {
			//printf("[%c] ve [%c]\n",username[i],name[i]); // TEST
			if(username[i]!=name[i])
				sayac++;
			i++;
		}
		return sayac;
	}
	return 1;	
}

void rehberGonder(SOCKET user,int userCount,USERLIST userlist[]) {
	/**
	 * @param serverReply : Serverın mesajını tutar. 
	 * Bu fonksiyonda server rehberdeki kullanıcı bilgileri
	 * isteyen kullanıcıya gonderir.
	 */
	char serverReply[200];
		
	for(int j=0 ; j<userCount ; j++) {
		strcpy(serverReply,userlist[j].username);
		send(user,serverReply , 100 , 0);
		}
		serverReply[0]='!'; 	//Tum bilgilerin aktarildigi isareti.
		send(user,serverReply , strlen(serverReply) , 0);	
}

void gelenKutusu(SOCKET user,int i) {
	/**
	 * @param serverReply : Serverden gelen mesaji tutar.
	 * @param messages : Tüm mesajların bulunduğu dosya.
	 * Bu fonksiyon messages dosyasının içinden kullanıcıya ait
	 * mesajları seçer ve onu kullanıcıya gonderir.
	 */
	FILE *messages;
	char serverReply[200];
	
	messages = fopen("messages.txt","r");
	while(!feof(messages)) {
		fgets(serverReply,200,messages); 
		strtok(serverReply,"\n");
		int x=serverReply[0]-'0';  //Mesajın gönderilmesi için ilk satırındaki 
		if(x==i){				   //gonderilen kişi bilgisi ile mesajı 
			serverReply[0]='-';    //isteyen kişinin numarasi eşleşmelidir.
			send(user, serverReply , 100 , 0);	
		}									
	}
	serverReply[0]='!';
	send(user,serverReply , strlen(serverReply) , 0);
	fclose(messages);	
}

void mesajAl(SOCKET user[],int i,USERLIST userlist[]) {
	/**
	 * @param userReply : Kullanicidan gelen mesaji tutar.
	 * @param messages : Tüm mesajların bulunduğu dosya.
	 * @param messagewithTime : Mesajlara gonderen kişi ve zaman bilgisi ekler.
	 * Bu fonksiyon kullanicidan mesaji alir
	 */
	FILE *messages;
	char userReply[100],messagewithTime[200];
	time_t curtime;  
	int cik=0;
	
	while(cik==0) {
		int replySize= recv(user[i],userReply,100,0);				 
		userReply[replySize] = '\0';
		if(userReply[0]!='q') {
			int x= userReply[0]-'0';
			printf("USER[%d] -> USER [%d] : %s\n",i,x,userReply);
			time(&curtime);
			strcpy(messagewithTime,userReply);
			strcat(messagewithTime, " (");
			strcat(messagewithTime, userlist[i].username);
			strcat(messagewithTime, ","	);
			strcat(messagewithTime, ctime(&curtime)	);
			strtok(messagewithTime,"\n");
			strcat(messagewithTime, ")" );
			messages = fopen("messages.txt","a+");
			fprintf(messages,"%s\n",messagewithTime);
			fclose(messages);	
		}
		else {
			cik=1;
		}
	}
}

void rehberiYazdir (FILE *contacts,int userCount,USERLIST userlist[]) {
	//Rehberi duzenli bir bicimde ekrana yazdirir.
    printf("GUNCEL REHBER\n");
    printf("ID\tKullaniciAdi\tIsim\tSoyisim\t\tTelefonNumarasi\t\n");
    for(int i=0 ; i<userCount ; i++) {
		printf("%d\t%s\t\t%s\t%s\t\t%s\t\n",userlist[i].userID,userlist[i].username,
						userlist[i].name,userlist[i].surname,userlist[i].pnumber);
	}
	
}
