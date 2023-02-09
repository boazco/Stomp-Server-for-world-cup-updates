package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompEncoderDecoder;
import bgu.spl.net.api.StompMessagingProtocolImpl;
import bgu.spl.net.srv.ConnectionImpl;
import bgu.spl.net.srv.Server;

public class StompServer {

    public static void main(String[] args) {
        if (args[0].equals("tpc"))
            Server.threadPerClient(7777, () -> new StompMessagingProtocolImpl(), StompEncoderDecoder::new).serve();

        else if (args[0].equals("reactor")){
            Server.reactor(Runtime.getRuntime().availableProcessors(), 7777, () -> new StompMessagingProtocolImpl(),
                    StompEncoderDecoder::new).serve();
        }
    }
}


/*
./bin/StompWCIClient 
login 127.0.0.1:7777 meni films
join Germany_Japan
report /home/spl211/Desktop/ASSIGNMENT3_11.1/client/data/events1.json
summary Germany_Japan meni /home/spl211/Desktop/ASSIGNMENT3_11.1/client/data/weAreTheKIngs
 
java bgu.spl.net.impl.stomp.StompServer reactor

mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 reactor"
 */
