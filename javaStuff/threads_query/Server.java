package javaStuff.threads_query;

import java.net.InetSocketAddress;
import java.nio.channels.ServerSocketChannel;

public class Server {


    public static void main(String[] args) {

        try {
            
            ServerSocketChannel serverSocket = ServerSocketChannel.open();
            InetSocketAddress serverIPAddr = new InetSocketAddress("127.0.0.1",50500);
            serverSocket.bind(serverIPAddr);
                //Configuramos el socket del servidor con la ip de la interfaz de loopback para hacer
                //pruebas, tendríamos q cambiar el 127.0.0.1 por la ip q tenga nuestro 
                //ordenador en la red si quieseramos q otro ordenador pudiese conectarse al servidor.

            while (true){

                try { //Evitar q el server pete si el .accept() da algun error
                    
                    ClientHandler clientThread = new ClientHandler(serverSocket.accept());
                        //Creamos un nuevo thread pasandole el socket del cliente. 

                    clientThread.setDaemon(true);
                    clientThread.start();
                        //Ponemos a currar al thread, a partir de aqui se ejecuta el código q hay
                        //en la fn run() del thread creado en un nuevo thread del os.

                } catch (Exception e) {
                    // TODO: handle exception
                }

            }//Rinse n repeat

        } catch (Exception e) {
            // TODO: handle exception
        } 
        
    }
}
