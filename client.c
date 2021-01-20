#include <netdb.h> 
#include <pthread.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h> 

#define BUFFER_SIZE 128
#define LOCALHOST "127.0.0.1"
#define PORT 8080

int main()
{
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    int clientSocket;
    struct sockaddr_in serverAddr;

  
    // Creazione socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0); 

    if (clientSocket == -1)
    { 
        puts("Creazione socket fallita\n");
        exit(0);
    }
    else
        puts("Creazione socket avvenuta con successo\n");

    bzero(&serverAddr, sizeof(serverAddr));


    // Assegnazione IP, porta e address family
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_addr.s_addr = inet_addr(LOCALHOST); 
    serverAddr.sin_port = htons(PORT);


    // Connessione client socket al server socket
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        puts("Connessione al server fallita");
        close(clientSocket);
        exit(0); 
    } 
    else
    {
        puts("Connessione al server stabilita\n");
    }


    // Inserimento delle due stringhe e invio al server

    printf("Inserisci la prima stringa: ");
    fgets(message, BUFFER_SIZE, stdin);
    strtok(message, "\n");

    if(send(clientSocket, message, strlen(message), 0) < 0)
        puts("Invio fallito");

    if(recv(clientSocket, buffer, BUFFER_SIZE, 0) < 0)
        puts("Nessuna risposta dal server");
    else
        printf("Il server ha risposto: %s\n", buffer);

    printf("Inserisci la seconda stringa: ");
    fgets(message, BUFFER_SIZE, stdin);
    strtok(message, "\n");
    
    if(send(clientSocket, message, strlen(message), 0) < 0)
        puts("Invio fallito");

    if(recv(clientSocket, buffer, BUFFER_SIZE, 0) < 0)
        puts("Nessuna risposta dal server");
    else
        printf("Il server ha risposto: %s\n", buffer);

    
    recv(clientSocket, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);

    close(clientSocket);
}