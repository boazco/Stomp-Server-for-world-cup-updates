package bgu.spl.net.impl.rci;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;

import java.io.Serializable;

public class RemoteCommandInvocationProtocol<T> implements StompMessagingProtocol<Serializable> {

    private T arg;

    public RemoteCommandInvocationProtocol(T arg) {
        this.arg = arg;
    }

    @Override
    public Serializable process(Serializable msg) {
        return ((Command) msg).execute(arg);
    }

    @Override
    public boolean shouldTerminate() {
        return false;
    }

    @Override
    public void start(int connectionId, Connections<Serializable> connections) {
        return;        
    }

}
