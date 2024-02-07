import socket

def runClient():

    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        #Creamos un nuevo socket TCP para conectarnos al servidor
        # socket.AF_INET -> Familia internet de sockets
        # socket.SOCK_STREAM -> Protocolo TCP 
    clientSocket.connect(('127.0.0.1',50500))
        #Pedimos conectarnos al servidor q esta escuchando en 127.0.0.1:50500
    

    while True:

        userInput = input("Introduce un msg:")
        clientSocket.send(userInput.encode('utf8'))
            #Cambiamos el formato del msg a bytes en UTF8 para q no de problemas al cambiar
            #de ordenador/sistema operativo y enviamos el msg escrito por el usuario

            #tl;dr: Lo q envias tiene q estar en bytes, por lo q hay q convertir el
            #string de turno a bytes. La function .encode('utf8') convierte el str 
            #a un churro de bytes codificados en UTF-8. 

        serverResponse=b''
            #Iniciamos el byte array en el q guardamos lo q el server nos mande

        while True:

            response=clientSocket.recv(2048)
                #Leemos hasta 2048 bytes de info del servidor. 
            
            if response.__len__() == 2048:
                #Como el paso anterior solo lee bloques de 2048 Bytes, si el server envia >2048 B
                #no vamos a recibir toda la info, por lo q si los datos recibidos son ==2048, volvemos
                #a leer para recibir la información restante.
                serverResponse=serverResponse+response
                response=clientSocket.recv(2048)
            else: 

                serverResponse=serverResponse+response
                break

        if serverResponse == b'': 
            #El servidor se ha cerrado, por lo q apagamos el cliente.
            print("Server shut down.")
            raise SystemExit

        serverResponse=serverResponse.decode('utf8')
            #Al igual q en el server, recibimos el msg y lo "traducimos" de vuelta
        
        print(serverResponse)

if __name__ == "__main__": 
    runClient()