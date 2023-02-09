package bgu.spl.net.impl.echo;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class EchoClient {

    public static void main(String[] args) throws IOException {

        if (args.length == 0) {
            args = new String[]{"localhost", "CONNECT\n accept-version:1.2\nhost: stomp . cs . bgu . ac . il\nlogin: meni\npasscode: films\n receipt:1\n\u0000"};
        }

        if (args.length < 2) {
            System.out.println("you must supply two arguments: host, message");
            System.exit(1);
        }

        //BufferedReader and BufferedWriter automatically using UTF-8 encoding
        try (Socket sock = new Socket(args[0], 7777);
                BufferedReader in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
                BufferedWriter out = new BufferedWriter(new OutputStreamWriter(sock.getOutputStream()))) {

            System.out.println("sending message to server");
            out.write(args[1]);
            out.newLine();
            out.flush();

            System.out.println("awaiting response");

            //this is for our tests need to adjust
            String output = "abbbb";
            while(output.charAt(output.length()-1) != '\u0000'){
                String line = in.readLine();
                output = output + "\n" + line;
            }
            System.out.println("message from server: " + output);
        }
    }
}
