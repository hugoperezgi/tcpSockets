package javaStuff.threads;

import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

/**
 * Client
 */
public class Client {

    public static void main(String[] args) {

        try {

            SocketChannel socket = SocketChannel.open(new InetSocketAddress("localhost", 50500));//localhost = 127.0.0.1
                //Crea un socket tcp y se conecta a la ip:port especificada (AKA ip:port en la q esté escuchando el servidor)
            Thread.sleep(5000);
            System.out.print("Escribe \"0\" para terminar.");

            String userInput=null;
            int readbytes=0;


            while (true) {


                System.out.print("\nDi algo: ");
                userInput=System.console().readLine(); //Obtener un msg del usuario
                    if(userInput.length()==0){continue;} //Controlar q el usuario no manda un string vacío

                socket.write(ByteBuffer.wrap(userInput.getBytes()));
                    //Enviar el user input al server, la función write/read solo funcionan con 
                    //Bytebuffers por lo q convertimos el string a bytes (.getBytes()) y luego
                    //lo convertimos a un bytebuffer.
                
                try {if(Integer.parseInt(userInput)==0){break;}} catch (NumberFormatException e) {}
                    //Salir del bucle si el user hace exit                

                do{
                    //Este bucle do-while es necesario para asegurarnos q recibimos toda la información
                    //enviada por el server. La funcion read va a intentar llenar el buffer q le demos,
                    //por lo que leerá en bloques de tamaño <= 2048 Bytes. Si el server nos envía 5kB,
                    //para leer toda la info tendríamos q realizar el bucle 3 veces, las dos primeras 
                    //llenarían el buffer, mientras q la tercera leería los bytes restantes y 
                    //readbytes <2048, por lo q se termina el bucle.

                    ByteBuffer byteBuffer = ByteBuffer.allocate(2048);
                        //Para recibir datos del server, leemos en bloques de hasta 2048 Bytes

                    readbytes = socket.read(byteBuffer);
                        //Intenta rellenar el buffer con bytes recibidos del server, la función devuelve
                        //el numero de bytes que ha leido (<=2048)

                    System.out.print(new String(byteBuffer.array()));
                        //Convertimos el churro de bytes de vuelta a un string normal + print

                }while(readbytes==2048);
                
            }


            socket.close();
            
        } catch (Exception e) {
            // TODO: handle exception
        }
    }
}