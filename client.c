/**
@file
SEMESTER PROJECT
Mesajlaşma Programı - Client
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
#include <conio.h>
#include <string.h>

void rehberGoster(SOCKET) ; //Servera rehbere ulasma istegi gonderir. Rehberi gosterir.
void gelenKutusu(SOCKET);	//Serverdan gelen mesajlari gosterir.
void mesajGonder(SOCKET);	//Mesaj gonderir.
void yeniKullanici(SOCKET); //Yeni kullanicinin bilgilerini alir.
 
int main()
{	
	/**
	 * @param clientSocket : Kullanıcının server ile bağlantı kuracağı soket.
	 * @param message : Gonderilecek mesajlarin bulunduğu dizi.
	 * @param serverReply : Serverden gelen cevapları tutan dizi.
	 * @param secim : Kullanicinin islem secimi tutar.
	 * @param replySize : Serverden gelen mesajin buyuklugunu tutar.
	 * */
	 
    WSADATA wsa;
    SOCKET clientSocket;
    struct sockaddr_in server;
    char message[100], serverReply[100],secim;
    int replySize;

  

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) { 		//Kütüphaneyi başlatma
        printf("HATA OLUSTU.\n");
        return 1;
    }
	
	//Socketin baslatilmasi.
	if((clientSocket = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET) { 
		printf("Socket olusturulamadi : %d", WSAGetLastError());
		return 2;
	}
	//Address Family : AF_INET (IP4), Type : SOCK_STREAM (TCP protocol)
	//Protocol : 0, socketin type değişkenine uygun protokülü kendisinin seçmesini sağlar
	
	//server.sin_addr.s_addr = inet_addr("192.168.1.25"); //IP
	server.sin_addr.s_addr = inet_addr("192.168.139.1");
    server.sin_family = AF_INET; //Type
    server.sin_port = htons(2018); //Baglanacagimiz port
	
	//Serverla baglanti kurulmasi
    if (connect(clientSocket,(struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Baglanti hatasi.");
        return 1;
    }
     
    printf("Servera baglanildi.\n");
    
    printf("Kullanici adinizi giriniz : ");		
	scanf("%s",message);
    if(send(clientSocket, message, strlen(message), 0) < 0) {
        printf("Gonderme Hatasi");
        return 1;
    }
    
    //Buradaki cevapta kullanicinin yeni bir kullanici olup olmadığı bilgisi gelir.
    if((replySize=recv(clientSocket , serverReply , 100 , 0)) == SOCKET_ERROR) {
			printf("Baglanti hatasi\n");
			return 2;
		}	
		serverReply[replySize] = '\0';
		if (serverReply[0]=='0') {			//Serverın cevabı sıfır ise bu kullanici
			yeniKullanici(clientSocket);	//ilk kez bağlanıyordur. Bilgileri alinir.
		}
		else {
			serverReply[replySize] = '\0';
			printf("%s",serverReply);
		}
		
	printf("Baslamak icin bir tusa basin.\n");
	getch();
    system("CLS");
    while(secim!='0') {		
		printf("Menu\n");
		printf("-1- Mesaj Gonder\n");
		printf("-2- Gelen Kutusu\n");
		printf("-3- Rehberi Goruntule\n");
		printf("-0- Cikis\n");
		printf("Gerceklestirmek istediginiz islem numarasini girin : ");
		scanf(" %c",&secim);
		
		send(clientSocket, &secim ,1 ,0); //Secim servere iletilir.
					
		switch(secim) {
			case '1':
			mesajGonder(clientSocket);
			break;
				
			case '2':
			gelenKutusu(clientSocket);		
			break;
			
			case '3':
			rehberGoster(clientSocket);
			break;
		}
		
		system("CLS");
	}
 
    closesocket(clientSocket);
	WSACleanup();

	return 0;
}

void rehberGoster(SOCKET clientSocket) {
	/**
	 * @param serverReply : Serverden gelen mesaji tutar.
	 * @param replySize : Serverden gelen mesajin buyuklugunu tutar.
	 * @param userCount : Kullanici numarasini tutar.
	 * Bu fonksiyonda serverdan rehberdeki kullanıcı bilgileri alınır
	 * ve ekrana yazdırılır.
	 */
	char serverReply[100];
	int replySize,userCount;
	printf("Rehber\n");
			printf("ID  Kullanici Adi\n");
			userCount=0;
			while ((replySize=recv(clientSocket, serverReply, 100, 0)) && serverReply[0]!='!') {
				printf("%d : %s\n",userCount,serverReply);
				userCount++;
			}
			
			printf("\nMenu icin bir tusa basiniz.\n");
			getch();	
}

void gelenKutusu(SOCKET clientSocket) {
	/**
	 * @param serverReply : Serverden gelen mesaji tutar.
	 * @param replySize : Serverden gelen mesajin buyuklugunu tutar.
	 * Bu fonksiyonda serverdan kendine ait mesajları alır
	 * ve ekrana yazdırır.
	 */
	char serverReply[100];
	int replySize;
		printf("Gelen Kutusu\n");
			while ((replySize=recv(clientSocket, serverReply, 100, 0)) && serverReply[0]!='!') {
					serverReply[replySize] = '\0';
					printf("%s\n",serverReply);
				}
			
			printf("\nMenu icin bir tusa basiniz.\n");
			getch();
	
}

void mesajGonder(SOCKET clientSocket) {
	/**
	 * @param message : Servera gönderilecek mesajı saklar.
	 * Bu fonksiyonda kullanıcıdan mesajı alır ve
	 * servara mesajı iletir.
	 */
	char message[100];
	printf("Mesaj Gonderme (ID-Mesaj) (q : cikis)\n");
			while(message[0]!='q') {
				gets(message);			
				if( send(clientSocket , message , strlen(message) , 0) < 0) {
					printf("Send failed\n");
					exit(0);
				}	
			}
			message[0]='0';		
}

void yeniKullanici(SOCKET clientSocket) {
	/**
	 * @param serverReply : Serverden gelen mesaji tutar.
	 * @param replySize : Serverden gelen mesajin buyuklugunu tutar.
	 * @param message : Servera gönderilecek mesajı saklar.
	 * Bu fonksiyonda yeni kullanicinin bilgileri alinir
	 * ve servera bu bilgiler gonderilir.
	 */
	char message[100],serverReply[100];
	int replySize=recv(clientSocket , serverReply , 100 , 0);
	serverReply[replySize] = '\0';
	printf("%s",serverReply);
	printf("Isim :");
	scanf(" %s",message);
	send(clientSocket , message , strlen(message) , 0);
	printf("Soyisim :");
	scanf(" %s",message);
	send(clientSocket , message , strlen(message) , 0);
	printf("Numara :");
	scanf(" %s",message);
	send(clientSocket , message , strlen(message) , 0);	
}
