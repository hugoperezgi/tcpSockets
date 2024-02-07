package javaStuff.threads_query;

import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.InetSocketAddress;
import java.nio.channels.SocketChannel;

/**
 * Client
 */
public class Client {

    public static void main(String[] args) {

        try {
            boolean running=true;
                //Control del bucle while true del cliente, si se pone ==False, el cliente se desconecta y el programa termina.
            SocketChannel socket = SocketChannel.open(new InetSocketAddress("localhost", 50500));//localhost = 127.0.0.1
                //Crea un socket tcp y se conecta a la ip:port especificada (AKA ip:port en la q esté escuchando el servidor)

            System.out.println("Escribe \"0\" para terminar.");
            System.out.println("Escribe \"shutdownserver\" para apagar el servidor.");

            String userInput=null;
            
                ObjectOutputStream oos = new ObjectOutputStream(socket.socket().getOutputStream());
                ObjectInputStream ois = new ObjectInputStream(socket.socket().getInputStream());

            while (running) {

                System.out.print("\nDi algo: ");
                userInput=System.console().readLine(); //Obtener un msg del usuario
                    if(userInput.length()==0){continue;} //Controlar q el usuario no mande un string vacío

                Query q = new Query();
                    //Crear un objeto query
                q.construct_Control_Query(userInput);
                    //Introducir los datos en la query
                oos.writeObject(q);
                    //Enviar la query al srv

                q=(Query) ois.readObject();
                    //leer un objeto query recibido del server
                
                
                if(q.getControlMsg().contains("shutdown")){
                    //Si el server nos responde shutdown, el cliente se apaga.
                    running=false;
                    break;
                } else {
                    System.out.println(q.getControlMsg());
                        //Imprimir x consola el msg q el server introdujo en la query
                }

            }

            socket.close();
            System.exit(0);
            
        } catch (Exception e) {
            // TODO: handle exception
        }
    }
}