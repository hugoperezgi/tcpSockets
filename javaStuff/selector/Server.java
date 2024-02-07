package javaStuff.selector;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Set;
import java.util.Iterator;

import java.io.File;
import java.util.Scanner;

/**
 * Server
 */
public class Server {

    public static void main(String[] args) {
        
        try {

            ServerSocketChannel serverSocket = ServerSocketChannel.open();
            InetSocketAddress serverIPAddr = new InetSocketAddress("127.0.0.1",50500);
            serverSocket.bind(serverIPAddr);
                //Configuramos el socket del servidor con la ip de la interfaz de loopback para hacer
                //pruebas, tendríamos q cambiar la ip por la ip q tenga nuestro ordenador en la red 
                //si quieseramos conectarnos desde otro ordenador.
                
            serverSocket.configureBlocking(false);
                //Para poder usar select los sockets han de estar en non-blocking AKA cuando intentas
                //leer info no se queda bloqueado esperando hasta recibir algo q leer.

            Selector socketSelector = Selector.open();
            serverSocket.register(socketSelector, SelectionKey.OP_ACCEPT, serverSocket);
                //Creamos el selector y añadimos el sock con la flag de accept, eso significa q solo nos
                //lo va a dar cuando tenga una petición q aceptar

            Set<SelectionKey> readySockets;
            Iterator<SelectionKey> readySocketIterator;
            SelectionKey selectedSocket;
            SocketChannel newSock=null;
                //Variables necesarias para obtener los sockets del selector

            int numberOfReadySockets=0;

            while(true){
                try {
                    
                    numberOfReadySockets=socketSelector.select();
                    // Selecciona los sockets q esten listos para aceptar/leer y nos dice
                    // cuantos ha seleccionado

                    if(numberOfReadySockets!=0){

                        readySockets=socketSelector.selectedKeys();
                            // Sacamos la lista de sockets ready del selector
                        readySocketIterator=readySockets.iterator();
                            // Creamos una lista iterable con los sockets ready

                        do{// Iterar a través de todos los sockets ready

                            selectedSocket=readySocketIterator.next();
                                //Seleccionamos el siguiente socket ready dentro de la lista iterable

                            try {

                                if(selectedSocket.isAcceptable()){
                                    // El unico socket q esta incluido en el selector con el flag
                                    // de accept es el socket principal del server, por lo q se trata
                                    // de una conexión nueva:
                                    
                                    newSock=serverSocket.accept();
                                    newSock.configureBlocking(false);
                                    newSock.register(socketSelector, SelectionKey.OP_READ, newSock);
                                        //Incluimos el nuevo socket del cliente en la lista de sockets 
                                        //del select, con flag de lectura.

                                }else if(selectedSocket.isReadable()){
                                    // Solo los sockets de clientes tienen flag de lectura, por lo q 
                                    // aquí haces lo q tengas q hacer con el cliente.

                                    SocketChannel sck=(SocketChannel)selectedSocket.channel();
                                        //Obtienes el sck a partir del socket channel

                                    ByteBuffer inByteBuffer = ByteBuffer.allocate(2048);
                                    sck.read(inByteBuffer);
                                        //Creas un buffer de X Bytes de capacidad y escribes lo
                                        //recibido en el 

                                    byte[] byteArray=inByteBuffer.array();
                                    String string= new String(byteArray);
                                        //Convertir el bytebuffer a un string
                                    
                                    try {if(string.charAt(0)=="0".charAt(0)){System.exit(0);}} catch (Exception e) {}
                                        //Same exit code than on client.

                                    System.out.println("Client "+sck.getRemoteAddress().toString()+" said: "+string);
                                        //Output to console both client ip addr and their msg

                                    if(string.contains("services")){
                                        //Control del msj q el server nos ha enviado
                                        

                                        File f = new File("javaStuff\\selector\\services");
                                        Scanner fileScanner = new Scanner(f);
                                        String data ="";
                                        do{
                                            data=data+fileScanner.nextLine();
                                        }while(fileScanner.hasNext());
                                            //Leer un file enorme

                                        sck.write(ByteBuffer.wrap(data.getBytes()));
                                            //Enviar todo lo q hemos leido al cliente
                                        fileScanner.close();

                                    } else {
                                        sck.write(ByteBuffer.wrap(("You said: "+string).getBytes()));
                                            //Escribir algo de vuelta al cliente
                                    }

                                        
                                }

                            } catch (IOException e) {
                                // EL cliente se ha desconectado

                                newSock=(SocketChannel)selectedSocket.channel();
                                    //Obtenemos el socket
                                System.out.println("Client "+newSock.getRemoteAddress().toString()+" disconnected.");
                                newSock.close();
                                    //Cierras el socket.
                            }

                            readySocketIterator.remove();
                                //quitas el socket que acabas de atender de la lista y 
                                //sigues atendiendo a los sck q hay en la lista.
                                
                        }while(readySocketIterator.hasNext()); 

                        numberOfReadySockets=0; 
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

        } catch (Exception e) {
            // TODO: handle exception
        }
            
    }
}


// Java es puta basura bro.