package bgu.spl.net.srv;
import java.io.IOException;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionImpl<T> implements Connections<T> {

    public ConcurrentHashMap<String, Integer> userToConnectionId; 
    public ConcurrentHashMap<Integer, String> connectionIdToUser;

    public ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionIdToConnectionHandle;

    public ConcurrentHashMap<String, String> UserNameToPassword;

    public ConcurrentHashMap<String, Vector<String>> channelToSubscribers; //game to subscribers(user names)
    public ConcurrentHashMap<String, ConcurrentHashMap<Integer, String>> userNameToChannels; //channel = <subscriptionId,topic>
    private int idCounter = 0;
    public int MessageIdCounter = 0;


    public ConnectionImpl(){
        userToConnectionId = new ConcurrentHashMap<String, Integer>();
        connectionIdToUser = new ConcurrentHashMap<Integer, String>();
        connectionIdToConnectionHandle = new ConcurrentHashMap<Integer, ConnectionHandler<T>>();
        UserNameToPassword = new ConcurrentHashMap<String, String>();
        channelToSubscribers = new ConcurrentHashMap<String, Vector<String>>();
        userNameToChannels = new ConcurrentHashMap<String, ConcurrentHashMap<Integer, String>>();
    }
    
    @Override
    public boolean send(int connectionId, T msg) {
        if(!connectionIdToConnectionHandle.containsKey(connectionId))
            return false;
        connectionIdToConnectionHandle.get(connectionId).send(msg);
        return true; 
    }

    @Override
    public void send(String channel, T msg) {
            //not in used any more, were sending from the protocol
    }

    public void disconnect(int connectionId) {
        if(connectionIdToConnectionHandle.get(connectionId) != null){
            String user = connectionIdToUser.get(connectionId);
            
            if(userNameToChannels.containsKey(user)){
                ConcurrentHashMap<Integer, String> userChannels = userNameToChannels.get(user);
                for(Integer subId : userChannels.keySet()){
                    //getting the channel for the specific key
                    String channel = userChannels.get(subId);
                    //remove the user from the channel subscribers list
                    channelToSubscribers.get(channel).remove(user);
                }
            }
            
            //deleting the user channels list
            userNameToChannels.remove(user);

            //removing the user <--> connectionId mapping
            userToConnectionId.remove(user);
            connectionIdToUser.remove(connectionId);

            //removing the connectionId <--> connectionHandler mapping
            connectionIdToConnectionHandle.remove(connectionId);
        }

    }

    //Givin a map of channels of a specific user, look for his subId for a specific topic
    public Integer topicToSubId(String topic, ConcurrentHashMap<Integer, String> channels){
        for (Integer subId : channels.keySet())
            if(channels.get(subId).equals(topic))
                return subId;
        return -1;
    }

    public synchronized int  generateId(){
        return idCounter++;
    }
    
}

