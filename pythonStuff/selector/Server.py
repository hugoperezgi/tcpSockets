import socket, select

def runServer():
    
    serverSocket=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        #Creamos un socket TCP 
    serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        #Cambiamos la opcion de reuseaddress para poder reutilizar la combinación ip:port
        #sin tener q esperar el timeout.
    serverSocket.bind(("127.0.0.1",50500))
        #Le asignamos una direccion ip y un puerto a nuestro servidor, esta sera la dir que el cli
        #ha de usar para conectarse a nuestro server
    serverSocket.listen()
        #Ponemos a nuestro socket a escuchar conexiones entrantes

    serverOpenSockets=[serverSocket,]
    
    while True:
        try:
            readySockets,_,_=select.select(serverOpenSockets,[],[]) 
            #A select le pasamos una lista con todos los sockets, cuando hay uno(o más) sockets ready
            #para leeer, nos devuelve una lista con todos los sockets q esten listos.

            #     Creo q select() no lo vamos a usar pq quiere q aprendamos a usar threads, el código sería
            #     igual, pero en vez de usar el select, cada vez q aceptas una conexión le pasas el nuevo socket
            #     a un thread nuevo (A efectos practicos es similar a hacer un fork y pasar al proceso hijo el
            #     nuevo socket)

            for sck in readySockets: #Iteramos a través de la lista de sockets ready q nos devuelve select

                if sck.fileno()==serverSocket.fileno(): 
                    #Comparamos el fileno (un identificador único para cada socket) de los sockets q están
                    #listos con el fileno del socket principal del server -> Si son iguales significa q el 
                    #socket es el sck principal del server y por tanto esto es una nueva conexión.

                    newSocket,_=sck.accept()
                    #Al aceptar una nueva conexión, se genera un nuevo socket para comunicarse con el nuevo 
                    #cliente, el socket principal sigue esperando nuevas conexiones. Cuando queremos hablar
                    #con un cliente, toda comunicación se hace a través de este nuevo sck.

                    serverOpenSockets.append(newSocket) #Incluimos el nuevo sck en la lista de sck del server.

                else: #El socket es un socket de un cliente ya conectado, por lo q será un msg

                    receivedMsg = sck.recv(2048) #Leemos lo q nos ha enviado el cliente

                    if receivedMsg.decode('utf8').lower()=='shutdown': raise SystemExit 
                    #msg para apagar el servidor

                    elif receivedMsg==b'': 
                        #El cliente se ha desconectado, quitamos el socket de la lista de clientes conectados
                        #y cerramos el socket
                        serverOpenSockets.remove(sck)
                        sck.close()

                    elif receivedMsg.decode('utf8').lower()=='services':

                        #Ejemplo de enviar un truño enorme de datos en binario. 
                        f=open("services",'rb',0)
                        while True:
                            data = f.read()
                            if data == b'': break
                            sck.send(data)

                    else:

                        receivedMsg=receivedMsg.decode('utf8') #Cambiamos el formato de bytes a letras normales
                        print(receivedMsg)
                        sck.send('You said: '.encode('utf8')+receivedMsg.encode('utf8'))
                            #Mandamos una respuesta al cliente, cambiamos el formato de string a bytes (utf8)       
                
        except Exception: pass

if __name__ == "__main__": 
    runServer()