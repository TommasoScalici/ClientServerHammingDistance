#include <netdb.h> 
#include <pthread.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <sys/types.h>

#define BUFFER_SIZE 128
#define PORT 8080

unsigned int hammingDistance(const char*, const char*);
void *socketThread(void*);


char clientMessage[BUFFER_SIZE];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    int newSocket, serverSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
  
    // Creazione socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0); 

    if (serverSocket == -1)
    { 
        puts("Creazione socket fallita"); 
        exit(0);
    }
    else
        puts("Creazione socket avvenuta con successo");

    bzero(&serverAddr, sizeof(serverAddr));
    

    // Assegnazione IP, porta e address family
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serverAddr.sin_port = htons(PORT);


    // Binding tra la nuova socket e l'IP ottenuto
    if ((bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) != 0)
    { 
        puts("Socket bind fallito");
        close(serverSocket);
        exit(0); 
    } 
    else
        puts("Socket bind eseguito con successo");



    // Mettiamo il server in ascolto per nuove richieste di connessione
    if ((listen(serverSocket, 5)) != 0)
    { 
        puts("Ascolto fallito");
        close(serverSocket);
        exit(0); 
    } 
    else
        printf("Server in ascolto sulla porta %u\n", PORT);


    pthread_t tid[10];
    unsigned int i = 0; // Contatore connessioni

    for(;;)
    {
        // La chiamata ad accept apre la nuova socket per le connessioni in arrivo
        addr_size = sizeof(serverStorage);
        newSocket = accept(serverSocket, (struct sockaddr*)&serverStorage, &addr_size);

        if(newSocket > 0)
        {
            if(pthread_create(&tid[i], NULL, socketThread, &newSocket) != 0)
                puts("Errore nella creazione di un nuovo thread");
            else
            {
                struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&serverStorage;
                struct in_addr ipAddr = pV4Addr->sin_addr;
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &ipAddr, ip, INET_ADDRSTRLEN);
                printf("Nuovo client connesso (ID: %u, IP: %s)\n", i, ip);
                i++; // Se la connessione è avvenuta incremento il contatore delle connessioni
            }
            
        }

        if(i >= 5)
        {
            i = 0;

            while(i < 5)
                pthread_join(tid[i++], NULL);

            i = 0;
        }
    }
}

unsigned int hammingDistance(const char* str1, const char* str2)
{
    unsigned int i = 0, count = 0;

    while (str1[i] != '\0' && str2[i] != '\0')
    {
        if (str1[i] != str2[i])
            count++;
        i++;
    }

    return count;
}

void *socketThread(void *socket)
{
    char strings [2][BUFFER_SIZE]; // Array di 2 stringhe di lunghezza BUFFER_SIZE
    int newSocket = *((int*)socket);
    unsigned int n = 0;

    while(n < 2)
    {
        bzero(&clientMessage, BUFFER_SIZE);
        recv(newSocket, clientMessage, BUFFER_SIZE, 0);

        pthread_mutex_lock(&lock);

        if(strlen(clientMessage) > 0 && clientMessage[0] != '\0' && clientMessage[0] != '\n')
        {
            printf("Il client ha inviato il messaggio: %s\n", clientMessage);
            strcpy(strings[n], clientMessage);
            send(newSocket, "Ricevuto", 9, 0);
        }
        else
            send(newSocket, "Nessun messaggio inviato", 25, 0);

        pthread_mutex_unlock(&lock);
        
        sleep(1);    
        
        n++;
    }

    if(strlen(strings[0]) == 0 || strlen(strings[1]) == 0)
        send(newSocket, "Una o più stringhe sono vuote!", 32, 0);

    else if(strlen(strings[0]) != strlen(strings[1]))
        send(newSocket, "Le due stringhe devono essere di lunghezza uguale!", 51, 0);

    else
    {
        char msg[BUFFER_SIZE];
        sprintf(msg, "La distanza di Hamming tra le due stringhe è: %u",
                 hammingDistance(strings[0], strings[1]));
        send(newSocket, msg, strlen(msg)+1, 0);
    }

    puts("Client disconnesso, chiusura socket");
    close(newSocket);
    pthread_exit(NULL);
}
