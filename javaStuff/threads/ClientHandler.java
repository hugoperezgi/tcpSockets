package javaStuff.threads;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

public class ClientHandler extends Thread {

    //Los threads en los cuales el servidor va a atender a los clientes.

    private SocketChannel socket;
    private ByteBuffer inByteBuffer;
    
    @Override
    public void run(){

        //El "main" del Thread, cuando haces thread.start() se ejecuta este codigo

        try {

            while(true){
                this.inByteBuffer= ByteBuffer.allocate(2048);
                this.socket.read(inByteBuffer);
                    //Leemos lo q el cliente nos dice

                byte[] byteArray=inByteBuffer.array();
                String string= new String(byteArray);
                    //Convertimos los bytes recibidos a un string

                try {if(string.charAt(0)=="0".charAt(0)){System.exit(0);}} catch (Exception e) {}
                    //Apagamos el server si nos envian un 0

                System.out.println("I am thread:" +this.threadId()+ ", Client "+this.socket.getRemoteAddress().toString()+" said: "+string);
                    //Imprimimos por consola la dirIp del cliente, su mensaje y el pid del thread

                this.socket.write(ByteBuffer.wrap(("You said: "+string).getBytes()));
                    //Enviamos una respuesta al cliente

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
    }
    
    //Constructor del thread, le pasamos el socket del cliente para q 
    //cuando se ejecute, el thread pueda comunicarse con dicho cliente

    public ClientHandler(SocketChannel sck){
        this.socket=sck;
    }


}
