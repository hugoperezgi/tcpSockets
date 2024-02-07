package javaStuff.threads_query;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.nio.channels.SocketChannel;

public class ClientHandler extends Thread {

    //Threads en los cuales el servidor va a atender a los clientes.

    private SocketChannel socket;
    boolean run = true;
    boolean exit = false;
    
    @Override
    public void run(){

        run=true;
        exit=false;
        
        //El "main" del Thread, cuando haces thread.start() se ejecuta este codigo

        try {

            ObjectInputStream ois = new ObjectInputStream(this.socket.socket().getInputStream());
            ObjectOutputStream oos = new ObjectOutputStream(this.socket.socket().getOutputStream());

            while(run){

                Query clientQuery=(Query) ois.readObject();
                Query servQuery = new Query();
                if(clientQuery==null){

                    servQuery.construct_Control_Query("FUCK");
                    System.out.println("fml");

                }else{
                    //Process the query if data type is correct
                    switch (clientQuery.getQueryType()) { 
                        case 1: 
                            if(clientQuery.getControlMsg().equalsIgnoreCase("0")){
                                run=false;
                                servQuery.construct_Control_Query("shutdown");
                            }else if(clientQuery.getControlMsg().equalsIgnoreCase("shutdownserver")){
                                run=false;
                                exit=true;
                                servQuery.construct_Control_Query("shutdown");
                            }else{
                                servQuery.construct_Control_Query("YAY");
                            }
                            System.out.println(clientQuery.getControlMsg());
                            break;
                        default:
                            servQuery.construct_Control_Query("NAY");
                            System.out.println("default");
                    }
                }

                oos.writeObject(servQuery);

            }
        
        }catch(IOException e){
            //El cliente ha cerrado la conexión

            try {
                System.out.println("I am thread:" +this.threadId()+ ", Client "+this.socket.getRemoteAddress().toString()+" disconnected.");
                this.socket.close();
            } catch (Exception e2) {
                // TODO: handle exception
            }
        } catch (Exception e) {
            // TODO: handle exception
        }

        if(exit){System.exit(0);}
    }
    
    //Constructor del thread, le pasamos el socket del cliente para q 
    //cuando se ejecute, el thread pueda comunicarse con dicho cliente

    public ClientHandler(SocketChannel sck){
        this.socket=sck;
    }


}
