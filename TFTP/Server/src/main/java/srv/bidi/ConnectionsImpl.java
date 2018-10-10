package bgu.spl171.net.srv.bidi;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import bgu.spl171.net.api.bidi.Connections;


public class ConnectionsImpl<T> implements Connections<T> {

	private ConcurrentHashMap<Integer, ConnectionHandler<T>> connectedUsers = new ConcurrentHashMap<Integer, ConnectionHandler<T>>();
	private AtomicInteger id = new AtomicInteger(-1);
	
	@Override
	public synchronized boolean send(int connectionId, T msg) {
		ConnectionHandler<T> tmp=connectedUsers.get(connectionId);
		if(tmp!=null){
			tmp.send(msg);
			return true;
		}
		return false; 
	}
	

	@Override
	public synchronized void broadcast(T msg) {
		for ( int i : connectedUsers.keySet()){
			if (connectedUsers.get(i)!=null)
				send(i,msg);
		}
	}
	
	public synchronized int connect(ConnectionHandler<T> handler){
		connectedUsers.put(id.incrementAndGet(), handler);
		return id.get();
	}

	@Override
	public synchronized void disconnect(int connectionId) {
		connectedUsers.remove(connectionId);
	}

}
