package bgu.spl.net.api;

import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionImpl;
import java.util.concurrent.ConcurrentHashMap;
import java.util.Vector;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<String> {

    int ownerConnectionId;
    ConnectionImpl<String> connections;
    StompEncoderDecoder<String> encdec;
    boolean shouldTerminate = false;

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connections = (ConnectionImpl<String>) connections;
        this.ownerConnectionId = connectionId;
    }

    @Override
    public String process(String message) {
        String command = message.substring(0, message.indexOf("\n"));

        String[] splitted = message.split("receipt:");
        String receipt = splitted[1].substring(0, splitted[1].indexOf("\n"));
        
        if(command.equals("CONNECT"))
            connect(message, receipt);
        else{
            if(isNotConnected()){
                connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Unknown user" 
                + "\n\n" + "The message:\n----\n" + message + "----\n" + "The client is not connected.\n");

                shouldTerminate = true;
                connections.disconnect(ownerConnectionId);
                return null;
            }
    
            if(command.equals("SEND"))
                send(message, receipt);
                
            else if(command.equals("SUBSCRIBE"))
                subscribe(message, receipt);
    
            else if(command.equals("UNSUBSCRIBE"))
                unsubscribe(message, receipt);
    
            else if(command.equals("DISCONNECT"))
                disconnect(message, receipt);
            else{
                connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Unvalid Frame" 
                + "\n\n" + "The message:\n----\n" + message + "----\n" + "An unknown frame was sent.\n"); 
                
                shouldTerminate = true;
                connections.disconnect(ownerConnectionId);
            }
        }
        return null; 
        
    }

   

    private void send(String message, String receipt){
        String[] splitted = message.split("/topic/");
        String topic = splitted[1].substring(0, splitted[1].indexOf("\n"));
        String bodyMessage = splitted[1].substring(0);

        String userSent = connections.connectionIdToUser.get(ownerConnectionId);
        Integer subId = connections.topicToSubId(topic, connections.userNameToChannels.get(userSent));
        if(subId == -1){
            connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Sending error" + "\n\n" 
            + "The message:\n----\n" + message + "----\n" + "The user is not subscribed to this topic.\n");
            
            shouldTerminate = true;
            connections.disconnect(ownerConnectionId);
            return;
        }


        Vector<String> subscribers = connections.channelToSubscribers.get(topic);

        for(String user : subscribers){
             int uniqueSubId = connections.topicToSubId(topic, connections.userNameToChannels.get(user));
             
             String newMsg = "MASSAGE\nsubscription:" + uniqueSubId +  "\nmessage-id:" + connections.MessageIdCounter++ 
             + "\ndestination:/topic/" + topic + "\n\n" + bodyMessage +"\n";

             connections.send(connections.userToConnectionId.get(user), newMsg);
        }

    }


    private void subscribe(String message, String receipt){
        if(!message.contains("/topic/") | !message.contains("id:")){
            connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Unvalid Frame" 
            + "\n\n" + "The message:\n----\n" + message + "----\n" + "Message does not contain all necessary headers.\n");
            
            shouldTerminate = true;
            connections.disconnect(ownerConnectionId);
            return;
        }

        String[] splitted = message.split("/topic/");
        String topic = splitted[1].substring(0, splitted[1].indexOf("\n"));
        splitted = message.split("id:");
        Integer subId = Integer.valueOf(splitted[1].substring(0, splitted[1].indexOf("\n")));
        String user = connections.connectionIdToUser.get(ownerConnectionId);

        if(connections.channelToSubscribers.containsKey(topic)){
            //the user is already subscribed to the channel
            if(connections.userNameToChannels.get(user).contains(topic)){
                return;
            }

            //the subscription id is already used
            if(connections.userNameToChannels.get(user).containsKey(subId)){
                connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Subscription error\n\n" 
                + "The message:\n----\n" + message + "----\n" + "The subscription id is already used for other channel.\n"); 
                
                shouldTerminate = true;
                connections.disconnect(ownerConnectionId);
                return;
            }
            
         
            //add the user to the channel subscription 
            connections.userNameToChannels.get(user).put(subId, topic);
            //add the topic to the user subscription list 
            connections.channelToSubscribers.get(topic).add(user);
        }
        else{ //May need to handle concurrencey  // did we handle it already using the vector and concurrentHashMap?
            //create the channel and his subscribers list 
            Vector<String> topicSubs = new Vector<String>();
            topicSubs.add(user);
            connections.channelToSubscribers.put(topic, topicSubs);

            //add the topic to the user subscription list 
            connections.userNameToChannels.get(user).put(subId, topic);
        } 
        connections.send(ownerConnectionId, "RECIPT\nreceipt-id:" + receipt + "\n\n");         
    }

    
    private void unsubscribe(String message, String receipt){
        if(!message.contains("id:")){
            connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Unvalid Frame" 
            + "\n\n" + "The message:\n----\n" + message + "----\n" + "Message does not contain all necessary headers.\n");
            
            shouldTerminate = true;
            connections.disconnect(ownerConnectionId);
            return;
        }
        String[] splitted = message.split("id:");
        String userName = connections.connectionIdToUser.get(ownerConnectionId);
        Integer subId = Integer.valueOf(splitted[1].substring(0, splitted[1].indexOf("\n")));
        String topic = connections.userNameToChannels.get(userName).get(subId);
        if(topic != null){
            //removing the user from the topic subscribers list
            connections.channelToSubscribers.get(topic).remove(userName);
            //removing the topic from the user topics list
            connections.userNameToChannels.get(userName).remove(subId);
            connections.send(ownerConnectionId, "RECIPT\nreceipt-id:" + receipt + "\n\n");                }
        else{
            connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Unsubscription error" 
            + "\n\n" + "The message:\n----\n" + message + "----\n" + "The user is not subscribed to this channel.\n"); 
            shouldTerminate = true;
            connections.disconnect(ownerConnectionId);
        }
    }


    private void connect(String message, String receipt){
        if(!message.contains("accept-version:") | !message.contains("host:") 
        | !message.contains("login:") | !message.contains("passcode:")){
            
            connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Unvalid Frame" 
            + "\n\n" + "The message:\n----\n" + message + "----\n" + "Message does not contain all necessary headers.\n");
            shouldTerminate = true;
            connections.disconnect(ownerConnectionId);
            return;
        }
        String[] splitted = message.split("accept-version:");
        String acceptVersion = splitted[1].substring(0, splitted[1].indexOf("\n"));

        splitted = message.split("host:");
        String host = splitted[1].substring(0, splitted[1].indexOf("\n"));

        splitted = message.split("login:");
        String user = splitted[1].substring(0, splitted[1].indexOf("\n"));

        splitted = message.split("passcode:");
        String passcode = splitted[1].substring(0, splitted[1].indexOf("\n"));

        //USER already logged in
        if(connections.userToConnectionId.containsKey(user)){
            connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Connection Error" 
            + "\n\n" + "The message:\n----\n" + message + "----\n" +"User already logged in.\n");
            shouldTerminate = true;
            connections.disconnect(ownerConnectionId);
            return;
        }

        //CLIENT already logged in
        if(connections.userToConnectionId.contains(ownerConnectionId)){
            connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Connection Error" 
            + "\n\n" + "The message:\n----\n" + message + "----\n" 
            +"The client is already logged in, log out before trying again.\n");
            
            shouldTerminate = true;
            connections.disconnect(ownerConnectionId);
            return;
        }

        //connecting successfully
        if( !connections.UserNameToPassword.containsKey(user) || connections.UserNameToPassword.get(user).equals(passcode)){
            //new user
            if(!connections.UserNameToPassword.containsKey(user))
                connections.UserNameToPassword.put(user, passcode);

            //mapping between user <--> connectionId
            connections.connectionIdToUser.put(ownerConnectionId, user);
            connections.userToConnectionId.put(user, ownerConnectionId);
            
            //making the user name channels hashMap
            connections.userNameToChannels.put(user, new ConcurrentHashMap<Integer, String>());
            
            //send back frame with ”Login successful”.
            connections.send(ownerConnectionId, "CONNECTED\nversion:" + acceptVersion + "\n\n");
            connections.send(ownerConnectionId, "RECEIPT\nreceipt-id::" + receipt + "\n\n");
            return;
        }

        //wrong password
        if(!connections.UserNameToPassword.get(user).equals(passcode)){
            connections.send(ownerConnectionId, "ERROR\nreceipt-id:" + receipt + "\n" + "message: Connection error" 
            + "\n\n" + "The message:\n----\n" + message + "----\n" + "Wrong password.\n");
            
            shouldTerminate = true;
            connections.disconnect(ownerConnectionId);
            return;
        }
    }


    private void disconnect(String message, String receipt){
        connections.send(ownerConnectionId, "RECIPT\nreceipt-id:" + receipt + "\n\n");
        shouldTerminate = true;
        connections.disconnect(ownerConnectionId);
    }

    private boolean isNotConnected(){
        return connections.connectionIdToConnectionHandle.get(ownerConnectionId) == null 
                || !connections.connectionIdToUser.containsKey(ownerConnectionId);
    }


    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
    
}

