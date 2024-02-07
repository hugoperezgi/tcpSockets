package javaStuff.threads_query;

import java.io.Serializable;

// import entities.*;

public class Query implements Serializable {
    static final long serialVersionUID = 42L;

    private byte queryType;

    private String controlMsg=null;


        
    Query(){}

    // /**
    //  * Constructor for Query Type 0 <b>Log in</b> <p>
    //  * Log in query, expected a control response with the role of the user
    //  * {@code Client -> Server} <p>
    //  */
    // public void construct_LogIn_Query(String name, byte[] psw){
    //     this.queryType = (byte) 0;
    //     this.user= new User(name, psw);
    // }

    /**
     * Constructor for Query Type 1 <b>Control</b> <p>
     * Control responses sent by server to client
     * {@code Server -> Client} <p>
     */
    public void construct_Control_Query(String msg){
        this.queryType = (byte) 1;
        this.controlMsg=msg;
    }
    
    /**Type of the query
     * <p>{@code 0} <b>Log in:</b> Cli -> Srv
     * <p>{@code 1} <b>Control:</b> Srv -> Cli (Error/ACK/Confirmations) 
    */
    public int getQueryType() {
        return queryType;
    }

    public String getControlMsg() {
        return controlMsg;
    }
}
