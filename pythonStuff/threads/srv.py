import socket, threading

def runServer():

    serverSocket=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        #Creamos un socket TCP 
    serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        #Cambiamos la opcion de reuseaddress para poder reutilizar la combinación ip:port
        #sin tener q esperar el timeout.
    serverSocket.bind(("127.0.0.1",50500))
        #Le asignamos una direccion ip y un puerto a nuestro servidor, esta sera la dir que el cli
        #ha de usar para conectarse a nuestro server
    serverSocket.settimeout(1)
    serverSocket.listen()
        #Ponemos a nuestro socket a escuchar conexiones entrantes


    global serverOn
    serverOn = True
        #Variable global compartida por todos los threads para poder apagar el server
    while serverOn:

        try:
            newSock,_=serverSocket.accept()
                #Aceptamos nueva conexión

            clientThread=threading.Thread(target=handleClient, args=(newSock,), daemon=True)
                #Creamos un nuevo thread que ejecute la funcion handleClient, y le pasamos el 
                #socket del cliente q se acaba de conectar.

            clientThread.start()        
                #Pones el thread a funcionar    

        except Exception: pass

    raise SystemExit

    

def handleClient(sck:socket.socket):

    #Funcion q va a hacer cada thread cuando es activado.

    while True:
        try:
            receivedMsg=sck.recv(2048)

            if receivedMsg.decode('utf8').lower()=="shutdown": 
                global serverOn
                serverOn = False
                sck.close()
                    #Codigo para apagar el servidor

            elif receivedMsg==b'': 
                #El cliente se ha desconectado, cerramos el socket y terminamos el thread
                sck.close()
                break
            
            elif receivedMsg.decode('utf8').lower()=='services':

                #Ejemplo de enviar un truño enorme de datos en binario. 
                f=open("services",'rb',0)
                while True:
                    data = f.read()
                    if data == b'': break
                    sck.send(data)
                f.close()

            else:

                receivedMsg=receivedMsg.decode('utf8') #Cambiamos el formato de bytes a letras normales
                print(sck.getpeername(),end="")
                print(" Said: "+receivedMsg)
                    #Imprimimos la dir ip del cliente y su msg

                sck.send('You said: '.encode('utf8')+receivedMsg.encode('utf8'))
                    #Mandamos una respuesta al cliente, cambiamos el formato de string a bytes (utf8)       

        except Exception: pass
    



if __name__ == "__main__": 

    runServer()

